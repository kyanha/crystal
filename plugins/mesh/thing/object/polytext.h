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

#ifndef __CS_POLYTEXT_H__
#define __CS_POLYTEXT_H__

#include "csgeom/math3d.h"
#include "csutil/csvector.h"
#include "csutil/cscolor.h"
#include "imesh/thing/polygon.h"
#include "iengine/light.h"

struct iFrustumView;
struct iMaterialHandle;
struct iPolygon3D;
struct iLight;
struct csRGBpixel;
struct iFrustumView;
struct csLightingPolyTexQueue;
class csPolygon3D;
class csPolyTexture;
class csLightMap;
class csLightPatch;
class csFrustumContext;
class csLight;
class csMatrix3;
class csVector3;
class csVector2;
class csColor;

SCF_VERSION (csLightingPolyTexQueue, 0, 0, 1);

/**
 * This is user-data for iFrustumView for the lighting process.
 * It represents a queue holding references to csPolyTexture
 * for all polygons that were hit by a light during the lighting
 * process.
 */
struct csLightingPolyTexQueue : public iLightingProcessData
{
private:
  // Vector containing csPolygonTexture pointers.
  csVector polytxts;
  iLight* light;

public:
  csLightingPolyTexQueue (iLight* light);
  virtual ~csLightingPolyTexQueue ();

  /**
   * Add a csPolyTexture to the queue. Only call this when the
   * polytexture is not already there! A csPolyTexture should be
   * added to the queue when it gets a shadow_bitmap.
   */
  void AddPolyTexture (csPolyTexture* pt);

  /**
   * Update all lightmaps or shadowmaps mentioned in the queue.
   */
  void UpdateMaps (iLight* light, const csVector3& lightpos,
  	const csColor& lightcolor);

  /// Finalize lighting.
  virtual void FinalizeLighting ()
  {
    UpdateMaps (light, light->GetCenter (), light->GetColor ());
  }

  SCF_DECLARE_IBASE;
};

/**
 * This class represents a shadow-bitmap. It is used while calculating
 * lighting for one light on a polygon. First shadows are collected on
 * this bitmap. Later the bitmap will be used to update the lightmap.
 */
class csShadowBitmap
{
private:
  char* light;		// Light bitmap.
  char* shadow;		// Shadow bitmap.
  int lm_w, lm_h;	// Original lightmap size.
  int sb_w, sb_h;	// Shadow bitmap size.
  int quality;		// Quality factor.
  int cnt_unshadowed;	// Number of bits still unshadowed in 'shadow' array.
  int cnt_unlit;	// Number of bits still unlit in 'light' array.
  int default_light;	// Default value to use initially for light map.

private:
  /**
   * Get the lighting level of a point in the lightmap (using lightmap
   * coordinates). This will be a number between 1 and 0 with 1 meaning
   * fully lit and 0 meaning fully shadowed.
   */
  float GetLighting (int lm_u, int lm_v);

  /*
   * For csAntialiasedPolyFill().
   */
  static void LightPutPixel (int x, int y, float area, void *arg);
  static void LightDrawBox (int x, int y, int w, int h, void *arg);
  static void ShadowPutPixel (int x, int y, float area, void *arg);
  static void ShadowDrawBox (int x, int y, int w, int h, void *arg);
  void _LightPutPixel (int x, int y, float area);
  void _LightDrawBox (int x, int y, int w, int h);
  void _ShadowPutPixel (int x, int y, float area);
  void _ShadowDrawBox (int x, int y, int w, int h);

public:
  /**
   * Make a new shadow bitmap of the given lightmap size and quality.
   * Quality will be a number indicating how much we want to enhance
   * or reduce size of this bitmap compared to the lightmap size.
   * A quality of 0 means no change (i.e. the bitmap will hold as many
   * shadow-points as lumels). A quality of -1 means that for every 2x2
   * lumels there will be one shadow-point. A quality of 1 means that
   * one lumel corresponds with 2x2 shadow-points.
   */
  csShadowBitmap (int lm_w, int lm_h, int quality, int default_light);

  /**
   * Destroy the shadow bitmap.
   */
  ~csShadowBitmap () { delete[] shadow; delete[] light; }

  /**
   * Render a polygon on this bitmap. The coordinates of this
   * polygon are given in lightmap coordinates. WARNING the given polygon
   * will be modified by this function!
   * 'val' can be 0 or 1 and will be used to fill the polygon. To render
   * a shadow you would use 1 and for light you would use 0.
   */
  void RenderPolygon (csVector2* poly, int num_vertices, int val);

  /**
   * Set the entire area of this bitmap to either completely shadowed
   * (val==1) or fully lit (val==0).
   */
  void RenderTotal (int val);

  /**
   * Take a light and update the lightmap using the information in
   * this shadow-bitmap.
   * <ul>
   * <li>lightcell_shift is the shift to scale lightmap space to
   *     texture space (with texture space meaning 0 to real texture size).
   * <li>The shf_u, shf_v, mul_u, and mul_v fields define how to translate
   *     the previous texture space to uv space (where uv goes between
   *     0 and 1 for a single texture).
   * <li>m_t2w and v_t2w transform uv space to world space coordinates.
   * <li>light is the light and lightpos is the position of that light (which
   *     can be different from the position of the light given by 'light'
   *     itself because we can have space warping).
   * <li>lightcolor can also be different from the color of the light because
   *     it could in principle be modified by portals.
   * </ul>
   */
  void UpdateLightMap (csRGBpixel* lightmap,
	int lightcell_shift,
	float shf_u, float shf_v,
	float mul_u, float mul_v,
	const csMatrix3& m_t2w, const csVector3& v_t2w,
	iLight* light, const csVector3& lightpos,
	const csColor& lightcolor,
	csPolygon3D* poly,
	float cosfact);
  /**
   * Take a light and update the shadowmap using the information in
   * this shadow-bitmap.
   * <ul>
   * <li>lightcell_shift is the shift to scale lightmap space to
   *     texture space (with texture space meaning 0 to real texture size).
   * <li>The shf_u, shf_v, mul_u, and mul_v fields define how to translate
   *     the previous texture space to uv space (where uv goes between
   *     0 and 1 for a single texture).
   * <li>m_t2w and v_t2w transform uv space to world space coordinates.
   * <li>light is the light and lightpos is the position of that light (which
   *     can be different from the position of the light given by 'light'
   *     itself because we can have space warping).
   * </ul>
   * Function returns false if there was no light hitting the polygon.
   */
  bool UpdateShadowMap (unsigned char* shadowmap,
	int lightcell_shift,
	float shf_u, float shf_v,
	float mul_u, float mul_v,
	const csMatrix3& m_t2w, const csVector3& v_t2w,
	iLight* light, const csVector3& lightpos,
	csPolygon3D* poly,
	float cosfact);
  /**
   * Return true if this bitmap is fully shadowed.
   */
  bool IsFullyShadowed () const
  {
    return cnt_unshadowed == 0;
  }

  /**
   * Return true if this bitmap is fully unlit.
   */
  bool IsFullyUnlit () const
  {
    return cnt_unlit == sb_w * sb_h;
  }

  /**
   * Return true if this bitmap is fully lit.
   */
  bool IsFullyLit () const
  {
    return cnt_unlit == 0 && (cnt_unshadowed == sb_w * sb_h);
  }
};


/**
 * This class represents a lighted texture for a polygon.
 */
class csPolyTexture : public iPolygonTexture
{
  friend class csPolygon3D;
  friend class csPolygon2D;

private:
  /// The corresponding polygon.
  csPolygon3D* polygon;

  /// How to map the lightmap on the polygon.
  csLightMapMapping mapping;

  /// LightMap.
  csLightMap* lm;
  /**
   * Shadow-bitmap used while doing lighting.
   */
  csShadowBitmap* shadow_bitmap;

  /// Internally used by (software) texture cache
  void **cache_data;

  /**
   * Compared against csThing version to know whether lightmap needs updating.
   */
  uint32 light_version;

public:
  /**
   * Option variable: control how much the angle of the light with the polygon
   * it hits affects the final light value. Values ranges from -1 to 1.
   * With -1 the polygons will get no light at all. With 0 it will be perfect
   * cosine rule. With 1 the cosine is ignored and it will be like Crystal Space
   * was in the past. Note that changing this value at runtime only has an
   * effect on dynamic lights.
   */
  static float cfg_cosinus_factor;

  /// Constructor.
  csPolyTexture ();
  /// Destructor.
  virtual ~csPolyTexture ();

  /**
   * Set the corresponding polygon for this polytexture.
   */
  void SetPolygon (csPolygon3D* p);

  /**
   * Return the polygon corresponding to this texture
   */
  csPolygon3D *GetCSPolygon () { return polygon; }

  /**
   * Set the lightmap for this polytexture (and call IncRef()
   * on the lightmap). Can also be used to clear the reference
   * to the lightmap if 'lightmap' is NULL.
   */
  void SetLightMap (csLightMap* lightmap);

  /// Get the cslightmap, for engine internal use (users see GetLightMap below)
  csLightMap *GetCSLightMap() { return lm; }

  /**
   * Calculate the bounding box in (u,v) space for the lighted texture.
   */
  void CreateBoundingTextureBox ();

  /**
   * Initialize the lightmaps.
   */
  void InitLightMaps ();

  /**
   * Update the lightmap for the given light.
   * 'vis' will be false if the polygon is totally shadowed. In this
   * case we should use 'subpoly' to see where the shadow must go and
   * not the base polygon which this csPolyTexture points too.
   */
  void FillLightMap (iFrustumView* lview, csLightingPolyTexQueue* lptq,
  	bool vis, csPolygon3D* subpoly);

  /**
   * Update the lightmap of this polygon using the current shadow-bitmap
   * and the light information below.
   */
  void UpdateFromShadowBitmap (iLight* light, const csVector3& lightpos,
  	const csColor& lightcolor);

  /**
   * Update the real lightmap for a given csLightPatch
   * (used for a dynamic light).
   */
  void ShineDynLightMap (csLightPatch* lp);

  //--------------------- iPolygonTexture implementation ---------------------
  SCF_DECLARE_IBASE;
  virtual iMaterialHandle *GetMaterialHandle ();
  virtual const csLightMapMapping& GetMapping () const
  {
    return mapping;
  }

  /// Check if dynamic lighting information should be recalculated
  virtual bool DynamicLightsDirty ();
  /**
   * Recalculate all pseudo and real dynamic lights if the
   * texture is dirty. The function returns true if there
   * was a recalculation (then the texture needs to be removed
   * from the texture cache).
   */
  virtual bool RecalculateDynamicLights ();

  ///
  virtual iLightMap *GetLightMap ();
  /// Query the size of one light cell
  virtual int GetLightCellSize ();
  /// Query log2 (cell size)
  virtual int GetLightCellShift ();

  /// Get data used internally by texture cache
  virtual void *GetCacheData (int idx);
  /// Set data used internally by texture cache
  virtual void SetCacheData (int idx, void *d);
};

#endif // __CS_POLYTEXT_H__
