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
#include "portal.h"
#include "polygon.h"
#include "pol2d.h"
#include "csutil/debug.h"
#include "csgeom/frustum.h"
#include "ivideo/texture.h"
#include "iengine/texture.h"
#include "iengine/rview.h"
#include "iengine/fview.h"
#include "iengine/camera.h"
#include "iengine/shadows.h"

SCF_IMPLEMENT_IBASE_EXT(csPortalObsolete)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iPortal)
SCF_IMPLEMENT_IBASE_EXT_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csPortalObsolete::Portal)
  SCF_IMPLEMENTS_INTERFACE(iPortal)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csPortalObsolete::csPortalObsolete (csPolygon3DStatic* parent)
{
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiPortal);
  DG_TYPE (this, "csPortalObsolete");
  filter_texture = 0;
  filter_r = 1;
  filter_g = 1;
  filter_b = 1;
  sector = 0;
  max_sector_visit = 5;
  csPortalObsolete::parent = parent;
}

csPortalObsolete::~csPortalObsolete ()
{
  if (filter_texture) filter_texture->DecRef ();
}

csPortalObsolete* csPortalObsolete::Clone ()
{
  csPortalObsolete* clone = new csPortalObsolete (0);
  clone->SetSector (sector);
  clone->flags.SetAll (flags.Get ());
  clone->warp_obj = warp_obj;
  clone->max_sector_visit = max_sector_visit;
  clone->filter_texture = filter_texture;
  clone->filter_r = filter_r;
  clone->filter_g = filter_g;
  clone->filter_b = filter_b;
  int i;
  for (i = 0 ; i < sector_cb_vector.Length () ; i++)
    clone->SetMissingSectorCallback (GetMissingSectorCallback (i));
  for (i = 0 ; i < portal_cb_vector.Length () ; i++)
    clone->SetPortalCallback (GetPortalCallback (i));
  return clone;
}

iSector *csPortalObsolete::GetSector () const
{
  return sector;
}

void csPortalObsolete::SetSector (iSector *s)
{
  sector = s;
}

csFlags &csPortalObsolete::GetFlags ()
{
  return flags;
}

const csVector3* csPortalObsolete::GetVertices () const
{
  return parent->GetParent ()->GetVertices ();
}

int* csPortalObsolete::GetVertexIndices () const
{
  return parent->GetVertexIndices ();
}

int csPortalObsolete::GetVertexIndicesCount () const
{
  return parent->GetVertexCount ();
}

const csPlane3& csPortalObsolete::GetObjectPlane ()
{
  return parent->GetObjectPlane ();
}

const csPlane3& csPortalObsolete::GetWorldPlane ()
{
  // @@@ WRONG: but going to be removed anyway...
  return parent->GetObjectPlane ();
}

void csPortalObsolete::ComputeCameraPlane (const csReversibleTransform& t,
  	csPlane3& camplane)
{
  // @@@ WRONG: but going to be removed anyway...
}

bool csPortalObsolete::CompleteSector (iBase *context)
{
  if (sector)
  {
    bool rc = true;
    int i;
    // Callback are traversed in reverse order so that they can safely
    // delete themselves.
    i = portal_cb_vector.Length ()-1;
    while (i >= 0)
    {
      iPortalCallback* cb = portal_cb_vector[i];
      rc = cb->Traverse (&(this->scfiPortal), context);
      if (!rc) break;
      i--;
    }
    return rc;
  }
  else
  {
    bool rc = false;
    // Callback are traversed in reverse order so that they can safely
    // delete themselves.
    int i = sector_cb_vector.Length ()-1;
    while (i >= 0)
    {
      iPortalCallback* cb = sector_cb_vector[i];
      rc = cb->Traverse (&(this->scfiPortal), context);
      if (rc == true) break;
      i--;
    }
    return rc;
  }

  return false;
}

void csPortalObsolete::ObjectToWorld (const csReversibleTransform &t,
  csReversibleTransform& warp_wor) const
{
  if (flags.Check (CS_PORTAL_STATICDEST))
    warp_wor = warp_obj * t;
  else
    warp_wor = t.GetInverse () * warp_obj * t;
}

void csPortalObsolete::HardTransform (const csReversibleTransform &t)
{
  if (flags.Check (CS_PORTAL_WARP))
    ObjectToWorld (t, warp_obj);
}

void csPortalObsolete::SetWarp (const csTransform &t)
{
  flags.Set (CS_PORTAL_WARP);
  warp_obj = t;

  csMatrix3 m = warp_obj.GetO2T ();
  flags.SetBool (
      CS_PORTAL_MIRROR,
      (((m.Col1 () % m.Col2 ()) * m.Col3 ()) < 0));
}

void csPortalObsolete::SetWarp (
  const csMatrix3 &m_w,
  const csVector3 &v_w_before,
  const csVector3 &v_w_after)
{
  flags.Set (CS_PORTAL_WARP);

  warp_obj = csTransform (m_w.GetInverse (), v_w_after - m_w * v_w_before);

  // If the three colunms of the transformation matrix are taken
  // as vectors, V1, V2, and V3, then V1 x V2 = ( + or - ) V3.
  // The result is positive for non-mirroring transforms, and
  // negative for mirroring transforms.  Thus, (V1 x V2) * V3
  // will equal +1 or -1, depending on whether the transform is
  // mirroring.
  csMatrix3 m = warp_obj.GetO2T ();
  flags.SetBool (
      CS_PORTAL_MIRROR,
      (((m.Col1 () % m.Col2 ()) * m.Col3 ()) < 0));
}

const csReversibleTransform &csPortalObsolete::GetWarp () const
{
  return warp_obj;
}

void csPortalObsolete::SetFilter (iTextureHandle *ft)
{
  if (ft) ft->IncRef ();
  if (filter_texture) filter_texture->DecRef ();
  filter_texture = ft;
}

iTextureHandle *csPortalObsolete::GetTextureFilter () const
{
  return filter_texture;
}

void csPortalObsolete::SetFilter (float r, float g, float b)
{
  filter_r = r;
  filter_g = g;
  filter_b = b;
  if (filter_texture)
  {
    filter_texture->DecRef ();
    filter_texture = 0;
  }
}

void csPortalObsolete::GetColorFilter (float &r, float &g, float &b) const
{
  r = filter_r;
  g = filter_g;
  b = filter_b;
}

csVector3 csPortalObsolete::Warp (const csReversibleTransform& t,
    const csVector3 &pos) const
{
  if (flags.Check (CS_PORTAL_WARP))
  {
    csReversibleTransform warp_wor;
    // @@@ Perhaps can be calculated more efficiently without having
    // to calculate a new transform?
    ObjectToWorld (t, warp_wor);
    return warp_wor.Other2This (pos);
  }
  else
  {
    return pos;
  }
}

void csPortalObsolete::WarpSpace (const csReversibleTransform& warp_wor,
    csReversibleTransform &t, bool &mirror) const
{
  // warp_wor is a world -> warp space transformation.
  // t is a world -> camera space transformation.
  // Set t to equal a warp -> camera space transformation by
  // reversing warp and then applying the old t.
  t /= warp_wor;
  if (flags.Check (CS_PORTAL_MIRROR)) mirror = !mirror;
}

bool csPortalObsolete::Draw (
  csPolygon2D *new_clipper,
  iPolygon3D *portal_polygon,
  const csReversibleTransform& t,
  iRenderView *rview,
  const csPlane3& camera_plane)
{
  if (!CompleteSector (rview)) return false;
  if (sector->GetRecLevel () >= max_sector_visit)
    return false;

  if (!new_clipper->GetVertexCount ()) return false;

  iCamera *icam = rview->GetCamera ();
  csPolygonClipper new_view (new_clipper, icam->IsMirrored (), true);

  csRenderContext *old_ctxt = rview->GetRenderContext ();
  rview->CreateRenderContext ();
  rview->SetRenderRecursionLevel (rview->GetRenderRecursionLevel () + 1);
  rview->SetClipper (&new_view);
  rview->ResetFogInfo ();
  rview->SetLastPortal (&scfiPortal, portal_polygon);
  rview->SetPreviousSector (rview->GetThisSector ());
  rview->SetClipPlane (camera_plane);
  rview->GetClipPlane ().Invert ();
  if (flags.Check (CS_PORTAL_CLIPDEST))
  {
    rview->UseClipPlane (true);
    rview->UseClipFrustum (true);
  }
  // When going through a portal we first remember the old clipper
  // and clip plane (if any). Then we set a new one. Later we restore.
  iGraphics3D *G3D = rview->GetGraphics3D ();
  iClipper2D *old_clipper = G3D->GetClipper ();
  if (old_clipper) old_clipper->IncRef ();

  int old_cliptype = G3D->GetClipType ();
  G3D->SetClipper (
      rview->GetClipper (),
      rview->IsClipperRequired () ? CS_CLIPPER_REQUIRED : CS_CLIPPER_OPTIONAL);

  csPlane3 old_near_plane = G3D->GetNearPlane ();
  bool old_do_near_plane = G3D->HasNearPlane ();
  csPlane3 cp;
  if (rview->GetClipPlane (cp))
    G3D->SetNearPlane (cp);
  else
    G3D->ResetNearPlane ();

  if (flags.Check (CS_PORTAL_WARP))
  {
    iCamera *inewcam = rview->CreateNewCamera ();

    bool mirror = inewcam->IsMirrored ();
    csReversibleTransform warp_wor;
    ObjectToWorld (t, warp_wor);
    WarpSpace (warp_wor, inewcam->GetTransform (), mirror);
    inewcam->SetMirrored (mirror);

    sector->Draw (rview);
  }
  else
    sector->Draw (rview);

  rview->RestoreRenderContext (old_ctxt);

  // Now restore our G3D clipper and plane.
  G3D->SetClipper (old_clipper, old_cliptype);
  if (old_clipper) old_clipper->DecRef ();
  if (old_do_near_plane)
    G3D->SetNearPlane (old_near_plane);
  else
    G3D->ResetNearPlane ();

  return true;
}

iMeshWrapper *csPortalObsolete::HitBeamPortals (
  const csReversibleTransform& t,
  const csVector3 &start,
  const csVector3 &end,
  csVector3 &isect,
  int* polygon_idx)
{
  if (!CompleteSector (0)) return 0;
  if (sector->GetRecLevel () >= max_sector_visit)
    return 0;
  if (flags.Check (CS_PORTAL_WARP))
  {
    csReversibleTransform warp_wor;
    // @@@ Perhaps can be calculated more efficiently without having
    // to calculate a new transform?
    ObjectToWorld (t, warp_wor);

    csVector3 new_start = warp_wor.Other2This (start);
    csVector3 new_end = warp_wor.Other2This (end);
    csVector3 new_isect;
    int pidx;
    iMeshWrapper *mesh = sector->HitBeam (new_start, new_end, new_isect, &pidx);
    if (mesh && pidx != -1) isect = warp_wor.This2Other (new_isect);
    if (mesh && polygon_idx) *polygon_idx = pidx;
    return mesh;
  }
  else
  {
    int pidx;
    iMeshWrapper *mesh = sector->HitBeam (start, end, isect, &pidx);
    if (mesh && polygon_idx) *polygon_idx = pidx;
    return mesh;
  }
}

#if 0
iMeshWrapper *csPortalObsolete::HitBeam (
  const csVector3 &start,
  const csVector3 &end,
  csVector3 &isect,
  iPolygon3D **polygonPtr)
{
  if (!CompleteSector (0)) return 0;
  if (sector->GetRecLevel () >= max_sector_visit)
    return 0;
  if (flags.Check (CS_PORTAL_WARP))
  {
    csVector3 new_start = warp_wor.Other2This (start);
    csVector3 new_end = warp_wor.Other2This (end);
    iMeshWrapper *o = sector->HitBeam (
        new_start, new_end, isect,
        polygonPtr);
    return o;
  }
  else
  {
    return sector->HitBeam (
        start, end, isect,
        polygonPtr);
  }
}
#endif

void csPortalObsolete::CheckFrustum (iFrustumView *lview,
    const csReversibleTransform& t, int alpha)
{
  if (!CompleteSector (lview)) return ;
  if (sector->GetRecLevel () > 1)
  //@@@@@ if (sector->GetRecLevel () > csSector::cfg_reflections)
    return ;

  csFrustumContext *old_ctxt = lview->GetFrustumContext ();
  lview->CreateFrustumContext ();

  csFrustumContext *new_ctxt = lview->GetFrustumContext ();
  if (old_ctxt->GetLightFrustum ())
    new_ctxt->SetNewLightFrustum (new csFrustum (*old_ctxt->GetLightFrustum ()));
  lview->StartNewShadowBlock ();

  // If copied_frustums is true we copied the frustums and we need to

  // delete them later.
  bool copied_frustums = false;

  // If true then we have to restore color.
  csRef<iLightingProcessInfo> linfo;
  bool restore_color = false;
  csColor old_color;

  if (flags.Check (CS_PORTAL_WARP))
  {
    csReversibleTransform warp_wor;
    // @@@ Perhaps can be calculated more efficiently without having
    // to calculate a new transform?
    ObjectToWorld (t, warp_wor);

    new_ctxt->GetLightFrustum ()->Transform (&warp_wor);

    if (flags.Check (CS_PORTAL_MIRROR))
      new_ctxt->SetMirrored (!old_ctxt->IsMirrored ());
    new_ctxt->GetLightFrustum ()->SetMirrored (new_ctxt->IsMirrored ());

    /*
     * Transform all shadow frustums. First make a copy.
     * Note that we only copy the relevant shadow frustums.
     * We know that csPolygon3D::CalculateLighting() called
     * csPolygon3D::MarkRelevantShadowFrustums() some time before
     */

    // calling this function so the 'relevant' flags are still valid.
    iShadowBlock *slist = old_ctxt->GetShadows ()->GetFirstShadowBlock ();
    while (slist)
    {
      iShadowBlock *copy_slist = new_ctxt->GetShadows ()->NewShadowBlock ();
      copy_slist->AddRelevantShadows (slist, &warp_wor);
      slist = old_ctxt->GetShadows ()->GetNextShadowBlock (slist);
    }

    copied_frustums = true;

    iFrustumViewUserdata *ud = lview->GetUserdata ();
    if (ud) linfo = SCF_QUERY_INTERFACE (ud, iLightingProcessInfo);
    if (linfo)
    {
      if (alpha)
      {
        float fr, fg, fb;
        if (filter_texture)
        {
          uint8 mr, mg, mb;
          filter_texture->GetMeanColor (mr, mg, mb);
          fr = mr / 255.0f;
          fg = mg / 255.0f;
          fb = mb / 255.0f;
        }
        else
        {
          fr = filter_r;
          fg = filter_g;
          fb = filter_b;
        }

        restore_color = true;
        old_color = linfo->GetColor ();
        linfo->SetColor (
            csColor (
              linfo->GetColor ().red * fr,
              linfo->GetColor ().green * fg,
              linfo->GetColor ().blue * fb));
      }

      // Don't go further if the light intensity is almost zero.
      if (
        linfo->GetColor ().red < SMALL_EPSILON &&
        linfo->GetColor ().green < SMALL_EPSILON &&
        linfo->GetColor ().blue < SMALL_EPSILON)
        goto stop;
    }
  }
  else
  {
    // There is no space warping. In this case we still want to
    // remove all non-relevant shadow frustums if there are any.
    // We know that csPolygon3D::CalculateLighting() called
    // csPolygon3D::MarkRelevantShadowFrustums() some time before
    // calling this function so the 'relevant' flags are still valid.
    iShadowBlock *slist = old_ctxt->GetShadows ()->GetFirstShadowBlock ();
    while (slist)
    {
      copied_frustums = true; // Only set to true here
      iShadowBlock *copy_slist = new_ctxt->GetShadows ()->NewShadowBlock ();
      copy_slist->AddRelevantShadows (slist);

      slist = old_ctxt->GetShadows ()->GetNextShadowBlock (slist);
    }
  }

  sector->CheckFrustum (lview);

  if (copied_frustums)
  {
    // Delete all copied frustums.
    new_ctxt->GetShadows ()->DeleteAllShadows ();
  }

stop:
  lview->RestoreFrustumContext (old_ctxt);
  if (restore_color)
  {
    linfo->SetColor (old_color);
  }
}

void csPortalObsolete::SetMirror (const csPlane3& plane)
{
  SetWarp (csTransform::GetReflect (plane));
}

void csPortalObsolete::SetPortalCallback (iPortalCallback *cb)
{
  portal_cb_vector.Push (cb);
}

iPortalCallback *csPortalObsolete::GetPortalCallback (int idx) const
{
  return portal_cb_vector[idx];
}

void csPortalObsolete::SetMissingSectorCallback (iPortalCallback *cb)
{
  sector_cb_vector.Push (cb);
}

iPortalCallback *csPortalObsolete::GetMissingSectorCallback (int idx) const
{
  return sector_cb_vector[idx];
}

