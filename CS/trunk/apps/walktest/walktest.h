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

#ifndef MAIN_H
#define MAIN_H

#include <stdarg.h>
#include "csgeom/math2d.h"
#include "csgeom/math3d.h"
#include "cssys/common/sysdriv.h"
#include "csengine/colldet/collider.h"

class Polygon3D;
class WalkTest;
class LanguageLayer;
class csView;
class InfiniteMaze;
class csSoundBuffer;
class csWorld;
class csSprite2D;
class PhysicsLibrary;

// Several map modes.
#define MAP_OFF 0
#define MAP_OVERLAY 1
#define MAP_ON 2

///
struct csKeyMap
{
  csKeyMap* next, * prev;
  int key, shift, alt, ctrl;
  char* cmd;
  int need_status,is_on;
};

///
class WalkTest : public SysSystemDriver
{
public:
  /// The world file to load
  static char world_file[100];
  /// A script to execute at startup.
  char* auto_script;

  /// Player position, orientation, and velocity
  csVector3 pos;
  csMatrix3 orient;
  csVector3 velocity;

  /// Camera angles. X and Y are user controllable, Z is not.
  float angle_x,angle_y,angle_z;

  /// Colliders for "legs" and "body". Intersections are handled differently
  csCollider *legs;
  csCollider *body;

  /// Was player already spawned?..
  bool player_spawned;

  /**
   * If this flag is true we move in 3D (old system). Otherwise we move more like
   * common 3D games do.
   */
  static bool move_3d;

  /**
   * If true we show edges around all polygons (debugging).
   */
  bool do_edges;

  /// The world.
  csWorld* world;

  /// For scripting.
  LanguageLayer* layer;

  /// The view on the world.
  csView* view;

  /// A sprite to display the Crystal Space Logo
  csSprite2D* cslogo;

  /// Our infinite maze object if used.
  InfiniteMaze* infinite_maze;

  /// Some sounds.
  csSoundBuffer* wMissile_boom;
  csSoundBuffer* wMissile_whoosh;

  PhysicsLibrary *pl;

  /// Some flags.
  bool do_fps;
  bool do_stats;
  bool do_clear;
  bool do_show_coord;
  bool busy_perf_test;
  bool do_show_z;
  bool do_infinite;
  bool do_cd;
  bool do_freelook;
  bool do_gravity;
  bool do_light_frust;

  /// The selected light.
  csLight* selected_light;
  /// The selected polygon.
  csPolygon3D* selected_polygon;

  /// A WireFrame object for the map.
  csWireFrameCam* wf;
  /// Map mode.
  int map_mode;

  /// Timing.
  float timeFPS;

  bool pressed_walk;
  bool pressed_strafe;
  bool on_ground;
  bool inverse_mouse;

public:
  ///
  WalkTest ();

  ///
  ~WalkTest ();

  ///
  virtual void NextFrame (long elapsed_time, long current_time);
  ///
  void PrepareFrame (long elapsed_time, long current_time);
  ///
  void DrawFrame (long elapsed_time, long current_time);

  /// Override ParseArg to handle additional arguments
  virtual bool ParseArg (int argc, char* argv[], int& i);
  /// Override Help to show additional arguments help
  virtual void Help ();
  /// Override DemoWrite for nice demo messaging
  virtual void DemoWrite (const char* msg);

  /// Inits all the collision detection stuff
  virtual void InitWorld(csWorld* world, csCamera* /*camera*/);

  /// Destroys all the collision detection stuff
  virtual void EndWorld(void);

  /// Creates Colliders
  virtual void CreateColliders(void);

  /// Gravity correct movement function: strafe().
  void strafe(float speed,int keep_old);
  /// Gravity correct movement function: step().
  void step(float speed,int keep_old);
  /// Gravity correct movement function: rotate().
  void rotate(float speed,int keep_old);
  /// Gravity correct movement function: look().
  void look(float speed,int keep_old);

  /// Immediate gravity incorrect movement functions.
  void imm_forward (float speed, bool slow, bool fast);
  void imm_backward (float speed, bool slow, bool fast);
  void imm_left (float speed, bool slow, bool fast);
  void imm_right (float speed, bool slow, bool fast);
  void imm_up (float speed, bool slow, bool fast);
  void imm_down (float speed, bool slow, bool fast);
  void imm_rot_left_camera (float speed, bool slow, bool fast);
  void imm_rot_left_world (float speed, bool slow, bool fast);
  void imm_rot_right_camera (float speed, bool slow, bool fast);
  void imm_rot_right_world (float speed, bool slow, bool fast);
  void imm_rot_left_xaxis (float speed, bool slow, bool fast);
  void imm_rot_right_xaxis (float speed, bool slow, bool fast);
  void imm_rot_left_zaxis (float speed, bool slow, bool fast);
  void imm_rot_right_zaxis (float speed, bool slow, bool fast);

  ///
  void handle_key_forward (float speed, bool shift, bool alt, bool ctrl);
  ///
  void handle_key_backwards (float speed, bool shift, bool alt, bool ctrl);
  ///
  void handle_key_left (float speed, bool shift, bool alt, bool ctrl);
  ///
  void handle_key_right (float speed, bool shift, bool alt, bool ctrl);
  ///
  void handle_key_pgup (float, bool shift, bool alt, bool ctrl);
  ///
  void handle_key_pgdn (float, bool shift, bool alt, bool ctrl);
  ///
  void eatkeypress (int status, int key, bool shift, bool alt, bool ctrl);
};

extern csVector2 coord_check_vector;
extern WalkTest* Sys;

#define FRAME_WIDTH Sys->FrameWidth
#define FRAME_HEIGHT Sys->FrameHeight

extern void perf_test ();
extern void CaptureScreen ();
extern void free_keymap ();

#endif // MAIN_H
