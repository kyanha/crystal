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

#include "sysdef.h"
#include "csengine/skeleton.h"


//---------------------------------------------------------------------------

csSkeletonLimb::~csSkeletonLimb ()
{
  CHK (delete [] vertices);
  while (children)
  {
    csSkeletonLimb* n = children->GetNext ();
    CHK (delete children);
    children = n;
  }
}

void csSkeletonLimb::AddVertex (int v)
{
  // We allocate 16 vertices at a time.
  int max_vertices = (num_vertices+15) & ~0xf;
  if (num_vertices >= max_vertices)
  {
    int* new_vertices;
    CHK (new_vertices = new int [max_vertices+16]);
    if (vertices) memcpy (new_vertices, vertices, num_vertices*sizeof (int));
    CHK (delete [] vertices);
    vertices = new_vertices;
  }
  vertices[num_vertices++] = v;
}

void csSkeletonLimb::AddChild (csSkeletonLimb* child)
{
  child->SetNext (children);
  children = child;
}

void csSkeletonLimbState::AddChild (csSkeletonLimbState* child)
{
  child->SetNext (children);
  children = child;
}

void csSkeletonLimb::UpdateState (csSkeletonLimbState* limb)
{
  limb->vertices = vertices;
  limb->num_vertices = num_vertices;
  limb->tmpl = this;
  csSkeletonLimb* c = children;
  while (c)
  {
    limb->AddChild (c->CreateState ());
    c = c->GetNext ();
  }
}

void csSkeletonLimb::RemapVertices (int* mapping)
{
  if (num_vertices)
  {
    int i;
    for (i = 0 ; i < num_vertices ; i++)
      vertices[i] = mapping[vertices[i]];
  }

  csSkeletonLimb* c = children;
  while (c)
  {
    c->RemapVertices (mapping);
    c = c->GetNext ();
  }
}

void csSkeletonLimb::ComputeBoundingBox (csVec3Vector* source)
{
  if (num_vertices)
  {

    box.StartBoundingBox (source->Get(vertices[0]));
    int i;
    for (i = 1 ; i < num_vertices ; i++)
    {
      box.AddBoundingVertexSmart (source->Get(vertices[i]));
    }
  }

  csSkeletonLimb* c = children;
  while (c)
  {
    c->ComputeBoundingBox (source);
    c = c->GetNext ();
  }
}

csSkeletonLimbState* csSkeletonLimb::CreateState ()
{
  CHK (csSkeletonLimbState* limb = new csSkeletonLimbState ());
  UpdateState (limb);
  return limb;
}

csSkeletonLimbState* csSkeletonConnection::CreateState ()
{
  CHK (csSkeletonConnectionState* con = new csSkeletonConnectionState ());
  UpdateState ((csSkeletonLimbState*)con);
  con->SetTransformation (trans);
  return (csSkeletonLimbState*)con;
}

csSkeletonLimbState* csSkeleton::CreateState ()
{
  CHK (csSkeletonState* skel = new csSkeletonState ());
  UpdateState ((csSkeletonLimbState*)skel);
  return (csSkeletonLimbState*)skel;
}

//---------------------------------------------------------------------------

IMPLEMENT_CSOBJTYPE (csSkeletonLimbState,csObject);
IMPLEMENT_CSOBJTYPE (csSkeletonConnectionState,csSkeletonLimbState);
IMPLEMENT_CSOBJTYPE (csSkeletonState,csSkeletonLimbState);

csSkeletonLimbState::~csSkeletonLimbState ()
{
  while (children)
  {
    csSkeletonLimbState* n = children->GetNext ();
    CHK (delete children);
    children = n;
  }
}

void csSkeletonLimbState::Transform (const csTransform& tr, csVector3* source, csVector3* dest)
{
  csSkeletonLimbState* c = children;
  while (c)
  {
    c->Transform (tr, source, dest);
    c = c->GetNext ();
  }

  int i;
  for (i = 0 ; i < num_vertices ; i++)
    dest [vertices [i]] = tr * source[vertices [i]];
}

void csSkeletonConnectionState::Transform (const csTransform& tr, csVector3* source,
	csVector3* dest)
{
  csTransform tr_new = tr * trans;
  csSkeletonLimbState::Transform (tr_new, source, dest);
}

void csSkeletonLimbState::ComputeBoundingBox (const csTransform& tr,
	csBox3& box)
{
  if (num_vertices)
  {
    csBox3 b;
    tmpl->GetBoundingBox (b);
    box.AddBoundingVertex (tr * b.GetCorner (0));
    box.AddBoundingVertexSmart (tr * b.GetCorner (1));
    box.AddBoundingVertexSmart (tr * b.GetCorner (2));
    box.AddBoundingVertexSmart (tr * b.GetCorner (3));
    box.AddBoundingVertexSmart (tr * b.GetCorner (4));
    box.AddBoundingVertexSmart (tr * b.GetCorner (5));
    box.AddBoundingVertexSmart (tr * b.GetCorner (6));
    box.AddBoundingVertexSmart (tr * b.GetCorner (7));
  }

  csSkeletonLimbState* c = children;
  while (c)
  {
    c->ComputeBoundingBox (tr, box);
    c = c->GetNext ();
  }
}

void csSkeletonConnectionState::ComputeBoundingBox (const csTransform& tr,
	csBox3& box)
{
  csTransform tr_new = tr * trans;
  csSkeletonLimbState::ComputeBoundingBox (tr_new, box);
}

void csSkeletonState::ComputeBoundingBox (const csTransform& tr,
	csBox3& box)
{
  box.StartBoundingBox ();
  csSkeletonLimbState::ComputeBoundingBox (tr, box);
}

