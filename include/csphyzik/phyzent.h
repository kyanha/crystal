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

#ifndef ctPhysicalEntity_H
#define ctPhysicalEntity_H

#include "csphyzik/phyztype.h"
#include "csphyzik/force.h"
#include "csphyzik/linklist.h"
#include "csphyzik/refframe.h"
//#include <iostream>

// flags
#define CTF_NOREWIND 0x1

#define DEFAULT_ENTITY_MASS 10.0

#define PHYSICALENTITY_STATESIZE 12  // Rmatrix, x

class ctSolver;
class ctCollidingContact;

// parent class of all physical bodies ( or even some non-physical ones... )
class ctPhysicalEntity
{
public:
/*
**	Constructors/destructors/statics
*/
	// default refrence frame will be inertial frame
	ctPhysicalEntity();
	// use specified reference frame
	ctPhysicalEntity( ctReferenceFrame &ref );
	virtual ~ctPhysicalEntity();

/*
**	Member functions
*/

	// compute the derivitive of this bodies state vector ( position, velocity... )
	// from applied forces.  Just passes control to the ctSolver for this object.
	virtual void solve( real t );

	// set the class responsible for solving change in state wrt time
	void set_solver( ctSolver *pslv ){ solver = pslv; }
	

	//**********  ODE interface ****************
	// init values that will influence the calculation of the change in state
	virtual void init_state(){ F[0] = 0; F[1] = 0; F[2] = 0; T[0] = 0; T[1] = 0; T[2] = 0; }
	// return the size of this enities state vector
	virtual int get_state_size(){ return PHYSICALENTITY_STATESIZE; }// = 0;
	// add this bodies state to the state vector buffer passed in. 
	// increment state buffer to point after added state.  upload
	virtual int set_state( real *sa );
	// download state from buffer into this entity
	virtual int get_state( const real *sa );
	// add change in state vector over time to state buffer parameter.
	virtual int set_delta_state( real *state_array ); 

	// change orientation in radians
	virtual void rotate_around_line( ctVector3 &paxis, real ptheta );
	ctVector3 get_v(){ return v; }
  // virtual because v is calculated from P ( momentum ) in rigid bodies
	virtual void set_v( const ctVector3 &pv );
	void set_pos( real px, real py, real pz ){ ctVector3 poff( px, py, pz ); RF.set_offset( poff ); }
  virtual void set_angular_v( const ctVector3 &pw );
	ctVector3 get_angular_v(){ return w; }
	
	ctVector3 get_F(){ return F; }
	ctVector3 get_torque(){ return T; }
	void set_F( const ctVector3 &pF ){ F = pF; }
	void set_torque( const ctVector3 &pT ){ T = pT; }
	void add_force( ctForce *f ){ forces.add_link( f ); }
	void sum_force( const ctVector3 &f ){ F += f; }
	void sum_torque( const ctVector3 &t ){ T += t; }
	void remove_force( ctForce *f ){ forces.remove_link( f ); }

	// add this force to the list of forces that will be applied each frame
	virtual void apply_given_F( ctForce &frc );
	void print_force(){/*cout << "F: " << F*F << "\n";*/ }

  // collision routines
  // resolve collision with a body.  warning: original cont may be modified
  virtual void resolve_collision( ctCollidingContact *cont );
  // can use this to impart and impulse to this object.
  virtual void apply_impulse( ctVector3 impulse_point, ctVector3 impulse_vector );
  virtual real get_impulse_m(){ return DEFAULT_ENTITY_MASS; }
  virtual ctMatrix3 get_impulse_I_inv(){ return ctMatrix3( 1.0/get_impulse_m() ); }
//	bool uses_ODE(){ return use_ODE; }

	const ctMatrix3 &get_R(){ return RF.get_R(); }
	const ctMatrix3 &get_T(){ return RF.get_T(); }
#define get_pos get_org_world
	const ctVector3 &get_org_world(){ return RF.get_offset(); }
	const ctMatrix3 &get_world_to_this(){ return RF.get_parent_to_this(); }
	const ctMatrix3 &get_this_to_world(){ return RF.get_this_to_parent(); }
	void v_this_to_world( ctVector3 &pv ){ RF.this_to_world( pv ); }
	ctVector3 get_v_this_to_world( ctVector3 &pv ){ 
		ctVector3 pret = pv;  RF.this_to_world( pret ); return pret;  
	}
	
	ctReferenceFrame *get_RF(){ return &RF; }

  // flags
  unsigned long flags;

protected:

	ctVector3 v;
	ctVector3 w;


  //!me gotta try to garauntee that this is not modified in the middle
	// of an ODE run.......
	//bool use_ODE;
	
	// frame of reference. orientation and position are stored here
	ctReferenceFrame &RF;

	// object responsible for calculating the change in state wrt time 
	// uses forces list and current state to do this.
	ctSolver *solver;

	ctVector3 F;	// total added Force for a frame
	ctVector3 T;	// total Torque for a frame
	ctLinkList_ctForce forces;   // list of all forces affecting this object

};

class ctSimpleDynamicsSolver;

// solved with ctSimpleDynamicsSolver
// just a body affected by simple forces
class ctDynamicEntity : public ctPhysicalEntity
{
public:
	ctDynamicEntity();
	ctDynamicEntity( ctReferenceFrame &ref );
	virtual ~ctDynamicEntity();
	
//	virtual void solve( real t );
	virtual void apply_given_F( ctForce &frc );

	friend class ctSimpleDynamicsSolver;

	virtual void set_m( real pm );
	real get_m(){ return m; }
  virtual real get_impulse_m(){ return m; }

protected:

	real m;			// mass
};

#endif
