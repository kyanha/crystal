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

#ifndef __CS_LIGHTMAP_H__
#define __CS_LIGHTMAP_H__

#include "csutil/scf.h"
#include "csutil/sarray.h"
#include "csgfx/rgbpixel.h"
#include "imesh/thing/lightmap.h"

class csPolyTexture;
class csThing;
class csPolygon3D;
class csCurve;
class csLight;
class csEngine;
class csDelayedLightingInfo;
class csObject;
struct iLight;
struct iCacheManager;
struct iFile;

CS_DECLARE_STATIC_ARRAY (csRGBMap, csRGBpixel);
CS_DECLARE_STATIC_ARRAY (csShadowMapHelper, unsigned char);

/// Shadow map.
class csShadowMap : public csShadowMapHelper
{
public:
  iLight *Light;
  csShadowMap *next;
  unsigned char max_shadow;

  csShadowMap ();
  virtual ~csShadowMap ();
  void Alloc (iLight *l, int w, int h);
  void Copy (const csShadowMap *other);
  void CalcMaxShadow();
};

/**
 * The static and all dynamic lightmaps for one polygon.
 */
class csLightMap : public iLightMap
{
  ///
  friend class csPolyTexture;

private:
  /**
   * A color lightmap containing all static lighting information
   * for the static lights (no pseudo-dynamic lights are here).
   */
  csRGBMap static_lm;

  /**
   * Stores the maximum r, g, b values for the static_lm array above.
   * This is used in lightmap merge optimizations.
   */
  csRGBpixel max_static_color_values;

  /**
   * The final lightmap that is going to be used for rendering.
   * In many cases this is just a copy of static_lm. But it may
   * contain extra lighting information obtained from all the
   * pseudo-dynamic shadowmaps and also the true dynamic lights.
   */
  csRGBMap real_lm;

  /**
   * A flag indicating whether the lightmaps needs recalculating
   * for dynamic lights
   */
  bool dyn_dirty;

  /**
   * Linked list of shadow-maps (for the pseudo-dynamic lights).
   * These shadowmaps are applied to static_lm to get real_lm.
   */
  csShadowMap* first_smap;

  /// Size of the lightmap.
  long lm_size;

  /// LightMap dims (possibly po2 depending on used 3D driver).
  int lwidth, lheight;
  /// Original lightmap dims (non-po2 possibly).
  int rwidth, rheight;

  /**
   * Mean lighting value of this lightmap.
   */
  csRGBpixel mean_color;

  /// need recalculation of mean color?
  bool mean_recalc;

  /// The hicolor cache ptr.
  void *cachedata;

  /// Used when computing lightmaps shared by several polygons
  csDelayedLightingInfo *delayed_light_info;

  /// Calculate and save max_static_color_values after static_lm is loaded.
  void CalcMaxStatic ();

#if 0
  /**
   * Convert three lightmap tables to the right mixing mode.
   * This function assumes that mixing mode is one of FAST_Wxx
   * and will not function correctly if called with NOCOLOR, TRUE_RGB
   * or FAST_RGB.<br>
   * This function correctly recognizes a dynamic lightmap which only
   * contains shadow information and does not do the conversion in that
   * case.
   */
  void ConvertToMixingMode (unsigned char* mr, unsigned char* mg,
			    unsigned char* mb, int sz);
#endif

  /**
   * Calculate the sizes of this lightmap.
   */
  void SetSize (int w, int h);

public:
  /// Option variable: shadow cell size
  static int lightcell_size;
  /// Log base 2 of lightcell_size
  static int lightcell_shift;

  /// Return the width of a lightmap given a texture size.
  static int CalcLightMapWidth (int w)
  {
    return 1 + ((w + lightcell_size - 1) >> lightcell_shift);
    //return 1 + ((w + lightcell_size) >> lightcell_shift); //@@@ EXP
  }
  /// Return the height of a lightmap given a texture size.
  static int CalcLightMapHeight (int h)
  {
    return 1 + ((h + lightcell_size - 1) >> lightcell_shift);
    //return 1 + ((h + lightcell_size) >> lightcell_shift); //@@@ EXP
  }

  ///
  csLightMap ();
  ///
  virtual ~csLightMap ();

  /// set the dirty flag for this lightmap
  void MakeDirtyDynamicLights () { dyn_dirty = true; mean_recalc = true; }

  /// Rebuilds the lightmap for the poly from all the lightmaps + the ambient light if specified
  bool UpdateRealLightMap (float dyn_ambient_r=0,
                           float dyn_ambient_g=0,
                           float dyn_ambient_b=0, 
                           bool  amb_dirty=false);

  ///
  csRGBMap& GetStaticMap () { return static_lm; }
  ///
  csRGBMap& GetRealMap () { return real_lm; }

  /**
   * Allocate the lightmap. 'w' and 'h' are the size of the
   * bounding box in lightmap space.
   * r,g,b is the ambient light color used to initialize the lightmap.
   */
  void Alloc (int w, int h, int r, int g, int b);

  /// Copy a lightmap.
  void CopyLightMap (csLightMap* source);

  /**
   * Create a ShadowMap for this LightMap.
   */
  csShadowMap* NewShadowMap (csLight* light, int w, int h);

  /**
   * Allocate the static csRGBMap.
   */
  void AllocStaticLM (int w, int h);

  /**
   * Find a ShadowMap for a specific pseudo-dynamic light.
   */
  csShadowMap* FindShadowMap (csLight* light);

  /**
   * Delete a ShadowMap. NOTE!!! This function only works
   * if the ShadowMap was the LAST CREATED for this LightMap!!!
   * It is ment for dynamic lights that do not reach the polygon
   * but this can only be seen after trying.
   */
  void DelShadowMap (csShadowMap* plm);

  /**
   * Read lightmap from a file. Return true if succesful.
   */
  bool ReadFromCache (iFile* file, int w, int h,
    csObject* obj, bool isPolygon, csEngine*);

  /**
   * Write lightmap to a file.
   */
  void Cache (iFile* file, csPolygon3D* poly,
  	csCurve* curve, csEngine*);

  /**
   * Convert the lightmaps to the correct mixing mode.
   * This function does nothing unless the mixing mode is
   * nocolor.
   */
  void ConvertToMixingMode ();

  /**
   * Convert the lightmaps to a 3D driver dependent size.
   */
  void ConvertFor3dDriver (bool requirePO2, int maxAspect = 32767);

  /**
   * Set the size of one lightmap cell (default = 16).
   * Do not directly assign to the lightcell_size variable, as
   * lightmap_shift also has to be updated.
   */
  static void SetLightCellSize (int size);

  /**
   * Calculate the mean color of csRGBpixel array.
   * Used by GetMeanLighting().
   */
  void CalcMeanLighting ();

  //------------------------ iLightMap implementation ------------------------
  SCF_DECLARE_IBASE;
  ///
  virtual csRGBpixel *GetMapData ();
  ///
  virtual int GetWidth ()
  { return lwidth; }
  ///
  virtual int GetHeight ()
  { return lheight; }
  ///
  virtual int GetRealWidth ()
  { return rwidth; }
  ///
  virtual int GetRealHeight ()
  { return rheight; }
  ///
  virtual void *GetCacheData ()
  { return cachedata; }
  ///
  virtual void SetCacheData (void *d)
  { cachedata = d; }
  /** 
   * calculate (if needed) and return mean lightmap color
   * Note: won't include true dynamic lights
   * until RecalculateDynamicLights() of the owning
   * csPolyTexture is called
   */
  virtual void GetMeanLighting (int &r, int &g, int &b);
  /// Get size of one lightmap
  virtual long GetSize ()
  { return lm_size; }
};

#endif // __CS_LIGHTMAP_H__
