/*
    Copyright (C) 2000 by Jorrit Tyberghein

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
#include "cssys/system.h"
#include "walktest/walktest.h"
#include "walktest/bot.h"
#include "walktest/infmaze.h"
#include "walktest/hugeroom.h"
#include "apps/support/command.h"
#include "cstools/simpcons.h"
#include "csengine/dumper.h"
#include "csengine/camera.h"
#include "csengine/octree.h"
#include "csengine/world.h"
#include "csengine/csview.h"
#include "csengine/wirefrm.h"
#include "csengine/cssprite.h"
#include "csengine/skeleton.h"
#include "csengine/triangle.h"
#include "csengine/polygon.h"
#include "csengine/light.h"
#include "csengine/sector.h"
#include "csengine/cspixmap.h"
#include "csengine/collider.h"
#include "csengine/particle.h"
#include "csutil/scanstr.h"
#include "csparser/impexp.h"
#include "csobject/dataobj.h"
#include "cssfxldr/common/snddata.h"
#include "csparser/snddatao.h"
#include "csparser/csloader.h"
#include "csparser/crossbld.h"
#include "csgeom/math3d.h"
#include "igraph3d.h"
#include "csengine/rapid.h"

extern WalkTest* Sys;

csKeyMap* mapping = NULL;

csSprite3D *FindNextClosestSprite(csSprite3D *baseSprite, csCamera *camera, csVector2 *screenCoord);

//===========================================================================
// Everything for key mapping and binding.
//===========================================================================

void map_key (const char* keyname, csKeyMap* map)
{
  map->shift = 0;
  map->alt = 0;
  map->ctrl = 0;
  map->need_status = 0;
  char* dash = strchr (keyname, '-');
  while (dash)
  {
    *dash = 0;
    if (!strcmp (keyname, "shift")) map->shift = 1;
    else if (!strcmp (keyname, "alt")) map->alt = 1;
    else if (!strcmp (keyname, "ctrl")) map->ctrl = 1;
    else if (!strcmp (keyname, "status")) map->need_status = 1;
    else Sys->Printf (MSG_CONSOLE, "Bad modifier '%s'!\n", keyname);

    *dash = '-';
    keyname = dash+1;
    dash = strchr (dash+1, '-');
  }

  if (!strcmp (keyname, "tab")) map->key = CSKEY_TAB;
  else if (!strcmp (keyname, "space")) map->key = ' ';
  else if (!strcmp (keyname, "esc")) map->key = CSKEY_ESC;
  else if (!strcmp (keyname, "enter")) map->key = CSKEY_ENTER;
  else if (!strcmp (keyname, "bs")) map->key = CSKEY_BACKSPACE;
  else if (!strcmp (keyname, "up")) map->key = CSKEY_UP;
  else if (!strcmp (keyname, "down")) map->key = CSKEY_DOWN;
  else if (!strcmp (keyname, "right")) map->key = CSKEY_RIGHT;
  else if (!strcmp (keyname, "left")) map->key = CSKEY_LEFT;
  else if (!strcmp (keyname, "pgup")) map->key = CSKEY_PGUP;
  else if (!strcmp (keyname, "pgdn")) map->key = CSKEY_PGDN;
  else if (!strcmp (keyname, "home")) map->key = CSKEY_HOME;
  else if (!strcmp (keyname, "end")) map->key = CSKEY_END;
  else if (!strcmp (keyname, "ins")) map->key = CSKEY_INS;
  else if (!strcmp (keyname, "del")) map->key = CSKEY_DEL;
  else if (!strcmp (keyname, "f1")) map->key = CSKEY_F1;
  else if (!strcmp (keyname, "f2")) map->key = CSKEY_F2;
  else if (!strcmp (keyname, "f3")) map->key = CSKEY_F3;
  else if (!strcmp (keyname, "f4")) map->key = CSKEY_F4;
  else if (!strcmp (keyname, "f5")) map->key = CSKEY_F5;
  else if (!strcmp (keyname, "f6")) map->key = CSKEY_F6;
  else if (!strcmp (keyname, "f7")) map->key = CSKEY_F7;
  else if (!strcmp (keyname, "f8")) map->key = CSKEY_F8;
  else if (!strcmp (keyname, "f9")) map->key = CSKEY_F9;
  else if (!strcmp (keyname, "f10")) map->key = CSKEY_F10;
  else if (!strcmp (keyname, "f11")) map->key = CSKEY_F11;
  else if (!strcmp (keyname, "f12")) map->key = CSKEY_F12;
  else if (*(keyname+1) != 0) Sys->Printf (MSG_CONSOLE, "Bad key '%s'!\n", keyname);
  else if ((*keyname >= 'A' && *keyname <= 'Z') || strchr ("!@#$%^&*()_+", *keyname))
  {
    map->shift = 1;
    map->key = *keyname;
  }
  else if (*keyname >= 'a' && *keyname <= 'z')
  {
    if (map->shift) map->key = (*keyname)+'A'-'a';
    else map->key = *keyname;
  }
  else map->key = *keyname;
}

char* keyname (csKeyMap* map)
{
  static char buf[100];
  buf[0] = 0;
  if (map->need_status) strcat (buf, "status-");
  if (map->shift) strcat (buf, "shift-");
  if (map->ctrl) strcat (buf, "ctrl-");
  if (map->alt) strcat (buf, "alt-");
  switch (map->key)
  {
    case CSKEY_TAB: strcat (buf, "tab"); break;
    case ' ': strcat (buf, "space"); break;
    case CSKEY_ESC: strcat (buf, "esc"); break;
    case CSKEY_ENTER: strcat (buf, "enter"); break;
    case CSKEY_BACKSPACE: strcat (buf, "bs"); break;
    case CSKEY_UP: strcat (buf, "up"); break;
    case CSKEY_DOWN: strcat (buf, "down"); break;
    case CSKEY_RIGHT: strcat (buf, "right"); break;
    case CSKEY_LEFT: strcat (buf, "left"); break;
    case CSKEY_PGUP: strcat (buf, "pgup"); break;
    case CSKEY_PGDN: strcat (buf, "pgdn"); break;
    case CSKEY_HOME: strcat (buf, "home"); break;
    case CSKEY_END: strcat (buf, "end"); break;
    case CSKEY_INS: strcat (buf, "ins"); break;
    case CSKEY_DEL: strcat (buf, "del"); break;
    case CSKEY_F1: strcat (buf, "f1"); break;
    case CSKEY_F2: strcat (buf, "f2"); break;
    case CSKEY_F3: strcat (buf, "f3"); break;
    case CSKEY_F4: strcat (buf, "f4"); break;
    case CSKEY_F5: strcat (buf, "f5"); break;
    case CSKEY_F6: strcat (buf, "f6"); break;
    case CSKEY_F7: strcat (buf, "f7"); break;
    case CSKEY_F8: strcat (buf, "f8"); break;
    case CSKEY_F9: strcat (buf, "f9"); break;
    case CSKEY_F10: strcat (buf, "f10"); break;
    case CSKEY_F11: strcat (buf, "f11"); break;
    case CSKEY_F12: strcat (buf, "f12"); break;
    default:
    {
      char* s = strchr (buf, 0);
      *s++ = map->key;
      *s = 0;
    }
  }
  return buf;
}

csKeyMap* find_mapping (const char* keyname)
{
  csKeyMap map;
  map_key (keyname, &map);

  csKeyMap* m = mapping;
  while (m)
  {
    if (map.key == m->key && map.shift == m->shift && map.ctrl == m->ctrl && map.alt == m->alt
    	&& map.need_status == m->need_status)
      return m;
    m = m->next;
  }
  return NULL;
}

void bind_key (const char* arg)
{
  if (!arg)
  {
    csKeyMap* map = mapping;
    while (map)
    {
      Sys->Printf (MSG_CONSOLE, "Key '%s' bound to '%s'.\n", keyname (map), map->cmd);
      map = map->next;
    }
    return;
  }
  char* space = strchr (arg, ' ');
  if (space)
  {
    *space = 0;
    csKeyMap* map = find_mapping (arg);
    if (map)
    {
      delete [] map->cmd;
    }
    else
    {
      map = new csKeyMap ();
      map->next = mapping;
      map->prev = NULL;
      if (mapping) mapping->prev = map;
      mapping = map;
      map_key (arg, map);
    }
    map->cmd = new char [strlen (space+1)+1];
    strcpy (map->cmd, space+1);
    *space = ' ';
  }
  else
  {
    csKeyMap* map = find_mapping (arg);
    if (map) Sys->Printf (MSG_CONSOLE, "Key bound to '%s'!\n", map->cmd);
    else Sys->Printf (MSG_CONSOLE, "Key not bound!\n");
  }
}

void free_keymap ()
{
  csKeyMap *prev, *cur = mapping;
  while (cur)
  {
    prev = cur;
    cur = cur->next;
    delete [] prev->cmd;
    delete prev;
  }
  mapping = NULL;
}

/*------------------------------------------------------------------
 * The following handle_key_... routines are general movement
 * routines that are called by do_update() for the new movement
 * system and by do_keypress() for the old movement system (see
 * system.h for an explanation of the difference between the two
 * systems).
 *------------------------------------------------------------------*/

extern csCamera c;
extern WalkTest* Sys;

void WalkTest::strafe (float speed,int keep_old)
{
  if (move_3d || map_mode) return;
  static bool pressed = false;
  static float strafe_speed = 0;
  static long start_time = Time ();

  long cur_time = Time ();
  if (!keep_old)
  {
    bool new_pressed = ABS (speed) > 0.001;
    if (new_pressed != pressed)
    {
      pressed = new_pressed;
      strafe_speed = speed * cfg_walk_accelerate;
      start_time = cur_time - 100;
    }
  }

  while ((cur_time - start_time) >= 100)
  {
    if (pressed)
    {
      // accelerate
      if (ABS (velocity.x) < cfg_walk_maxspeed)
        velocity.x += strafe_speed;
    }
    else
    {
      // brake!
      if (velocity.x > cfg_walk_brake)
        velocity.x -= cfg_walk_brake;
      else if (velocity.x < -cfg_walk_brake)
        velocity.x += cfg_walk_brake;
      else
        velocity.x = 0;
    }
    start_time += 100;
  }
}

void WalkTest::step (float speed,int keep_old)
{
  if (move_3d || map_mode) return;

  static bool pressed = false;
  static float step_speed = 0;
  static long start_time = Time ();

  long cur_time = Time ();
  if (!keep_old)
  {
    bool new_pressed = ABS (speed) > 0.001;
    if (new_pressed != pressed)
    {
      pressed = new_pressed;
      step_speed = speed * cfg_walk_accelerate;
      start_time = cur_time - 100;
    }
  }

  while ((cur_time - start_time) >= 100)
  {
    if (pressed)
    {
      // accelerate
      if (ABS (velocity.z) < cfg_walk_maxspeed)
        velocity.z += step_speed;
    }
    else
    {
      // brake!
      if (velocity.z > cfg_walk_brake)
        velocity.z -= cfg_walk_brake;
      else if (velocity.z < -cfg_walk_brake)
        velocity.z += cfg_walk_brake;
      else
        velocity.z = 0;
    }
    start_time += 100;
  }
}

void WalkTest::rotate (float speed,int keep_old)
{
  if (move_3d || map_mode) return;

  static bool pressed = false;
  static float angle_accel = 0;
  static long start_time = Time ();

  long cur_time = Time ();
  if (!keep_old)
  {
    bool new_pressed = ABS (speed) > 0.001;
    if (new_pressed != pressed)
    {
      pressed = new_pressed;
      angle_accel = speed * cfg_rotate_accelerate;
      start_time = cur_time - 100;
    }
  }

  while ((cur_time - start_time) >= 100)
  {
    if (pressed)
    {
      // accelerate rotation
      if (ABS (angle_velocity.y) < cfg_rotate_maxspeed)
        angle_velocity.y += angle_accel;
    }
    else
    {
      // brake!
      if (angle_velocity.y > cfg_rotate_brake)
        angle_velocity.y -= cfg_rotate_brake;
      else if (angle_velocity.y < -cfg_rotate_brake)
        angle_velocity.y += cfg_rotate_brake;
      else
        angle_velocity.y = 0;
    }
    start_time += 100;
  }
}

void WalkTest::look (float speed,int keep_old)
{
  if (move_3d || map_mode) return;
  static float step_speed = 0;
  if (!keep_old)
    step_speed = speed*cfg_look_accelerate;
  if (ABS (angle.x+step_speed) <= (355.0/113.0/4))
    angle.x += step_speed;
}

void WalkTest::imm_forward (float speed, bool slow, bool fast)
{
  if (map_mode) { wf->KeyUp (speed, slow, fast); return; }
  if (slow)
    view->GetCamera ()->Move (speed * 0.01 * VEC_FORWARD, do_cd);
  else if (fast)
    view->GetCamera ()->Move (speed * 4.0 * VEC_FORWARD, do_cd);
  else
    view->GetCamera ()->Move (speed * 1.0 * VEC_FORWARD, do_cd);
}

void WalkTest::imm_backward (float speed, bool slow, bool fast)
{
  if (map_mode) { wf->KeyDown (speed, slow, fast); return; }
  if (slow)
    view->GetCamera ()->Move (speed*.01*VEC_BACKWARD, do_cd);
  else if (fast)
    view->GetCamera ()->Move (speed*1.2*VEC_BACKWARD, do_cd);
  else
    view->GetCamera ()->Move (speed*.6*VEC_BACKWARD, do_cd);
}

void WalkTest::imm_left (float speed, bool slow, bool fast)
{
  if (map_mode) return;
  if (slow)
    view->GetCamera ()->Move (speed * 0.01 * VEC_LEFT, do_cd);
  else if (fast)
    view->GetCamera ()->Move (speed * 4.0 * VEC_LEFT, do_cd);
  else
    view->GetCamera ()->Move (speed * 1.0 * VEC_LEFT, do_cd);
}

void WalkTest::imm_right (float speed, bool slow, bool fast)
{
  if (map_mode) return;
  if (slow)
    view->GetCamera ()->Move (speed * 0.01 * VEC_RIGHT, do_cd);
  else if (fast)
    view->GetCamera ()->Move (speed * 4.0 * VEC_RIGHT, do_cd);
  else
    view->GetCamera ()->Move (speed * 1.0 * VEC_RIGHT, do_cd);
}

void WalkTest::imm_up (float speed, bool slow, bool fast)
{
  if (map_mode) return;
  if (slow)
    view->GetCamera ()->Move (speed * 0.01 * VEC_UP, do_cd);
  else if (fast)
    view->GetCamera ()->Move (speed * 4.0 * VEC_UP, do_cd);
  else
    view->GetCamera ()->Move (speed * 1.0 * VEC_UP, do_cd);
}

void WalkTest::imm_down (float speed, bool slow, bool fast)
{
  if (map_mode) return;
  if (slow)
    view->GetCamera ()->Move (speed * 0.01 * VEC_DOWN, do_cd);
  else if (fast)
    view->GetCamera ()->Move (speed * 4.0 * VEC_DOWN, do_cd);
  else
    view->GetCamera ()->Move (speed * 1.0 * VEC_DOWN, do_cd);
}

void WalkTest::imm_rot_left_camera (float speed, bool slow, bool fast)
{
  if (map_mode == MAP_TXT) { wf->KeyLeftStrafe (speed, slow, fast); return; }
  if (map_mode) { wf->KeyLeft (speed, slow, fast); return; }
  if (slow)
    view->GetCamera ()->Rotate (VEC_ROT_LEFT, speed * .005);
  else if (fast)
    view->GetCamera ()->Rotate (VEC_ROT_LEFT, speed * .4);
  else
    view->GetCamera ()->Rotate (VEC_ROT_LEFT, speed * .2);
}

void WalkTest::imm_rot_left_world (float speed, bool slow, bool fast)
{
  if (map_mode) return;
  if (slow)
    view->GetCamera ()->RotateWorld (VEC_ROT_LEFT, speed * .005);
  else if (fast)
    view->GetCamera ()->RotateWorld (VEC_ROT_LEFT, speed * .4);
  else
    view->GetCamera ()->RotateWorld (VEC_ROT_LEFT, speed * .2);
}

void WalkTest::imm_rot_right_camera (float speed, bool slow, bool fast)
{
  if (map_mode == MAP_TXT) { wf->KeyRightStrafe (speed, slow, fast); return; }
  if (map_mode) { wf->KeyRight (speed, slow, fast); return; }
  if (slow)
    view->GetCamera ()->Rotate (VEC_ROT_RIGHT, speed * .005);
  else if (fast)
    view->GetCamera ()->Rotate (VEC_ROT_RIGHT, speed * .4);
  else
    view->GetCamera ()->Rotate (VEC_ROT_RIGHT, speed * .2);
}

void WalkTest::imm_rot_right_world (float speed, bool slow, bool fast)
{
  if (map_mode) return;
  if (slow)
    view->GetCamera ()->RotateWorld (VEC_ROT_RIGHT, speed * .005);
  else if (fast)
    view->GetCamera ()->RotateWorld (VEC_ROT_RIGHT, speed * .4);
  else
    view->GetCamera ()->RotateWorld (VEC_ROT_RIGHT, speed * .2);
}

void WalkTest::imm_rot_left_xaxis (float speed, bool slow, bool fast)
{
  if (map_mode) { wf->KeyPgDn (speed, slow, fast); return; }
  if (slow)
    view->GetCamera ()->Rotate (VEC_TILT_DOWN, speed * .005);
  else if (fast)
    view->GetCamera ()->Rotate (VEC_TILT_DOWN, speed * .4);
  else
    view->GetCamera ()->Rotate (VEC_TILT_DOWN, speed * .2);
}

void WalkTest::imm_rot_right_xaxis (float speed, bool slow, bool fast)
{
  if (map_mode) { wf->KeyPgUp (speed, slow, fast); return; }
  if (slow)
    view->GetCamera ()->Rotate (VEC_TILT_UP, speed * .005);
  else if (fast)
    view->GetCamera ()->Rotate (VEC_TILT_UP, speed * .4);
  else
    view->GetCamera ()->Rotate (VEC_TILT_UP, speed * .2);
}

void WalkTest::imm_rot_left_zaxis (float speed, bool slow, bool fast)
{
  if (map_mode) return;
  if (slow)
    view->GetCamera ()->Rotate (VEC_TILT_LEFT, speed * .005);
  else if (fast)
    view->GetCamera ()->Rotate (VEC_TILT_LEFT, speed * .4);
  else
    view->GetCamera ()->Rotate (VEC_TILT_LEFT, speed * .2);
}

void WalkTest::imm_rot_right_zaxis (float speed, bool slow, bool fast)
{
  if (map_mode) return;
  if (slow)
    view->GetCamera ()->Rotate (VEC_TILT_RIGHT, speed * .005);
  else if (fast)
    view->GetCamera ()->Rotate (VEC_TILT_RIGHT, speed * .4);
  else
    view->GetCamera ()->Rotate (VEC_TILT_RIGHT, speed * .2);
}

void WalkTest::eatkeypress (int status, int key, bool shift, bool alt, bool ctrl)
{
  if (System->Console->IsActive () && status)
    ((csSimpleConsole *)System->Console)->AddChar (key);
  else switch (key)
  {
    case CSKEY_TAB:
      if (status)
        System->Console->Show ();
      break;

    default:
      {
        csKeyMap *m = mapping;
        while (m)
        {
          if (key == m->key && shift == m->shift
           && alt == m->alt && ctrl == m->ctrl)
          {
            if (m->need_status)
            {
              // Don't perform the command again if the key is already down
              if (m->is_on != status)
              {
                char buf [256];
                sprintf (buf,"%s %d", m->cmd, status);
                Command::perform_line (buf);
                m->is_on = status;
              }
            }
            else
            {
              if (status)
                Command::perform_line (m->cmd);
            }
          }
          else if (!status && key == m->key && m->is_on && m->need_status)
          {
            if (key == m->key || shift != m->shift || alt != m->alt || ctrl != m->ctrl)
            {
              char buf [256];
              sprintf (buf,"%s 0", m->cmd);
              Command::perform_line (buf);
              m->is_on = 0;
            }
          }
	  m = m->next;
        }
      }
      break;
  }
}


// left mouse button
void WalkTest::MouseClick1Handler(csEvent &Event)  
{
  (void)Event;
  move_forward = true;
}

// middle mouse button
void WalkTest::MouseClick2Handler(csEvent &Event)
{
  csVector3 v;
  v.z = System->G3D->GetZBuffValue(Event.Mouse.x, Event.Mouse.y);
  v.x = (Event.Mouse.x-FRAME_WIDTH/2) * v.z / view->GetCamera ()->GetFOV ();
  v.y = (FRAME_HEIGHT-1-Event.Mouse.y-FRAME_HEIGHT/2) * v.z / view->GetCamera ()->GetFOV ();
  csVector3 vw = view->GetCamera ()->Camera2World (v);

  Sys->Printf (MSG_CONSOLE, "LMB down : cam:(%f,%f,%f) world:(%f,%f,%f)\n", v.x, v.y, v.z, vw.x, vw.y, vw.z);
  Sys->Printf (MSG_DEBUG_0, "LMB down : cam:(%f,%f,%f) world:(%f,%f,%f)\n", v.x, v.y, v.z, vw.x, vw.y, vw.z);

  csSector* sector = view->GetCamera ()->GetSector ();
  csVector3 origin = view->GetCamera ()->GetW2CTranslation ();
  csVector3 isect;
  csPolygon3D* sel = sector->HitBeam (origin, origin + (vw-origin) * 10, isect);
  if (sel)
  {
    if (Sys->selected_polygon == sel) 
      Sys->selected_polygon = NULL;
    else 
      Sys->selected_polygon = sel;

    csPolygonSet* ps = (csPolygonSet*)(sel->GetParent ());
    Sys->Printf (MSG_DEBUG_0, "Hit polygon '%s/%s'\n", ps->GetName (), sel->GetName ());
    Dumper::dump (sel);
  }

  extern csVector2 coord_check_vector;
  coord_check_vector.x = Event.Mouse.x;
  coord_check_vector.y = FRAME_HEIGHT-Event.Mouse.y;
  extern bool check_light;
  extern void select_object (csRenderView* rview, int type, void* entity);
  check_light = true;
  view->GetWorld ()->DrawFunc (view->GetCamera (), view->GetClipper (), select_object);
}

// right mouse button
void WalkTest::MouseClick3Handler(csEvent &Event)  
{
  csVector2   screenPoint;
  csSprite3D *closestSprite;

  screenPoint.x = Event.Mouse.x;
  screenPoint.y = Event.Mouse.y;
  closestSprite = FindNextClosestSprite(NULL, view->GetCamera(), &screenPoint);
  if (closestSprite)
    Sys->Printf (MSG_CONSOLE, "Selected sprite %s\n", closestSprite->GetName ());
  else
    Sys->Printf (MSG_CONSOLE, "No sprite selected!\n");
}


bool WalkTest::HandleEvent (csEvent &Event)
{
  // First pass the event to all plugins
  if (SysSystemDriver::HandleEvent (Event))
    return true;

  switch (Event.Type)
  {
    case csevBroadcast:
    {
      if (Event.Command.Code == cscmdContextResize)
      {
	int w = G2D->GetWidth();
	int h = G2D->GetHeight();
	view->GetCamera ()->SetPerspectiveCenter (w/2, h/2);
	if (wf)
	  wf->GetCamera ()->SetPerspectiveCenter (w/2, h/2);
	Sys->FrameWidth = w;
	Sys->FrameHeight = h;
	break;
      }
    }
    case csevKeyDown:
      eatkeypress (1,Event.Key.Code,
        (Event.Key.Modifiers & CSMASK_SHIFT) != 0,
        (Event.Key.Modifiers & CSMASK_ALT) != 0,
        (Event.Key.Modifiers & CSMASK_CTRL) != 0);
      break;
    case csevKeyUp:
      eatkeypress (0,Event.Key.Code,
        (Event.Key.Modifiers & CSMASK_SHIFT) != 0,
        (Event.Key.Modifiers & CSMASK_ALT) != 0,
        (Event.Key.Modifiers & CSMASK_CTRL) != 0);
      break;
    case csevMouseDown:
      {
      if (Event.Mouse.Button == 1)
        MouseClick1Handler(Event);
      else if (Event.Mouse.Button == 2)
        MouseClick2Handler(Event);
      else if (Event.Mouse.Button == 3)
        MouseClick3Handler(Event);
      break;
      }

    case csevMouseMove:
      // additional command by Leslie Saputra -> freelook mode.
      {
        static bool first_time = true;
        if (do_freelook)
        {
          int last_x, last_y;
          last_x = Event.Mouse.x;
          last_y = Event.Mouse.y;

          System->G2D->SetMousePosition (FRAME_WIDTH / 2, FRAME_HEIGHT / 2);
          if (!first_time)
          {
          /*
            if(move_3d)
              view->GetCamera ()->Rotate (VEC_ROT_RIGHT, ((float)( last_x - (FRAME_WIDTH / 2) )) / (FRAME_WIDTH*2) );
            else
              view->GetCamera ()->RotateWorld (VEC_ROT_RIGHT, ((float)( last_x - (FRAME_WIDTH / 2) )) / (FRAME_WIDTH*2) );
            view->GetCamera ()->Rotate (VEC_TILT_UP, -((float)( last_y - (FRAME_HEIGHT / 2) )) / (FRAME_HEIGHT*2) );
          */

            this->angle.y+=((float)(last_x - (FRAME_WIDTH / 2) )) / (FRAME_WIDTH*2);
            this->angle.x+=((float)(last_y - (FRAME_HEIGHT / 2) )) / (FRAME_HEIGHT*2)*(1-2*(int)inverse_mouse);
          }
          else
            first_time = false;
        }
        else
          first_time = true;
      }
      break;
    case csevMouseUp:
      if (Event.Mouse.Button == 1)
        move_forward = false;
      break;
  }

  return false;
}

csSprite3D *FindNextClosestSprite(csSprite3D *baseSprite, csCamera *camera, csVector2 *screenCoord)
{
  int spriteIndex;
  float thisZLocation;
  float closestZLocation;
  csSprite3D *closestSprite;
  csSprite3D *nextSprite;
  csBox2 screenBoundingBox;

  if (baseSprite)
  {
    closestSprite = baseSprite;
    closestZLocation = baseSprite->GetScreenBoundingBox(*camera, screenBoundingBox);
    // if the baseSprite isn't in front of the camera, return
    if (closestZLocation < 0)
      return NULL;
  }
  else
  {
    closestSprite = NULL;
    closestZLocation = 32000;
  }

  // @@@ This routine ignores 2D sprites for the moment.
  for (spriteIndex = 0; spriteIndex < Sys->world->sprites.Length(); spriteIndex++)
  {
    csSprite* sp = (csSprite*)Sys->world->sprites[spriteIndex];
    if (sp->GetType () != csSprite3D::Type) continue;
    nextSprite = (csSprite3D*)sp;

//  Sys->Printf(MSG_CONSOLE, "Checking sprite %s\n", nextSprite->GetName ());
    if (nextSprite != baseSprite)
    {
      thisZLocation = nextSprite->GetScreenBoundingBox(*camera, screenBoundingBox);
      if ((thisZLocation > 0) && (thisZLocation < closestZLocation))
      {
        if (screenBoundingBox.In(screenCoord->x, screenCoord->y))
        {
          closestZLocation = thisZLocation;
          closestSprite = nextSprite;
        }
      }
    }
  }

  return closestSprite;
}
