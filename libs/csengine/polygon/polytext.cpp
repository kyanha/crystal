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

#include <math.h>

#include "sysdef.h"
#include "qint.h"
#include "csengine/sysitf.h"
#include "csengine/polygon/polytext.h"
#include "csengine/polygon/polyplan.h"
#include "csengine/polygon/polygon.h"
#include "csengine/light/light.h"
#include "csengine/sector.h"
#include "csengine/world.h"
#include "csengine/light/lghtmap.h"
#include "igraph3d.h"
#include "itexture.h"

// Option variable: do accurate lighting of things (much slower)?
bool csPolyTexture::do_accurate_things = true;
// Option variable: cosinus factor.
float csPolyTexture::cfg_cosinus_factor = 0;

int csPolyTexture::subtex_size = DEFAULT_SUBTEX_SIZE;
bool csPolyTexture::subtex_dynlight = true;

//---------------------------------------------------------------------------

IMPLEMENT_UNKNOWN_NODELETE( csPolyTexture )

BEGIN_INTERFACE_TABLE( csPolyTexture )
	IMPLEMENTS_COMPOSITE_INTERFACE( PolygonTexture )
END_INTERFACE_TABLE()
	
csPolyTexture::csPolyTexture ()
{
  dyn_dirty = true;
  dirty_matrix = NULL;
  lm = NULL;
  tcache_data = NULL;
}

csPolyTexture::~csPolyTexture ()
{
  CHK (delete [] dirty_matrix);
}

void csPolyTexture::SetMipmapSize (int mm)
{
  mipmap_size = mm;
  switch (mipmap_size)
  {
    case 2: mipmap_shift = 1; break;
    case 4: mipmap_shift = 2; break;
    case 8: mipmap_shift = 3; break;
    case 16: mipmap_shift = 4; break;
    case 32: mipmap_shift = 5; break;
    case 64: mipmap_shift = 6; break;
    default: CsPrintf (MSG_FATAL_ERROR, "Bad mipmap value!\n");
             fatal_exit (0, false);
             mipmap_shift = 0;
  }
}

#if 0
bool verbose = false;
#  define DB(x) if (verbose) CsPrintf##x
#  define DBCHECK(sector,poly) verbose = !strcmp (polygon->get_name (), poly) && !strcmp (((PolygonSet*)(polygon->get_parent ()))->get_name (), sector)
#else
#  define DB(x)
#  define DBCHECK(sector,poly)
#endif

void csPolyTexture::CreateBoundingTextureBox ()
{
  DBCHECK ("moor", "southU");
  DB ((MSG_DEBUG_0, "---------------------------------------------\n"));

  // First we compute the bounding box in 2D texture space (uv-space).
  float min_u = 1000000000.;
  float min_v = 1000000000.;
  float max_u = -1000000000.;
  float max_v = -1000000000.;

  csPolyPlane* pl = polygon->GetPlane ();

  int i;
  csVector3 v1, v2;
  DB ((MSG_DEBUG_0, "  Vertices in world-space:\n"));
  for (i = 0 ; i < polygon->GetNumVertices () ; i++)
  {
    v1 = polygon->Vwor (i);	 	// Coordinates of vertex in world space.
    DB ((MSG_DEBUG_0, "      %d:(%f,%f,%f)\n", i, v1.x, v1.y, v1.z));
    v1 -= pl->v_world2tex;
    v2 = (pl->m_world2tex) * v1;	// Coordinates of vertex in texture space.
    if (v2.x < min_u) min_u = v2.x;
    if (v2.x > max_u) max_u = v2.x;
    if (v2.y < min_v) min_v = v2.y;
    if (v2.y > max_v) max_v = v2.y;
  }

  DB ((MSG_DEBUG_0, "  min_u=%f max_u=%f min_v=%f max_v=%f\n", min_u, max_u, min_v, max_v));

  int ww, hh;
  txt_handle->GetMipMapDimensions (mipmap_level, ww, hh);
  Imin_u = QRound (min_u*ww);
  Imin_v = QRound (min_v*hh);
  Imax_u = QRound (max_u*ww);
  Imax_v = QRound (max_v*hh);
  
  DB ((MSG_DEBUG_0, "  ww=%d hh=%d\n", ww, hh));
  DB ((MSG_DEBUG_0, "  Imin_u=%d Imax_u=%d Imin_v=%d Imax_v=%d\n", Imin_u, Imax_u, Imin_v, Imax_v));

  // DAN: used in hardware accel drivers
  Fmin_u = min_u;
  Fmax_u = max_u;
  Fmin_v = min_v;
  Fmax_v = max_v;

  h = Imax_v-Imin_v;
  w_orig = Imax_u-Imin_u;
  w = 1;
  shf_u = 0;
  and_u = 0;
  while (true)
  {
    if (w_orig <= w) break;
    w <<= 1;
    shf_u++;
    and_u = (and_u<<1)+1;
  }

  fdu = min_u*ww;
  fdv = min_v*hh;
  du = QInt16 (min_u*ww);
  dv = QInt16 (min_v*hh);

  DB ((MSG_DEBUG_0, "  w_orig=%d w=%d h=%d\n", w_orig, w, h));
  DB ((MSG_DEBUG_0, "  fdu=%f fdv=%f du=%d dv=%d\n", fdu, fdv, du, dv));
  DB ((MSG_DEBUG_0, "  and_u=%d shf_u=%d\n", and_u, shf_u));

  // The size of the whole texture is extended by an upper and lower
  // margin to prevent overflow in the texture mapper.
  size = w*(H_MARGIN+H_MARGIN+h);

  DB ((MSG_DEBUG_0, "  size=%d\n", size));
}

void csPolyTexture::MakeDirtyDynamicLights ()
{
  dyn_dirty = true;
}

bool csPolyTexture::RecalcDynamicLights ()
{
  if (!dyn_dirty) return false;
  if (!lm) return false;

  dyn_dirty = false;
  bool dm = csPolyTexture::subtex_size && csPolyTexture::subtex_dynlight && dirty_matrix;

  //---
  // First copy the static lightmap to the real lightmap.
  // Remember the real lightmap first so that we can see if
  // there were any changes.
  //---
  long lm_size = lm->GetSize ();
  csRGBLightMap& stmap = lm->GetStaticMap ();
  csRGBLightMap& remap = lm->GetRealMap ();
  csRGBLightMap oldmap;

  if (dm) { oldmap.AllocRed (lm_size); memcpy (oldmap.mapR, remap.mapR, lm_size); }
  memcpy (remap.mapR, stmap.mapR, lm_size);

  if (remap.mapG)
  {
    if (dm) { oldmap.AllocGreen (lm_size); memcpy (oldmap.mapG, remap.mapG, lm_size); }
    memcpy (remap.mapG, stmap.mapG, lm_size);
  }

  if (remap.mapB)
  {
    if (dm) { oldmap.AllocBlue (lm_size); memcpy (oldmap.mapB, remap.mapB, lm_size); }
    memcpy (remap.mapB, stmap.mapB, lm_size);
  }

  //---
  // Then add all pseudo-dynamic lights.
  //---
  csLight* light;
  unsigned char* mapR, * mapG, * mapB;
  float red, green, blue;
  unsigned char* p, * last_p;
  int l, s;

  if (lm->first_smap)
  {
    csShadowMap* smap = lm->first_smap;

    //@@@
    //if (Textures::mixing != MIX_NOCOLOR)
    {
      // Color mode.
      do
      {
        mapR = remap.mapR;
        mapG = remap.mapG;
        mapB = remap.mapB;
        light = smap->light;
        red = light->GetColor ().red;
        green = light->GetColor ().green;
        blue = light->GetColor ().blue;
        csLight::CorrectForNocolor (&red, &green, &blue);
	p = smap->map;
	last_p = p+lm_size;
	do
	{
	  s = *p++;
          l = *mapR + QRound (red * s);
	  if (l > 255) l = 255;
          *mapR++ = l;
          l = *mapG + QRound (green * s); if (l > 255) l = 255;
	  if (l > 255) l = 255;
	  *mapG++ = l;
          l = *mapB + QRound (blue * s); if (l > 255) l = 255;
	  if (l > 255) l = 255;
          *mapB++ = l;
	}
	while (p < last_p);

        smap = smap->next;
      }
      while (smap);
    }
//@@@
#if 0
    else
    {
      // NOCOLOR mode.
      do
      {
        mapR = remap.mapR;
        light = smap->light;
        red = light->get_red ();
        green = light->get_green ();
        blue = light->get_blue ();
        csLight::mixing_dependent_strengths (&red, &green, &blue);
	p = smap->map;
	last_p = p+lm_size;
	do
	{
          l = *mapR + QRound (red * (*p++));
	  if (l > 255) l = 255;
          *mapR++ = l;
	}
	while (p < last_p);

        smap = smap->next;
      }
      while (smap);
    }
#endif
  }

  //---
  // Now add all dynamic lights.
  //---
  csLightPatch* lp = polygon->GetLightpatches ();
  while (lp)
  {
    ShineDynLightmap (lp);
    lp = lp->GetNextPoly ();
  }

  if (dm)
  {
    //---
    // Now compare the old map with the new one and
    // see where there are changes. Tag the corresponding
    // sub-textures as dirty.
    //---
    int lw = lm->GetWidth ();
    int lh = lm->GetHeight ();
    int ru, rv, idx;
    int lv, lu, luv, luv_v;
    int num = csPolyTexture::subtex_size >> mipmap_shift;	// Horiz/vert number of lightmap boxes in one sub-texture
    int numu, numv;
    int uu, vv;

    idx = 0;
    for (rv = 0 ; rv < dirty_h ; rv++)
    {
      lv = (rv * csPolyTexture::subtex_size) >> mipmap_shift;
      luv_v = lv * lw;
      if (lv+num >= lh) numv = lh-lv-1;
      else numv = num;

      for (ru = 0 ; ru < dirty_w ; ru++, idx++)
      {
        // If already dirty we don't need to check.
        if (!dirty_matrix[idx])
	{
          lu = (ru * csPolyTexture::subtex_size) >> mipmap_shift;
	  luv = luv_v + lu;
          if (lu+num >= lw-1) numu = lw-lu-1;
	  else numu = num;
	  // <= num here because I want to include the boundaries of the next
	  // sub-texture in the test as well.
	  for (vv = 0 ; vv <= numv ; vv++)
	  {
	    
	    for (uu = 0 ; uu <= numu ; uu++)
	    {
	      // If we find a difference this sub-texture is dirty. I would like to have
	      // a multi-break statement but unfortunatelly C++ does not have this. That's
	      // why I use the goto. Yes I know! goto's are EVIL!
	      if ((oldmap.mapR[luv] != remap.mapR[luv]) ||
	          (oldmap.mapG && (oldmap.mapG[luv] != remap.mapG[luv])) ||
	      	  (oldmap.mapB && (oldmap.mapB[luv] != remap.mapB[luv])))
	      {
	        dirty_matrix[idx] = 1;
		dirty_cnt++;
		goto stop;
	      }
	      luv++;
	    }
	    luv += lw-num-1;
	  }
	  stop: ;
	}
      }
    }
  }

  return true;
}

void csPolyTexture::InitLightmaps ()
{
}

void csPolyTexture::ShineLightmaps (csLightView& lview)
{
  if (!lm) return;
  DBCHECK ("moor", "southU");
  csStatLight* light = (csStatLight*)lview.l;
  DB ((MSG_DEBUG_0, "#### shine_lightmaps #### light:(%f,%f,%f)\n", light->GetCenter ().x, light->GetCenter ().y, light->GetCenter ().z));

  int lw = lm->GetWidth ();	// @@@ DON'T NEED TO GO TO PO2 SIZES
  int lh = lm->GetHeight ();

  int u, uv;

  int l1, l2 = 0, l3 = 0;
  float d, dl;

  int ww, hh;
  txt_handle->GetMipMapDimensions (mipmap_level, ww, hh);

  DB ((MSG_DEBUG_0, "lw=%d lh=%d ww=%d hh=%d mipmap_shift=%d mipmap_size=%d\n", lw, lh, ww, hh, mipmap_shift, mipmap_size));

  csPolyPlane* pl = polygon->GetPlane ();
  float cosfact = polygon->GetCosinusFactor ();
  if (cosfact == -1) cosfact = cfg_cosinus_factor;

  // From: T = Mwt * (W - Vwt)
  // ===>
  // Mtw * T = W - Vwt
  // Mtw * T + Vwt = W
  csMatrix3 m_t2w = pl->m_world2tex.GetInverse ();
  csVector3 vv = pl->v_world2tex;

  // From: Ax+By+Cz+D = 0
  // From: T = Mwt * (W - Vwt)
  // ===>
  // Mtw * T + Vwt = W
  // Get 'w' from 'u' and 'v' (using u,v,w plane).
  float A = pl->GetWorldPlane ().A ();
  float B = pl->GetWorldPlane ().B ();
  float C = pl->GetWorldPlane ().C ();
  float D = pl->GetWorldPlane ().D ();
  float txt_A = A*m_t2w.m11 + B*m_t2w.m21 + C*m_t2w.m31;
  float txt_B = A*m_t2w.m12 + B*m_t2w.m22 + C*m_t2w.m32;
  float txt_C = A*m_t2w.m13 + B*m_t2w.m23 + C*m_t2w.m33;
  float txt_D = A*pl->v_world2tex.x + B*pl->v_world2tex.y + C*pl->v_world2tex.z + D;

  DB ((MSG_DEBUG_0, "(A,B,C,D)=(%f,%f,%f,%f) txt_(A,B,C,D)=(%f,%f,%f,%f)\n", A, B, C, D, txt_A, txt_B, txt_C, txt_D));

  csVector3 v1, v2, v3;

  int ru, rv;
  float invww, invhh;
  invww = 1. / (float)ww;
  invhh = 1. / (float)hh;

  bool hit;			// Set to true if there is a hit
  bool first_time = false;	// Set to true if this is the first pass for the dynamic light
  int dyn;

  unsigned char* mapR;
  unsigned char* mapG;
  unsigned char* mapB;
  csShadowMap* smap;

  int i;
  smap = NULL;
  hit = false;

  dyn = light->IsDynamic ();
  if (dyn)
  {
    smap = lm->FindShadowMap (light);
    if (!smap) { smap = lm->NewShadowMap (light, w, h, mipmap_size); first_time = true; }
    else first_time = false;
    mapR = smap->map;
    mapG = NULL;
    mapB = NULL;
  }
  else
  {
    mapR = lm->GetStaticMap ().mapR;
    mapG = lm->GetStaticMap ().mapG;
    mapB = lm->GetStaticMap ().mapB;
  }

  float miny = 1000000, maxy = -1000000;
  int MaxIndex = -1, MinIndex = -1;

  // Calculate the uv's for all points of the frustrum (the
  // frustrum is actually a clipped version of the polygon).
  csVector2* f_uv = NULL;
  if (lview.frustrum)
  {
    int mi;
    CHK (f_uv = new csVector2 [lview.num_frustrum]);
    for (i = 0 ; i < lview.num_frustrum ; i++)
    {
      if (lview.mirror) mi = lview.num_frustrum-i-1;
      else mi = i;

      // T = Mwt * (W - Vwt)
      v1 = pl->m_world2tex * (lview.frustrum[mi] + lview.center - pl->v_world2tex);
      f_uv[i].x = (v1.x*ww-Imin_u) / mipmap_size;
      f_uv[i].y = (v1.y*hh-Imin_v) / mipmap_size;
      if (f_uv[i].y < miny) miny = f_uv[MinIndex = i].y;
      if (f_uv[i].y > maxy) maxy = f_uv[MaxIndex = i].y;
      DB ((MSG_DEBUG_0, "    %d: frust:(%f,%f,%f) f_uv:(%f,%f) ", i, lview.frustrum[mi].x, lview.frustrum[mi].y, lview.frustrum[mi].z, f_uv[i].x, f_uv[i].y));
      DB ((MSG_DEBUG_0, "frust+lcenter:(%f,%f,%f)\n", lview.frustrum[mi].x+lview.center.x, lview.frustrum[mi].y+lview.center.y, lview.frustrum[mi].z+lview.center.z));
    }
  }

  float r200d = lview.r * NORMAL_LIGHT_LEVEL / light->GetRadius ();
  float g200d = lview.g * NORMAL_LIGHT_LEVEL / light->GetRadius ();
  float b200d = lview.b * NORMAL_LIGHT_LEVEL / light->GetRadius ();

  int scanL1, scanL2, scanR1, scanR2;   // scan vertex left/right start/final
  float sxL, sxR, dxL, dxR;             // scanline X left/right and deltas
  int sy, fyL, fyR;                     // scanline Y, final Y left, final Y right
  int xL, xR;

  sxL = sxR = dxL = dxR = 0;            // avoid GCC warnings about "uninitialized variables"
  scanL2 = scanR2 = MaxIndex;
  sy = fyL = fyR = (QRound (f_uv[scanL2].y)>lh-1)?lh-1:QRound (f_uv[scanL2].y);

  for ( ; ; )
  {
    //-----
    // We have reached the next segment. Recalculate the slopes.
    //-----
    bool leave;
    do
    {
      leave = true;
      if (sy <= fyR)
      {
        // Check first if polygon has been finished
        if (scanR2 == MinIndex) goto finish;
        scanR1 = scanR2;
        scanR2 = (scanR2 + 1) % lview.num_frustrum;

        fyR = QRound(f_uv[scanR2].y);
        float dyR = (f_uv[scanR1].y - f_uv[scanR2].y);
	sxR = f_uv[scanR1].x;
        if (dyR != 0)
        {
          dxR = (f_uv[scanR2].x - sxR) / dyR;
	  // horizontal pixel correction
          sxR += dxR * (f_uv[scanR1].y - ((float)sy));
        }
	else dxR = 0;
        leave = false;
      }
      if (sy <= fyL)
      {
        scanL1 = scanL2;
        scanL2 = (scanL2 - 1 + lview.num_frustrum) % lview.num_frustrum;

        fyL = QRound(f_uv[scanL2].y);
        float dyL = (f_uv[scanL1].y - f_uv[scanL2].y);
	sxL = f_uv[scanL1].x;
        if (dyL != 0)
        {
          dxL = (f_uv[scanL2].x - sxL) / dyL;
          // horizontal pixel correction
          sxL += dxL * (f_uv[scanL1].y - ((float)sy));
        }
	else dxL = 0;
        leave = false;
      }
    }
    while (!leave);

    // Find the trapezoid top (or bottom in inverted Y coordinates)
    int fin_y;
    if (fyL > fyR) fin_y = fyL;
    else fin_y = fyR;

    while (sy >= fin_y)
    {
      // Compute the rounded screen coordinates of horizontal strip
      xL = QRound (sxL)+1;
      xR = QRound (sxR);
      if (xR > xL) { int xswap = xR; xR = xL; xL = xswap; }

      if (xR < 0) xR = 0;
      if (xL > lw) xL = lw;

      for (u = xR; u < xL ; u++)
      {
        uv = sy*lw+u;

        ru = u  << mipmap_shift;
        rv = sy << mipmap_shift;

        bool rc = false;
        int tst;
        static int shift_u[5] = { 0, 2, 0, -2, 0 };
        static int shift_v[5] = { 0, 0, 2, 0, -2 };
        for (tst = 0 ; tst < 5 ; tst++)
        {
          v1.x = (float)(ru+shift_u[tst]+Imin_u)*invww;
          v1.y = (float)(rv+shift_v[tst]+Imin_v)*invhh;
          if (ABS (txt_C) < SMALL_EPSILON)
            v1.z = 0;
          else
            v1.z = - (txt_D + txt_A*v1.x + txt_B*v1.y) / txt_C;
          v2 = vv + m_t2w * v1;

          // Perform the transformation given by m_warp and v_warp.
          // This is the inverse portal warping transformation required
          // to transform the end point back to original world space
          // coordinates.
          //v3 = ( info[i].m_warp ) * v2 - ( info[i].v_warp );
          v3 = v2;

          // Use the original center of the light here because we need to check
          // for blocking things starting in the original sector before
          // any space warping occured.
          csPolygon3D* p;
	  csVector3 path_end = lview.beam2source * v3;
          rc = light->GetSector ()->BlockingThings (light->GetCenter (),
	    path_end, &p, light->GetSector () == polygon->GetSector ());
          if (!rc || p == polygon || p->GetParent () == polygon->GetParent ())
	  {
	    rc = false;
	    break;
	  }
	  if (!do_accurate_things) break;
	  rc = true;
        }

        if (!rc)
        {
          d = csSquaredDist::PointPoint (light->GetCenter (), v3);
          DB ((MSG_DEBUG_0, "    -> In viewing frustrum (distance %f compared with radius %f)\n", sqrt (d), light->GetRadius ()));

	  if (d >= light->GetSquaredRadius ()) continue;
	  d = sqrt (d);
          DB ((MSG_DEBUG_0, "    -> *** HIT ***\n"));

	  hit = true;

	  l1 = mapR[uv];

	  float cosinus = (v3-light->GetCenter ())*polygon->GetPolyPlane ()->Normal ();
	  cosinus /= d;
	  cosinus += cosfact;
	  if (cosinus < 0) cosinus = 0;
	  else if (cosinus > 1) cosinus = 1;
	  if (dyn)
	  {
	    dl = NORMAL_LIGHT_LEVEL/light->GetRadius ();
	    l1 = l1 + QRound (cosinus * (NORMAL_LIGHT_LEVEL - d*dl));
	    if (l1 > 255) l1 = 255;
	    mapR[uv] = l1;
	  }
	  else
	  {
	    if (lview.r > 0)
	    {
	      l1 = l1 + QRound (cosinus * r200d*(light->GetRadius () - d));
	      if (l1 > 255) l1 = 255;
	      mapR[uv] = l1;
	    }
	    if (lview.g > 0 && mapG)
	    {
	      l2 = mapG[uv] + QRound (cosinus * g200d*(light->GetRadius () - d));
	      if (l2 > 255) l2 = 255;
	      mapG[uv] = l2;
	    }
	    if (lview.b > 0 && mapB)
	    {
	      l3 = mapB[uv] + QRound (cosinus * b200d*(light->GetRadius () - d));
	      if (l3 > 255) l3 = 255;
	      mapB[uv] = l3;
	    }
	  }
        }
        //mapR[uv] = 128;
        //mapG[uv] = 128;
        //mapB[uv] = 128;
        //if (u == 0 && (v & 1)) { mapR[uv] = 255; mapG[uv] = 0; mapB[uv] = 0; }
        //else if (v == 0 && (u & 1)) { mapR[uv] = 0; mapG[uv] = 255; mapB[uv] = 0; }
        //else if (u == lw-1 && (v & 1)) { mapR[uv] = 0; mapG[uv] = 0; mapB[uv] = 255; }
        //else if (v == lh-1 && (u & 1)) { mapR[uv] = 255; mapG[uv] = 0; mapB[uv] = 255; }
        //else if (u == v) { mapR[uv] = 255; mapG[uv] = 255; mapB[uv] = 0; }
        //else if (u == lh-1-v) { mapR[uv] = 0; mapG[uv] = 255; mapB[uv] = 255; }
      }

      if(!sy) goto finish;
      sxL += dxL;
      sxR += dxR;
      sy--;
    }
  }

finish:

  if (f_uv) CHKB (delete [] f_uv);

  if (dyn && first_time)
  {
    if (!hit)
    {
      // There was no hit. Just remove the dynamic light map from the polygon
      // unless it was not allocated this turn.
      lm->DelShadowMap (smap);
    }
    else
    {
      // There was a hit. Register this polygon with the light.
      light->RegisterPolygon (polygon);
    }
  }
}

void csPolyTexture::ShineDynLightmap (csLightPatch* lp)
{
  int lw = (w>>mipmap_shift)+2;		// @@@ DON'T NEED TO GO TO 'W', 'W_ORIG' SHOULD BE SUFFICIENT!
  int lh = (h>>mipmap_shift)+2;

  int u, uv;

  int l1, l2 = 0, l3 = 0;
  float d;

  int ww, hh;
  txt_handle->GetMipMapDimensions (mipmap_level, ww, hh);

  csPolyPlane* pl = polygon->GetPlane ();
  float cosfact = polygon->GetCosinusFactor ();
  if (cosfact == -1) cosfact = cfg_cosinus_factor;

  // From: T = Mwt * (W - Vwt)
  // ===>
  // Mtw * T = W - Vwt
  // Mtw * T + Vwt = W
  csMatrix3 m_t2w = pl->m_world2tex.GetInverse();
  csVector3 vv = pl->v_world2tex;

  // From: Ax+By+Cz+D = 0
  // From: T = Mwt * (W - Vwt)
  // ===>
  // Mtw * T + Vwt = W
  // Get 'w' from 'u' and 'v' (using u,v,w plane).
  float A = pl->GetWorldPlane ().A ();
  float B = pl->GetWorldPlane ().B ();
  float C = pl->GetWorldPlane ().C ();
  float D = pl->GetWorldPlane ().D ();
  float txt_A = A*m_t2w.m11 + B*m_t2w.m21 + C*m_t2w.m31;
  float txt_B = A*m_t2w.m12 + B*m_t2w.m22 + C*m_t2w.m32;
  float txt_C = A*m_t2w.m13 + B*m_t2w.m23 + C*m_t2w.m33;
  float txt_D = A*pl->v_world2tex.x + B*pl->v_world2tex.y + C*pl->v_world2tex.z + D;

  csVector3 v1, v2, v3;

  int ru, rv;
  float invww, invhh;
  invww = 1. / (float)ww;
  invhh = 1. / (float)hh;

  csRGBLightMap& remap = lm->GetRealMap ();
  csDynLight* light = (csDynLight*)(lp->light);
  unsigned char* mapR = remap.mapR;
  unsigned char* mapG = remap.mapG;
  unsigned char* mapB = remap.mapB;

  int i;
  float miny = 1000000, maxy = -1000000;
  int MaxIndex = -1, MinIndex = -1;

  // Calculate the uv's for all points of the frustrum (the
  // frustrum is actually a clipped version of the polygon).
  csVector2* f_uv = NULL;
  if (lp->vertices)
  {
    int mi;
    CHK (f_uv = new csVector2 [lp->num_vertices]);
    for (i = 0 ; i < lp->num_vertices ; i++)
    {
      //if (lview.mirror) mi = lview.num_frustrum-i-1;
      //else mi = i;
      mi = i;

      // T = Mwt * (W - Vwt)
      //v1 = pl->m_world2tex * (lp->vertices[mi] + lp->center - pl->v_world2tex);
      v1 = pl->m_world2tex * (lp->vertices[mi] + light->GetCenter () - pl->v_world2tex);
      f_uv[i].x = (v1.x*ww-Imin_u) / mipmap_size;
      f_uv[i].y = (v1.y*hh-Imin_v) / mipmap_size;
      if (f_uv[i].y < miny) miny = f_uv[MinIndex = i].y;
      if (f_uv[i].y > maxy) maxy = f_uv[MaxIndex = i].y;
    }
  }

  //float r200d = lview.r * NORMAL_LIGHT_LEVEL / light->GetRadius ();
  //float g200d = lview.g * NORMAL_LIGHT_LEVEL / light->GetRadius ();
  //float b200d = lview.b * NORMAL_LIGHT_LEVEL / light->GetRadius ();
  float r = light->GetColor ().red;
  float g = light->GetColor ().green;
  float b = light->GetColor ().blue;
  float r200d = r * NORMAL_LIGHT_LEVEL / light->GetRadius ();
  float g200d = g * NORMAL_LIGHT_LEVEL / light->GetRadius ();
  float b200d = b * NORMAL_LIGHT_LEVEL / light->GetRadius ();

  int new_lw = lm->GetWidth ();

  int scanL1, scanL2, scanR1, scanR2;   // scan vertex left/right start/final
  float sxL, sxR, dxL, dxR;             // scanline X left/right and deltas
  int sy, fyL, fyR;                     // scanline Y, final Y left, final Y right
  int xL, xR;

  sxL = sxR = dxL = dxR = 0;            // avoid GCC warnings about "uninitialized variables"
  scanL2 = scanR2 = MaxIndex;
  sy = fyL = fyR = (QRound (f_uv[scanL2].y)>lh-1)?lh-1:QRound (f_uv[scanL2].y);

  for ( ; ; )
  {
    //-----
    // We have reached the next segment. Recalculate the slopes.
    //-----
    bool leave;
    do
    {
      leave = true;
      if (sy <= fyR)
      {
        // Check first if polygon has been finished
        if (scanR2 == MinIndex) goto finish;
        scanR1 = scanR2;
        scanR2 = (scanR2 + 1) % lp->num_vertices;

        fyR = QRound(f_uv[scanR2].y);
        float dyR = (f_uv[scanR1].y - f_uv[scanR2].y);
	sxR = f_uv[scanR1].x;
        if (dyR != 0)
        {
          dxR = (f_uv[scanR2].x - sxR) / dyR;
	  // horizontal pixel correction
          sxR += dxR * (f_uv[scanR1].y - ((float)sy));
        }
	else dxR = 0;
        leave = false;
      }
      if (sy <= fyL)
      {
        scanL1 = scanL2;
        scanL2 = (scanL2 - 1 + lp->num_vertices) % lp->num_vertices;

        fyL = QRound(f_uv[scanL2].y);
        float dyL = (f_uv[scanL1].y - f_uv[scanL2].y);
	sxL = f_uv[scanL1].x;
        if (dyL != 0)
        {
          dxL = (f_uv[scanL2].x - sxL) / dyL;
          // horizontal pixel correction
          sxL += dxL * (f_uv[scanL1].y - ((float)sy));
        }
	else dxL = 0;
        leave = false;
      }
    }
    while (!leave);

    // Find the trapezoid top (or bottom in inverted Y coordinates)
    int fin_y;
    if (fyL > fyR) fin_y = fyL;
    else fin_y = fyR;

    while (sy >= fin_y)
    {
      // Compute the rounded screen coordinates of horizontal strip
      xL = QRound (sxL)+1;
      xR = QRound (sxR);
      if (xR > xL) { int xswap = xR; xR = xL; xL = xswap; }

      if (xR < 0) xR = 0;
      if (xL > lw) xL = lw;

      for (u = xR; u < xL ; u++)
      {
        uv = sy*new_lw+u;

        ru = u  << mipmap_shift;
        rv = sy << mipmap_shift;

        v1.x = (float)(ru+Imin_u)*invww;
        v1.y = (float)(rv+Imin_v)*invhh;
        if (ABS (txt_C) < SMALL_EPSILON)
          v1.z = 0;
        else
          v1.z = - (txt_D + txt_A*v1.x + txt_B*v1.y) / txt_C;
        v2 = vv + m_t2w * v1;

        // Perform the transformation given by m_warp and v_warp.
        // This is the inverse portal warping transformation required
        // to transform the end point back to original world space
        // coordinates.
        //v3 = ( info[i].m_warp ) * v2 - ( info[i].v_warp );
        v3 = v2;

        // Use the original center of the light here because we need to check
        // for blocking things starting in the original sector before
        // any space warping occured.
        //csPolygon3D* p;
        //bool rc = light->GetSector ()->BlockingThings (light->GetCenter (), v3, &p,
      	  //light->GetSector () == polygon->GetSector ());

        //if (!rc || p == polygon || p->get_parent () == polygon->get_parent ())
        {
          d = csSquaredDist::PointPoint (light->GetCenter (), v3);

	  if (d >= light->GetSquaredRadius ()) continue;
	  d = sqrt (d);

	  float cosinus = (v3-light->GetCenter ())*polygon->GetPolyPlane ()->Normal ();
	  cosinus /= d;
	  cosinus += cosfact;
	  if (cosinus < 0) cosinus = 0;
	  else if (cosinus > 1) cosinus = 1;
	  if (r > 0)
	  {
	    l1 = QRound (cosinus*r200d*(light->GetRadius () - d));
	    if (l1)
	    {
	      l1 += mapR[uv];
	      if (l1 > 255) l1 = 255;
	      mapR[uv] = l1;
	    }
	  }
	  if (g > 0 && mapG)
	  {
	    l2 = QRound (cosinus*g200d*(light->GetRadius () - d));
	    if (l2)
	    {
	      l2 += mapG[uv];
	      if (l2 > 255) l2 = 255;
	      mapG[uv] = l2;
	    }
	  }
	  if (b > 0 && mapB)
	  {
	    l3 = QRound (cosinus*b200d*(light->GetRadius () - d));
	    if (l3)
	    {
	      l3 += mapB[uv];
	      if (l3 > 255) l3 = 255;
	      mapB[uv] = l3;
	    }
	  }
        }
      }

      if(!sy) goto finish;
      sxL += dxL;
      sxR += dxR;
      sy--;
    }
  }

finish:

  if (f_uv) CHKB (delete [] f_uv);
}

void csPolyTexture::CreateDirtyMatrix ()
{
  int dw = w/csPolyTexture::subtex_size + 1;
  int dh = h/csPolyTexture::subtex_size + 1;
  if (!dirty_matrix || dw != dirty_w || dh != dirty_h)
  {
    // Dirty matrix does not exist or the size is not correct
    if (dirty_matrix) { CHK (delete [] dirty_matrix); }
    dirty_w = dw;
    dirty_h = dh;
    dirty_size = dw*dh;
    CHK (dirty_matrix = new UByte [dirty_size]);
    MakeAllDirty ();
  }
}

void csPolyTexture::MakeAllDirty ()
{
  if (dirty_matrix)
  {
    memset (dirty_matrix, 1, dirty_size);
    dirty_cnt = dirty_size;
  }
}

//---------------------------------------------------------------------------
