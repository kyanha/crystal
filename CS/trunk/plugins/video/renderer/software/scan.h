/*
    Copyright (C) 1998 by Jorrit Tyberghein
    Written by Andrew Zabolotny

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

#ifndef __SCAN_H__
#define __SCAN_H__

#include "sysdef.h"
#include "cscom/com.h"

//---//---//---//---//---//---//---//---//---//---/ Forward declarations //---//

class csTexture;
class csTextureMMSoftware;
class csGraphics3DSoftware;
interface IPolygon3D;
interface IPolygonTexture;
typedef unsigned char RGB8map[256];	// do we need entire soft_txt.h?

//---//---//---//---//---//---//---//---//---//---//---//---// Constants //---//

#define INTER_MODE_SMART	0
#define INTER_MODE_STEP8	1
#define INTER_MODE_STEP16	2
#define INTER_MODE_STEP32	3

// Specifies boundary where texel filtering does not occur
#define BAILOUT_CONSTANT	32768

//---//---//---//---//---//---//---//---//---//---//---/ Precomputed data /---//

/*
 * Structure with precalculated data<p>
 * There is generally one global structure of this type.
 * However, to make assembler life easier we do it in a structure
 * rather than defining many static variables.<p>
 * <b>WARNING:</p> Do not forget to synchronise all changes
 * made to this structure with i386/scan.ash!
 */
struct csScanSetup
{
  /// Interpolation step for semi-perspective correct texture mapping
  int InterpolStep;
  /// Interpolation step (shift-value) for semi-perspective correct texture mapping
  int InterpolShift;
  /// Interpolation mode
  int InterpolMode;

  /// Fog color
  int FogR;
  int FogG;
  int FogB;
  /// Fog density
  unsigned long FogDensity;

  /// A pointer to the texture.
  csTextureMMSoftware *Texture;

  /// The lighted texture bitmap from the texture cache.
  unsigned char *tmap2;
  /// Width of the texture from the texture cache.
  int tw2;
  /// Height of the texture from the texture cache.
  int th2;
  /// Difference with u for untiled textures.
  float fdu;
  /// Difference with v for untiled textures.
  float fdv;
  /// Texture width in 16:16 fixed-point format - 1
  int tw2fp;
  /// Texture height in 16:16 fixed-point format - 1
  int th2fp;

  /// The unlighted texture bitmap.
  unsigned char *tmap;
  /// Width of unlighted texture.
  int tw;
  /// Height of unlighted texture.
  int th;

  /// SJI: dynamic lighting.
  unsigned char *LightTable;

  /**
   * The following fields are used by the polygon drawer
   * and contain information fot calculating the 1/z, u/z, and v/z linear
   * equations.
   */

  /// Difference value for every horizontal dx.
  float M;
  /// Difference value for every horizontal dx.
  float J1;
  /// Difference value for every horizontal dx.
  float K1;
  /// Difference for every 16 pixels.
  float dM;
  /// Difference for every 16 pixels.
  float dJ1;
  /// Difference for every 16 pixels.
  float dK1;

  /// Mean color value.
  int FlatColor;

  /// Alpha mask for 16-bit renderer.
  unsigned int AlphaMask;
  /// General alpha factor for 16-bit renderer (0 to 255).
  int AlphaFact;

  /// log2 (texture_u)
  int shf_u;

  /**
   * The following fields are only used when drawing
   * unlighted tiled textures (the draw_scanline_..._map_...
   * routines don't need them).
   */

  /// log2(texture_width)
  int shf_w;
  /// 1 << shf_w - 1
  int and_w;
  /// log2(texture_height)
  int shf_h;
  /// 1 << shf_h - 1
  int and_h;

  /// Local texture palette -> global palette conversion table
  unsigned char *PrivToGlobal;
  /// 8-bit to 16-bit conversion table
  unsigned short *PaletteTable;
  /// Set up by poly renderer to alpha blending table
  RGB8map *AlphaMap;
};

/// The only instance of this structure
extern csScanSetup Scan;

/// The interface definition for all draw_scanline_XXX routines
typedef void (*csDrawScanline)
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// The interface definition for all draw_pi_scanline_XXX routines
typedef void (*csDrawPIScanline)
  (void *dest, int len, unsigned long *zbuff, long u, long du, long v, long dv,
   unsigned long z, long dz, unsigned char *bitmap, int bitmap_log2w);
typedef void (*csDrawPIScanlineGouraud)
  (void *dest, int len, unsigned long *zbuff, long u, long du, long v, long dv,
   unsigned long z, long dz, unsigned char *bitmap, int bitmap_log2w,
   long r, long g, long b, long dr, long dg, long db);

//---//---//---//---//---//---//---//---//---//---//---//---//- Routines //---//

/// Initialize the scanline variables
extern "C" void csScan_InitDraw (csGraphics3DSoftware* g3d,
  IPolygonTexture* tex, csTextureMMSoftware* texture, csTexture* untxt);
/// Dump debugging information about last polygon
extern "C" void csScan_dump (csGraphics3DSoftware* pG3D);
/// Pixel-depth independent routine
extern "C" void csScan_draw_scanline_zfil
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);

//---//---//---//---//---//---//---//---//---//-- 8-bit drawing routines //---//

/// Draw one horizontal scanline with no texture mapping and fill Z buffer
extern "C" void csScan_8_draw_scanline_flat_zfil
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline with no texture mapping (but use Z buffer).
extern "C" void csScan_8_draw_scanline_flat_zuse
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline (no lighting).
extern "C" void csScan_8_draw_scanline_tex_zfil
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline (no lighting, private mode).
extern "C" void csScan_8_draw_scanline_tex_priv_zfil
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline (Z buffer and no lighting).
extern "C" void csScan_8_draw_scanline_tex_zuse
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline (Z buffer and no lighting, private mode).
extern "C" void csScan_8_draw_scanline_tex_priv_zuse
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline (lighting).
extern "C" void csScan_8_draw_scanline_map_zfil
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline (Z buffer and lighting).
extern "C" void csScan_8_draw_scanline_map_zuse
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline (lighting and filtering).
extern "C" void csScan_8_draw_scanline_map_filt_zfil
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline (transparent and no lighting).
extern "C" void csScan_8_draw_scanline_tex_key_zfil
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline (transparent and no lighting, private mode).
extern "C" void csScan_8_draw_scanline_tex_priv_key_zfil
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline (transparent with lighting).
extern "C" void csScan_8_draw_scanline_map_key_zfil
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);

/// Draw one horizontal scanline for fog.
extern "C" void csScan_8_draw_scanline_fog
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);

/// Draw one horizontal scanline (lighting and alpha transparency).
extern "C" void csScan_8_draw_scanline_map_alpha1
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline (lighting and alpha transparency).
extern "C" void csScan_8_draw_scanline_map_alpha2
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);

/// Draw one horizontal scanline (lighting and uniform lighting).
extern "C" void csScan_8_draw_scanline_map_light
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline (Z buffer, lighting and uniform lighting).
extern "C" void csScan_8_draw_scanline_map_light_zuse
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);

#ifdef DO_MMX
/// Draw one horizontal scanline (lighting) using MMX
extern "C" void csScan_8_mmx_draw_scanline_map_zfil
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline (no lighting) using MMX
extern "C" void csScan_8_mmx_draw_scanline_tex_zfil
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
#endif

/*
 * The following methods are used by DrawPolygonQuick() and do not require
 * perspective-correct texture mapping. They do not require InitDraw ()
 * to be called before using.
 */
/// Draw a perspective-incorrect texture mapped polygon scanline. Z fill only
extern "C" void csScan_8_draw_pi_scanline_tex_zfil
  (void *dest, int len, unsigned long *zbuff, long u, long du, long v, long dv,
   unsigned long z, long dz, unsigned char *bitmap, int bitmap_log2w);
/// Draw a perspective-incorrect texture mapped polygon scanline
extern "C" void csScan_8_draw_pi_scanline_tex_zuse
  (void *dest, int len, unsigned long *zbuff, long u, long du, long v, long dv,
   unsigned long z, long dz, unsigned char *bitmap, int bitmap_log2w);
#ifdef DO_MMX
/// Draw a perspective-incorrect texture mapped polygon scanline using MMX
extern "C" void csScan_8_mmx_draw_pi_scanline_tex_zuse
  (void *dest, int len, unsigned long *zbuff, long u, long du, long v, long dv,
   unsigned long z, long dz, unsigned char *bitmap, int bitmap_log2w);
#endif

//---//---//---//---//---//---//---//---//---//- 16-bit drawing routines //---//

/// Draw one horizontal scanline (no texture mapping).
extern "C" void csScan_16_draw_scanline_flat_zfil
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline (Z buffer and no texture mapping).
extern "C" void csScan_16_draw_scanline_flat_zuse
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline (no lighting).
extern "C" void csScan_16_draw_scanline_tex_zfil
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline (no lighting, private mode).
extern "C" void csScan_16_draw_scanline_tex_priv_zfil
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline (Z buffer and no lighting).
extern "C" void csScan_16_draw_scanline_tex_zuse
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline (no lighting, private mode).
extern "C" void csScan_16_draw_scanline_tex_priv_zuse
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline (lighting).
extern "C" void csScan_16_draw_scanline_map_zfil
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline (Z buffer and lighting).
extern "C" void csScan_16_draw_scanline_map_zuse
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline (lighting and filtering).
extern "C" void csScan_16_draw_scanline_map_filt_zfil
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline (lighting and more filtering).
extern "C" void csScan_16_draw_scanline_map_filt2_zfil
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline (transparent with lighting).
extern "C" void csScan_16_draw_scanline_map_key_zfil
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);

/// Draw one horizontal scanline for fog.
extern "C" void csScan_16_draw_scanline_fog_555
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline for fog.
extern "C" void csScan_16_draw_scanline_fog_565
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline for fog assuming the camera is in fog.
extern "C" void csScan_16_draw_scanline_fog_view_555
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline for fog assuming the camera is in fog.
extern "C" void csScan_16_draw_scanline_fog_view_565
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw a fogged horizontal scanline (no texture)
extern "C" void csScan_16_draw_scanline_fog_plane_555
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw a fogged horizontal scanline (no texture)
extern "C" void csScan_16_draw_scanline_fog_plane_565
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);

/// Draw one horizontal scanline (lighting and alpha transparency).
extern "C" void csScan_16_draw_scanline_map_alpha50
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline (lighting and alpha transparency). General case.
extern "C" void csScan_16_draw_scanline_map_alpha_555
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline (lighting and alpha transparency). General case.
extern "C" void csScan_16_draw_scanline_map_alpha_565
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);

#ifdef DO_MMX
/// Draw one horizontal scanline (lighting) using MMX
extern "C" void csScan_16_mmx_draw_scanline_map_zfil
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline (no lighting).
extern "C" void csScan_16_mmx_draw_scanline_tex_zfil
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
#endif

/// Draw a perspective-incorrect texture mapped polygon scanline. Z fill only
extern "C" void csScan_16_draw_pi_scanline_tex_zfil
  (void *dest, int len, unsigned long *zbuff, long u, long du, long v, long dv,
   unsigned long z, long dz, unsigned char *bitmap, int bitmap_log2w);
/// Draw a perspective-incorrect texture mapped polygon scanline
extern "C" void csScan_16_draw_pi_scanline_tex_zuse
  (void *dest, int len, unsigned long *zbuff, long u, long du, long v, long dv,
   unsigned long z, long dz, unsigned char *bitmap, int bitmap_log2w);
/// Draw a flat-lighted perspective-incorrect texture mapped polygon scanline
extern "C" void csScan_16_draw_pi_scanline_flat_zuse
  (void *dest, int len, unsigned long *zbuff, long u, long du, long v, long dv,
   unsigned long z, long dz, unsigned char *bitmap, int bitmap_log2w);
/// Draw a flat-lighted perspective-incorrect textured scanline with Z-fill
extern "C" void csScan_16_draw_pi_scanline_flat_zfil
  (void *dest, int len, unsigned long *zbuff, long u, long du, long v, long dv,
   unsigned long z, long dz, unsigned char *bitmap, int bitmap_log2w);
#ifdef DO_MMX
/// Draw a perspective-incorrect texture mapped polygon scanline
extern "C" void csScan_16_mmx_draw_pi_scanline_tex_zuse
  (void *dest, int len, unsigned long *zbuff, long u, long du, long v, long dv,
   unsigned long z, long dz, unsigned char *bitmap, int bitmap_log2w);
#endif

/// Draw a perspective-incorrect texture mapped polygon scanline with gouraud shading. Z fill only
extern "C" void csScan_16_draw_pi_scanline_tex_gouraud_zfil_555
  (void *dest, int len, unsigned long *zbuff, long u, long du, long v, long dv,
   unsigned long z, long dz, unsigned char *bitmap, int bitmap_log2w,
   long r, long g, long b, long dr, long dg, long db);
/// Draw a perspective-incorrect texture mapped polygon scanline with gouraud shading. Z fill only
extern "C" void csScan_16_draw_pi_scanline_tex_gouraud_zfil_565
  (void *dest, int len, unsigned long *zbuff, long u, long du, long v, long dv,
   unsigned long z, long dz, unsigned char *bitmap, int bitmap_log2w,
   long r, long g, long b, long dr, long dg, long db);
/// Draw a perspective-incorrect texture mapped polygon scanline with gouraud shading.
extern "C" void csScan_16_draw_pi_scanline_tex_gouraud_zuse_555
  (void *dest, int len, unsigned long *zbuff, long u, long du, long v, long dv,
   unsigned long z, long dz, unsigned char *bitmap, int bitmap_log2w,
   long r, long g, long b, long dr, long dg, long db);
/// Draw a perspective-incorrect texture mapped polygon scanline with gouraud shading.
extern "C" void csScan_16_draw_pi_scanline_tex_gouraud_zuse_565
  (void *dest, int len, unsigned long *zbuff, long u, long du, long v, long dv,
   unsigned long z, long dz, unsigned char *bitmap, int bitmap_log2w,
   long r, long g, long b, long dr, long dg, long db);

/// Draw a single-color Gouraud-shaded polygon in 5-5-5 pixel format with Z-fill
extern "C" void csScan_16_draw_pi_scanline_flat_gouraud_zfil_555
  (void *dest, int len, unsigned long *zbuff, long u, long du, long v, long dv,
   unsigned long z, long dz, unsigned char *bitmap, int bitmap_log2w,
   long r, long g, long b, long dr, long dg, long db);
/// Draw a single-color Gouraud-shaded polygon in 5-6-5 pixel format with Z-fill
extern "C" void csScan_16_draw_pi_scanline_flat_gouraud_zfil_565
  (void *dest, int len, unsigned long *zbuff, long u, long du, long v, long dv,
   unsigned long z, long dz, unsigned char *bitmap, int bitmap_log2w,
   long r, long g, long b, long dr, long dg, long db);
/// Draw a single-color Gouraud-shaded polygon in 5-5-5 pixel format
extern "C" void csScan_16_draw_pi_scanline_flat_gouraud_zuse_555
  (void *dest, int len, unsigned long *zbuff, long u, long du, long v, long dv,
   unsigned long z, long dz, unsigned char *bitmap, int bitmap_log2w,
   long r, long g, long b, long dr, long dg, long db);
/// Draw a single-color Gouraud-shaded polygon in 5-6-5 pixel format
extern "C" void csScan_16_draw_pi_scanline_flat_gouraud_zuse_565
  (void *dest, int len, unsigned long *zbuff, long u, long du, long v, long dv,
   unsigned long z, long dz, unsigned char *bitmap, int bitmap_log2w,
   long r, long g, long b, long dr, long dg, long db);

extern "C" void csScan_16_draw_pi_scanline_transp_gouraud_565
  (void *dest, int len, long *zbuff, long u, long du,
  long v, long dv, long z, long dz, unsigned char *bitmap, int bitmap_log2w,
  long r, long g, long b, long dr, long dg, long db);

extern "C" void csScan_16_draw_pi_scanline_transp_gouraud_555
  (void *dest, int len, long *zbuff, long u, long du,
  long v, long dv, long z, long dz, unsigned char *bitmap, int bitmap_log2w,
  long r, long g, long b, long dr, long dg, long db);

//---//---//---//---//---//---//---//---//---//- 32-bit scanline drawers //---//

/// Draw one horizontal scanline (no texture mapping).
extern "C" void csScan_32_draw_scanline_flat_zfil
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline (Z buffer and no texture mapping).
extern "C" void csScan_32_draw_scanline_flat_zuse
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline (no lighting).
extern "C" void csScan_32_draw_scanline_tex_zfil
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline (Z buffer and no lighting).
extern "C" void csScan_32_draw_scanline_tex_zuse
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline (lighting).
extern "C" void csScan_32_draw_scanline_map_zfil
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline (Z buffer and lighting).
extern "C" void csScan_32_draw_scanline_map_zuse
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);




/// Draw one horizontal scanline for fog.
extern "C" void csScan_32_draw_scanline_fog
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline for fog assuming the camera is in fog.
extern "C" void csScan_32_draw_scanline_fog_view
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw a fogged horizontal scanline (no texture)
extern "C" void csScan_32_draw_scanline_fog_plane
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);

/// Draw one horizontal scanline (lighting and alpha transparency).
extern "C" void csScan_32_draw_scanline_map_alpha50
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);
/// Draw one horizontal scanline (lighting and alpha transparency). General case.
extern "C" void csScan_32_draw_scanline_map_alpha
  (int xx, unsigned char* d, unsigned long* z_buf, float inv_z,
   float u_div_z, float v_div_z);

/// Draw a perspective-incorrect flat shaded polygon scanline with Z-fill
extern "C" void csScan_32_draw_pi_scanline_flat_zfil
  (void *dest, int len, unsigned long *zbuff, long u, long du, long v, long dv,
   unsigned long z, long dz, unsigned char *bitmap, int bitmap_log2w);
/// Draw a perspective-incorrect flat shaded polygon scanline
extern "C" void csScan_32_draw_pi_scanline_flat_zuse
  (void *dest, int len, unsigned long *zbuff, long u, long du, long v, long dv,
   unsigned long z, long dz, unsigned char *bitmap, int bitmap_log2w);
/// Draw a perspective-incorrect texture mapped polygon scanline. Z fill only
extern "C" void csScan_32_draw_pi_scanline_tex_zfil
  (void *dest, int len, unsigned long *zbuff, long u, long du, long v, long dv,
   unsigned long z, long dz, unsigned char *bitmap, int bitmap_log2w);
/// Draw a perspective-incorrect texture mapped polygon scanline
extern "C" void csScan_32_draw_pi_scanline_tex_zuse
  (void *dest, int len, unsigned long *zbuff, long u, long du, long v, long dv,
   unsigned long z, long dz, unsigned char *bitmap, int bitmap_log2w);

/// Draw a perspective-incorrect flat shaded polygon scanline with Gouraud and Z-fill
extern "C" void csScan_32_draw_pi_scanline_flat_gouraud_zfil
  (void *dest, int len, unsigned long *zbuff, long u, long du, long v, long dv,
   unsigned long z, long dz, unsigned char *bitmap, int bitmap_log2w,
   long r, long g, long b, long dr, long dg, long db);
/// Draw a perspective-incorrect flat shaded polygon scanline with Gouraud shading
extern "C" void csScan_32_draw_pi_scanline_flat_gouraud_zuse
  (void *dest, int len, unsigned long *zbuff, long u, long du, long v, long dv,
   unsigned long z, long dz, unsigned char *bitmap, int bitmap_log2w,
   long r, long g, long b, long dr, long dg, long db);
/// Draw a perspective-incorrect texture mapped polygon scanline with gouraud shading.
extern "C" void csScan_32_draw_pi_scanline_tex_gouraud_zfil
  (void *dest, int len, unsigned long *zbuff, long u, long du, long v, long dv,
   unsigned long z, long dz, unsigned char *bitmap, int bitmap_log2w,
   long r, long g, long b, long dr, long dg, long db);
/// Draw a perspective-incorrect texture mapped polygon scanline with gouraud shading. Z fill only
extern "C" void csScan_32_draw_pi_scanline_tex_gouraud_zuse
  (void *dest, int len, unsigned long *zbuff, long u, long du, long v, long dv,
   unsigned long z, long dz, unsigned char *bitmap, int bitmap_log2w,
   long r, long g, long b, long dr, long dg, long db);

#endif // __SCAN_H__
