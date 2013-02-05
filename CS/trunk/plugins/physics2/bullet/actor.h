/*
    Copyright (C) 2011-2012 Christian Van Brussel, Institute of Information
      and Communication Technologies, Electronics and Applied Mathematics
      at Universite catholique de Louvain, Belgium
      http://www.uclouvain.be/en-icteam.html

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
#ifndef __CS_BULLET_ACTOR_H__
#define __CS_BULLET_ACTOR_H__

#include "ivaria/collisions.h"

CS_PLUGIN_NAMESPACE_BEGIN (Bullet2)
{

class csActor : public virtual CS::Collisions::iActor
{
public:
  /// Take care of actor-specific stuff, before the simulation step
  virtual void UpdatePreStep (float delta) = 0;
  
  /// Take care of actor-specific stuff, after the simulation step
  virtual void UpdatePostStep (float delta) = 0;
};

}
CS_PLUGIN_NAMESPACE_END (Bullet2)

#endif // __CS_BULLET_ACTOR_H__
