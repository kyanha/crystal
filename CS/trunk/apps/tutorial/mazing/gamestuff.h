/*
    Copyright (C) 2006 by Jorrit Tyberghein

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

#ifndef __appmazing_maze_h
#define __appmazing_maze_h

class AppMazing;

// World defines.
#define ROOM_DIMENSION 10.0
#define MAZE_DIMENSION 3

// Adversary defines.
#define ADVERSARY_DIMENSION 1
#define ADVERSARY_MOVETIME 1.0

// Explosion defines.
#define EXPLOSION_EMITTIME 2000
#define EXPLOSION_PARTTIME 500
#define EXPLOSION_PARTVARTIME 100
#define EXPLOSION_TIME (EXPLOSION_PARTTIME+EXPLOSION_EMITTIME+EXPLOSION_PARTVARTIME)

// Laserbeam defines.
#define LASER_WIDTH 0.05
#define LASER_LENGTH 100.0
#define LASER_LIFETIME 300	// Milliseconds the laser will be visible.
#define LASER_FLICKTIME 20
#define LASER_OFFSET csVector3(0,-.5,0)

// Movement defines.
#define MOVE_DISTANCE 5.0	// In one keypress we will move this far.
#define STEP_DISTANCE 1.0	// Step size for collision detection along path.
#define MOVECAMERA_SPEED 5.0	// Speed with which we move the camera.
#define ROTATE_ANGLE 0.5	// How much we want to rotate in one keypress.
#define ROTATECAMERA_SPEED 3.0	// Speed to rotate with.
#define PLAYER_SIZE 2.0

#define PORTAL_UP 0
#define PORTAL_DOWN 1
#define PORTAL_LEFT 2
#define PORTAL_RIGHT 3
#define PORTAL_FRONT 4
#define PORTAL_BACK 5

class RoomCoordinate
{
public:
  int x, y, z;
  RoomCoordinate ()
  {
    x = y = z = 0;
  }
  RoomCoordinate (int x, int y, int z)
  {
    RoomCoordinate::x = x;
    RoomCoordinate::y = y;
    RoomCoordinate::z = z;
  }
  RoomCoordinate (const RoomCoordinate& rc)
  {
    x = rc.x;
    y = rc.y;
    z = rc.z;
  }
# ifdef CS_DEBUG
  static bool IsValid (int x, int y, int z)
  {
    return (x >= 0 && x < MAZE_DIMENSION &&
	    y >= 0 && y < MAZE_DIMENSION &&
	    z >= 0 && z < MAZE_DIMENSION);
  }
  bool IsValid () const
  {
    return IsValid (x, y, z);
  }
# endif
};

class Maze
{
private:
  AppMazing* app;
  iSector* rooms[MAZE_DIMENSION][MAZE_DIMENSION][MAZE_DIMENSION];
  // This array keeps a list of room coordinates that are reachable from
  // a given room.
  csArray<RoomCoordinate> connections[MAZE_DIMENSION][MAZE_DIMENSION]
  	[MAZE_DIMENSION];
  bool occupied[MAZE_DIMENSION][MAZE_DIMENSION][MAZE_DIMENSION];

public:
  Maze (AppMazing* app);

  iSector* GetSector (int x, int y, int z) const
  {
    return rooms[x][y][z];
  }
  iSector* GetSector (const RoomCoordinate& rc) const
  {
    return rooms[rc.x][rc.y][rc.z];
  }
  bool CreateSector (int x, int y, int z);

  const csArray<RoomCoordinate>& GetConnections (const RoomCoordinate& rc) const
  {
    return connections[rc.x][rc.y][rc.z];
  }

  void MakeConnection (const RoomCoordinate& from, const RoomCoordinate& to);

  void FreeSpace (const RoomCoordinate& rc);
  void OccupySpace (const RoomCoordinate& rc);
  bool IsSpaceFree (const RoomCoordinate& rc) const;

  bool CreateGeometry ();
  bool CreateWallOrPortal (iThingFactoryState* factory_state,
  	const csVector3& v1, const csVector3& v2,
  	const csVector3& v3, const csVector3& v4,
	bool do_portal,
	const RoomCoordinate& source,
	const RoomCoordinate& dest);
  bool CreateRoom (iMaterialWrapper* wall_material,
  	int x, int y, int z,
	char* portals);
  bool CreateLight (const csColor& color,
  	int x, int y, int z);
};

class Player
{
private:
  AppMazing* app;

  csRef<iCollider> player_collider;

  // For the movement system.
  csVector3 desired_location;
  csVector3 desired_lookat;
  float amount_moved;
  float amount_rotated;
  csVector3 start_location;
  csVector3 start_lookat;

public:
  Player (AppMazing* app);

  bool InitCollisionDetection ();

  void StartMovement (const csVector3& dir);
  void StartRotation (const csVector3& rot);
  void MoveAndRotateCamera (float elapsed_seconds);
};

class Adversary : public scfImplementationExt1<Adversary,
	csObject, scfFakeInterface<Adversary> >
{
private:
  csWeakRef<iMeshWrapper> mesh;
  RoomCoordinate current_location;
  AppMazing* app;

  bool moving;
  csVector3 start, end;
  float remaining_seconds;

public:
  SCF_INTERFACE(Adversary, 1, 0, 0);

  Adversary (AppMazing* app, iMeshWrapper* mesh, const RoomCoordinate& rc);
  virtual ~Adversary () {}

  void ThinkAndMove (float elapsed_seconds);

  iMeshWrapper* GetMesh () const { return mesh; }
};

class Explosion
{
private:
  csRef<iMeshWrapper> mesh;
  int timeleft;

public:
  Explosion (iMeshWrapper* mesh, int timeleft);

  bool Handle (csTicks elapsed_ticks);

  iMeshWrapper* GetMesh () const { return mesh; }
};

class Laser
{
private:
  AppMazing* app;

  /// Our laserbeam mesh.
  csRef<iMeshWrapper> laserbeam;
  // Lifetime of the laserbeam.
  int lasertime;
  // The current laserbeam coordinates.
  csVector3 laserstart, laserend;
  iSector* lasersector;

public:
  Laser (AppMazing* app);

  void SetMeshWrapper (iMeshWrapper* laser)
  {
    laserbeam = laser;
  }

  /// Start the laser.
  void Start ();
  /// Handle life time of the laser.
  void Handle (csTicks ticks);
  /// Check if the laser hits anything.
  void Check ();
};

#endif // __appmazing_maze_h

