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
#include "csutil/garray.h"
#include "csgfx/rgbpixel.h"
#include "imesh/thing/lightmap.h"

class csPolyTexture;
class csThing;
class csPolygon3D;
class csDelayedLightingInfo;
class csObject;
struct iLight;
struct iCacheManager;
struct iFile;
struct iEngine;

typedef csDirtyAccessArray<csRGBpixel> csRGBMap;
typedef csDirtyAccessArray<unsigned char> csShadowMapHelper;

/// Shadow map.
class csShadowMap : public csShadowMapHelper
{
public:
  iLight *Light;
  csShadowMap *next;
  unsigned char max_shadow;

  csShadowMap ();
  ~csShadowMap ();
  void Alloc (iLight *l, int w, int h);
  void CalcMaxShadow();
};

/**
 * The static and all dynamic lightmaps for one polygon.
 */
class csLightMap
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

  /// Calculate and save max_static_color_values after static_lm is loaded.
  void CalcMaxStatic ();

  /**
   * Calculate the sizes of this lightmap.
   */
  void SetSize (int w, int h);

public:
  /// Option variable: shadow cell size
  static int lightcell_size;
  /// Log base 2 of lightcell_size
  static int lightcell_shift;
  /// Default.
  static int default_lightmap_cell_size;

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
  ~csLightMap ();

  /**
   * Rebuilds the lightmap for the poly from all the lightmaps + the ambient
   * light if specified.
   */
  bool UpdateRealLightMap (float dyn_ambient_r,
                           float dyn_ambient_g,
                           float dyn_ambient_b, 
                           bool dyn_dirty);

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

  /**
   * Create a ShadowMap for this LightMap.
   */
  csShadowMap* NewShadowMap (iLight* light, int w, int h);

  /**
   * Allocate the static csRGBMap.
   */
  void AllocStaticLM (int w, int h);

  /**
   * Find a ShadowMap for a specific pseudo-dynamic light.
   */
  csShadowMap* FindShadowMap (iLight* light);

  /**
   * Delete a ShadowMap.
   * It is ment for pseudo-dynamic lights that do not reach the polygon.
   * but this can only be seen after trying.
   */
  void DelShadowMap (csShadowMap* plm);

  /**
   * Read lightmap from a file. Return 0 if succesful and
   * otherwise a description of the error.
   */
  const char* ReadFromCache (iFile* file, int w, int h,
    csPolygon3D* poly, iEngine*);

  /**
   * Write lightmap to a file.
   */
  void Cache (iFile* file, csPolygon3D* poly, iEngine*);

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

  ///
  csRGBpixel *GetMapData ();
  ///
  int GetWidth () const { return lwidth; }
  ///
  int GetHeight () const { return lheight; }
  ///
  int GetRealWidth () const { return rwidth; }
  ///
  int GetRealHeight () const { return rheight; }
  /** 
   * calculate (if needed) and return mean lightmap color
   * Note: won't include true dynamic lights
   * until RecalculateDynamicLights() of the owning
   * csPolyTexture is called
   */
  void GetMeanLighting (int &r, int &g, int &b);
  /// Get size of one lightmap
  long GetSize () const { return lm_size; }
};

#endif // __CS_LIGHTMAP_H__
