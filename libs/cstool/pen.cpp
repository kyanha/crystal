/*
  Copyright (C) 2005 by Christopher Nelson

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
#include "cstool/pen.h"


csPen::csPen (iGraphics3D *_g3d) : g3d (_g3d)
{

}

csPen::~csPen ()
{

}

void csPen::Start ()
{
  va.MakeEmpty ();
  ia.SetLength (0);  
  colors.SetLength (0);
}

void csPen::AddVertex (float x, float y)
{
  ia.Push((uint)va.AddVertexSmart(x,y,0));
  colors.Push(color);
}

void csPen::SetupMesh ()
{
  mesh.vertices = va.GetVertices ();
  mesh.vertexCount = (uint)va.GetVertexCount ();

  mesh.indices = ia.GetArray ();
  mesh.indexCount = (uint)ia.Length ();

  mesh.colors = colors.GetArray ();
  //mesh.colorCount = static_cast<uint>(colors.Length());  
}

void csPen::DrawMesh (csRenderMeshType mesh_type)
{
  mesh.meshtype = mesh_type;
  g3d->DrawSimpleMesh (mesh, csSimpleMeshScreenspace);
}

void csPen::SetColor (float r, float g, float b, float a)
{
  color.Set (r,g,b,a);
}

/** Draws a single line. */
void csPen::DrawLine (uint x1, uint y1, uint x2, uint y2)
{
  Start ();
  AddVertex (x1,y1);
  AddVertex (x2,y2);

  SetupMesh ();
  DrawMesh (CS_MESHTYPE_LINES);
}

/** Draws a single point. */
void csPen::DrawPoint (uint x1, uint y1)
{
  Start ();
  AddVertex (x1,y1);
  
  SetupMesh (); 
  DrawMesh (CS_MESHTYPE_POINTS);
}

/** Draws a rectangle. */
void csPen::DrawRect (uint x1, uint y1, uint x2, uint y2, bool fill)
{
  Start ();
  AddVertex (x1, y1);
  AddVertex (x1, y2);
  AddVertex (x2, y2);
  AddVertex (x1, y2);

  SetupMesh ();
  DrawMesh (fill ? CS_MESHTYPE_QUADS : CS_MESHTYPE_LINESTRIP);
}

/** Draws a mitered rectangle. The miter value should be between 0.0 and 1.0, and determines how
* much of the corner is mitered off and beveled. */    
void csPen::DrawMiteredRect (uint x1, uint y1, uint x2, uint y2, 
                             float miter, bool fill)
{
  if (miter == 0.0f) 
  { 
    DrawRect (x1,y1,x2,y2,fill); 
    return; 
  }
			
  float width = x2-x1;
  float height = y2-y1;

  float y_miter = (height*miter)*0.5;
  float x_miter = (width*miter)*0.5;
  		
  Start ();

  AddVertex (x1, y2-y_miter);
  AddVertex (x1, y1+y_miter);
  AddVertex (x1+x_miter, y1);
  AddVertex (x2-x_miter, y1);
  AddVertex (x2, y1+y_miter);
  AddVertex (x2, y2-y_miter);
  AddVertex (x2-x_miter, y2);
  AddVertex (x1+x_miter, y2);

  SetupMesh ();
  DrawMesh (fill ? CS_MESHTYPE_POLYGON : CS_MESHTYPE_LINESTRIP);
}

/** Draws a rounded rectangle. The roundness value should be between 0.0 and 1.0, and determines how
  * much of the corner is rounded off. */
void csPen::DrawRoundedRect (uint x1, uint y1, uint x2, uint y2, 
                             float roundness, bool fill)
{
  if (roundness == 0.0f) 
  { 
    DrawRect (x1,y1,x2,y2,fill); 
    return; 
  }
			
  float width = x2-x1;
  float height = y2-y1;

  float y_round = (height*roundness)*0.5;
  float x_round = (width*roundness)*0.5;
  float   steps = roundness * 12;
  float   delta = (PI/4.0)/steps;

  Start();

  float angle;
  			
  for(angle=(HALF_PI)*3.0f; angle>PI; angle-=delta)
  {
    AddVertex (x1+x_round+(cosf (angle)*x_round), y2-y_round-(sinf (angle)*y_round));
  }
	  
  AddVertex (x1, y2-y_round);
  AddVertex (x1, y1+y_round);
  
  for(angle=PI; angle>HALF_PI; angle-=delta)
  {
	  AddVertex (x1+x_round+(cosf (angle)*x_round), y1+y_round-(sinf (angle)*y_round));
  }
  
  AddVertex (x1+x_round, y1);
  AddVertex (x2-x_round, y1);
  
  for(angle=HALF_PI; angle>0; angle-=delta)
  {
    AddVertex (x2-x_round+(cosf (angle)*x_round), y1+y_round-(sinf (angle)*y_round));
  }
  
  AddVertex (x2, y1+y_round);
  AddVertex (x2, y2-y_round);
  
  for(angle=TWO_PI; angle>HALF_PI*3.0; angle-=delta)
  {
    AddVertex (x2-x_round+(cosf (angle)*x_round), y2-y_round-(sinf (angle)*y_round));
  }
  
  AddVertex (x2-x_round, y2);
  AddVertex (x1+x_round, y2);				

  SetupMesh ();
  DrawMesh (fill ? CS_MESHTYPE_POLYGON : CS_MESHTYPE_LINESTRIP);
}
