/*
    Copyright (C) 1998-2000 by Jorrit Tyberghein

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
#include "states.h"
#include "cssys/system.h"

States::States ()
{
  zone_dim = ZONE_DIM;
  new_zone_dim = ZONE_DIM;
  InitStates();
}

States::~States ()
{
}

void States::InitStates ()
{
  Init_game_cube();
  score = 0;
  cur_speed = MIN_SPEED;
  speed = 0;
  transition = false;
}

void States::Init_game_cube()
{
  for (int k = 0; k < ZONE_SAFETY + ZONE_HEIGHT + ZONE_SAFETY; k++)
    for (int j = 0; j < ZONE_SAFETY + zone_dim + ZONE_SAFETY; j++)
      for (int i = 0; i < ZONE_SAFETY + zone_dim + ZONE_SAFETY; i++)
        game_cube[i][j][k] =
	  i < ZONE_SAFETY || j < ZONE_SAFETY || k < ZONE_SAFETY ||
	  i >= ZONE_SAFETY + zone_dim || j >= ZONE_SAFETY + zone_dim ||
	  k >= ZONE_SAFETY + ZONE_HEIGHT;
}

void States::AddScore (int dscore)
{
  float bonus_fact = (cur_speed - MIN_SPEED) / (MAX_SPEED - MIN_SPEED);
  bonus_fact = bonus_fact * 2 + 1;
  score += (int)((float)dscore * bonus_fact);
}

void States::UpdateScore ()
{
  int increase = 0;
  for (int i = 0; i < ZONE_HEIGHT; i++)
    if (filled_planes[i]) increase++;
  AddScore (zone_dim * zone_dim * 10 * increase * increase);
}

void States::checkForPlane ()
{
  for (int z = 0; z < ZONE_HEIGHT; z++)
  {
    bool plane_hit = true;
    for (int x = 0; plane_hit && x < zone_dim; x++)
      for (int y = 0; y < zone_dim; y++)
	if (!get_cube (x, y, z))
	  { plane_hit = false; break; }
    filled_planes[z] = plane_hit;
    if (plane_hit)
    {
      // We've got at least one plane, switch to transition mode.
      transition = true;
      removePlane (z);
      move_down_todo = CUBE_DIM; // Distance cubes above will drop.
      UpdateScore ();
    }
  }
}

void States::removePlane (int z)
{
  for (int x = 0; x < zone_dim; x++)
    for (int y = 0; y < zone_dim; y++)
      set_cube (x, y, z, false);
}

bool States::CheckEmptyPlayArea ()
{
  for (int z = 0; z < ZONE_HEIGHT; z++)
    for (int x = 0; x < zone_dim; x++)
      for (int y = 0; y < zone_dim; y++)
        if (get_cube (x, y, z))
	  return false;
  return true;
}

// @@@ FIXME: Fails to take endian into account when transporting integers.
// FIXME: This code needs a major redesign and rewrite.
void States::EncodeStates ()
{
  sprintf ((char*)encodedData,"%f9,%f9,", speed, cur_speed);
  int i = strlen((char*)encodedData);
  // i is at the start of the ints.

  int tints[ST_NUM_INTS];
  tints[0] = score;
  tints[1] = cube_x;
  tints[2] = cube_y;
  tints[3] = cube_z;
  tints[4] = zone_dim;
  tints[5] = new_zone_dim;
  memcpy(encodedData + i, tints, ST_NUM_INTS * ST_SIZE_INT);  
  
  i += ST_NUM_INTS * ST_SIZE_INT + 1;
  encodedData[i] = ',';
  i++;
  // i here is at the bitset.
  
  csBitSet tempBitSet(ST_NUM_BITS);
  int pos = 0;
  for (int l = 0; l < ZONE_HEIGHT; l++)
    for (int k = 0; k < ZONE_DIM; k++)
      for (int j = 0; j < ZONE_DIM; j++, pos++)
	if (get_cube(j, k, l))
          tempBitSet.Set(pos);
  memcpy(encodedData + i, tempBitSet.GetBits(), tempBitSet.GetByteCount());  
}

// @@@ FIXME: Fails to take endian into account when transporting integers.
// FIXME: This code needs a major redesign and rewrite.
bool States::DecodeStates ()
{
  int i, numSections;
  // Find the floats.
  for(i = 0, numSections = 0; i < (int)ST_ENCODED_LENGTH; i++)
  {
    if (encodedData[i] == ',')
    {
      numSections++;
      // If there are 2 floats.
      if(numSections == ST_NUM_FLOATS)
        break;
    }
  }

  if (numSections != ST_NUM_FLOATS)
    return false;

  // copy the floats into temps.
  encodedData[i] = '\0';
  float tspeed, tcur_speed;
  if(sscanf((char*)encodedData,"%f,%f", &tspeed, &tcur_speed) != ST_NUM_FLOATS)
    return false;
    
  speed = tspeed;
  cur_speed = tcur_speed;
   
  encodedData[i] = ',';
  i++;

  int tints[ST_NUM_INTS];
  memcpy(tints, encodedData + i, ST_NUM_INTS * ST_SIZE_INT);
  score = tints[0];
  cube_x = tints[1];
  cube_y = tints[2];
  cube_z = tints[3];
  zone_dim = tints[4];
  new_zone_dim = tints[5];

  i += ST_NUM_INTS * ST_SIZE_INT + 2;
  // here i is at the start of the bitset.

  csBitSet tempBitSet(ST_NUM_BITS);
  memcpy(tempBitSet.GetBits(), encodedData + i, tempBitSet.GetByteCount());
  // i here is just after the ",".

  int pos = 0;
  for (int l = 0; l < ZONE_HEIGHT; l++)
    for (int k = 0; k < ZONE_DIM; k++)
      for (int j = 0; j < ZONE_DIM; j++, pos++)
        set_cube(j, k, l, tempBitSet.Get(pos));
  return true;
}

bool States::PrintData (const char* fileName) const
{
  printf("\n--------------------------------------------------\n"); 
  printf("%s\n",encodedData);
  printf("\n--------------------------------------------------\n");
  printf("%f,%f,%d,%d,%d,%d,%d,%d\n",
    speed, cur_speed, score, cube_x, cube_y, cube_z, zone_dim, new_zone_dim);
  for (int k = 0; k < ZONE_SAFETY + ZONE_HEIGHT + ZONE_SAFETY ; k++)
    for (int j = 0; j < ZONE_SAFETY + ZONE_DIM + ZONE_SAFETY ; j++)
      for (int i = 0; i < ZONE_SAFETY + ZONE_DIM + ZONE_SAFETY ; i++)
        putchar(game_cube[i][j][k] ? '1' : '0');
  putchar('\n');

  FILE* fd = fopen(fileName, "wb");
  if (!fd)
    return false;
  bool ok = (fwrite(encodedData, ST_ENCODED_LENGTH, 1, fd) == 1);
  fclose(fd);
  return ok;  
}
