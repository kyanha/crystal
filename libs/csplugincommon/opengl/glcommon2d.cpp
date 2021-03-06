/*
    Copyright (C) 1998-2001 by Jorrit Tyberghein

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "cssysdef.h"
#include "csqint.h"

#include "igraphic/image.h"
#include "igraphic/imageio.h"
#include "iutil/document.h"
#include "iutil/eventq.h"
#include "iutil/objreg.h"
#include "iutil/vfs.h"
#include "ivaria/reporter.h"

#include "csgeom/box.h"
#include "csgeom/csrect.h"
#include "csgeom/math.h"
#include "csgeom/projections.h"
#include "csgeom/vector3.h"
#include "csplugincommon/opengl/assumedstate.h"
#include "csplugincommon/opengl/glcommon2d.h"
#include "csplugincommon/opengl/glstates.h"
#include "csutil/xmltiny.h"
#include "csutil/eventnames.h"

namespace CS
{
  namespace PluginCommon
  {
    namespace GL
    {
      Graphics2DCommon::Graphics2DCommon () :
        statecache (0), statecontext (0),
          hasRenderTarget (false)
      {
        multiFavorQuality = false;
        fontCache = 0;
        useCombineTE = false;

        ssPool = 0;
      }

      bool Graphics2DCommon::Initialize (iObjectRegistry *object_reg)
      {
        if (!CS::PluginCommon::Graphics2DCommon::Initialize (object_reg))
          return false;

        csRef<iOpenGLInterface> gli (scfQueryInterface<iOpenGLInterface> (this));
        ext.Initialize (object_reg, gli);

        csConfigAccess config (object_reg);
        multiFavorQuality = config->GetBool ("Video.OpenGL.MultisampleFavorQuality");

        return true;
      }

      Graphics2DCommon::~Graphics2DCommon ()
      {
        Close ();

        while (ssPool)
        {
          csGLScreenShot* next = ssPool->poolNext;
          delete ssPool;
          ssPool = next;
        }
      }

      bool Graphics2DCommon::Open ()
      {
        if (is_open) return true;

        ext.Open ((const char*)glGetString (GL_EXTENSIONS));
        OpenDriverDB ();

        statecache = new csGLStateCache (&ext);
        statecontext = new csGLStateCacheContext (&ext);
        statecache->SetContext (statecontext);
        statecontext->InitCache();

        // initialize font cache object
        csGLFontCache* GLFontCache = new csGLFontCache (this);
        fontCache = GLFontCache;

        statecache->Enable_GL_SCISSOR_TEST ();
        /* Some drivers need that to get the initial scissor right
        * (Mesa DRI Intel(R) 915GM 20050225 in this case) */
        glScissor (0, 0, 1, 1);

        if (!CS::PluginCommon::Graphics2DCommon::Open ())
          return false;

        const char *renderer = (const char *)glGetString (GL_RENDERER);
        const char *vendor = (const char *)glGetString (GL_VENDOR);
        const char *version = (const char *)glGetString (GL_VERSION);
        if (renderer || version || vendor)
          Report (CS_REPORTER_SEVERITY_NOTIFY,
            "OpenGL renderer: %s (vendor: %s) version %s",
            renderer ? renderer : "unknown", vendor ? vendor: "unknown",
            version ? version : "unknown");

        int fbWidth, fbHeight;
        GetCanvas()->GetFramebufferDimensions (fbWidth, fbHeight);
        Report (CS_REPORTER_SEVERITY_NOTIFY,
          "Using %s mode at resolution %dx%d.",
          GetFullScreen() ? "full screen" : "windowed", fbWidth, fbHeight);

        if (version)
        {
          // initialize GL version pseudo-extensions
          int n, vMajor, vMinor, vRelease;
          n = sscanf (version, "%d.%d.%d", &vMajor, &vMinor, &vRelease);
          if (n >= 2)
          {
            // Sanity check
            if ((vMajor < 1) || ((vMajor == 1) && (vMinor < 1)))
            {
                Report (CS_REPORTER_SEVERITY_ERROR,
                  "OpenGL >= 1.1 is required, but only %d.%d is present.",
                  vMajor, vMinor);
            }
            if ((vMajor > 1) || ((vMajor == 1) && (vMinor >= 2)))
            {
                //ext.InitGL_version_1_2 ();
            }
            if ((vMajor > 1) || ((vMajor == 1) && (vMinor >= 3)))
            {
                //ext.InitGL_version_1_3 ();
            }
            if ((vMajor > 2) || ((vMajor == 2) && (vMinor >= 0)))
            {
                    ext.InitGL_version_2_0 ();
            }
            if ((vMajor > 2) || ((vMajor == 2) && (vMinor >= 1)))
            {
                    ext.InitGL_version_2_1 ();
            }
          }
        }

        ext.InitGL_ARB_multitexture ();
        ext.InitGL_ARB_texture_env_combine ();
        if (!ext.CS_GL_ARB_texture_env_combine)
          ext.InitGL_EXT_texture_env_combine ();
        useCombineTE = ext.CS_GL_ARB_multitexture &&
          (ext.CS_GL_ARB_texture_env_combine || ext.CS_GL_EXT_texture_env_combine);
        if (useCombineTE)
        {
          for (GLint u = statecache->GetNumTexCoords(); u-- > 0; )
          {
            statecache->SetCurrentTCUnit (u);
            statecache->ActivateTCUnit (csGLStateCache::activateTexEnv);
            glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
          }
        }
        ext.InitGL_EXT_blend_func_separate ();
        ext.InitGL_ARB_multisample();

        csRef<iOpenGLCanvas> gl_canvas (scfQueryInterface<iOpenGLCanvas> (GetCanvas()));
        CS_ASSERT(gl_canvas);
        if (ext.CS_GL_ARB_multisample)
        {
          GLint glmultisamp = gl_canvas->GetMultiSamples();
          glGetIntegerv (GL_SAMPLES_ARB, &glmultisamp);

          if (glmultisamp)
          {
            if (glmultisamp != gl_canvas->GetMultiSamples())
                Report (CS_REPORTER_SEVERITY_NOTIFY,
                  "Multisample: actually %d samples", (int)glmultisamp);

            ext.InitGL_NV_multisample_filter_hint();
            if (ext.CS_GL_NV_multisample_filter_hint)
            {
              glHint (GL_MULTISAMPLE_FILTER_HINT_NV,
                multiFavorQuality ? GL_NICEST : GL_FASTEST);

              GLint msHint;
              glGetIntegerv (GL_MULTISAMPLE_FILTER_HINT_NV, &msHint);
              Report (CS_REPORTER_SEVERITY_NOTIFY,
                "Multisample settings: %s", ((msHint == GL_NICEST) ? "quality" :
                ((msHint == GL_FASTEST) ? "performance" : "unknown")));
            }
          }
          else
          {
            Report (CS_REPORTER_SEVERITY_NOTIFY,
                "Multisample: disabled");
          }
        }

        GLFontCache->Setup();

        CS::PluginCommon::GL::SetAssumedState (statecache, &ext);
        glClearColor (0., 0., 0., 0.);

        statecache->SetMatrixMode (GL_MODELVIEW);
        glLoadIdentity ();

        glViewport (0, 0, vpWidth, vpHeight);
        Clear (0);

        return true;
      }

      void Graphics2DCommon::Close ()
      {
        if (!is_open) return;
        CS::PluginCommon::Graphics2DCommon::Close ();
        delete statecontext; statecontext = 0;
        delete statecache; statecache = 0;
        ext.Close ();
        driverdb.Close ();
      }

      void Graphics2DCommon::SetClipRect (int xmin, int ymin, int xmax, int ymax)
      {
        if (fontCache) ((csGLFontCache*)fontCache)->FlushText ();

        int fbWidth, fbHeight;
        GetCanvas()->GetFramebufferDimensions (fbWidth, fbHeight);
        CS::PluginCommon::Graphics2DCommon::SetClipRect (xmin, ymin, xmax, ymax);
        glScissor (vpLeft + ClipX1, fbHeight - (vpTop + ClipY2),
          ClipX2 - ClipX1, ClipY2 - ClipY1);
      }

      bool Graphics2DCommon::BeginDraw ()
      {
        if (!CS::PluginCommon::Graphics2DCommon::BeginDraw ())
          return false;
        //if (FrameBufferLocked != 1)
          //return true;

        /* Note: the renderer relies on this function to setup
        * matrices etc. So be careful when changing stuff. */

        int fbWidth, fbHeight;
        GetCanvas()->GetFramebufferDimensions (fbWidth, fbHeight);
        glViewport (vpLeft, fbHeight - (vpTop + vpHeight), vpWidth, vpHeight);
        if (!hasRenderTarget)
        {
          statecache->SetMatrixMode (GL_PROJECTION);
          glLoadIdentity ();
          glOrtho (0, vpWidth, 0, vpHeight, -1.0, 10.0);
        }
        else
        {
          // Assume renderer does the correct setup for RT
        }

        statecache->SetMatrixMode (GL_MODELVIEW);
        glLoadIdentity ();
        glClearColor (0., 0., 0., 0.);

        statecache->SetShadeModel (GL_FLAT);
        if (useCombineTE)
        {
          glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
          glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, GL_SRC_COLOR);
          glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_PRIMARY_COLOR);
          glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, GL_SRC_COLOR);
          glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_MODULATE);
          glTexEnvf (GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, 1.0f);

          glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, GL_TEXTURE);
          glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_ARB, GL_SRC_ALPHA);
          glTexEnvi (GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_ARB, GL_PRIMARY_COLOR);
          glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_ARB, GL_SRC_ALPHA);
          glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_MODULATE);
          glTexEnvf (GL_TEXTURE_ENV, GL_ALPHA_SCALE, 1.0f);
        }
        else
          glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        statecache->SetColorMask (true, true, true, true);

        statecache->Enable_GL_BLEND ();
        if (ext.CS_GL_EXT_blend_func_separate)
          // Blending mode that's premultiplied alpha friendly
          statecache->SetBlendFuncSeparate (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
                            GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        else
          statecache->SetBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        return true;
      }

      void Graphics2DCommon::FinishDraw ()
      {
        ((csGLFontCache*)fontCache)->FlushText ();
        CS::PluginCommon::Graphics2DCommon::FinishDraw();
        if (FrameBufferLocked != 0) return;
        //statecache->Disable_GL_SCISSOR_TEST ();
      }

      void Graphics2DCommon::DecomposeColor (int iColor,
        GLubyte &oR, GLubyte &oG, GLubyte &oB, GLubyte &oA)
      {
        oA = 255 - ((iColor >> 24) & 0xff);
        oR = (iColor >> 16) & 0xff;
        oG = (iColor >> 8) & 0xff;
        oB = iColor & 0xff;
      }

      void Graphics2DCommon::DecomposeColor (int iColor,
        float &oR, float &oG, float &oB, float &oA)
      {
        GLubyte r, g, b, a;
        DecomposeColor (iColor, r, g, b, a);
        oR = r / 255.0;
        oG = g / 255.0;
        oB = b / 255.0;
        oA = a / 255.0;
      }

      void Graphics2DCommon::setGLColorfromint (int color)
      {
        GLubyte oR, oG, oB, oA;
        DecomposeColor (color, oR, oG, oB, oA);
        glColor4ub (oR, oG, oB, oA);
      }

      csGLScreenShot* Graphics2DCommon::GetScreenShot ()
      {
        csGLScreenShot* res;
        if (ssPool)
        {
          res = ssPool;
          ssPool = ssPool->poolNext;
        }
        else
        {
      #include "csutil/custom_new_disable.h"
          res = new csGLScreenShot (this);
      #include "csutil/custom_new_enable.h"
        }
        IncRef();
        return res;
      }

      void Graphics2DCommon::RecycleScreenShot (csGLScreenShot* shot)
      {
        shot->poolNext = ssPool;
        ssPool = shot;
        DecRef();
      }

      void Graphics2DCommon::Report (int severity, const char* msg, ...)
      {
        va_list args;
        va_start (args, msg);
        csReportV (object_reg, severity,
          "crystalspace.graphics2d.openglcommon",
          msg, args);
        va_end (args);
      }

      #include "csutil/custom_new_disable.h"
      void Graphics2DCommon::OpenDriverDB (const char* phase)
      {
        csConfigAccess config (object_reg);
        const char* driverDB = config->GetStr ("Video.OpenGL.DriverDB.Path",
          "/config/gldrivers.xml");
        int driverDBprio = config->GetInt ("Video.OpenGL.DriverDB.Priority",
          iConfigManager::ConfigPriorityPlugin + 10);

        csRef<iVFS> vfs = csQueryRegistry<iVFS> (object_reg);
        csRef<iFile> dbfile = vfs->Open (driverDB, VFS_FILE_READ);
        if (!dbfile)
        {
          Report (CS_REPORTER_SEVERITY_WARNING,
            "Could not open driver database file %s", CS::Quote::Single (driverDB));
          return;
        }

        csRef<iDocumentSystem> docsys =
          csQueryRegistry<iDocumentSystem> (object_reg);
        if (!docsys.IsValid())
          docsys.AttachNew (new csTinyDocumentSystem ());
        csRef<iDocument> doc (docsys->CreateDocument ());

        const char* err = doc->Parse (dbfile, true);
        if (err != 0)
        {
          Report (CS_REPORTER_SEVERITY_WARNING,
            "Error parsing driver database: %s", err);
          return;
        }

        csRef<iDocumentNode> dbRoot (doc->GetRoot()->GetNode ("gldriverdb"));
        if (!dbRoot.IsValid())
        {
          Report (CS_REPORTER_SEVERITY_WARNING,
            "Driver database lacks <gldriverdb> node");
          return;
        }

        driverdb.Open (this, dbRoot, phase, driverDBprio);
      }
      #include "csutil/custom_new_enable.h"

      const char* Graphics2DCommon::GetHWRenderer()
      {
          return (char*)glGetString (GL_RENDERER);
      }

      const char* Graphics2DCommon::GetHWGLVersion()
      {
          return (char*)glGetString (GL_VERSION);
      }

      const char* Graphics2DCommon::GetHWVendor()
      {
          return (char*)glGetString (GL_VENDOR);
      }

      const char* Graphics2DCommon::GetRendererString (const char* str)
      {
        if (strcmp (str, "renderer") == 0)
        {
          return (char*)glGetString (GL_RENDERER);
        }
        else if (strcmp (str, "vendor") == 0)
        {
          return (char*)glGetString (GL_VENDOR);
        }
        else if (strcmp (str, "glversion") == 0)
        {
          return (char*)glGetString (GL_VERSION);
        }
        else if (strcmp (str, "platform") == 0)
        {
          return CS_PLATFORM_NAME;
        }
        return 0;
      }

      const char* Graphics2DCommon::GetVersionString (const char* ver)
      {
        if (strcmp (ver, "gl") == 0)
        {
          return (char*)glGetString (GL_VERSION);
        }
        else if (strcmp (ver, "glvendor") == 0)
        {
          /* The "vendor" part of the GL version
          * (separated from the normal version by a space) */
          const char* glv = (char*)glGetString (GL_VERSION);
          const char* space = strchr (glv, ' ');
          if (space) return space+1;
        }
        return 0;
      }

      void Graphics2DCommon::Clear (int color)
      {
        ((csGLFontCache*)fontCache)->FlushText ();

        float r, g, b, a;
        DecomposeColor (color, r, g, b, a);
        glClearColor (r, g, b, a);
        glClear (GL_COLOR_BUFFER_BIT);
      }

      void Graphics2DCommon::DrawLine (
        float x1, float y1, float x2, float y2, int color)
      {
        ((csGLFontCache*)fontCache)->FlushText ();

        // prepare for 2D drawing--so we need no fancy GL effects!
        statecache->Disable_GL_TEXTURE_2D ();
        bool gl_alphaTest = (glIsEnabled(GL_ALPHA_TEST) == GL_TRUE);
        if (gl_alphaTest) statecache->Disable_GL_ALPHA_TEST ();
        setGLColorfromint (color);

        // opengl doesn't draw the last pixel of a line, but we
        // want that pixel anyway, add the pixel.
        /*if(y1==y2){ // horizontal lines
          if(x2>x1) x2++;
          else if(x1>x2) x1++;
        }
        if(x1==x2) { // vertical lines
          if(y2>y1) y2++;
          else if(y1>y2) y1++;
        }
        if(x1!=x2 && y1!=y2) // diagonal lines
        {
          if(x2>x1) x2++;
          else if(x1>x2) x1++;
        }*/

        // This extends the line enough to get the last pixel of the line on GL
        // Note! If this doesn't work in OR, just revert to old way for OR and
        // not for NR. It's tested (at least a bit :) and seems to work in NR.
        csVector2 delta (x2-x1, y2-y1);
        if (delta.SquaredNorm ()>EPSILON*EPSILON)
        {
          delta *= 1.4142135623731/delta.Norm ();
          x2 += delta.x;
          y2 += delta.y;
        }

        // This is a workaround for a hard-to-really fix problem with OpenGL:
        // whole Y coordinates are "rounded" up, this leads to one-pixel-shift
        // compared to software line drawing. This is not exactly a bug (because
        // this is an on-the-edge case) but it's different, thus we'll slightly
        // shift whole coordinates down.
        // but csQint(y1) == y1 is too coarse a test.
        if (fabs(float(int(y1))-y1) < 0.1f) { y1 += 0.05f; }
        if (fabs(float(int(y2))-y2) < 0.1f) { y2 += 0.05f; }
        if (fabs(float(int(x1))-x1) < 0.1f) { x1 += 0.05f; }
        if (fabs(float(int(x2))-x2) < 0.1f) { x2 += 0.05f; }

        // Notice: using height-y has range 1..height, but this is OK.
        //    This is because on opengl y=0.0 is off screen, as is y=height.
        //    using height-sy gives output on screen which is identical to
        //    using the software canvas.
        //    the same goes for all the other DrawX functions.

        glBegin (GL_LINES);
        glVertex2f (x1, vpHeight - y1);
        glVertex2f (x2, vpHeight - y2);
        glEnd ();

        if (gl_alphaTest) statecache->Enable_GL_ALPHA_TEST ();
      }

      void Graphics2DCommon::DrawLineProjected
        (const csVector3& _v1, const csVector3& _v2, float fov, int color)
      {
        csVector3 v1 (_v1), v2 (_v2);
        if (!DrawLineNearClip (v1, v2)) return;

        float iz1 = fov / v1.z;
        int px1 = csQint (v1.x * iz1 + (vpWidth / 2));
        int py1 = vpHeight - 1 - csQint (v1.y * iz1 + (vpHeight / 2));
	// The equations commented in are if the fov is aquired through
	// iPerspectiveCamera::GetVerticalFOV()
        //int px1 = csQint (v1.x * iz1 * vpWidth + (vpWidth / 2));
        //int py1 = vpHeight - 1 - csQint (v1.y * iz1 * vpHeight + (vpHeight / 2));
        float iz2 = fov / v2.z;
        int px2 = csQint (v2.x * iz2 + (vpWidth / 2));
        int py2 = vpHeight - 1 - csQint (v2.y * iz2 + (vpHeight / 2));
        //int px2 = csQint (v2.x * iz2 * vpWidth + (vpWidth / 2));
        //int py2 = vpHeight - 1 - csQint (v2.y * iz2 * vpHeight + (vpHeight / 2));

        DrawLine (px1, py1, px2, py2, color);
      }

      void Graphics2DCommon::DrawLineProjected
        (const csVector3& _v1, const csVector3& _v2, float fov, float aspect, int color)
      {
        csVector3 v1 (_v1), v2 (_v2);
        if (!DrawLineNearClip (v1, v2)) return;

        float iz1 = fov / v1.z;
        int px1 = csQint (v1.x * iz1 * vpWidth / aspect + (vpWidth / 2));
        int py1 = vpHeight - 1 - csQint (v1.y * iz1 * vpHeight + (vpHeight / 2));
        float iz2 = fov / v2.z;
        int px2 = csQint (v2.x * iz2 * vpWidth / aspect + (vpWidth / 2));
        int py2 = vpHeight - 1 - csQint (v2.y * iz2 * vpHeight + (vpHeight / 2));

        DrawLine (px1, py1, px2, py2, color);
      }

      void Graphics2DCommon::DrawLineProjected
        (const csVector3& _v1, const csVector3& _v2,
        const CS::Math::Matrix4& projection, int color)
      {
        csVector3 v1 (_v1), v2 (_v2);
        if (!DrawLineNearClip (v1, v2)) return;

        csVector4 v1p (projection * csVector4 (v1));
        v1p /= v1p.w;
        csVector4 v2p (projection * csVector4 (v2));
        v2p /= v2p.w;

        int px1 = csQint ((v1p.x + 1) * (vpWidth / 2));
        int py1 = vpHeight - 1 - csQint ((v1p.y + 1) * (vpHeight / 2));
        int px2 = csQint ((v2p.x + 1) * (vpWidth / 2));
        int py2 = vpHeight - 1 - csQint ((v2p.y + 1) * (vpHeight / 2));

        DrawLine (px1, py1, px2, py2, color);
      }

      bool Graphics2DCommon::DrawLineNearClip (csVector3 & v1, csVector3 & v2)
      {
        if (v1.z < SMALL_Z && v2.z < SMALL_Z)
          return false;

        if (v1.z < SMALL_Z)
        {
          // x = t*(x2-x1)+x1;
          // y = t*(y2-y1)+y1;
          // z = t*(z2-z1)+z1;
          float t = (SMALL_Z - v1.z) / (v2.z - v1.z);
          v1.x = t * (v2.x - v1.x) + v1.x;
          v1.y = t * (v2.y - v1.y) + v1.y;
          v1.z = SMALL_Z;
        }
        else if (v2.z < SMALL_Z)
        {
          // x = t*(x2-x1)+x1;
          // y = t*(y2-y1)+y1;
          // z = t*(z2-z1)+z1;
          float t = (SMALL_Z - v1.z) / (v2.z - v1.z);
          v2.x = t * (v2.x - v1.x) + v1.x;
          v2.y = t * (v2.y - v1.y) + v1.y;
          v2.z = SMALL_Z;
        }
        return true;
      }

      void Graphics2DCommon::DrawBoxProjected
      (const csBox3& box, const csTransform& object2camera,
      const CS::Math::Matrix4& projection, int color)
      {
        csVector3 vxyz = object2camera * box.GetCorner (CS_BOX_CORNER_xyz);
        csVector3 vXyz = object2camera * box.GetCorner (CS_BOX_CORNER_Xyz);
        csVector3 vxYz = object2camera * box.GetCorner (CS_BOX_CORNER_xYz);
        csVector3 vxyZ = object2camera * box.GetCorner (CS_BOX_CORNER_xyZ);
        csVector3 vXYz = object2camera * box.GetCorner (CS_BOX_CORNER_XYz);
        csVector3 vXyZ = object2camera * box.GetCorner (CS_BOX_CORNER_XyZ);
        csVector3 vxYZ = object2camera * box.GetCorner (CS_BOX_CORNER_xYZ);
        csVector3 vXYZ = object2camera * box.GetCorner (CS_BOX_CORNER_XYZ);
        DrawLineProjected (vxyz, vXyz, projection, color);
        DrawLineProjected (vXyz, vXYz, projection, color);
        DrawLineProjected (vXYz, vxYz, projection, color);
        DrawLineProjected (vxYz, vxyz, projection, color);
        DrawLineProjected (vxyZ, vXyZ, projection, color);
        DrawLineProjected (vXyZ, vXYZ, projection, color);
        DrawLineProjected (vXYZ, vxYZ, projection, color);
        DrawLineProjected (vxYZ, vxyZ, projection, color);
        DrawLineProjected (vxyz, vxyZ, projection, color);
        DrawLineProjected (vxYz, vxYZ, projection, color);
        DrawLineProjected (vXyz, vXyZ, projection, color);
        DrawLineProjected (vXYz, vXYZ, projection, color);
      }

      void Graphics2DCommon::DrawBox (int x, int y, int w, int h, int color)
      {
        ((csGLFontCache*)fontCache)->FlushText ();

        statecache->Disable_GL_TEXTURE_2D ();
        y = vpHeight - y;
        // prepare for 2D drawing--so we need no fancy GL effects!
        setGLColorfromint (color);

        glBegin (GL_QUADS);
        glVertex2i (x, y);
        glVertex2i (x + w, y);
        glVertex2i (x + w, y - h);
        glVertex2i (x, y - h);
        glEnd ();
      }

      void Graphics2DCommon::DrawPixel (int x, int y, int color)
      {
        ((csGLFontCache*)fontCache)->FlushText ();

        // prepare for 2D drawing--so we need no fancy GL effects!
        statecache->Disable_GL_TEXTURE_2D ();

        // using floating point pixel addresses to fix an on-the-edge case.
        // offsetting the y by a little just like for DrawLine.
        // The whole pixels get rounded up, shifting the drawpixel.
        float y1 = y;
        if (fabs(float(int(y1))-y1) < 0.1f) { y1 += 0.05f; }
        float x1 = x;
        if (fabs(float(int(x1))-x1) < 0.1f) { x1 += 0.05f; }
        setGLColorfromint (color);
        glBegin (GL_POINTS);
        glVertex2f (x1, vpHeight - y1);
        glEnd ();
      }

      void Graphics2DCommon::DrawPixels (
        csPixelCoord const* pixels, int num_pixels, int color)
      {
        ((csGLFontCache*)fontCache)->FlushText ();

        // prepare for 2D drawing--so we need no fancy GL effects!
        statecache->Disable_GL_TEXTURE_2D ();

        setGLColorfromint (color);

        int i;
        glBegin (GL_POINTS);
        for (i = 0 ; i < num_pixels ; i++)
        {
          int x = pixels->x;
          int y = pixels->y;
          pixels++;
          glVertex2i (x, vpHeight - y);
        }
        glEnd ();
      }

      void Graphics2DCommon::Blit (int x, int y, int w, int h,
          unsigned char const* data)
      {
        ((csGLFontCache*)fontCache)->FlushText ();

        int orig_x = x;
        int orig_y = y;

        // If vertical clipping is needed we skip the initial part.
        data += 4*w*(y-orig_y);
        // Same for horizontal clipping.
        data += 4*(x-orig_x);

        statecache->Disable_GL_TEXTURE_2D ();
        bool gl_alphaTest = (glIsEnabled(GL_ALPHA_TEST) == GL_TRUE);
        if (gl_alphaTest) statecache->Disable_GL_ALPHA_TEST ();

        glColor3f (0., 0., 0.);
        /*
          @@@ HACK When a render target was set, the screen is set up
          so every drawing takes place in a rect in the upper left, but flipped.
          However, the raster position is transformed, but glDrawPixels() always
          takes those as the lower left dest coord (in window.) So it has to drawn
          h pixels farther down.
        */
        glRasterPos2i (x, vpHeight-y);
        if (!hasRenderTarget)
        {
          glPixelZoom (1.0f, -1.0f);
        }
        glDrawPixels (w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
        if (!hasRenderTarget)
          glPixelZoom (1.0f, 1.0f);

        if (gl_alphaTest) statecache->Enable_GL_ALPHA_TEST ();
      }

      void Graphics2DCommon::GetPixel (int x, int y, uint8 &oR, uint8 &oG, uint8 &oB)
      {
        uint8 dummy;
        this->GetPixel (x, y, oR, oG, oB, dummy);
      }

      void Graphics2DCommon::GetPixel (int x, int y, uint8 &oR, uint8 &oG, uint8 &oB, uint8 &oA)
      {
        uint8 px[4];
        if (!hasRenderTarget)
          y = vpHeight - y;
        glReadPixels (x, y, 1, 1, GL_RGBA,
          GL_UNSIGNED_BYTE, px);
        oR = px[0];
        oG = px[1];
        oB = px[2];
        oA = px[3];
      }

      csPtr<iImage> Graphics2DCommon::ScreenShot ()
      {
        ((csGLFontCache*)fontCache)->FlushText ();

        // Need to resolve pixel alignment issues
        int screen_width = vpWidth * (4);
        uint8* screen_shot = new uint8 [screen_width * vpHeight];
        //if (!screen_shot) return 0;

        glReadPixels (0, 0, vpWidth, vpHeight, GL_RGBA,
          GL_UNSIGNED_BYTE, screen_shot);

        csGLScreenShot* ss = GetScreenShot ();
        ss->SetData (screen_shot);

        delete[] screen_shot;

        return ss;
      }

      bool Graphics2DCommon::ScreenShot (unsigned char* buffer)
	  {
        glReadPixels (0, 0, vpWidth, vpHeight, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
		return true;
	  }

      bool Graphics2DCommon::PerformExtensionV (char const* command, va_list args)
      {
        if (!strcasecmp (command, "flush"))
        {
          glFlush ();
          glFinish ();
          return true;
        }
        if (!strcasecmp (command, "getstatecache"))
        {
          csGLStateCache** cache = va_arg (args, csGLStateCache**);
          *cache = statecache;
          return true;
        }
        if (!strcasecmp (command, "getextmanager"))
        {
          csGLExtensionManager** extmgr = va_arg (args, csGLExtensionManager**);
          *extmgr = &ext;
          return true;
        }
        if (!strcasecmp (command, "glflushtext"))
        {
          ((csGLFontCache*)fontCache)->FlushText ();
          return true;
        }
        if (!strcasecmp (command, "userendertarget"))
        {
          int hasRenderTarget = va_arg (args, int);
          this->hasRenderTarget = (hasRenderTarget != 0);
          return true;
        }
        else
          return CS::PluginCommon::Graphics2DCommon::PerformExtensionV (command, args);
      }

      bool Graphics2DCommon::DebugCommand (const char* cmdstr)
      {
        CS_ALLOC_STACK_ARRAY(char, cmd, strlen (cmdstr) + 1);
        strcpy (cmd, cmdstr);
        char* param = 0;
        char* space = strchr (cmd, ' ');
        if (space)
        {
          param = space + 1;
          *space = 0;
        }

        if (strcasecmp (cmd, "dump_fontcache") == 0)
        {
          csRef<iImageIO> imgsaver = csQueryRegistry<iImageIO> (object_reg);
          if (!imgsaver)
          {
            Report (CS_REPORTER_SEVERITY_WARNING,
              "Could not get image saver.");
            return false;
          }

          csRef<iVFS> vfs = csQueryRegistry<iVFS> (object_reg);
          if (!vfs)
          {
            Report (CS_REPORTER_SEVERITY_WARNING,
          "Could not get VFS.");
            return false;
          }

          const char* dir =
            ((param != 0) && (*param != 0)) ? param : "/tmp/fontcachedump/";
          csRefArray<iImage> images;
          ((csGLFontCache*)fontCache)->DumpFontCache (images);

          csString outfn;
          for (size_t i = 0; i < images.GetSize (); i++)
          {
            csRef<iDataBuffer> buf = imgsaver->Save (images[i], "image/png");
            if (!buf)
            {
                Report (CS_REPORTER_SEVERITY_WARNING,
                  "Could not save font cache page.");
            }
            else
            {
                outfn.Format ("%s%zu.png", dir, i);
                if (!vfs->WriteFile (outfn, (char*)buf->GetInt8 (), buf->GetSize ()))
                {
                  Report (CS_REPORTER_SEVERITY_WARNING,
                    "Could not write to %s.", outfn.GetData ());
                }
                else
                {
                  Report (CS_REPORTER_SEVERITY_NOTIFY,
                    "Dumped font cache page to %s", outfn.GetData ());
                }
            }
          }

          return true;
        }

        return false;
      }

      void Graphics2DCommon::SetViewport (int left, int top, int width, int height)
      {
        int fbWidth, fbHeight;
        GetCanvas()->GetFramebufferDimensions (fbWidth, fbHeight);
        vpLeft = left; vpTop = top; vpWidth = width; vpHeight = height;
        glViewport (vpLeft, fbHeight - (vpTop + vpHeight), vpWidth, vpHeight);
        glScissor (vpLeft + ClipX1, fbHeight - (vpTop + ClipY2),
          ClipX2 - ClipX1, ClipY2 - ClipY1);
      }

      bool Graphics2DCommon::Resize (int width, int height)
      {
        if (is_open)
          ((csGLFontCache*)fontCache)->FlushText ();

        if (!CS::PluginCommon::Graphics2DCommon::Resize (width, height))
          return false;

        return true;
      }
    } // namespace GL
  } // namespace PluginCommon
} // namespace CS

csGraphics2DGLCommon::csGraphics2DGLCommon (iBase *iParent)
  : scfImplementationType (this, iParent)
{
}

csGraphics2DGLCommon::~csGraphics2DGLCommon ()
{
}

bool csGraphics2DGLCommon::Initialize (iObjectRegistry *object_reg)
{
  if (!csGraphics2D::Initialize (object_reg))
    return false;
  if (!CS::PluginCommon::GL::Graphics2DCommon::Initialize (object_reg))
    return false;

  CS_ASSERT (object_reg != 0);
  /* Note: r3dopengl.cfg is also added by the renderer. This is done because
  * either the canvas or the renderer may be loaded before the other, but
  * both need settings from that file. */
  config.AddConfig (object_reg, "/config/r3dopengl.cfg");
  CS::PluginCommon::GL::CanvasCommonBase::Initialize (object_reg);

  return true;
}
