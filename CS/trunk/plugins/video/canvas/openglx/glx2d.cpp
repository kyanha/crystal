/*
    Copyright (C) 1998 by Jorrit Tyberghein

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
#include "cssys/sysfunc.h"
#include "video/canvas/openglx/glx2d.h"
#include "csutil/scf.h"
#include "csutil/csinput.h"
#include "csgeom/csrect.h"
#include "csutil/cfgacc.h"
#include "iutil/plugin.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "ivideo/texture.h"
#include "iengine/texture.h"
#include "iutil/cfgfile.h"
#include "iutil/eventq.h"
#include "iutil/objreg.h"
#include "ivaria/reporter.h"

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_IBASE_EXT (csGraphics2DGLX)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iOpenGLInterface)
SCF_IMPLEMENT_IBASE_EXT_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csGraphics2DGLX::eiOpenGLInterface)
  SCF_IMPLEMENTS_INTERFACE (iOpenGLInterface)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_FACTORY (csGraphics2DGLX)

SCF_EXPORT_CLASS_TABLE (glx2d)
  SCF_EXPORT_CLASS_DEP (csGraphics2DGLX, "crystalspace.graphics2d.glx",
    "GL/X OpenGL 2D graphics driver for Crystal Space", "crystalspace.font.server.")
SCF_EXPORT_CLASS_TABLE_END

#define DEF_OGLDISP "crystalspace.graphics2d.glx.disp.empty"
#define XWIN_SCF_ID "crystalspace.window.x"
// csGraphics2DGLX function
csGraphics2DGLX::csGraphics2DGLX (iBase *iParent) :
  csGraphics2DGLCommon (iParent), cmap (0)
{
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiOpenGLInterface);
}

void csGraphics2DGLX::Report (int severity, const char* msg, ...)
{
  va_list arg;
  va_start (arg, msg);
  iReporter* rep = CS_QUERY_REGISTRY (object_reg, iReporter);
  if (rep)
  {
    rep->ReportV (severity, "crystalspace.canvas.glx2d", msg, arg);
    rep->DecRef ();
  }
  else
  {
    csPrintfV (msg, arg);
    csPrintf ("\n");
  }
  va_end (arg);
}

bool csGraphics2DGLX::Initialize (iObjectRegistry *object_reg)
{
  const char *strDriver;
  dispdriver = NULL;
  xvis = NULL;

  if (!csGraphics2DGLCommon::Initialize (object_reg))
    return false;

  csConfigAccess config(object_reg, "/config/opengl.cfg");

  iPluginManager* plugin_mgr = CS_QUERY_REGISTRY (object_reg, iPluginManager);
  if ((strDriver = config->GetStr ("Video.OpenGL.Display.Driver", NULL)))
  {
    dispdriver = CS_LOAD_PLUGIN (plugin_mgr, strDriver, iOpenGLDisp);
    if (!dispdriver)
    {
      Report (CS_REPORTER_SEVERITY_WARNING,
	      "Could not create an instance of %s ! Using NULL instead.",
	      strDriver);
    }
    else if (!dispdriver->open ())
    {
      Report (CS_REPORTER_SEVERITY_ERROR,
	      "open of displaydriver %s failed!", strDriver);
      return false;
    }
  }

  xwin = CS_LOAD_PLUGIN (plugin_mgr, XWIN_SCF_ID, iXWindow);
  if (!xwin)
  {
    Report (CS_REPORTER_SEVERITY_WARNING,
	    "Could not create an instance of %s !", XWIN_SCF_ID);
    return false;
  }

  dpy = xwin->GetDisplay ();
  screen_num = xwin->GetScreen ();

  // The texture manager only needs to know this:
  pfmt.PalEntries = 0;

  // Create the event outlet
  iEventQueue* q = CS_QUERY_REGISTRY(object_reg, iEventQueue);
  if (q != 0)
  {
    EventOutlet = q->CreateEventOutlet (this);
    q->DecRef ();
  }

  plugin_mgr->DecRef ();

  return true;
}

csGraphics2DGLX::~csGraphics2DGLX ()
{
  // Destroy your graphic interface
  XFree ((void*)xvis);
  Close ();
  if (dispdriver)
    dispdriver->DecRef();

  if (xwin)
    xwin->DecRef ();
}

bool csGraphics2DGLX::Open()
{
  if (is_open) return true;

  Report (CS_REPORTER_SEVERITY_NOTIFY, "Opening GLX2D\n");
  // We now select the visual here as with a mesa bug it is not possible
  // to destroy double buffered contexts and then create a single buffered
  // one.

  if (!CreateVisuals ())
    return false;

  xwin->SetColormap (cmap);
  xwin->SetVisualInfo (xvis);
  xwin->SetCanvas ((iGraphics2D *)this);

  if (!xwin->Open ())
  {
    Report (CS_REPORTER_SEVERITY_ERROR,
	    "Failed to open the X-Window!");
    return false;
  }
  window = xwin->GetWindow ();


  // this makes the context we created in Initialize() the current
  // context, so that all subsequent OpenGL calls will set state and
  // draw stuff on this context.  You could of couse make
  // some more contexts and switch around between several of them...
  // but we use only one here.
  glXMakeCurrent (dpy, window, active_GLContext);

  // Open your graphic interface
  if (!csGraphics2DGLCommon::Open ())
    return false;

  return true;
}

void csGraphics2DGLX::Close(void)
{
  if (!is_open) return;
    
  // Close your graphic interface
  csGraphics2DGLCommon::Close ();
  if (active_GLContext != NULL)
  {
    glXDestroyContext(dpy,active_GLContext);
    active_GLContext = NULL;
  }

  if ( dispdriver ){
      dispdriver->close();
  }

  if (xwin)
    xwin->Close ();
}

static const char *visual_class_name (int cls)
{
   switch (cls) {
      case StaticColor:
         return "StaticColor";
      case PseudoColor:
         return "PseudoColor";
      case StaticGray:
         return "StaticGray";
      case GrayScale:
         return "GrayScale";
      case TrueColor:
         return "TrueColor";
      case DirectColor:
         return "DirectColor";
      default:
         return "";
   }
}

bool csGraphics2DGLX::CreateVisuals ()
{
  int desired_attributes[] =
  {
    GLX_RGBA,
    GLX_DEPTH_SIZE, 8,
    GLX_RED_SIZE, 4,
    GLX_BLUE_SIZE, 4,
    GLX_GREEN_SIZE, 4,
    GLX_STENCIL_SIZE, 1,
    GLX_DOUBLEBUFFER,
    None
  };
  Report (CS_REPORTER_SEVERITY_NOTIFY, "Creating Context\n");
  // find a visual that supports all the features we need
  xvis = glXChooseVisual (dpy, screen_num, desired_attributes);

  // if a visual was found that we can use, make a graphics context which
  // will be bound to the application window.  If a visual was not
  // found, then try to figure out why it failed
  if (!xvis)
  {
    Report (CS_REPORTER_SEVERITY_WARNING,
    	"Could not find proper GLX visual");

    // what attribute was not supplied? we know that trying to get
    // all the attributes at once doesn't work.  provide more user info by
    // trying each of the pieces and seeing if any single piece is not provided

    // try to get a visual with 12 bit color
    int generic_attributes [] = {GLX_RGBA, GLX_DOUBLEBUFFER, GLX_DEPTH_SIZE, 1, None};
    if (!(xvis=glXChooseVisual(dpy, screen_num, generic_attributes)) )
    {
        Report (CS_REPORTER_SEVERITY_WARNING,
        	"Graphics display does not support a generic visual with double buffer and depth buffer");
		
        int doublebuffer_attributes [] = {GLX_RGBA, GLX_DOUBLEBUFFER, None};
        if (!(xvis=glXChooseVisual (dpy, screen_num, doublebuffer_attributes)))
	{
           Report (CS_REPORTER_SEVERITY_WARNING,
	   "Graphics display does not provide double buffering");

           int depthbuffer_attributes [] = {GLX_RGBA, GLX_DEPTH_SIZE,1, None};
           if (!(xvis=glXChooseVisual (dpy, screen_num, depthbuffer_attributes)))
           {
              Report (CS_REPORTER_SEVERITY_WARNING,
        	"Graphics display does not support a depth buffer");

              int color_attributes[] =
              {GLX_RGBA, GLX_RED_SIZE,4, GLX_BLUE_SIZE,4,GLX_GREEN_SIZE,4,None};

              if (!(xvis=glXChooseVisual(dpy, screen_num, color_attributes)) )
              {
                Report (CS_REPORTER_SEVERITY_WARNING,
             	"Graphics display does not support at least 12 bit color");
                return false;
	      }
	   }
	}
     }
  }

  active_GLContext = glXCreateContext(dpy, xvis, 0, GL_TRUE);
  cmap = XCreateColormap (dpy, RootWindow (dpy, xvis->screen),
			    xvis->visual, AllocNone);

  Report (CS_REPORTER_SEVERITY_NOTIFY, "Video driver GL/X version %s",
    glXIsDirect (dpy, active_GLContext) ? "(direct renderer)" : "");

  Depth = xvis->depth;

  if (Depth == 24 || Depth == 32)
    pfmt.PixelBytes = 4;
  else pfmt.PixelBytes = 2;

  Report (CS_REPORTER_SEVERITY_NOTIFY, "Visual ID: %x, %dbit %sn",
	    xvis->visualid, Depth,
	    visual_class_name (xvis->c_class));

  int ctype, frame_buffer_depth, size_depth_buffer, level;
  glXGetConfig(dpy, xvis, GLX_RGBA, &ctype);
//glXGetConfig(dpy, xvis, GLX_DOUBLEBUFFER, &double_buffer);
  glXGetConfig(dpy, xvis, GLX_BUFFER_SIZE, &frame_buffer_depth);
  glXGetConfig(dpy, xvis, GLX_DEPTH_SIZE, &size_depth_buffer);
  glXGetConfig(dpy, xvis, GLX_LEVEL, &level);

  int alpha_bits = 0;
  if (ctype)
  {
    pfmt.RedMask = xvis->red_mask;
    pfmt.GreenMask = xvis->green_mask;
    pfmt.BlueMask = xvis->blue_mask;
    glXGetConfig(dpy, xvis, GLX_RED_SIZE, &pfmt.RedBits);
    glXGetConfig(dpy, xvis, GLX_GREEN_SIZE, &pfmt.GreenBits);
    glXGetConfig(dpy, xvis, GLX_BLUE_SIZE, &pfmt.BlueBits);
    glXGetConfig(dpy, xvis, GLX_ALPHA_SIZE, &alpha_bits);
    int bit;
    bit=0; while (!(pfmt.RedMask & (1<<bit))) bit++; pfmt.RedShift = bit;
    bit=0; while (!(pfmt.GreenMask & (1<<bit))) bit++; pfmt.GreenShift = bit;
    bit=0; while (!(pfmt.BlueMask & (1<<bit))) bit++; pfmt.BlueShift = bit;
  }

  // Report Info
  Report (CS_REPORTER_SEVERITY_NOTIFY,
  	"Frame buffer: %dbit ", frame_buffer_depth);
  if (ctype)
  {
    if (pfmt.RedMask > pfmt.BlueMask)
    {
      Report (CS_REPORTER_SEVERITY_NOTIFY,
		"R%d:G%d:B%d:A%d, ",
		pfmt.RedBits, pfmt.GreenBits, pfmt.BlueBits, alpha_bits);
    }
    else
    {
      Report (CS_REPORTER_SEVERITY_NOTIFY,
		"B%d:G%d:R%d:A%d, ",
		pfmt.BlueBits, pfmt.GreenBits, pfmt.RedBits, alpha_bits);
    }
  }
  Report (CS_REPORTER_SEVERITY_NOTIFY, "level %d, double buffered", level);
  Report (CS_REPORTER_SEVERITY_NOTIFY, "Depth buffer: %dbit", size_depth_buffer);

  pfmt.complete ();
  return true;
}

bool csGraphics2DGLX::PerformExtensionV (char const* command, va_list args)
{
  if (!strcasecmp (command, "fullscreen"))
  {
    xwin->SetFullScreen (!xwin->GetFullScreen ());
    return true;
  }
  return csGraphics2DGLCommon::PerformExtensionV (command, args);
}

void csGraphics2DGLX::Print (csRect * /*area*/)
{
  glXSwapBuffers (dpy,window);
  //glFlush (); // not needed?
  XSync (dpy, False);
}


void csGraphics2DGLX::SetFullScreen (bool yesno)
{
  csGraphics2D::SetFullScreen (yesno);
  xwin->SetFullScreen (yesno);
}

void csGraphics2DGLX::AllowResize (bool iAllow)
{
  AllowResizing = iAllow;
  xwin->AllowResize (iAllow);
}


#undef XWIN_SCF_ID

