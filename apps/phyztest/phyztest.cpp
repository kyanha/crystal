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

#include "cssysdef.h"
#include "cssys/system.h"
#include "apps/phyztest/phyztest.h"
#include "csengine/sector.h"
#include "csengine/world.h"
#include "csengine/csview.h"
#include "csengine/camera.h"
#include "csengine/light.h"
#include "csengine/polygon.h"
#include "csengine/collider.h"
#include "csparser/csloader.h"
#include "igraph3d.h"
#include "itxtmgr.h"

#include "csphyzik/phyziks.h"
#include "csgeom/math3d.h"
#include "csengine/cssprite.h"
#include "cstso.h"

// PHYZTEST DEMO
// hit del key to create a swinging chain
// hit tab key for a mass on a spring

Phyztest *Sys;


//------------------------------------------------- We need the 3D engine -----

REGISTER_STATIC_LIBRARY (engine)

//-----------------------------------------------------------------------------

// The physics world.  Main object for physics stuff
ctWorld phyz_world;

// data for mass on spring demo
csSprite3D *bot = NULL;
ctRigidBody *rb_bot = NULL;
  
// data for swinging chain demo
bool chain_added = false;

// holds all data needed to represent a swinging chain
class ChainLink
{
public:
  ChainLink( csSprite3D *psprt, ctRigidBody *prb, ctArticulatedBody *pab )
  {
    sprt = psprt;
    rb = prb;
    ab = pab;   
  }

  csSprite3D *sprt;         // sprite that represents a link
  ctRigidBody *rb;          // rigidbody object for this link
  ctArticulatedBody * ab;   // articulated body ( jointed body ) for this link
};

// increasing to more will cause instability for low frame-rates
#define NUM_LINKS 5

// all the links of the chain
ChainLink *chain[NUM_LINKS];

// create a rigidbody at pos with some preset mass and Inertia tensor
ctRigidBody *add_test_body( ctVector3 ppos )
{

  ctRigidBody *arb;

  arb = ctRigidBody::new_ctRigidBody();
  arb->set_m( 2.0 );
  arb->set_pos( ctVector3(ppos[0],ppos[1],ppos[2]) );
  arb->calc_simple_I_tensor( .1,0.2,.1 );
  return arb;
}

csSprite3D *add_test_sprite( csSpriteTemplate *tmpl, csSector *aroom, csView *view )
{
  csSprite3D *tsprt;
  
  tsprt = new csSprite3D(view->GetWorld ());
  tsprt->SetTemplate( tmpl );
  view->GetWorld ()->sprites.Push (tsprt);
  tsprt->GetMovable ().SetSector (aroom);
  csMatrix3 m; m.Identity ();
  tsprt->GetMovable ().SetTransform (m);
  tsprt->GetMovable ().SetPosition (csVector3( 0, 10, 0 ));    // only matters for root in chain demo
  tsprt->GetMovable ().UpdateMove ();
  tsprt->SetAction ("default");
  tsprt->InitSprite ();

  return tsprt;
}

//-------------- Phyztest

Phyztest::Phyztest ()
{
  debug_level = 1;
  view = NULL;
  world = NULL;
  dynlight = NULL;
  motion_flags = 0;
  cdsys = NULL;
}

Phyztest::~Phyztest ()
{
  if (cdsys) cdsys->DecRef ();
  delete view;
}

void cleanup ()
{
  Sys->console_out ("Cleaning up...\n");
  delete Sys;
}

bool Phyztest::Initialize (int argc, const char* const argv[], const char *iConfigName)
{
  if (!superclass::Initialize (argc, argv, iConfigName))
    return false;

  // Find the pointer to world plugin
  iWorld *World = QUERY_PLUGIN (this, iWorld);
  if (!World)
  {
    CsPrintf (MSG_FATAL_ERROR, "No iWorld plugin!\n");
    abort ();
  }
  world = World->GetCsWorld ();
  World->DecRef ();

  // Open the main system. This will open all the previously loaded plug-ins.
  if (!Open ("Phyztest Crystal Space Application"))
  {
    Printf (MSG_FATAL_ERROR, "Error opening system!\n");
    cleanup ();
    exit (1);
  }

  cdsys = LOAD_PLUGIN (Sys, "crystalspace.colldet.rapid", "CollDet", iCollideSystem);

  // Some commercials...
  Printf (MSG_INITIALIZATION, "Phyztest Crystal Space Application version 0.1.\n");
  iTextureManager* txtmgr = G3D->GetTextureManager ();
  txtmgr->SetVerbose (true);

  // First disable the lighting cache. Our app is simple enough
  // not to need this.
  world->EnableLightingCache (false);

  // Create our world.
  Printf (MSG_INITIALIZATION, "Creating world!...\n");

  if( !csLoader::LoadLibraryFile (world, "/lib/std/library" ) ){
    Printf (MSG_INITIALIZATION, "LIBRARY NOT LOADED!...\n");
    Shutdown = true;
    return false;
  }
  csLoader::LoadTexture (world, "stone", "/lib/std/stone4.gif");
  csMaterialWrapper* tm = world->GetMaterials ()->FindByName ("stone");

  room = world->CreateCsSector ("room");
  csPolygon3D* p;
  p = room->NewPolygon (tm);
  p->AddVertex (-5, 5, 5);
  p->AddVertex (5, 5, 5);
  p->AddVertex (5, 5, -5);
  p->AddVertex (-5, 5, -5);
  p->SetTextureSpace (p->Vobj (0), p->Vobj (1), 3);

  p = room->NewPolygon (tm);
  p->AddVertex (-5, 17, -5);
  p->AddVertex (5, 17, -5);
  p->AddVertex (5, 17, 5);
  p->AddVertex (-5, 17, 5);
  p->SetTextureSpace (p->Vobj (0), p->Vobj (1), 3);

  p = room->NewPolygon (tm);
  p->AddVertex (-5, 17, 5);
  p->AddVertex (5, 17, 5);
  p->AddVertex (5, 5, 5);
  p->AddVertex (-5, 5, 5);
  p->SetTextureSpace (p->Vobj (0), p->Vobj (1), 3);

  p = room->NewPolygon (tm);
  p->AddVertex (5, 17, 5);
  p->AddVertex (5, 17, -5);
  p->AddVertex (5, 5, -5);
  p->AddVertex (5, 5, 5);
  p->SetTextureSpace (p->Vobj (0), p->Vobj (1), 3);

  p = room->NewPolygon (tm);
  p->AddVertex (-5, 17, -5);
  p->AddVertex (-5, 17, 5);
  p->AddVertex (-5, 5, 5);
  p->AddVertex (-5, 5, -5);
  p->SetTextureSpace (p->Vobj (0), p->Vobj (1), 3);

  p = room->NewPolygon (tm);
  p->AddVertex (5, 17, -5);
  p->AddVertex (-5, 17, -5);
  p->AddVertex (-5, 5, -5);
  p->AddVertex (5, 5, -5);
  p->SetTextureSpace (p->Vobj (0), p->Vobj (1), 3);

  csStatLight* light;
  light = new csStatLight (-3, 7, 0, 10, 1, 0, 0, false);
  room->AddLight (light);
  light = new csStatLight (3, 7, 0, 10, 0, 0, 1, false);
  room->AddLight (light);
  light = new csStatLight (0, 7, -3, 10, 0, 1, 0, false);
  room->AddLight (light);

  iPolygonMesh* mesh = QUERY_INTERFACE (room, iPolygonMesh);
  (void)new csCollider(*room, cdsys, mesh);

  world->Prepare ();

  // Create a dynamic light.
 /* angle = 0;
  dynlight = new csDynLight (cos (angle)*3, 17, sin (angle)*3, 7, 1, 0, 0);
  world->AddDynLight (dynlight);
  dynlight->SetSector (room);
  dynlight->Setup ();
*/
  Printf (MSG_INITIALIZATION, "--------------------------------------\n");

  // csView is a view encapsulating both a camera and a clipper.
  // You don't have to use csView as you can do the same by
  // manually creating a camera and a clipper but it makes things a little
  // easier.
  view = new csView (world, G3D);
  view->SetSector (room);
  view->GetCamera ()->SetPosition (csVector3 (0, 8, -4));
  view->SetRectangle (2, 2, FrameWidth - 4, FrameHeight - 4);

  txtmgr->SetPalette ();
  return true;
}

void Phyztest::NextFrame ()
{
  SysSystemDriver::NextFrame ();
  cs_time elapsed_time, current_time;
  GetElapsedTime (elapsed_time, current_time);

  int i;
  csMatrix3 m; 
  ctMatrix3 M;

  // Now rotate the camera according to keyboard state
  float speed = (elapsed_time / 1000.) * (0.03 * 20);

  if (GetKeyState (CSKEY_RIGHT))
    view->GetCamera ()->Rotate (VEC_ROT_RIGHT, speed);
  if (GetKeyState (CSKEY_LEFT))
    view->GetCamera ()->Rotate (VEC_ROT_LEFT, speed);
  if (GetKeyState (CSKEY_PGUP))
    view->GetCamera ()->Rotate (VEC_TILT_UP, speed);
  if (GetKeyState (CSKEY_PGDN))
    view->GetCamera ()->Rotate (VEC_TILT_DOWN, speed);
  if (GetKeyState (CSKEY_UP))
    view->GetCamera ()->Move (VEC_FORWARD * 4.0f * speed);
  if (GetKeyState (CSKEY_DOWN))
    view->GetCamera ()->Move (VEC_BACKWARD * 4.0f * speed);

  // add a chain
  if (GetKeyState (CSKEY_DEL) && !chain_added ){
    

 //   CsPrintf (MSG_DEBUG_0, "adding chain\n");
    // use box template
    csSpriteTemplate* bxtmpl = (csSpriteTemplate*)view->GetWorld ()->sprite_templates.FindByName ("box");
    if (!bxtmpl){     
      Printf (MSG_INITIALIZATION, "couldn't load template 'box'\n");
      return;
    }

    // root of chain.  invisible ( no sprite )
    // this body doesn't rotate or translate if it is rooted. 
    csSprite3D *sprt;
    ctArticulatedBody *ab_parent;
    ctArticulatedBody *ab_child;
    // each link of chain has a rigid body 
    ctRigidBody *rb = add_test_body( ctVector3( 0.0,8.0,0.0 ));
    // which is used in the creation of an articulated body ( linked to others via a joint )
    ab_parent = new ctArticulatedBody( rb );
    // the world only needs to have a pointer to the root of the articulated body tree.
    phyz_world.add_entity( ab_parent );
    ab_parent->make_grounded();  // make the root fixed to the world. can be non-rooted as well
  
    // add all the links that will be seen swinging.
    for( i = 0; i < NUM_LINKS; i++ ){
      // position is irrelevent. it will be determined by root offset and joint angles
      rb = add_test_body( ctVector3( 0.0,1.0,0.0 ) );  
      ab_child = new ctArticulatedBody( rb );
      // link this body to the previous one.  first 2 vectors are joint offsets, 
      // the 3rd is the line the joint revolves around
      ctVector3 joint_offset( 0, -0.1, 0 );
      ctVector3 joint_action( 0,0,1 );
      ab_parent->link_revolute( ab_child, joint_offset, joint_offset, joint_action );
  
      // make something to draw
      sprt = add_test_sprite( bxtmpl, room, view );
      chain[i] = new ChainLink( sprt, rb, ab_child );
      
      // this will be parent of next body
      ab_parent = ab_child;
    }
    
    // rotate them so we can see some action.
    chain[0]->ab->rotate_around_axis( degree_to_rad(80) );
    //!me uncomment if you have a good frame-rate
  // chain[2]->ab->rotate_around_axis( degree_to_rad(60) );
    chain_added = true;
  }

  // simple mass on a spring demo
  if( GetKeyState (CSKEY_TAB) && bot == NULL ){
    // add a sprite
    csSpriteTemplate* tmpl = (csSpriteTemplate*)view->GetWorld ()->sprite_templates.FindByName ("box");
    if (!tmpl){     
      Printf (MSG_INITIALIZATION, "couldn't load template 'bot'\n");
      return;
    }
    bot = new csSprite3D(view->GetWorld ());
    bot->SetTemplate( tmpl );
    view->GetWorld ()->sprites.Push (bot);
    bot->GetMovable ().SetSector (room);
    m.Identity (); //m = m * 2.0;
    bot->GetMovable ().SetTransform (m);
    bot->GetMovable ().SetPosition (csVector3( 0, 10, 0 ));
    bot->GetMovable ().UpdateMove ();
    bot->SetAction ("default");
    bot->InitSprite ();

    // add the rigidbody physics object
    rb_bot = ctRigidBody::new_ctRigidBody();
    rb_bot->set_m( 15.0 );
    rb_bot->set_pos(ctVector3(0,10,0));
    rb_bot->set_v( ctVector3( 1.0,0, 0));
    rb_bot->calc_simple_I_tensor( 0.2,0.4, 0.2 );
    phyz_world.add_entity( rb_bot );

    // create a spring force object and add it to our test body
    ctSpringF *sf = new ctSpringF( rb_bot, ctVector3( 0, 0.2, 0 ) , NULL, ctVector3( 0,12, 0 ) );
    sf->set_rest_length( 0 );
    sf->set_magnitude( 300.0 );
  //  rb_bot->add_force( sf );
    ctVector3 rotaxisz( 0,0,1 );
    ctVector3 rotaxisy( 0,1,0 );
 
    rb_bot->rotate_around_line( rotaxisy, degree_to_rad(45) );  
    rb_bot->rotate_around_line( rotaxisz, degree_to_rad(60) );
    rb_bot->set_angular_v( ctVector3( 0,0,0) );
    (void)new csRigidSpaceTimeObj( cdsys, bot, rb_bot );
  }
  
  // Move the dynamic light around.
/*  angle += elapsed_time * 0.4 / 1000.;
  while (angle >= 2.*3.1415926) angle -= 2.*3.1415926;
  dynlight->Move (room, cos (angle)*3, 17, sin (angle)*3);
  dynlight->Setup ();
*/
  // evolve the physics world by time step.  Slowed down by 4x due to speed of demo objects
  //!me phyz_world.evolve( 0, 0.25*elapsed_time / 1000.0 );  //!me .25 needed to balance test samples..
  csRigidSpaceTimeObj::evolve_system( 0, 0.25*elapsed_time / 1000.0, &phyz_world, world );

  // if we have a spring and mass demo started
  if( bot ){
    csVector3 new_p = rb_bot->get_pos();
    csLight* lights[2];
    int num_lights = world->GetNearbyLights (room, new_p, CS_NLIGHT_STATIC|CS_NLIGHT_DYNAMIC, lights, 2);
    bot->UpdateLighting (lights, num_lights);  
  
  }
 
  // if we have a swinging chain demo started
  if( chain_added == true ){
    csLight* lights[2];
    csVector3 new_p;
    int num_lights;

    // update position and orientation of all sprites for the chain.
    // queries the physics world for rigidbody data then sets the sprites properties accordingly
    for( i = 0; i < NUM_LINKS; i++ ){
      if( chain[i] != NULL ){
        //  get the position of this link
        new_p = chain[i]->rb->get_pos();
  //      CsPrintf (MSG_DEBUG_0, "chain pos %d = %f, %f, %f\n",
    //      i, new_p.x, new_p.y, new_p.z);
        chain[i]->sprt->GetMovable ().SetPosition ( new_p );
        
        M = chain[i]->rb->get_R();   // get orientation for this link
        // ctMatrix3 and csMatrix3 not directly compatable yet
        m.Set( M[0][0], M[0][1], M[0][2],
               M[1][0], M[1][1], M[1][2],
               M[2][0], M[2][1], M[2][2]);    // set orientation of sprite
        csMatrix3 M_scale;   // chain is half size of box
        M_scale.Identity();
        M_scale *= 0.5;
        m *= M_scale;
        chain[i]->sprt->GetMovable ().SetTransform(m);
	chain[i]->sprt->GetMovable ().UpdateMove ();
        num_lights = world->GetNearbyLights (room, new_p, CS_NLIGHT_STATIC|CS_NLIGHT_DYNAMIC, lights, 2);
              chain[i]->sprt->UpdateLighting (lights, num_lights);      
      }
    }
  }

  // Tell 3D driver we're going to display 3D things.
  if (!G3D->BeginDraw (CSDRAW_3DGRAPHICS)) return;

  view->Draw ();

  // Drawing code ends here.
  G3D->FinishDraw ();
  // Print the final output.
  G3D->Print (NULL);
}

bool Phyztest::HandleEvent (iEvent &Event)
{
  if (superclass::HandleEvent (Event))
    return true;

  if ((Event.Type == csevKeyDown) && (Event.Key.Code == CSKEY_ESC))
  {
    Shutdown = true;
    return true;
  }

  return false;
}

/*---------------------------------------------------------------------*
 * Main function
 *---------------------------------------------------------------------*/
int main (int argc, char* argv[])
{
  srand (time (NULL));

  // add gravity to the world.  enviro forces affect all bodies in the world
  //ctGravityF *gf = new ctGravityF( 9.81 / M_PER_WORLDUNIT );
  ctGravityF *gf = new ctGravityF( 2.0 / M_PER_WORLDUNIT );
  phyz_world.add_enviro_force( gf );
  // add air resistance
  ctAirResistanceF *af = new ctAirResistanceF();
  phyz_world.add_enviro_force( af );

  // register collision detection catastrophe manager
  ctLameCollisionCatastrophe *cdm = new ctLameCollisionCatastrophe();
  phyz_world.register_catastrophe_manager( cdm );

  // Create our main class.
  Sys = new Phyztest ();
  // temp hack until we find a better way
  csWorld::System = Sys;

  // We want at least the minimal set of plugins
  System->RequestPlugin ("crystalspace.kernel.vfs:VFS");
  System->RequestPlugin ("crystalspace.graphics3d.software:VideoDriver");
  System->RequestPlugin ("crystalspace.engine.core:Engine");

  // Initialize the main system. This will load all needed plug-ins
  // (3D, 2D, network, sound, ...) and initialize them.
  if (!Sys->Initialize (argc, argv, NULL))
  {
    Sys->Printf (MSG_FATAL_ERROR, "Error initializing system!\n");
    cleanup ();
    exit (1);
  }

  // Main loop.
  Sys->Loop ();

  // Cleanup.
  cleanup ();

  return 0;
}
