/*
    Dynamics/Kinematics modeling and simulation library.
    Copyright (C) 1999 by Michael Alexander Ewert

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

#ifndef CT_COLLIDING_H
#define CT_COLLIDING_H

#include "csphyzik/phyztype.h"

#define MIN_CONTACT MIN_REAL * 100

class ctPhysicalEntity;
class ctRigidBody;

class ctCollidingContact
{
public:

  ctCollidingContact() :
    contact_p(0), ea(0), eb(0), n(0)
  {
    body_b = NULL;
    restitution = 1.0;
    next = NULL;
  }

  // set body_b to NULL if it is an immovable object. e.g. the ground.
  ctReferenceFrameEntity *body_a;  // body a.  this. ignored in collisions
  ctReferenceFrameEntity *body_b;  // body b.  other body involved in the contact

  ctVector3 contact_p;  // point of contact in world coords
	
	ctVector3 ea;  // edge direction for a
	ctVector3 eb;  // edge direction for b

  //!me bad code: fix it.  Should be enumed types
	bool vf;  // true if it is a vertex-face collision. false if edge edge

  // pointed away from body b, towards body a
  ctVector3 n;   // collision normal

  // coefficent of restitution.  what % of v bounces back. +ve
  // should be a number between 0.0 and 1.0
  real restitution;  

  // link to next contact
  ctCollidingContact *next;

};


class ctContact
{
public:

  ctContact() :
    contact_p(0), ea(0), eb(0), n(0)
  {
    body_b = NULL;
    next = NULL;
  }

  // set body_b to NULL if it is an immovable object. e.g. the ground.
  ctRigidBody *body_a;  // body a.  this. ignored in collisions
  ctRigidBody *body_b;  // body b.  other body involved in the contact

  ctVector3 contact_p;  // point of contact in world coords
	
	ctVector3 ea;  // edge direction for a
	ctVector3 eb;  // edge direction for b

	bool vf;

  // pointed away from body b, towards body a
  ctVector3 n;   // collision normal

  // link to next contact
  ctContact *next;

};

#endif
