/*
    Copyright (C) 1998 by Ayal Zwi Pinkus
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
#include "qint.h"
#include "qsqrt.h"
#include "csengine/bezier.h"
#include "csengine/curve.h"
#include "csengine/light.h"
#include "csengine/polytext.h"
#include "csengine/polygon.h"
#include "csengine/thing.h"
#include "csengine/engine.h"
#include "csengine/lppool.h"
#include "ivideo/graph3d.h"
#include "ivideo/vbufmgr.h"

struct csCoverageMatrix
{
  // Each float corresponds to a lightmap grid cell and contains
  // the area of light cell that is covered by light.
  float* coverage;
  int width, height;
  csCoverageMatrix (int w, int h)
  {
    width = w;
    height = h;
    coverage = new float [w * h];
    memset (coverage, 0, w*h*sizeof (float));
  }
  ~csCoverageMatrix ()
  {
    delete[] coverage;
  }
};

// @@@ What value should this macro have, and why?
// #define CURVE_LM_SIZE 32
#define CURVE_LM_SIZE (8 - 2) /*this is the real value - 2*/

/// Calculate inverse perspective corrected point for this camera.
static void Perspective (const csVector3& v, csVector2& p,
	float aspect, float shift_x, float shift_y)
{
  float iz = aspect / v.z;
  p.x = v.x * iz + shift_x;
  p.y = v.y * iz + shift_y;
}

// --- csCurveTesselated -----------------------------------------------------

csCurveTesselated::csCurveTesselated (int num_v, int num_t)
{
  NumVertices = num_v;
  ObjectCoords = new csVector3[num_v];
  TextureCoords = new csVector2[num_v];
  ControlPoints = new csVector2[num_v];
  Colors = new csColor[num_v];
  NumTriangles = num_t;
  Triangles = new csTriangle[num_t];
  ColorsValid = false;
}

csCurveTesselated::~csCurveTesselated ()
{
  delete[] ObjectCoords;
  delete[] TextureCoords;
  delete[] ControlPoints;
  delete[] Colors;
  delete[] Triangles;
}

void csCurveTesselated::UpdateColors (csLightMap* LightMap)
{
  csRGBMap& rgbmap = LightMap->GetRealMap ();
  csRGBpixel* map = rgbmap.GetArray ();
  int lm_width = LightMap->GetWidth ();
  int lm_height = LightMap->GetWidth ();

  int j;
  for (j = 0 ; j < GetTriangleCount () ; j++)
  {
    csTriangle& ct = Triangles[j];
    int lm_idx;
    int cx, cy;
    cx = QInt (ControlPoints[ct.a].x * (lm_width - 1));
    cy = QInt (ControlPoints[ct.a].y * (lm_height - 1));
    lm_idx = cy*lm_width + cx;
    Colors[ct.a].red = ((float)map[lm_idx].red)/256.;
    Colors[ct.a].green = ((float)map[lm_idx].green)/256.;
    Colors[ct.a].blue = ((float)map[lm_idx].blue)/256.;
    cx = QInt (ControlPoints[ct.b].x * (lm_width - 1));
    cy = QInt (ControlPoints[ct.b].y * (lm_height - 1));
    lm_idx = cy*lm_width + cx;
    Colors[ct.b].red = ((float)map[lm_idx].red)/256.;
    Colors[ct.b].green = ((float)map[lm_idx].green)/256.;
    Colors[ct.b].blue = ((float)map[lm_idx].blue)/256.;
    cx = QInt (ControlPoints[ct.c].x * (lm_width - 1));
    cy = QInt (ControlPoints[ct.c].y * (lm_height - 1));
    lm_idx = cy*lm_width + cx;
    Colors[ct.c].red = ((float)map[lm_idx].red)/256.;
    Colors[ct.c].green = ((float)map[lm_idx].green)/256.;
    Colors[ct.c].blue = ((float)map[lm_idx].blue)/256.;
  }

  ColorsValid = true;
}

// --- csCurve ---------------------------------------------------------------

SCF_IMPLEMENT_IBASE_EXT (csCurve)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iCurve)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iVertexBufferManagerClient)
SCF_IMPLEMENT_IBASE_EXT_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csCurve::Curve)
  SCF_IMPLEMENTS_INTERFACE (iCurve)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csCurve::eiVertexBufferManagerClient)
  SCF_IMPLEMENTS_INTERFACE (iVertexBufferManagerClient)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

unsigned long csCurve::LastCurveID = 0;

csCurve::csCurve (csCurveTemplate* parent_tmpl) : csObject (),
    Material (NULL), CurveTemplate (parent_tmpl), LightPatches (NULL), 
    O2W (NULL), uv2World (NULL), uv2Normal (NULL), ParentThing (NULL),
    LightMap (NULL), LightmapUpToDate (false)
{
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiCurve);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiVertexBufferManagerClient);

  CurveID = LastCurveID++;
  Material = NULL;
  vbuf = NULL;
  vbufmgr = NULL;
  SetupVertexBuffer ();
} 

csCurve::~csCurve ()
{
  while (LightPatches)
    csEngine::current_engine->lightpatch_pool->Free (LightPatches);
  delete O2W;
  delete LightMap;
  delete[] uv2World;
  delete[] uv2Normal;
  SCF_DEC_REF (Material);
  if (vbufmgr) vbufmgr->RemoveClient (&scfiVertexBufferManagerClient);
  if (vbuf) vbuf->DecRef ();
}

void csCurve::SetupVertexBuffer ()
{
  if (!vbuf)
  {
    vbufmgr = csEngine::current_engine->G3D->GetVertexBufferManager ();
    // @@@ priority should be a parameter.
    vbuf = vbufmgr->CreateBuffer (2);
    vbufmgr->AddClient (&scfiVertexBufferManagerClient);
  }
}

void csCurve::SetMaterial (iMaterialWrapper *m)
{
  SCF_SET_REF (Material, m);
}

void csCurve::MakeDirtyDynamicLights () 
{ 
  LightmapUpToDate = false;
  LightMap->MakeDirtyDynamicLights (); 
};

void csCurve::AddLightPatch (csLightPatch* lp)
{
  lp->AddPolyList (LightPatches);
  lp->SetPolyCurve (this);

  /// set the dynamic lights to dirty
  LightMap->MakeDirtyDynamicLights ();
}

void csCurve::UnlinkLightPatch (csLightPatch* lp)
{
  lp->RemovePolyList (LightPatches);
  LightMap->MakeDirtyDynamicLights ();
}

bool csCurve::RecalculateDynamicLights ()
{
  // first combine the static and pseudo-dynamic lights
  if (!LightMap || !LightMap->UpdateRealLightMap() )
    return false;

  //---
  // Now add all dynamic lights.
  //---
  csLightPatch* lp = LightPatches;
  while (lp)
  {
    ShineDynLight (lp);
    lp = lp->GetNextPoly ();
  }

  return true;
}

void csCurve::ShineDynLight (csLightPatch* lp)
{
  CS_ASSERT(O2W);
  if (!uv2World) CalcUVBuffers ();
  CS_ASSERT(uv2World);
  CS_ASSERT(uv2Normal);

  int lm_width = LightMap->GetWidth ();
  int lm_height = LightMap->GetHeight ();

  csDynLight *light = lp->GetLight ();

  iShadowIterator* shadow_it = lp->GetShadowBlock ().GetShadowIterator ();
  bool has_shadows = shadow_it->HasNext ();

  csColor color = light->GetColor() * NORMAL_LIGHT_LEVEL;

  csRGBpixel *map = LightMap->GetRealMap().GetArray ();
  csVector3& center = lp->GetLightFrustum ()->GetOrigin ();

  int lval;
  float cosfact = csPolyTexture::cfg_cosinus_factor;

  // now add to the map
  csVector3 pos;
  csVector3 normal;
  float d;
  int ui, vi;
  int uv;
  for (ui = 0 ; ui < lm_width; ui++)
  {
    for (vi = 0 ; vi < lm_height; vi++)
    {
      uv = vi*lm_width + ui;

      pos = uv2World[uv];

      // is the point contained within the light frustrum? 
      if (!lp->GetLightFrustum ()->Contains (pos - center))
        // No, skip it
        continue;

      // if we have any shadow frustrumsq
      if (has_shadows)
      {
        shadow_it->Reset ();
	bool shad = false;
	while (shadow_it->HasNext ())
	{
          csFrustum* csf = shadow_it->Next ();
          // is this point in shadow
          if (csf->Contains(pos - csf->GetOrigin()))
	  {
	    shad = true;
            break;
	  }
        }
                  
        // if it was found in shadow skip it
        if (shad)
          continue;
      }

      d = csSquaredDist::PointPoint (center, pos);
      if (d >= light->GetSquaredRadius ()) continue;
      d = qsqrt (d);
      normal = uv2Normal[uv];
      float cosinus = (pos-center)*normal;
      cosinus /= d;
      cosinus += cosfact;
      if (cosinus < 0) cosinus = 0;
      else if (cosinus > 1) cosinus = 1;

      float brightness = cosinus * light->GetBrightnessAtDistance (d);

      if (color.red > 0)
      {
        lval = map[uv].red + QRound (color.red * brightness);
        if (lval > 255) lval = 255;
        map[uv].red = lval;
      }
      if (color.green > 0)
      {
        lval = map[uv].green + QRound (color.green * brightness);
        if (lval > 255) lval = 255;
        map[uv].green = lval;
      }
      if (color.blue > 0)
      {
        lval = map[uv].blue + QRound (color.blue * brightness);
        if (lval > 255) lval = 255;
        map[uv].blue = lval;
      }
    }
  }

  shadow_it->DecRef ();
}

void csCurve::SetObject2World (const csReversibleTransform *o2w) 
{ 
  if (!LightMap) return;	// Return if there is no LightMap yet.
  int lm_width = LightMap->GetWidth ();
  int lm_height = LightMap->GetHeight ();

  // current index into buffers
  int uv, ui, vi;
  
  // if there was already an object to world transform specified
  if (O2W && uv2World) 
  {
    // untransform our buffers
    for(ui=0; ui < lm_width; ui++)
    {
      for(vi=0; vi < lm_height; vi++)
      {
        uv = vi*lm_width + ui;

        uv2World[uv] = O2W->This2Other(uv2World[uv]);
      }
    }
  }
  
  // intialize the new transform
  delete O2W; 
  O2W = new csReversibleTransform(*o2w);

  if (uv2World)
  {
    // transform our uv2World buffer
    for (ui=0; ui < lm_width; ui++)
    {
      for (vi=0; vi < lm_height; vi++)
      {
        uv = vi*lm_width + ui;
        uv2World[uv] = O2W->Other2This(uv2World[uv]);
      }
    }
  }
}

void csCurve::CalculateLightingStatic (csFrustumView* lview, bool vis)
{
  if (!vis) return;
  csLightingPolyTexQueue* lptq = (csLightingPolyTexQueue*)
    lview->GetUserdata ();
  const csColor& col = lptq->GetColor ();

  // If our UV buffers have not been intialized, create them.
  if (!uv2World)
  {
    CalcUVBuffers ();
  }

  if (!LightMap || LightmapUpToDate) 
    return;

  int lm_width = LightMap->GetWidth ();
  int lm_height = LightMap->GetHeight ();

  csStatLight *light = (csStatLight *)(lptq->GetCsLight ());

  bool dyn = light->IsDynamic ();

  csShadowMap* smap;
  UByte *ShadowMap = 0;
  csRGBpixel *Lightmap = 0;

  /* initialize color to something to avoid compiler warnings */
  csColor color (0,0,0);

  if (dyn)
  {
    smap = LightMap->FindShadowMap (light);
    if (!smap)
    {
      smap = LightMap->NewShadowMap (light, CURVE_LM_SIZE, CURVE_LM_SIZE );
    }
    ShadowMap = smap->GetArray ();
  }
  else
  {
    Lightmap = LightMap->GetStaticMap ().GetArray ();
    color = col * NORMAL_LIGHT_LEVEL;
  }

  int lval;

  float cosfact = csPolyTexture::cfg_cosinus_factor;

  // get our coverage matrix
  csCoverageMatrix *shadow_matrix = new csCoverageMatrix (lm_width, lm_height);
  GetCoverageMatrix (*lview, *shadow_matrix);

  csFrustumContext* ctxt = lview->GetFrustumContext ();
  csVector3& center = ctxt->GetLightFrustum ()->GetOrigin ();

  // calculate the static LightMap
  csVector3 pos;
  csVector3 normal;
  float d;
  int ui, vi;
  int uv;
  for (ui = 0 ; ui < lm_width; ui++)
  {
    for (vi = 0 ; vi < lm_height; vi++)
    {
      uv = vi*lm_width + ui;

      // is the point lit by our current light
      if (shadow_matrix->coverage[uv] <= SMALL_EPSILON)
        // No, go to next point then
        continue;

      // What are the world coordinates of this texel?
      pos = uv2World[uv];

      d = csSquaredDist::PointPoint (center, pos);
      if (d >= light->GetSquaredRadius ()) continue;
      d = qsqrt (d);

      normal = uv2Normal[uv];
      float cosinus = (pos-center)*normal;
      cosinus /= d;
      cosinus += cosfact;

      if (cosinus < 0) 
        cosinus = 0;
      else if (cosinus > 1) 
        cosinus = 1;

      float brightness = cosinus * light->GetBrightnessAtDistance (d);

      if (dyn)
      {
        lval = ShadowMap[uv] + QRound (NORMAL_LIGHT_LEVEL * brightness);
        if (lval > 255) lval = 255;
        ShadowMap[uv] = lval;
      }
      else
      {
        if (col.red > 0)
        {
          lval = Lightmap[uv].red + QRound (color.red * brightness);
          if (lval > 255) lval = 255;
          Lightmap[uv].red = lval;
        }
        if (col.green > 0)
        {
          lval = Lightmap[uv].green + QRound (color.green * brightness);
          if (lval > 255) lval = 255;
          Lightmap[uv].green = lval;
        }
        if (col.blue > 0)
        {
          lval = Lightmap[uv].blue + QRound (color.blue * brightness);
          if (lval > 255) lval = 255;
          Lightmap[uv].blue = lval;
        }
      }
    }
  }

  delete shadow_matrix;
}

void csCurve::CalculateLightingDynamic (csFrustumView* lview)
{
  csLightingPolyTexQueue* lptq = (csLightingPolyTexQueue*)
    lview->GetUserdata ();

  // We are working for a dynamic light. In this case we create
  // a light patch for this polygon.
  csLightPatch* lp = csEngine::current_engine->lightpatch_pool->Alloc ();

  AddLightPatch (lp);
  
  csDynLight* dl = (csDynLight*)(lptq->GetCsLight ());
  dl->AddLightpatch (lp);

  // This light patch has exactly 4 vertices because it fits around our
  // LightMap
  lp->Initialize (4);

  // Copy shadow frustums.
  lp->GetShadowBlock ().DeleteShadows ();
  // @@@: It would be nice if we could optimize earlier 
  // to determine relevant shadow frustums in curves and use
  // AddRelevantShadows instead.
  lp->GetShadowBlock ().AddAllShadows (lview->GetFrustumContext ()
  	->GetShadows ());

  lp->SetLightFrustum (new csFrustum (*lview->GetFrustumContext ()->
  	GetLightFrustum ()));

  MakeDirtyDynamicLights ();
}

void csCurve::InitializeDefaultLighting ()
{
  if (!IsLightable ()) return;
  LightMap = new csLightMap ();

  // Allocate space for the LightMap and initialize it to ambient color.
  int r, g, b;
  r = csLight::ambient_red;
  g = csLight::ambient_green;
  b = csLight::ambient_blue;
  LightMap->Alloc (CURVE_LM_SIZE*csLightMap::lightcell_size, 
                   CURVE_LM_SIZE*csLightMap::lightcell_size, r, g, b);
  LightmapUpToDate = false;
}

bool csCurve::ReadFromCache (int id)
{
  if (!IsLightable ()) return true;
  LightMap = new csLightMap ();

  // Allocate space for the LightMap and initialize it to ambient color.
  int r, g, b;
  r = csLight::ambient_red;
  g = csLight::ambient_green;
  b = csLight::ambient_blue;
  LightMap->Alloc (CURVE_LM_SIZE*csLightMap::lightcell_size, 
                   CURVE_LM_SIZE*csLightMap::lightcell_size, r, g, b);

  LightMap->ReadFromCache (id, CURVE_LM_SIZE*csLightMap::lightcell_size,
           CURVE_LM_SIZE*csLightMap::lightcell_size, this, false, 
           csEngine::current_engine);
  LightmapUpToDate = true;
  return true;
}

bool csCurve::WriteToCache (int id)
{
  if (!LightMap) return true;
  if (!LightmapUpToDate)
  {
    LightmapUpToDate = true;
    if (csEngine::current_engine->GetLightingCacheMode ()
    	& CS_ENGINE_CACHE_WRITE)
      LightMap->Cache (id, NULL, this, csEngine::current_engine);
  }
  return true;
}

void csCurve::PrepareLighting ()
{
  if (LightMap)
    LightMap->ConvertToMixingMode ();
}


void csCurve::GetCoverageMatrix (csFrustumView& lview, 
                                 csCoverageMatrix &cm) const
{
  csVector3 pos;
  int uv, ui, vi;

  csFrustumContext* ctxt = lview.GetFrustumContext ();
  iShadowIterator* shadow_it = ctxt->GetShadows ()->GetShadowIterator ();
  bool has_shadows = shadow_it->HasNext ();
  csVector3& center = ctxt->GetLightFrustum ()->GetOrigin ();
  
  int lm_width = LightMap->GetWidth ();
  int lm_height = LightMap->GetHeight ();

  cm.height = lm_height;
  cm.width = lm_width;

  for (ui = 0 ; ui < lm_width; ui++)
  {
    for (vi = 0 ; vi < lm_height; vi++)
    {
      uv = vi*lm_width + ui;
      pos = uv2World[uv];

      // is the point contained within the light frustrum? 
      if (!ctxt->GetLightFrustum ()->Contains (pos-center))
        // No, skip it
        continue;

      // if we have any shadow frustrums
      if (has_shadows)
      {
        csFrustum* csf = NULL;
	bool shadowed = false;
	shadow_it->Reset ();
	while (shadow_it->HasNext ())
	{
	  csf = shadow_it->Next ();
          // is this point in shadow
          if (csf->Contains (pos - csf->GetOrigin()))
	  {
	    shadowed = true;
            break;
	  }
        }
              
        // if it was found in shadow skip it
        if (shadowed)
          continue;
      }

      cm.coverage[uv] = 1.0;
    }
  }

  shadow_it->DecRef ();
}

float csCurve::GetArea()
{
  float area = 0.0;

  // Tesselate at resolution 10000 to get the best approximation of area
  csCurveTesselated* ct = Tesselate( 10000 );

  csVector3 *vertex = ct->GetVertices ();
  csTriangle t;

  // loop through all of our triangles and sum thier areas
  int i;
  for(i=0; i<ct->GetTriangleCount(); i++)
  {
    t = ct->GetTriangle(i);
    area += ABS(csMath3::Area3 (vertex[t.a], vertex[t.b], vertex[t.c]) );
  }

  return area / 2.0;
}

void csCurve::CalcUVBuffers()
{
  delete[] uv2World;
  delete[] uv2Normal;

  int lm_width = LightMap->GetWidth ();
  int lm_height = LightMap->GetHeight ();

  uv2World = new csVector3[lm_width * lm_height];
  uv2Normal = new csVector3[lm_width * lm_height];

  // our texture space coordinates
  float u,v;
  int uv, ui, vi;

  // now loop over every texel in the LightMap
  for (ui = 0 ; ui < lm_width; ui++)
  {
    // calculate the real u coordinate in texture space
    u = ((float)ui + 0.5F)/(float)lm_width; // offset 0.5 for texel center
    for (vi = 0 ; vi < lm_height; vi++)
    {
      uv = vi*lm_width + ui;

      // calculate the real v coordinate in texture space
      v = ((float)vi + 0.5F)/(float)lm_height;// offset 0.5 for texel center

      // ask our curve to find the object space coordinate at u,v
      PosInSpace (uv2World[uv], u, v);
      
      // ask our curve to find the normal at u,v
      Normal (uv2Normal[uv], u, v);

      // if we have an object to world transformation, use it
      if (O2W)
      {
        uv2World[uv] = O2W->Other2This(uv2World[uv]);
      }
    }
  }
}

void csCurve::GetCameraBoundingBox (const csTransform& obj2cam, csBox3& cbox)
{
  csBox3 box;
  GetObjectBoundingBox (box);
  cbox.StartBoundingBox (obj2cam * box.GetCorner (0));
  cbox.AddBoundingVertexSmart (obj2cam * box.GetCorner (1));
  cbox.AddBoundingVertexSmart (obj2cam * box.GetCorner (2));
  cbox.AddBoundingVertexSmart (obj2cam * box.GetCorner (3));
  cbox.AddBoundingVertexSmart (obj2cam * box.GetCorner (4));
  cbox.AddBoundingVertexSmart (obj2cam * box.GetCorner (5));
  cbox.AddBoundingVertexSmart (obj2cam * box.GetCorner (6));
  cbox.AddBoundingVertexSmart (obj2cam * box.GetCorner (7));
}

float csCurve::GetScreenBoundingBox (const csTransform& obj2cam,
	iCamera* camera, csBox3& cbox, csBox2& boundingBox)
{
  csVector2   oneCorner;

  float aspect = camera->GetFOV ();
  float shift_x = camera->GetShiftX ();
  float shift_y = camera->GetShiftY ();

  // @@@ Note. The bounding box created by this function greatly
  // exagerates the real bounding box. However, this function
  // needs to be fast. I'm not sure how to do this more accuratelly.

  GetCameraBoundingBox (obj2cam, cbox);

  // if the entire bounding box is behind the camera, we're done
  if ((cbox.MinZ () < 0) && (cbox.MaxZ () < 0))
    return -1;

  // Transform from camera to screen space.
  if (cbox.MinZ () <= 0)
  {
    // Sprite is very close to camera.
    // Just return a maximum bounding box.
    boundingBox.Set (-10000, -10000, 10000, 10000);
  }
  else
  {
    Perspective (cbox.Max (), oneCorner, aspect, shift_x, shift_y);
    boundingBox.StartBoundingBox (oneCorner);
    csVector3 v (cbox.MinX (), cbox.MinY (), cbox.MaxZ ());
    Perspective (v, oneCorner, aspect, shift_x, shift_y);
    boundingBox.AddBoundingVertexSmart (oneCorner);
    Perspective (cbox.Min (), oneCorner, aspect, shift_x, shift_y);
    boundingBox.AddBoundingVertexSmart (oneCorner);
    v.Set (cbox.MaxX (), cbox.MaxY (), cbox.MinZ ());
    Perspective (v, oneCorner, aspect, shift_x, shift_y);
    boundingBox.AddBoundingVertexSmart (oneCorner);
  }

  return cbox.MaxZ ();
}

// Default IsLightable returns false, because we don't know how to calculate
// x,y,z and normals for the curve by default
bool csCurve::IsLightable()
{
  return false;
}

// Default PosInSpace does nothing
void csCurve::PosInSpace (csVector3& /*vec*/, double /*u*/, double /*v*/)
{
  return;
}

// Default Normal does nothing
void csCurve::Normal (csVector3& /*vec*/, double /*u*/, double /*v*/)
{
  return;
}

void csCurve::HardTransform (const csReversibleTransform& /*trans*/)
{
  /// @@@ where must the transformation be used???
  int i;
  for (i = 0 ; i < GetParentTemplate ()->GetVertexCount () ; i++)
    SetControlPoint (i, GetParentTemplate ()->GetVertex (i));
  if (uv2World) CalcUVBuffers ();
}

iCurveTemplate* csCurve::Curve::GetParentTemplate ()
{ 
  return &(scfParent->GetParentTemplate ()->scfiCurveTemplate);
}

void csCurve::eiVertexBufferManagerClient::ManagerClosing ()
{
  if (scfParent->vbuf)
  {
    scfParent->vbuf->DecRef ();
    scfParent->vbuf = NULL;
    scfParent->vbufmgr = NULL;
  }
}

// --- csCurveTemplate -------------------------------------------------------

SCF_IMPLEMENT_IBASE_EXT (csCurveTemplate)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iCurveTemplate)
  SCF_IMPLEMENTS_INTERFACE (csCurveTemplate)
SCF_IMPLEMENT_IBASE_EXT_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csCurveTemplate::CurveTemplate)
  SCF_IMPLEMENTS_INTERFACE (iCurveTemplate)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csCurveTemplate::csCurveTemplate () : csObject ()
{
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiCurveTemplate);
  csEngine::current_engine->AddToCurrentRegion (this);
  Material = NULL;
}

csCurveTemplate::~csCurveTemplate ()
{
  SCF_DEC_REF (Material);
}

void csCurveTemplate::SetMaterial (iMaterialWrapper* m)
{
  SCF_SET_REF (Material, m);
}

// --- code for Bezier curves follows ----------------------------------------

// Initialize the cache for Bezier curves
static csBezier2 bezierCache;

// Should not be necessary, but without this buggy NextStep compiler
// incorrectly calls csObject::QueryInterface() rather than correctly calling
// csCurveTemplate::QueryInterface().
SCF_IMPLEMENT_IBASE_EXT(csBezierTemplate)
SCF_IMPLEMENT_IBASE_EXT_END

csCurveTesselated* csBezierCurve::Tesselate (int res)
{
  if (res<2)
    res=2;
  else if (res>9)
    res=9;
  
  if (res == previous_resolution && previous_tesselation)
    return previous_tesselation;

  previous_resolution = res;
  delete previous_tesselation;

  previous_tesselation = 
       new csCurveTesselated ((res+1)*(res+1), 2*res*res);

  double *controls[9] = 
  {
    cpt[0], cpt[1], cpt[2], cpt[3], cpt[4],
    cpt[5], cpt[6], cpt[7], cpt[8],
  };

  int i,j;

  for (i=0;i<=res;i++)
  for (j=0;j<=res;j++)
  {
    int idx = i+(res+1)*j;

    csVector3* vtx_coord = previous_tesselation->GetVertices ()+idx;
    csVector2* vtx_txtcoord = previous_tesselation->GetTxtCoords ()+idx;
    csVector2* vtx_control = previous_tesselation->GetControlPoints ()+idx;
    *vtx_coord = csBezier2::GetPoint (controls, i, j, res);
    //
    *vtx_txtcoord = csBezier2::GetTextureCoord (controls, i, j, res);
    //
    vtx_control->x = ((float)i)/(float)res;
    vtx_control->y = ((float)j)/(float)res;
  }

  for (i=0;i<res;i++)
  {
    for (j=0;j<res;j++)
    {
      csTriangle& up = previous_tesselation->GetTriangle (2*(i+j*res));
      csTriangle& down = previous_tesselation->GetTriangle (2*(i+j*res)+1);
      
      int tl = i+(res+1)*j;
      int tr = i+(res+1)*j+1;

      int bl = i+(res+1)*(j+1);
      int br = i+(res+1)*(j+1)+1;
      up.a = tl;
      up.b = br;
      up.c = tr;

      down.a = br;
      down.b = tl;
      down.c = bl;
    }
  }

  return previous_tesselation;
}

void csBezierCurve::GetObjectBoundingBox (csBox3& bbox)
{
  // @@@ This algo uses the control points to compute
  // the bounding box. Is this right?
  if (!valid_bbox)
  {
    valid_bbox = true;
    object_bbox.StartBoundingBox ();
    int i,j;
    for (i=0 ; i<3 ; i++)
      for (j=0 ; j<3 ; j++)
        object_bbox.AddBoundingVertex (points[i][j]);
  }
  bbox = object_bbox;
}

csBezierCurve::csBezierCurve (csBezierTemplate* parent_tmpl) :
  csCurve (parent_tmpl) 
{
  int i,j;
  for (i=0 ; i<3 ; i++)
    for (j=0 ; j<3 ; j++)
    {
      texture_coords[i][j].x = (0.5*i);
      texture_coords[i][j].y = (0.5*j);
    }
  previous_tesselation = NULL;
  previous_resolution = -1;
  valid_bbox = false;
}

csBezierCurve::~csBezierCurve ()
{
  delete previous_tesselation;
}

void csBezierCurve::SetControlPoint (int index, int control_id)
{
  GetControlPoint(index) = ParentThing->CurveVertex (control_id);
  GetTextureCoord(index) = ParentThing->CurveTexel (control_id);
  cpt[index][0] = GetControlPoint(index).x;
  cpt[index][1] = GetControlPoint(index).y;
  cpt[index][2] = GetControlPoint(index).z;
  cpt[index][3] = GetTextureCoord(index).x;
  cpt[index][4] = GetTextureCoord(index).y;
}


bool csBezierCurve::IsLightable ()
{
  return true;
}

void csBezierCurve::PosInSpace (csVector3& vec, double u, double v)
{
  double *controls[9] = 
  {
    cpt[0], cpt[1], cpt[2], cpt[3], cpt[4],
    cpt[5], cpt[6], cpt[7], cpt[8],
  };

  vec = csBezier2::GetPoint (controls,  u, v);
}

void csBezierCurve::Normal (csVector3& vec, double u, double v)
{
  double *controls[9] = 
  {
    cpt[0], cpt[1], cpt[2], cpt[3], cpt[4],
    cpt[5], cpt[6], cpt[7], cpt[8],
  };
  vec = csBezier2::GetNormal (controls, u, v);
}

void csBezierCurve::HardTransform (const csReversibleTransform& trans)
{
  valid_bbox = false;
  csCurve::HardTransform (trans);
}

//------------------------------------------------------------------

csBezierTemplate::csBezierTemplate ()
  : csCurveTemplate () 
{
  int i;
  for (i=0 ; i<9 ; i++)
    ver_id[i] = 0;
}

void csBezierTemplate::SetVertex (int index, int ver_ind)
{
  ver_id[index] = ver_ind;
}
int csBezierTemplate::GetVertex (int index) 
{
  return ver_id[index];
}

int csBezierTemplate::GetVertexCount ()
{
  return 9;
}

csCurve* csBezierTemplate::MakeCurve ()
{
  csBezierCurve* p = new csBezierCurve (this);
  p->SetMaterial (Material);
  return p;
}
