/*
    Copyright (C) 2003 by Jorrit Tyberghein
	      (C) 2003 by Frank Richter

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

#include "iutil/objreg.h"

#include "iengine/material.h"
#include "csgfx/rgbpixel.h"
#include "csengine/renderloop.h"
#include "csengine/engine.h"

#ifdef CS_NR_ALTERNATE_RENDERLOOP

void csEngine::StartDraw (iCamera *c, iClipper2D *view, csRenderView &rview)
{
}

void csEngine::Draw (iCamera *c, iClipper2D *view)
{
  DefaultRenderLoop->Draw (c, view);
}

//---------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE(csAmbientRenderStep)
  SCF_IMPLEMENTS_INTERFACE(iRenderStep)
SCF_IMPLEMENT_IBASE_END

csAmbientRenderStep::csAmbientRenderStep (csRenderLoop* rl)
{
  SCF_CONSTRUCT_IBASE (NULL);

  csAmbientRenderStep::rl = rl;
}

void csAmbientRenderStep::Perform (csRenderView* rview, iSector* sector)
{
  iRender3D* r3d = rl->engine->G3D;

  iVisibilityCuller* viscull = sector->GetVisibilityCuller ();
  viscull->VisTest (rview);
  uint32 visnr = viscull->GetCurrentVisibilityNumber ();

  r3d->EnableZOffset ();
  iSectorRenderMeshList* meshes = sector->GetRenderMeshes ();
  
  int i, meshnum = meshes->GetCount();
  for (i = 0; i < meshnum; i++)
  {
    iMeshWrapper* mw;
    iVisibilityObject* visobj;
    csRenderMesh* mesh;
    meshes->Get (i, mw, visobj, mesh);

    if (!mesh) continue;
    if (visobj->GetVisibilityNumber() != visnr) continue;
    // haacky.
    // DrawTest() should be called (so the mesh has a chance to do some
    // preparation before it's drawed), but not here.
    if (!mw->GetMeshObject()->DrawTest (rview, mw->GetMovable()))
    {
      visobj->SetVisibilityNumber (visnr - 1);
      continue;
    }

    iMaterialWrapper *matsave;
    //matsave = mesh->mathandle;
    //mesh->mathandle = NULL;
    matsave = mesh->material;
    mesh->material = NULL;
    uint mixsave = mesh->mixmode;
    mesh->mixmode = CS_FX_COPY;
    
    r3d->DrawMesh (mesh);

    //mesh->mathandle = matsave;
    mesh->material = matsave;;
    mesh->mixmode = mixsave;
  }

  r3d->DisableZOffset ();
};

//---------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE(csLightingRenderStep)
  SCF_IMPLEMENTS_INTERFACE(iRenderStep)
SCF_IMPLEMENT_IBASE_END

csLightingRenderStep::csLightingRenderStep (csRenderLoop* rl)
{
  SCF_CONSTRUCT_IBASE (NULL);

  csLightingRenderStep::rl = rl;
}

void csLightingRenderStep::RenderMeshes (iRender3D* r3d,
					 iShader* shader, 
					 csRenderMesh** meshes, 
					 int num)
{
  if (num == 0) return;

  iShaderTechnique *tech = shader->GetBestTechnique ();

  for (int p=0; p<tech->GetPassCount (); p++)
  {
    iShaderPass *pass = tech->GetPass (p);
    pass->Activate (NULL);

    int j;
    for (j = 0; j < num; j++)
    {
      csRenderMesh* mesh = meshes[j];

      pass->SetupState (mesh);
      csZBufMode zsave = mesh->z_buf_mode;
      uint mixsave = mesh->mixmode;
      
      uint mixmode = pass->GetMixmodeOverride ();
      if (mixmode != 0)
	mesh->mixmode = mixmode;
      
      mesh->z_buf_mode = CS_ZBUF_TEST;
      rl->engine->G3D->DrawMesh (mesh);
      mesh->z_buf_mode = zsave;
      mesh->mixmode = mixsave;

      pass->ResetState ();
    }
    pass->Deactivate ();
  }
}

void csLightingRenderStep::Perform (csRenderView* rview, iSector* sector)
{
  iRender3D* r3d = rl->engine->G3D;

  iVisibilityCuller* viscull = sector->GetVisibilityCuller ();
  viscull->VisTest (rview);
  uint32 visnr = viscull->GetCurrentVisibilityNumber ();

  csReversibleTransform camTransR = 
    rview->GetCamera()->GetTransform();
  r3d->SetObjectToCamera (&camTransR);

  r3d->SetLightParameter (0, CS_LIGHTPARAM_POSITION,
  	csVector3 (0, 0, 0));
  r3d->SetLightParameter (0, CS_LIGHTPARAM_ATTENUATION,
  	csVector3 (1, 0, 0));
  r3d->SetLightParameter (0, CS_LIGHTPARAM_DIFFUSE, 
    csVector3 (1, 1, 1));
  r3d->SetLightParameter (0, CS_LIGHTPARAM_SPECULAR, 
    csVector3 (0, 0, 0));

  iSectorRenderMeshList* meshes = sector->GetRenderMeshes ();
  int i, meshnum = meshes->GetCount();
  CS_ALLOC_STACK_ARRAY (csRenderMesh*, sameShaderMeshes, meshnum);
  int numSSM = 0;
  iShader* shader = NULL;
  for (i = 0; i < meshnum; i++)
  {
    iMeshWrapper* mw;
    iVisibilityObject* visobj;
    csRenderMesh* mesh;
    meshes->Get (i, mw, visobj, mesh);

    if (!mesh) continue;
    if (visobj->GetVisibilityNumber() != visnr) continue;

    iShader* meshShader = mesh->material->GetMaterialHandle()->GetShader();
    if (meshShader != shader)
    {
      RenderMeshes (r3d, meshShader, sameShaderMeshes, numSSM);

      shader = meshShader;
      numSSM = 0;
    }
    sameShaderMeshes[numSSM++] = mesh;
  }

  if (numSSM != 0)
  {
    RenderMeshes (r3d, shader, sameShaderMeshes, numSSM);
  }
};

//---------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE(csRenderLoop)
  SCF_IMPLEMENTS_INTERFACE(iRenderLoop)
SCF_IMPLEMENT_IBASE_END

csRenderLoop::csRenderLoop (csEngine* engine)
{
  SCF_CONSTRUCT_IBASE (NULL);

  csRenderLoop::engine = engine;

  csRef<iRenderStep> tmp;
  tmp.AttachNew (new csAmbientRenderStep (this));
  steps.Push (tmp);
  tmp.AttachNew (new csLightingRenderStep (this));
  steps.Push (tmp);
}

void csRenderLoop::StartDraw (iCamera *c, iClipper2D *view, csRenderView &rview)
{
  rview.SetEngine (engine);
  rview.SetOriginalCamera (c);

/*  iEngineSequenceManager* eseqmgr = GetEngineSequenceManager ();
  if (eseqmgr)
  {
    eseqmgr->SetCamera (c);
  }*/

  // This flag is set in HandleEvent on a cscmdContextResize event
/*  if (resize)
  {
    resize = false;
    Resize ();
  }*/

  rview.GetClipPlane ().Set (0, 0, 1, -1);      //@@@CHECK!!!

  // Calculate frustum for screen dimensions (at z=1).
  float leftx = -c->GetShiftX () * c->GetInvFOV ();
  float rightx = (engine->frame_width - c->GetShiftX ()) * c->GetInvFOV ();
  float topy = -c->GetShiftY () * c->GetInvFOV ();
  float boty = (engine->frame_height - c->GetShiftY ()) * c->GetInvFOV ();
  rview.SetFrustum (leftx, rightx, topy, boty);
}

void csRenderLoop::Draw (iCamera *c, iClipper2D *view)
{
  engine->ControlMeshes ();

  csRenderView rview (c, view, engine->G3D, engine->G2D);
  StartDraw (c, view, rview);

  // First initialize G3D with the right clipper.
  engine->G3D->SetClipper (view, CS_CLIPPER_TOPLEVEL);  // We are at top-level.
  engine->G3D->ResetNearPlane ();
  engine->G3D->SetPerspectiveAspect (c->GetFOV ());

  iSector *s = c->GetSector ();
  //if (s) s->Draw (&rview);
  if (s)
  {
    rview.SetThisSector (s);

    int i;
    for (i = 0; i < steps.Length(); i++)
    {
      steps[i]->Perform (&rview, s);
    }
/*    csRenderMeshList meshes;
    s->CollectMeshes (&rview, meshes);

    if (meshes.num) 
    {
      int i;
      for (i = 0; i < steps.Length(); i++)
      {
	steps[i]->Perform (&rview, &meshes);
      }
    }*/
  }

  // draw all halos on the screen
/*  if (halos.Length () > 0)
  {
    csTicks elapsed = virtual_clock->GetElapsedTicks ();
    for (int halo = halos.Length () - 1; halo >= 0; halo--)
      if (!halos[halo]->Process (elapsed, *this)) halos.Delete (halo);
  }*/

  engine->G3D->SetClipper (NULL, CS_CLIPPER_NONE);
}

#endif
