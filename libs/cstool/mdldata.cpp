/*
    Copyright (C) 2001 by Martin Geisse <mgeisse@gmx.net>

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
#include "cstool/mdldata.h"
#include "csutil/nobjvec.h"
#include "csutil/objiter.h"
#include "igraphic/image.h"
#include "igraphic/imageio.h"
#include "iengine/texture.h"
#include "ivideo/material.h"
#include "iengine/material.h"
#include "iutil/databuff.h"
#include "isys/vfs.h"

#define CS_IMPLEMENT_ARRAY_INTERFACE_NONUM(clname,type,sing_name,mult_name) \
  type clname::Get##sing_name (int n) const				\
  { return mult_name[n]; }						\
  void clname::Set##sing_name (int n, type val)				\
  { mult_name[n] = val; }

#define CS_IMPLEMENT_ARRAY_INTERFACE(clname,type,sing_name,mult_name)	\
  CS_IMPLEMENT_ARRAY_INTERFACE_NONUM (clname, type, sing_name, mult_name)	\
  int clname::Get##sing_name##Count () const				\
  { return mult_name.Length (); }					\
  int clname::Add##sing_name (type v)					\
  { mult_name.Push (v); return mult_name.Length () - 1; }		\
  void clname::Delete##sing_name (int n)				\
  { mult_name.Delete (n); }

#define CS_IMPLEMENT_ACCESSOR_METHOD(clname,type,name)			\
  type clname::Get##name () const					\
  { return name; }							\
  void clname::Set##name (type val)					\
  { name = val; }

#define CS_IMPLEMENT_ACCESSOR_METHOD_REF(clname,type,name)		\
  type clname::Get##name () const					\
  { return name; }							\
  void clname::Set##name (type val)					\
  { if (name) name->DecRef (); name = val; if (name) name->IncRef (); }

#define CS_IMPLEMENT_OBJECT_INTERFACE(clname)				\
  CS_IMPLEMENT_EMBEDDED_OBJECT (clname::Embedded_csObject);		\
  iObject* clname::QueryObject ()					\
  { return &scfiObject; }

SCF_DECLARE_FAST_INTERFACE (iModelDataTexture);
SCF_DECLARE_FAST_INTERFACE (iModelDataMaterial);
CS_DECLARE_OBJECT_ITERATOR (csModelDataTextureIterator, iModelDataTexture);
CS_DECLARE_OBJECT_ITERATOR (csModelDataMaterialIterator, iModelDataMaterial);

//----------------------------------------------------------------------------

/*** csModelDataTexture ***/

SCF_IMPLEMENT_IBASE (csModelDataTexture)
  SCF_IMPLEMENTS_INTERFACE (iModelDataTexture)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iObject)
SCF_IMPLEMENT_IBASE_END

CS_IMPLEMENT_OBJECT_INTERFACE (csModelDataTexture);

csModelDataTexture::csModelDataTexture ()
{
  SCF_CONSTRUCT_IBASE (NULL);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiObject);
  FileName = NULL;
  Image = NULL;
  TextureWrapper = NULL;
}

csModelDataTexture::~csModelDataTexture ()
{
  delete[] FileName;
  SCF_DEC_REF (Image);
  SCF_DEC_REF (TextureWrapper);
}

void csModelDataTexture::SetFileName (const char *fn)
{
  delete[] FileName;
  FileName = csStrNew (fn);
}

const char *csModelDataTexture::GetFileName () const
{
  return FileName;
}

CS_IMPLEMENT_ACCESSOR_METHOD_REF (csModelDataTexture, iImage*, Image);
CS_IMPLEMENT_ACCESSOR_METHOD_REF (csModelDataTexture, iTextureWrapper*, TextureWrapper);

void csModelDataTexture::LoadImage (iVFS *vfs, iImageIO *io, int Format)
{
  if (!FileName) return;
  SCF_DEC_REF (Image);
  Image = NULL;

  iDataBuffer *dbuf = vfs->ReadFile (FileName);
  if (!dbuf) return;

  Image = io->Load (dbuf->GetUint8 (), dbuf->GetSize (), Format);
  dbuf->DecRef ();
}

void csModelDataTexture::Register (iTextureList *tl)
{
  if (!Image) return;
  SetTextureWrapper (tl->NewTexture (Image));
}

iModelDataTexture *csModelDataTexture::Clone () const
{
  csModelDataTexture *t = new csModelDataTexture ();
  t->SetFileName (FileName);
  t->SetImage (Image);
  t->SetTextureWrapper (TextureWrapper);
  return t;
}

/*** csModelDataMaterial ***/

SCF_IMPLEMENT_IBASE (csModelDataMaterial)
  SCF_IMPLEMENTS_INTERFACE (iModelDataMaterial)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iObject)
SCF_IMPLEMENT_IBASE_END

CS_IMPLEMENT_OBJECT_INTERFACE (csModelDataMaterial);

csModelDataMaterial::csModelDataMaterial ()
{
  SCF_CONSTRUCT_IBASE (NULL);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiObject);
  BaseMaterial = NULL;
  MaterialWrapper = NULL;
}

csModelDataMaterial::~csModelDataMaterial ()
{
  SCF_DEC_REF (BaseMaterial);
  SCF_DEC_REF (MaterialWrapper);
}

CS_IMPLEMENT_ACCESSOR_METHOD_REF (csModelDataMaterial, iMaterial*, BaseMaterial);
CS_IMPLEMENT_ACCESSOR_METHOD_REF (csModelDataMaterial, iMaterialWrapper*, MaterialWrapper);

void csModelDataMaterial::Register (iMaterialList *ml)
{
  if (!BaseMaterial) return;
  SetMaterialWrapper (ml->NewMaterial (BaseMaterial));
}

iModelDataMaterial *csModelDataMaterial::Clone () const
{
  csModelDataMaterial *m = new csModelDataMaterial ();
  m->SetBaseMaterial (BaseMaterial);
  m->SetMaterialWrapper (MaterialWrapper);
  return m;
}

/*** csModelDataVertices ***/

SCF_IMPLEMENT_IBASE (csModelDataVertices)
  SCF_IMPLEMENTS_INTERFACE (iModelDataVertices)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iObject)
SCF_IMPLEMENT_IBASE_END

CS_IMPLEMENT_OBJECT_INTERFACE (csModelDataVertices);

CS_IMPLEMENT_ARRAY_INTERFACE (csModelDataVertices,
	const csVector3 &, Vertex, Vertices);
CS_IMPLEMENT_ARRAY_INTERFACE (csModelDataVertices,
	const csVector3 &, Normal, Normals);
CS_IMPLEMENT_ARRAY_INTERFACE (csModelDataVertices,
	const csColor &, Color, Colors);
CS_IMPLEMENT_ARRAY_INTERFACE (csModelDataVertices,
	const csVector2 &, Texel, Texels);

csModelDataVertices::csModelDataVertices ()
{
  SCF_CONSTRUCT_IBASE (NULL);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiObject);
}

csModelDataVertices::csModelDataVertices (const iModelDataVertices *orig,
  const iModelDataVertices *orig2)
{
  SCF_CONSTRUCT_IBASE (NULL);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiObject);
  CopyFrom (orig);
  CopyFrom (orig2);
}

void csModelDataVertices::CopyFrom (const iModelDataVertices *v)
{
  if (!v) return;
  int i;
  for (i=0; i<v->GetVertexCount (); i++)
    AddVertex (v->GetVertex (i));
  for (i=0; i<v->GetNormalCount (); i++)
    AddNormal (v->GetNormal (i));
  for (i=0; i<v->GetColorCount (); i++)
    AddColor (v->GetColor (i));
  for (i=0; i<v->GetTexelCount (); i++)
    AddTexel (v->GetTexel (i));
}

iModelDataVertices *csModelDataVertices::Clone () const
{
  csModelDataVertices *v = new csModelDataVertices ();
  int i;

  for (i=0; i<Vertices.Length (); i++)
    v->AddVertex (Vertices [i]);
  for (i=0; i<Normals.Length (); i++)
    v->AddNormal (Normals [i]);
  for (i=0; i<Colors.Length (); i++)
    v->AddColor (Colors [i]);
  for (i=0; i<Texels.Length (); i++)
    v->AddTexel (Texels [i]);
  return v;
}

int csModelDataVertices::FindVertex (const csVector3 &v) const
{
  int i;
  for (i=0; i<Vertices.Length (); i++)
    if (Vertices [i] - v < EPSILON)
      return i;
  return -1;
}

int csModelDataVertices::FindNormal (const csVector3 &v) const
{
  int i;
  for (i=0; i<Normals.Length (); i++)
    if (Normals [i] - v < EPSILON)
      return i;
  return -1;
}

int csModelDataVertices::FindColor (const csColor &v) const
{
  int i;
  for (i=0; i<Colors.Length (); i++)
    if ((Colors[i].red - v.red < EPSILON) &&
        (Colors[i].green - v.green < EPSILON) &&
        (Colors[i].blue - v.blue < EPSILON))
      return i;
  return -1;
}

int csModelDataVertices::FindTexel (const csVector2 &v) const
{
  int i;
  for (i=0; i<Texels.Length (); i++)
    if (Texels [i] - v < EPSILON)
      return i;
  return -1;
}

/*** csModelDataAction ***/

SCF_IMPLEMENT_IBASE (csModelDataAction)
  SCF_IMPLEMENTS_INTERFACE (iModelDataAction)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iObject)
SCF_IMPLEMENT_IBASE_END

CS_IMPLEMENT_OBJECT_INTERFACE (csModelDataAction);

csModelDataAction::csModelDataAction ()
{
  SCF_CONSTRUCT_IBASE (NULL);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiObject);
}
  
int csModelDataAction::GetFrameCount () const
{
  return Times.Length ();
}

float csModelDataAction::GetTime (int Frame) const
{
  return Times[Frame];
}

iObject *csModelDataAction::GetState (int Frame) const
{
  return States.Get (Frame);
}

void csModelDataAction::SetTime (int Frame, float NewTime)
{
  // save the object
  iObject *obj = States.Get (Frame);
  obj->IncRef ();

  // remove it from the vectors
  Times.Delete (Frame);
  States.Delete (Frame);

  // add it again with the new time value
  AddFrame (NewTime, obj);

  // release it
  obj->DecRef ();
}

void csModelDataAction::SetState (int Frame, iObject *State)
{
  States.Replace (Frame, State);
}

void csModelDataAction::AddFrame (float Time, iObject *State)
{
  int i;
  for (i=0; i<Times.Length (); i++)
    if (Times.Get (i) > Time) break;
  Times.Insert (i, Time);
  States.Insert (i, State);
}

void csModelDataAction::DeleteFrame (int n)
{
  Times.Delete (n);
  States.Delete (n);
}

float csModelDataAction::GetTotalTime () const
{
  return (Times.Length () > 0) ? (Times [Times.Length () - 1]) : 0;
}

/*** csModelDataPolygon ***/

SCF_IMPLEMENT_IBASE (csModelDataPolygon)
  SCF_IMPLEMENTS_INTERFACE (iModelDataPolygon)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iObject)
SCF_IMPLEMENT_IBASE_END

CS_IMPLEMENT_OBJECT_INTERFACE (csModelDataPolygon);

csModelDataPolygon::csModelDataPolygon ()
{
  SCF_CONSTRUCT_IBASE (NULL);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiObject);
  Material = NULL;
}

csModelDataPolygon::~csModelDataPolygon ()
{
  if (Material) Material->DecRef ();
}

int csModelDataPolygon::GetVertexCount () const
{
  return Vertices.Length ();
}

int csModelDataPolygon::AddVertex (int ver, int nrm, int col, int tex)
{
  Vertices.Push (ver);
  Normals.Push (nrm);
  Colors.Push (col);
  Texels.Push (tex);
  return Vertices.Length () - 1;
}

void csModelDataPolygon::DeleteVertex (int n)
{
  Vertices.Delete (n);
  Normals.Delete (n);
  Colors.Delete (n);
  Texels.Delete (n);
}

CS_IMPLEMENT_ACCESSOR_METHOD_REF (csModelDataPolygon, iModelDataMaterial*, Material);
CS_IMPLEMENT_ARRAY_INTERFACE_NONUM (csModelDataPolygon, int, Vertex, Vertices);
CS_IMPLEMENT_ARRAY_INTERFACE_NONUM (csModelDataPolygon, int, Normal, Normals);
CS_IMPLEMENT_ARRAY_INTERFACE_NONUM (csModelDataPolygon, int, Color, Colors);
CS_IMPLEMENT_ARRAY_INTERFACE_NONUM (csModelDataPolygon, int, Texel, Texels);

iModelDataPolygon *csModelDataPolygon::Clone () const
{
  csModelDataPolygon *p = new csModelDataPolygon ();
  for (int i=0; i<Vertices.Length (); i++)
    p->AddVertex (Vertices[i], Normals[i], Colors[i], Texels[i]);
  p->SetMaterial (Material);
  return p;
}

/*** csModelDataObject ***/

SCF_IMPLEMENT_IBASE (csModelDataObject)
  SCF_IMPLEMENTS_INTERFACE (iModelDataObject)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iObject)
SCF_IMPLEMENT_IBASE_END

CS_IMPLEMENT_OBJECT_INTERFACE (csModelDataObject);

CS_IMPLEMENT_ACCESSOR_METHOD_REF (csModelDataObject,
	iModelDataVertices *, DefaultVertices);

csModelDataObject::csModelDataObject ()
{
  SCF_CONSTRUCT_IBASE (NULL);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiObject);
  DefaultVertices = NULL;
}

csModelDataObject::~csModelDataObject ()
{
  SCF_DEC_REF (DefaultVertices);
}

/*** csModelDataCamera ***/

SCF_IMPLEMENT_IBASE (csModelDataCamera)
  SCF_IMPLEMENTS_INTERFACE (iModelDataCamera)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iObject)
SCF_IMPLEMENT_IBASE_END

CS_IMPLEMENT_OBJECT_INTERFACE (csModelDataCamera);

CS_IMPLEMENT_ACCESSOR_METHOD (csModelDataCamera, const csVector3 &, Position);
CS_IMPLEMENT_ACCESSOR_METHOD (csModelDataCamera, const csVector3 &, UpVector);
CS_IMPLEMENT_ACCESSOR_METHOD (csModelDataCamera, const csVector3 &, FrontVector);
CS_IMPLEMENT_ACCESSOR_METHOD (csModelDataCamera, const csVector3 &, RightVector);

csModelDataCamera::csModelDataCamera ()
{
  SCF_CONSTRUCT_IBASE (NULL);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiObject);
}

void csModelDataCamera::ComputeUpVector ()
{
  UpVector = FrontVector % RightVector;
}

void csModelDataCamera::ComputeFrontVector ()
{
  FrontVector = RightVector % UpVector;
}

void csModelDataCamera::ComputeRightVector ()
{
  RightVector = UpVector % FrontVector;
}

void csModelDataCamera::Normalize ()
{
  UpVector.Normalize ();
  FrontVector.Normalize ();
  RightVector.Normalize ();
}

bool csModelDataCamera::CheckOrthogonality () const
{
  float x = UpVector * FrontVector;
  float y = RightVector * FrontVector;
  float z = UpVector * RightVector;
  return (ABS(x) < SMALL_EPSILON) && (ABS(y) < SMALL_EPSILON) &&
    (ABS(z) < SMALL_EPSILON);
}

iModelDataCamera *csModelDataCamera::Clone () const
{
  csModelDataCamera *c = new csModelDataCamera ();
  c->SetPosition (Position);
  c->SetUpVector (UpVector);
  c->SetFrontVector (FrontVector);
  c->SetRightVector (RightVector);
  return c;
}

/*** csModelDataLight ***/

SCF_IMPLEMENT_IBASE (csModelDataLight)
  SCF_IMPLEMENTS_INTERFACE (iModelDataLight)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iObject)
SCF_IMPLEMENT_IBASE_END

CS_IMPLEMENT_OBJECT_INTERFACE (csModelDataLight);

CS_IMPLEMENT_ACCESSOR_METHOD (csModelDataLight, float, Radius);
CS_IMPLEMENT_ACCESSOR_METHOD (csModelDataLight, const csColor &, Color);
CS_IMPLEMENT_ACCESSOR_METHOD (csModelDataLight, const csVector3 &, Position);

csModelDataLight::csModelDataLight ()
{
  SCF_CONSTRUCT_IBASE (NULL);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiObject);
}

iModelDataLight *csModelDataLight::Clone () const
{
  csModelDataLight *l = new csModelDataLight ();
  l->SetPosition (Position);
  l->SetColor (Color);
  l->SetRadius (Radius);
  return l;
}

/*** csModelData ***/

SCF_IMPLEMENT_IBASE (csModelData)
  SCF_IMPLEMENTS_INTERFACE (iModelData)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iObject)
SCF_IMPLEMENT_IBASE_END

CS_IMPLEMENT_OBJECT_INTERFACE (csModelData);

csModelData::csModelData ()
{
  SCF_CONSTRUCT_IBASE (NULL);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiObject);
}

void csModelData::LoadImages (iVFS *vfs, iImageIO *io, int Format)
{
  csModelDataTextureIterator it (&scfiObject);
  while (!it.IsFinished ())
  {
    it.Get ()->LoadImage (vfs, io, Format);
    it.Next ();
  }
}

void csModelData::RegisterTextures (iTextureList *tm)
{
  csModelDataTextureIterator it (&scfiObject);
  while (!it.IsFinished ())
  {
    it.Get ()->Register (tm);
    it.Next ();
  }
}

void csModelData::RegisterMaterials (iMaterialList *ml)
{
  csModelDataMaterialIterator it (&scfiObject);
  while (!it.IsFinished ())
  {
    it.Get ()->Register (ml);
    it.Next ();
  }
}
