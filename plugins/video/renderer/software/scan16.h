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

#ifndef SCANNER16_H
#define SCANNER16_H

/**
 * Scanline drawing routines for 16-bit mode.
 */
class Scan16
{
public:
  static UShort* pal_table;

  /// Draw one horizontal scanline (no texture mapping).
  static void draw_scanline_flat (int xx, unsigned char* d, unsigned long* z_buf,
  			  float inv_z, float u_div_z, float v_div_z);
  /// Draw one horizontal scanline (Z buffer and no texture mapping).
  static void draw_scanline_z_buf_flat (int xx, unsigned char* d, unsigned long* z_buf,
  			  float inv_z, float u_div_z, float v_div_z);
  /// Draw one horizontal scanline (no lighting).
  static void draw_scanline (int xx, unsigned char* d, unsigned long* z_buf,
  			  float inv_z, float u_div_z, float v_div_z);
  /// Draw one horizontal scanline (no lighting, private mode).
  static void draw_scanline_private (int xx, unsigned char* d, unsigned long* z_buf,
  			  float inv_z, float u_div_z, float v_div_z);
  /// Draw one horizontal scanline for fog.
  static void draw_scanline_fog_555 (int xx, unsigned char* d, unsigned long* z_buf,
  			  float inv_z, float u_div_z, float v_div_z);
  /// Draw one horizontal scanline for fog.
  static void draw_scanline_fog_565 (int xx, unsigned char* d, unsigned long* z_buf,
  			  float inv_z, float u_div_z, float v_div_z);
  /// Draw one horizontal scanline for fog assuming the camera is in fog.
  static void draw_scanline_fog_view_555 (int xx, unsigned char* d, unsigned long* z_buf,
  			  float inv_z, float u_div_z, float v_div_z);
  /// Draw one horizontal scanline for fog assuming the camera is in fog.
  static void draw_scanline_fog_view_565 (int xx, unsigned char* d, unsigned long* z_buf,
  			  float inv_z, float u_div_z, float v_div_z);
  /// Draw one horizontal scanline for fog in planed mode.
  static void draw_scanline_fog_plane_555 (int xx, unsigned char* d, unsigned long* z_buf,
  			  float inv_z, float u_div_z, float v_div_z);
  /// Draw one horizontal scanline for fog in planed mode.
  static void draw_scanline_fog_plane_565 (int xx, unsigned char* d, unsigned long* z_buf,
  			  float inv_z, float u_div_z, float v_div_z);
  /// Draw one horizontal scanline with z buffer update only (no read, only fill)
  static void draw_scanline_zfill_only (int xx, unsigned char* d, unsigned long* z_buf,
  			  float inv_z, float u_div_z, float v_div_z);
  /// Draw one horizontal scanline (lighting and alpha transparency).
  static void draw_scanline_map_alpha50 (int xx, unsigned char* d, unsigned long* z_buf,
			  float inv_z, float u_div_z, float v_div_z);
  /// Draw one horizontal scanline (lighting and alpha transparency). General case.
  static void draw_scanline_map_alpha_555 (int xx, unsigned char* d, unsigned long* z_buf,
			  float inv_z, float u_div_z, float v_div_z);
  /// Draw one horizontal scanline (lighting and alpha transparency). General case.
  static void draw_scanline_map_alpha_565 (int xx, unsigned char* d, unsigned long* z_buf,
			  float inv_z, float u_div_z, float v_div_z);
  /// Draw one horizontal scanline (lighting).
  static void draw_scanline_map (int xx, unsigned char* d, unsigned long* z_buf,
			  float inv_z, float u_div_z, float v_div_z);
  /// Draw one horizontal scanline (lighting and filtering).
  static void draw_scanline_map_filter (int xx, unsigned char* d, unsigned long* z_buf,
			  float inv_z, float u_div_z, float v_div_z);
  /// Draw one horizontal scanline (lighting and filtering). Second version.
  static void draw_scanline_map_filter2 (int xx, unsigned char* d, unsigned long* z_buf,
			  float inv_z, float u_div_z, float v_div_z);
#if 0
  /// Draw one horizontal scanline (transparent and no lighting).
  static void draw_scanline_transp (int xx, unsigned char* d, unsigned long* z_buf,
			  float inv_z, float u_div_z, float v_div_z);
#endif
  /// Draw one horizontal scanline (transparent with lighting).
  static void draw_scanline_transp_map (int xx, unsigned char* d, unsigned long* z_buf,
			  float inv_z, float u_div_z, float v_div_z);
  /// Draw one horizontal scanline (Z buffer and no lighting).
  static void draw_scanline_z_buf (int xx, unsigned char* d, unsigned long* z_buf,
			  float inv_z, float u_div_z, float v_div_z);
  /// Draw one horizontal scanline (Z buffer and lighting).
  static void draw_scanline_z_buf_map (int xx, unsigned char* d, unsigned long* z_buf,
			  float inv_z, float u_div_z, float v_div_z);
  /// Draw one horizontal scanline (no lighting, private mode).
  static void draw_scanline_z_buf_private (int xx, unsigned char* d, unsigned long* z_buf,
  			  float inv_z, float u_div_z, float v_div_z);
#if 0
  /// Draw one horizontal scanline (lighting and uniform lighting).
  static void draw_scanline_map_light (int xx, unsigned char* d, unsigned long* z_buf,
			  float inv_z, float u_div_z, float v_div_z);
  /// Draw one horizontal scanline (Z buffer, lighting and uniform lighting).
  static void draw_scanline_z_buf_map_light (int xx, unsigned char* d, unsigned long* z_buf,
			  float inv_z, float u_div_z, float v_div_z);
#endif

#ifdef DO_MMX
  /// Draw one horizontal scanline (lighting) using MMX
  static void mmx_draw_scanline_map (int xx, unsigned char* d, unsigned long* z_buf,
			  float inv_z, float u_div_z, float v_div_z);
  /// Draw one horizontal scanline (no lighting).
  static void mmx_draw_scanline (int xx, unsigned char* d, unsigned long* z_buf,
  			  float inv_z, float u_div_z, float v_div_z);
#endif

  /// Draw a perspective-incorrect texture mapped polygon scanline
  static void draw_pi_scanline (void *dest, int len, long *zbuff,
                                long u, long du, long v, long dv, long z, long dz,
                                unsigned char *bitmap, int bitmap_log2w);
  /// Draw a perspective-incorrect texture mapped polygon scanline. Z fill only
  static void draw_pi_scanline_zfill (void *dest, int len, long *zbuff,
                                long u, long du, long v, long dv, long z, long dz,
                                unsigned char *bitmap, int bitmap_log2w);
  /// Draw a perspective-incorrect texture mapped polygon scanline with gouraud shading. Z fill only
  static void draw_pi_scanline_gouraud_zfill_555 (void *dest, int len, long *zbuff,
                                long u, long du, long v, long dv, long z, long dz,
                                unsigned char *bitmap, int bitmap_log2w,
				long r, long g, long b, long dr, long dg, long db);
  /// Draw a perspective-incorrect texture mapped polygon scanline with gouraud shading. Z fill only
  static void draw_pi_scanline_gouraud_zfill_565 (void *dest, int len, long *zbuff,
                                long u, long du, long v, long dv, long z, long dz,
                                unsigned char *bitmap, int bitmap_log2w,
				long r, long g, long b, long dr, long dg, long db);
  /// Draw a perspective-incorrect texture mapped polygon scanline with gouraud shading.
  static void draw_pi_scanline_gouraud_555 (void *dest, int len, long *zbuff,
                                long u, long du, long v, long dv, long z, long dz,
                                unsigned char *bitmap, int bitmap_log2w,
				long r, long g, long b, long dr, long dg, long db);
  /// Draw a perspective-incorrect texture mapped polygon scanline with gouraud shading.
  static void draw_pi_scanline_gouraud_565 (void *dest, int len, long *zbuff,
                                long u, long du, long v, long dv, long z, long dz,
                                unsigned char *bitmap, int bitmap_log2w,
				long r, long g, long b, long dr, long dg, long db);
  /// Draw a perspective-incorrect transparent texture mapped polygon scanline with gouraud shading.
  static void draw_pi_scanline_transp_gouraud_555 (void *dest, int len, long *zbuff,
                                long u, long du, long v, long dv, long z, long dz,
                                unsigned char *bitmap, int bitmap_log2w,
				long r, long g, long b, long dr, long dg, long db);
  /// Draw a perspective-incorrect transparent texture mapped polygon scanline with gouraud shading.
  static void draw_pi_scanline_transp_gouraud_565 (void *dest, int len, long *zbuff,
                                long u, long du, long v, long dv, long z, long dz,
                                unsigned char *bitmap, int bitmap_log2w,
				long r, long g, long b, long dr, long dg, long db);
  /// Draw a flat shaded polygon scanline
  static void draw_pi_scanline_flat (void *dest, int len, long *zbuff,
                                long u, long du, long v, long dv, long z, long dz,
                                unsigned char *bitmap, int bitmap_log2w);
  /// Draw a flat shaded polygon scanline. Z fill only
  static void draw_pi_scanline_flat_zfill (void *dest, int len, long *zbuff,
                                long u, long du, long v, long dv, long z, long dz,
                                unsigned char *bitmap, int bitmap_log2w);
  /// Draw a flat shaded polygon scanline with gouraud shading. Z fill only
  static void draw_pi_scanline_flat_gouraud_zfill_555 (void *dest, int len, long *zbuff,
                                long u, long du, long v, long dv, long z, long dz,
                                unsigned char *bitmap, int bitmap_log2w,
				long r, long g, long b, long dr, long dg, long db);
  /// Draw a flat shaded polygon scanline with gouraud shading. Z fill only
  static void draw_pi_scanline_flat_gouraud_zfill_565 (void *dest, int len, long *zbuff,
                                long u, long du, long v, long dv, long z, long dz,
                                unsigned char *bitmap, int bitmap_log2w,
				long r, long g, long b, long dr, long dg, long db);
  /// Draw a flat shaded polygon scanline with gouraud shading.
  static void draw_pi_scanline_flat_gouraud_555 (void *dest, int len, long *zbuff,
                                long u, long du, long v, long dv, long z, long dz,
                                unsigned char *bitmap, int bitmap_log2w,
				long r, long g, long b, long dr, long dg, long db);
  /// Draw a flat shaded polygon scanline with gouraud shading.
  static void draw_pi_scanline_flat_gouraud_565 (void *dest, int len, long *zbuff,
                                long u, long du, long v, long dv, long z, long dz,
                                unsigned char *bitmap, int bitmap_log2w,
				long r, long g, long b, long dr, long dg, long db);
#ifdef DO_MMX
  /// Draw a perspective-incorrect texture mapped polygon scanline
  static void mmx_draw_pi_scanline (void *dest, int len, long *zbuff,
                                long u, long du, long v, long dv, long z, long dz,
                                unsigned char *bitmap, int bitmap_log2w);
#endif
};

#endif /*SCANNER16_H*/
