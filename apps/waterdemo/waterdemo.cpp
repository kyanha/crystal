/*
    Copyright (C) 2001 by Jorrit Tyberghein

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
#include "iutil/vfs.h"
#include "csgeom/polyclip.h"
#include "csgeom/transfrm.h"
#include "csutil/cscolor.h"
#include "cstool/initapp.h"
#include "iengine/camera.h"
#include "iengine/engine.h"
#include "iengine/material.h"
#include "iengine/mesh.h"
#include "iengine/movable.h"
#include "iengine/sector.h"
#include "igeom/clip2d.h"
#include "iutil/cmdline.h"
#include "iutil/eventq.h"
#include "iutil/event.h"
#include "iutil/objreg.h"
#include "iutil/object.h"
#include "iutil/csinput.h"
#include "iutil/document.h"
#include "iutil/virtclk.h"
#include "iutil/strset.h"
#include "imesh/thing/polygon.h"
#include "imesh/thing/thing.h"
#include "imesh/object.h"
#include "ivideo/graph2d.h"
#include "ivideo/txtmgr.h"
#include "ivideo/texture.h"
#include "ivideo/material.h"
#include "ivideo/fontserv.h"
#include "igraphic/imageio.h"
#include "imap/parser.h"
#include "ivaria/conout.h"
#include "ivaria/reporter.h"
#include "ivaria/stdrep.h"
#include "csutil/cmdhelp.h"
#include "ivideo/graph3d.h"
#include "iutil/plugin.h"
#include "imesh/genmesh.h"
#ifdef CS_USE_NEW_RENDERER
#  include "ivideo/shader/shader.h"
#  include "ivideo/rndbuf.h"
#  include "iengine/renderloop.h"
#  include "ivideo/rendersteps/irenderstep.h"
#  include "ivideo/rendersteps/irsfact.h"
#  include "ivideo/rendersteps/igeneric.h"
#endif
#include "cstool/csview.h"
#include "csgfx/csimgvec.h"
#include "csutil/event.h"

#include "waterdemo.h"

CS_IMPLEMENT_APPLICATION

//-----------------------------------------------------------------------------

static const float SIMPERSECOND = 60.0f;

// The global pointer to simple
csWaterDemo *waterdemo;

csWaterDemo::csWaterDemo (iObjectRegistry* object_reg)
{
  csWaterDemo::object_reg = object_reg;
  water = water1 = water2 = 0;
}

csWaterDemo::~csWaterDemo ()
{
  delete [] water;
  delete [] water1;
  delete [] water2;
}

void csWaterDemo::SetupFrame ()
{
  // First get elapsed time from the virtual clock.
  csTicks elapsed_time = vc->GetElapsedTicks ();

  updateWater (elapsed_time);

  // Now rotate the camera according to keyboard state
  static int frame;
  bool stop = false;
  if(++frame == 100)
  {
    frame=0;
    stop=true;
  }
  float speed = (elapsed_time / 1000.0) * (0.03 * 20);

  int w = r3d->GetDriver2D ()->GetWidth()/2;
  int h = r3d->GetDriver2D ()->GetHeight()/2;
  int x = mouse->GetLastX();
  int y = mouse->GetLastY();

  bool moved = false;

  if (hasfocus)
  {
    view->GetCamera ()->GetTransform ().RotateThis (CS_VEC_TILT_UP, (y-h) * 0.01);
    view->GetCamera ()->GetTransform ().RotateOther (CS_VEC_ROT_RIGHT, (x-w) * 0.01);
    r3d->GetDriver2D ()->SetMousePosition (w, h);
    moved |= (y-h);
    moved |= (x-w);
  }

  if (kbd->GetKeyState (CSKEY_UP))
  {
    view->GetCamera ()->Move (CS_VEC_FORWARD * speed * 25.0);
    moved = true;
  }
  if (kbd->GetKeyState (CSKEY_DOWN))
  {
    view->GetCamera ()->Move (CS_VEC_BACKWARD * speed * 25.0);
    moved = true;
  }
  if (kbd->GetKeyState (CSKEY_LEFT))
  {
    view->GetCamera ()->Move (CS_VEC_LEFT * speed * 25.0);
    moved = true;
  }
  if (kbd->GetKeyState (CSKEY_RIGHT))
  {
    view->GetCamera ()->Move (CS_VEC_RIGHT * speed * 25.0);
    moved = true;
  }
  if (kbd->GetKeyState (CSKEY_HOME))
  {
    view->GetCamera ()->Move (CS_VEC_UP * speed * 25.0);
    moved = true;
  }
  if (kbd->GetKeyState (CSKEY_END))
  {
    view->GetCamera ()->Move (CS_VEC_DOWN * speed * 25.0);
    moved = true;
  }

/*
  // dump camera position. might be useful for debugging.
  if (moved)
  {
    csReversibleTransform ct = view->GetCamera()->GetTransform();
    const csVector3 camPos = ct.GetOrigin();
    const csVector3 camPlaneZ = ct.GetT2O().Col3 ();
    printf ("(%g,%g,%g) (%g,%g,%g)\n", camPos.x, camPos.y, camPos.z,
      camPlaneZ.x, camPlaneZ.y, camPlaneZ.z);
  }
  */

  r3d->SetPerspectiveAspect (r3d->GetDriver2D ()->GetHeight ());
  r3d->SetPerspectiveCenter (r3d->GetDriver2D ()->GetWidth ()/2,
                             r3d->GetDriver2D ()->GetHeight ()/2);

  // Tell 3D driver we're going to display 3D things.
  if (!r3d->BeginDraw (CSDRAW_3DGRAPHICS | CSDRAW_CLEARSCREEN | CSDRAW_CLEARZBUFFER))
    return;

  view->Draw ();

  console->Draw3D ();
  if (!r3d->BeginDraw (CSDRAW_2DGRAPHICS))
    return;
  console->Draw2D ();

  if(stop)
    stop=false;
}

void csWaterDemo::FinishFrame ()
{
  r3d->Print (0);
}

bool csWaterDemo::HandleEvent (iEvent& ev)
{
  if (ev.Type == csevBroadcast && ev.Command.Code == cscmdFocusChanged)
  {
    hasfocus = (bool)ev.Command.Info;
    if (hasfocus)
    {
      int w = r3d->GetDriver2D ()->GetWidth()/2;
      int h = r3d->GetDriver2D ()->GetHeight()/2;
      r3d->GetDriver2D ()->SetMousePosition (w, h);
      r3d->GetDriver2D()->SetMouseCursor (csmcNone);
    }
    else
    {
      r3d->GetDriver2D()->SetMouseCursor (csmcArrow);
    }
  }
  else if (ev.Type == csevBroadcast && ev.Command.Code == cscmdProcess)
  {
    waterdemo->SetupFrame ();
    return true;
  }
  else if (ev.Type == csevBroadcast && ev.Command.Code == cscmdFinalProcess)
  {
    waterdemo->FinishFrame ();
    return true;
  }
  else if ((ev.Type == csevKeyboard) &&
    (csKeyEventHelper::GetEventType (&ev) == csKeyEventTypeDown))
  {
    switch (csKeyEventHelper::GetCookedCode (&ev))
    {
    case CSKEY_ESC:
      {
        csRef<iEventQueue> q (CS_QUERY_REGISTRY (object_reg, iEventQueue));
        if (q) q->GetEventOutlet()->Broadcast (cscmdQuit);
        return true;
      }
    case CSKEY_TAB:
      console->SetVisible (!console->GetVisible ());
      break;
    case CSKEY_SPACE:
      pushDownPoint ( ((float)rand()/(float)RAND_MAX)*(Height-1), ((float)rand()/(float)RAND_MAX)*(Width-1), 0.5f);
      break;
    case '1':
      WaveSpeed -= 0.01f;
      WaveSpeed = MAX(WaveSpeed,0);
      break;
    case '2':
      WaveSpeed += 0.01f;
      WaveSpeed = MIN(WaveSpeed,0.5f);
      break;
    case '3':
      TimeDelta -= 0.01f;
      break;
    case '4':
      TimeDelta += 0.01f;
      break;
    case '5':
      WaveLife -= 0.01f;
      break;
    case '6':
      WaveLife += 0.01f;
      break;
    }
  }
  return false;
}

bool csWaterDemo::SimpleEventHandler (iEvent& ev)
{
  return waterdemo->HandleEvent (ev);
}

bool csWaterDemo::Initialize ()
{
  if (!csInitializer::SetupConfigManager (object_reg, 
    "/config/waterdemo.cfg"))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR, 
      "crystalspace.application.waterdemo",
      "Failed to initialize config!");
    return false;
  }

  if (!csInitializer::RequestPlugins (object_reg,
  	CS_REQUEST_VFS,
        CS_REQUEST_PLUGIN ("crystalspace.graphics3d.opengl", iGraphics3D),
        CS_REQUEST_ENGINE,
	CS_REQUEST_IMAGELOADER,
	CS_REQUEST_FONTSERVER,
	CS_REQUEST_REPORTER,
	CS_REQUEST_REPORTERLISTENER,
        CS_REQUEST_CONSOLEOUT,
        CS_REQUEST_LEVELLOADER,
 	CS_REQUEST_END))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.waterdemo",
	"Can't initialize plugins!");
    return false;
  }

  if (!csInitializer::SetupEventHandler (object_reg, SimpleEventHandler))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.waterdemo",
	"Can't initialize event handler!");
    return false;
  }

  // Check for commandline help.
  if (csCommandLineHelper::CheckHelp (object_reg))
  {
    csCommandLineHelper::Help (object_reg);
    return false;
  }

  vc = CS_QUERY_REGISTRY (object_reg, iVirtualClock);
  if (vc == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.waterdemo",
    	"No iVirtualClock plugin!");
    return false;
  }

  vfs = CS_QUERY_REGISTRY (object_reg, iVFS);
  if (vfs == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.waterdemo",
    	"No iVFS plugin!");
    return false;
  }

  r3d = CS_QUERY_REGISTRY (object_reg, iGraphics3D);
  if (r3d == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.waterdemo",
    	"No iGraphics3D plugin!");
    return false;
  }


  engine = CS_QUERY_REGISTRY (object_reg, iEngine);
  if (engine == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.waterdemo",
    	"No iEngine plugin!");
    return false;
  }

  kbd = CS_QUERY_REGISTRY (object_reg, iKeyboardDriver);
  if (kbd == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.waterdemo",
    	"No iKeyboardDriver plugin!");
    return false;
  }

  mouse = CS_QUERY_REGISTRY (object_reg, iMouseDriver);
  if (mouse == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.waterdemo",
    	"No iMouseDriver plugin!");
    return false;
  }

  csRef<iLoader> loader = CS_QUERY_REGISTRY (object_reg, iLoader);
  if (loader == 0)
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.waterdemo",
    	"No iLoader plugin!");
    return false;
  }

  console = CS_QUERY_REGISTRY (object_reg, iConsoleOutput);

  // Open the main system. This will open all the previously loaded plug-ins.
  if (!csInitializer::OpenApplication (object_reg))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
    	"crystalspace.application.waterdemo",
    	"Error opening system!");
    return false;
  }


  //loader->LoadMapFile("/this/data/test.xml", false);

  csRef<iSector> room = engine->CreateSector ("room");

  view = csPtr<iView> (new csView (engine, r3d));
  view->GetCamera ()->SetSector (room);
  view->GetCamera ()->GetTransform ().SetOrigin (csVector3 (0, 5, 0));
  view->GetCamera ()->GetTransform ().LookAt (csVector3(5,-5,20), csVector3(0,1,0));
  csRef<iGraphics2D> g2d = r3d->GetDriver2D ();
  view->SetRectangle (0, 0, g2d->GetWidth (), g2d->GetHeight ());

  bool hasAccel;
  if (g2d->PerformExtension ("hardware_accelerated", &hasAccel))
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_NOTIFY,
      "crystalspace.application.waterdemo",
      "Hardware acceleration %s.\n",
      hasAccel ? "present" : "not present");
  }
  else
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_NOTIFY,
      "crystalspace.application.waterdemo",
      "Hardware acceleration check not available.\n");
  }

  r3d->GetDriver2D ()->SetMouseCursor( csmcNone );

#ifdef CS_USE_NEW_RENDERER
  //get a custom renderloop
  csRef<iRenderLoop> rl = engine->GetRenderLoopManager ()->Create ();
  
  csRef<iPluginManager> plugin_mgr (
    CS_QUERY_REGISTRY (object_reg, iPluginManager));

  csRef<iRenderStepType> genType =
    CS_LOAD_PLUGIN (plugin_mgr,
    "crystalspace.renderloop.step.generic.type",
    iRenderStepType);

  csRef<iRenderStepFactory> genFact = genType->NewFactory ();

  csRef<iRenderStep> step;
  csRef<iGenericRenderStep> genStep;

  step = genFact->Create ();
  rl->AddStep (step);
  genStep = SCF_QUERY_INTERFACE (step, iGenericRenderStep);

  genStep->SetShaderType ("general");
  genStep->SetZBufMode (CS_ZBUF_USE);
  genStep->SetZOffset (false);

  engine->GetRenderLoopManager ()->Register ("waterdemoRL", rl);
  engine->SetCurrentDefaultRenderloop (rl);

  csRef<iStringSet> strings = 
    CS_QUERY_REGISTRY_TAG_INTERFACE (object_reg, 
    "crystalspace.shared.stringset", iStringSet);



  // Load in lighting shaders
  csRef<iVFS> vfs (CS_QUERY_REGISTRY(object_reg, iVFS));
  csRef<iFile> shaderFile = vfs->Open ("/shader/water.xml", VFS_FILE_READ);

  csRef<iDocumentSystem> docsys (
    CS_QUERY_REGISTRY(object_reg, iDocumentSystem));
  csRef<iDocument> shaderDoc = docsys->CreateDocument ();
  shaderDoc->Parse (shaderFile);

  csRef<iShader> shader;
  /*csRef<iShaderCompiler> shcom (CS_LOAD_PLUGIN (plugin_mgr,
    "crystalspace.graphics3d.shadercompiler.xmlshader",
    iShaderCompiler));*/
  csRef<iShaderManager> shmgr (CS_QUERY_REGISTRY(object_reg, iShaderManager));
  //shmgr->RegisterCompiler (shcom);
  csRef<iShaderCompiler> shcom (shmgr->GetCompiler ("XMLShader"));
  shader = shcom->CompileShader (shaderDoc->GetRoot ()->GetNode ("shader"));
#if 0
  if(shmgr)
  {
    shader = shmgr->CreateShader();
    if(shader)
    {
      shader->Load(csRef<iDataBuffer>(vfs->ReadFile("/shader/water.xml")));
      shader->SetName ("water");
      if(shader->Prepare())
      {
        /*
        for (int i=0; i<engine->GetMaterialList ()->GetCount (); i++)
                {
        	  csRef<iShaderWrapper> wrapper (shmgr->CreateWrapper (shader));
                  engine->GetMaterialList ()->Get (i)->GetMaterial ()->
                    SetShader(strings->Request ("general"), wrapper);
                }*/
      }
    }
  }
#endif

#endif // CS_USE_NEW_RENDERER

  // setup the mesh 
  csRef<iMeshObjectType> gType = CS_QUERY_PLUGIN_CLASS(plugin_mgr, 
    "crystalspace.mesh.object.genmesh", iMeshObjectType);
  
  if (!gType)
  {
    gType = CS_LOAD_PLUGIN(plugin_mgr, 
      "crystalspace.mesh.object.genmesh", iMeshObjectType);
    if (!gType)
    {
      csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
        "crystalspace.application.waterdemo",
        "Error loading genmesh baseobject!");
      return false;
    }
  }

  
  gFact = gType->NewFactory ();
  csRef<iMeshFactoryWrapper> fw = engine->CreateMeshFactory (gFact, "waterFactory");
  gFactState = SCF_QUERY_INTERFACE(gFact, iGeneralFactoryState);

  gMesh = gFact->NewInstance ();
  gMeshState = SCF_QUERY_INTERFACE(gMesh, iGeneralMeshState);

  //setup a wrapper too
  gMeshW = engine->CreateMeshWrapper (gMesh, "water", room);
  csMatrix3 m;
  m.Identity ();
  gMeshW->GetMovable ()->SetTransform (m);
  gMeshW->GetMovable ()->SetPosition (csVector3(0,-5,0));
  gMeshW->GetMovable ()->UpdateMove ();
  gMeshW->DeferUpdateLighting (CS_NLIGHT_STATIC | CS_NLIGHT_DYNAMIC, 10);
  
  //setup a material
  csRef<iMaterial> mat = engine->CreateBaseMaterial (0);
  mat->SetShader (strings->Request ("general"), shader/*shmgr->GetShader ("water")*/);
  csRef<iMaterialWrapper> matW = engine->GetMaterialList ()->NewMaterial (mat);
  matW->QueryObject ()->SetName ("waterMaterial");

  csRef<iImageVector> imgvec = csPtr<iImageVector> (new csImageVector ());
  csRef<iImage> img;


  img = loader->LoadImage ("/lib/cubemap/cubemap_rt.jpg");
  imgvec->AddImage (img);
  img = loader->LoadImage ("/lib/cubemap/cubemap_lf.jpg");
  imgvec->AddImage (img);

 

  img = loader->LoadImage ("/lib/cubemap/cubemap_up.jpg");
  imgvec->AddImage (img);
  img = loader->LoadImage ("/lib/cubemap/cubemap_dn.jpg");
  imgvec->AddImage (img);

  img = loader->LoadImage ("/lib/cubemap/cubemap_fr.jpg");
  imgvec->AddImage (img);
  img = loader->LoadImage ("/lib/cubemap/cubemap_bk.jpg");
  imgvec->AddImage (img);




  csRef<iTextureHandle> tex = r3d->GetTextureManager ()->RegisterTexture (
    imgvec, CS_TEXTURE_3D | CS_TEXTURE_CLAMP | CS_TEXTURE_NOMIPMAPS, 
    iTextureHandle::CS_TEX_IMG_CUBEMAP);

  //csRef<iTextureHandle> myTex = loader->LoadTexture ("/lib/std/stone4.gif");
  /*csRef<csShaderVariable> attvar = shmgr->CreateVariable(
    strings->Request ("tex diffuse"));*/
  csRef<csShaderVariable> attvar (csPtr<csShaderVariable> (
    new csShaderVariable (strings->Request ("tex diffuse"))));
  attvar->SetValue (tex);
  mat->AddVariable (attvar);  


  gMesh->SetMaterialWrapper (matW);
  
  Width = Height = 64;

  water = new float[Width*Height];
  water1 = new float[Width*Height];
  water2 = new float[Width*Height];

  memset(water,0,Width*Height*sizeof(float));
  memset(water1,0,Width*Height*sizeof(float));
  memset(water2,0,Width*Height*sizeof(float));

  WaveSpeed = 0.3f;
  WaveLife = 0.1f;
  GridSize = 0.25f;
  TimeDelta = 0.12f;


  //setup the mesh
  gFactState->SetVertexCount (Width*Height);
  gFactState->SetTriangleCount (2*((Width-1)*(Height-1)));

  //setup ibuf
  int x, z, cnt=0,idx;
  csTriangle *ibuf=gFactState->GetTriangles ();
  for(x=0;x<(Height-1);x++)
  {
    for(z=0;z<(Width-1);z++)
    {
      idx = 2*(x*(Width-1)+z);
      ibuf[idx].a = x*Width+z;
      ibuf[idx].b = x*Width+z+1;
      ibuf[idx].c = (x+1)*Width+z;
      idx++;
      ibuf[idx].a = x*Width+z+1;
      ibuf[idx].b = (x+1)*Width+(z+1);
      ibuf[idx].c = (x+1)*Width+z;
    }
  }

  //setup our vbuf
  csVector3 *vbuf = gFactState->GetVertices ();
  cnt=0;
  for(x=0;x<Height;x++)
  {
    for(z=0;z<Width;z++)
    {
      idx = x*Width+z;
      vbuf[idx].x = x*GridSize;
      vbuf[idx].y = water[idx];
      vbuf[idx].z = z*GridSize;
    }
  }

  //setup texture
  csVector2 *tbuf = gFactState->GetTexels ();
  for(x=0;x<Height;x++)
  {
    for(z=0;z<Width;z++)
    {
      idx = x*Width+z;
      tbuf[idx].x = (float)x/(float)Height;
      tbuf[idx].y = (float)z/(float)Width;
    }
  }

  lastSimTime = nextSimTime = 0.0f;

  gFactState->Invalidate ();

  engine->Prepare ();

  console->SetVisible (false);
  hasfocus = true;
  int w = r3d->GetDriver2D ()->GetWidth()/2;
  int h = r3d->GetDriver2D ()->GetHeight()/2;
  r3d->GetDriver2D ()->SetMousePosition (w, h);

  return true;
}

void csWaterDemo::updateWater (float time)
{
  bool haveRan=false;
  
  if (time>1000) time =0;
  lastSimTime += (time/1000.0f);
  

  while (lastSimTime > nextSimTime)
  {
  
    csVector3 *vbuf = gFactState->GetVertices ();

    C1 = (4 - ((8*WaveSpeed*WaveSpeed*TimeDelta*TimeDelta) / (GridSize*GridSize))) / (WaveLife*TimeDelta + 2);
    C2 = (WaveLife*TimeDelta - 2) / (WaveLife*TimeDelta + 2);
    C3 = ((2*WaveSpeed*WaveSpeed*TimeDelta*TimeDelta) / (GridSize*GridSize)) / (WaveLife*TimeDelta + 2);
    float C4, C5;
    C4 = C3*0.5858;
    C5 = C3*0.4142;

    float *tmp;
    tmp=water2;
    water2=water1;
    water1=water;
    water=tmp;
    memset(water, 0, Width*Height*sizeof(float));

    for (int z = 1; z < (Height-1); z++) {
      for (int x = 1; x < (Width-1); x++) {
        int ind = x*Width+z;

        water[ind] = C1*water1[ind] + 
                     C2*water2[ind] + 
                     C4*(water1[ind+1] + water1[ind-1] + water1[ind+Width] + water1[ind-Width])+
                     C5*(water1[ind+1+Width] + water1[ind-1+Width] + water1[ind-Width+1] + water1[ind-Width-1]);

        vbuf[ind].y = water[ind];
      }
    }  
    nextSimTime += 1/SIMPERSECOND;
    haveRan = true;
  }

  if (haveRan)
    generateNormals ();
}

void csWaterDemo::pushDownPoint (float x, float z, float depth)
{
  int xn, xu, zn, zu;
  float power, dist;
  xn = (int)floorf (x);
  xu = (int)ceilf (x);
  zn = (int)floorf (z);
  zu = (int)ceilf (z);

  xn = MAX(xn,0);
  xu = MIN(xu,Height);
  
  zn = MAX(zn,0);
  zu = MIN(zu,Width);

  dist = sqrtf((x-xn)*(x-xn)+(z-zn)*(z-zn));
  power = MAX(1-dist,0);
  water[xn*Width+zn] += depth*power;

  dist = sqrtf((x-xn)*(x-xn)+(z-zu)*(z-zu));
  power = MAX(1-dist,0);
  water[xn*Width+zu] += depth*power;
  
  dist = sqrtf((x-xu)*(x-xu)+(z-zn)*(z-zn));
  power = MAX(1-dist,0);
  water[xu*Width+zn] += depth*power;

  dist = sqrtf((x-xu)*(x-xu)+(z-zu)*(z-zu));
  power = MAX(1-dist,0);
  water[xu*Width+zu] += depth*power;
}

void csWaterDemo::generateNormals ()
{
  csVector3 *vbuf = gFactState->GetVertices ();
  csVector3 *nbuf = gFactState->GetNormals ();
  csColor *cbuf = gFactState->GetColors ();
  csTriangle *ibuf = gFactState->GetTriangles ();

  int numVerts = gFactState->GetVertexCount ();
  int numTris = gFactState->GetTriangleCount ();
  int i;

  //zeroout the normals
  for(i=0;i<numVerts;i++)
  {
    nbuf[i] = csVector3(0,0,0);
  }

  //accumulate facenormals
  for(i=0;i<numTris;i++)
  {
    int a, b, c;
    a=ibuf[i].a;
    b=ibuf[i].b;
    c=ibuf[i].c;

    csVector3 v0(vbuf[a]);
    csVector3 v1(vbuf[b]);
    csVector3 v2(vbuf[c]);
    csVector3 diff1 (v2-v1);
    csVector3 diff2 (v0-v1);
    csVector3 n = diff1 % diff2;
    nbuf[a] += n;
    nbuf[b] += n;
    nbuf[c] += n;
  }

  //normalize
  for(i=0;i<numVerts;i++)
  {
    nbuf[i].Normalize ();
    cbuf[i].red = nbuf[i].x;
    cbuf[i].green = nbuf[i].y;
    cbuf[i].blue = nbuf[i].z;
  }
}

void csWaterDemo::Start ()
{
  csDefaultRunLoop (object_reg);
}


/*---------------------------------------------------------------------*
 * Main function
 *---------------------------------------------------------------------*/
int main (int argc, char* argv[])
{
  iObjectRegistry* object_reg = csInitializer::CreateEnvironment (argc, argv);
  srand(time(0));

  waterdemo = new csWaterDemo (object_reg);
  if (waterdemo->Initialize ())
    waterdemo->Start ();
  delete waterdemo;

  csInitializer::DestroyApplication (object_reg);
  return 0;
}

