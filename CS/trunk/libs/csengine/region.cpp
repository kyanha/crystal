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
#include "csengine/region.h"
#include "csengine/cssprite.h"
#include "csengine/cscoll.h"
#include "csengine/thing.h"
#include "csengine/texture.h"
#include "csengine/sector.h"
#include "csengine/engine.h"
#include "csengine/campos.h"
#include "csengine/material.h"
#include "csengine/polytmap.h"
#include "csengine/curve.h"
#include "csobject/objiter.h"
#include "csutil/csvector.h"
#include "itxtmgr.h"

//---------------------------------------------------------------------------

IMPLEMENT_IBASE (csRegion)
  IMPLEMENTS_EMBEDDED_INTERFACE (iRegion)
IMPLEMENT_IBASE_END

IMPLEMENT_EMBEDDED_IBASE (csRegion::Region)
  IMPLEMENTS_INTERFACE (iRegion)
IMPLEMENT_EMBEDDED_IBASE_END

IMPLEMENT_CSOBJTYPE (csRegion,csObjectNoDel);

csRegion::csRegion (csEngine* e) : csObjectNoDel ()
{
  CONSTRUCT_IBASE (NULL);
  CONSTRUCT_EMBEDDED_IBASE (scfiRegion);
  engine = e;
}

csRegion::~csRegion ()
{
  scfiRegion.Clear ();
}

void csRegion::Region::Clear ()
{
  csObject* obj = scfParent->GetChild (csObject::Type, true);
  while (obj)
  {
    scfParent->ObjRelease (obj);
    obj = scfParent->GetChild (csObject::Type, true);
  }
}

void csRegion::Region::DeleteAll ()
{
  // First we need to copy the objects to a csVector to avoid
  // messing up the iterator while we are deleting them.
  csVector copy;
  for (csObjIterator iter = scfParent->GetIterator (csObject::Type, true);
  	!iter.IsFinished () ; ++iter)
  {
    csObject* o = iter.GetObj ();
    copy.Push (o);
  }

  // Now we iterate over all objects in the 'copy' vector and
  // delete them. This will release them as csObject children
  // from this region parent.
  // Note that we traverse the list again and again for every
  // object type since the order in which objects types are deleted
  // is important. i.e. we should first delete all sprites and things
  // and only then delete the sectors.
  int i;
  for (i = 0 ; i < copy.Length () ; i++)
    if (((csObject*)copy[i])->GetType () == csCollection::Type)
    {
      csCollection* o = (csCollection*)copy[i];
      scfParent->engine->RemoveCollection (o);
      copy[i] = NULL;
    }

  for (i = 0 ; i < copy.Length () ; i++)
    if (copy[i] && ((csObject*)copy[i])->GetType () >= csSprite::Type)
    {
      csSprite* o = (csSprite*)copy[i];
      scfParent->engine->RemoveSprite (o);
      copy[i] = NULL;
    }

  // @@@ Should sprite templates be deleted if there are still sprites
  // (in other regions) using them? Maybe a ref counter. Also make
  // sure to ObjRelease when you don't delete a sprite template.
  for (i = 0 ; i < copy.Length () ; i++)
    if (copy[i] && ((csObject*)copy[i])->GetType () == csSpriteTemplate::Type)
    {
      csSpriteTemplate* o = (csSpriteTemplate*)copy[i];
      scfParent->engine->sprite_templates.Delete (
        scfParent->engine->sprite_templates.Find (o));
      copy[i] = NULL;
    }

  for (i = 0 ; i < copy.Length () ; i++)
    if (copy[i] && ((csObject*)copy[i])->GetType () == csCurveTemplate::Type)
    {
      csCurveTemplate* o = (csCurveTemplate*)copy[i];
      scfParent->engine->curve_templates.Delete (
        scfParent->engine->curve_templates.Find (o));
      copy[i] = NULL;
    }

  // First things and only then thing templates.
  for (i = 0 ; i < copy.Length () ; i++)
    if (copy[i] && ((csObject*)copy[i])->GetType () == csThing::Type)
    {
      csThing* o = (csThing*)copy[i];
      int idx = scfParent->engine->thing_templates.Find (o);
      if (idx == -1)
      {
        delete o;
	copy[i] = NULL;
      }
    }

  // @@@ Should thing templates be deleted if there are still things
  // (in other regions) using them? Maybe a ref counter. Also make
  // sure to ObjRelease when you don't delete a thing template.
  for (i = 0 ; i < copy.Length () ; i++)
    if (copy[i] && ((csObject*)copy[i])->GetType () == csThing::Type)
    {
      csThing* o = (csThing*)copy[i];
      int idx = scfParent->engine->thing_templates.Find (o);
      if (idx != -1)
      {
        scfParent->engine->thing_templates.Delete (idx);
	copy[i] = NULL;
      }
    }

  for (i = 0 ; i < copy.Length () ; i++)
    if (copy[i] && ((csObject*)copy[i])->GetType () == csSector::Type)
    {
      csSector* o = (csSector*)copy[i];
      int idx = scfParent->engine->sectors.Find (o);
      if (idx != -1)
        scfParent->engine->sectors.Delete (idx);
      else
        delete o;
      copy[i] = NULL;
    }

  for (i = 0 ; i < copy.Length () ; i++)
    if (copy[i] && ((csObject*)copy[i])->GetType() == csMaterialWrapper::Type)
    {
      csMaterialWrapper* o = (csMaterialWrapper*)copy[i];
      int idx = scfParent->engine->GetMaterials ()->Find (o);
      if (idx != -1)
        scfParent->engine->GetMaterials ()->Delete (idx);
      else
        delete o;
      copy[i] = NULL;
    }

  for (i = 0 ; i < copy.Length () ; i++)
    if (copy[i] && ((csObject*)copy[i])->GetType () == csTextureWrapper::Type)
    {
      csTextureWrapper* o = (csTextureWrapper*)copy[i];
      int idx = scfParent->engine->GetTextures ()->Find (o);
      if (idx != -1)
        scfParent->engine->GetTextures ()->Delete (idx);
      else
        delete o;
      copy[i] = NULL;
    }

  for (i = 0 ; i < copy.Length () ; i++)
    if (copy[i] && ((csObject*)copy[i])->GetType () == csCameraPosition::Type)
    {
      csCameraPosition* o = (csCameraPosition*)copy[i];
      int idx = scfParent->engine->camera_positions.Find (o);
      if (idx != -1)
        scfParent->engine->camera_positions.Delete (idx);
      else
        delete o;
      copy[i] = NULL;
    }

  for (i = 0 ; i < copy.Length () ; i++)
    if (copy[i] && ((csObject*)copy[i])->GetType () == csPolyTxtPlane::Type)
    {
      csPolyTxtPlane* o = (csPolyTxtPlane*)copy[i];
      // Do a release here because the plane may still be used by other
      // polygons not belonging to this sector and we want to be sure
      // to release it from this region.
      scfParent->ObjRelease (o);
      int idx = scfParent->engine->planes.Find (o);
      o->DecRef ();
      if (idx != -1)
        scfParent->engine->planes[idx] = 0;
      copy[i] = NULL;
    }

#if defined(CS_DEBUG)
  // Sanity check (only in debug mode). There should be no more
  // non-NULL references in the copy array now.
  for (i = 0 ; i < copy.Length () ; i++)
    if (copy[i])
    {
      csObject* o = (csObject*)copy[i];
      CsPrintf (MSG_INTERNAL_ERROR, "\
There is still an object in the array after deleting region contents!\n\
Object name is '%s', object type is '%s'\n",
	o->GetName () ? o->GetName () : "<NoName>",
	o->GetType ().ID);
    }
#endif // CS_DEBUG
}

bool csRegion::Region::PrepareTextures ()
{
  iTextureManager* txtmgr = csEngine::current_engine->G3D->GetTextureManager();
  txtmgr->ResetPalette ();

  // First register all textures to the texture manager.
  {
    for (csObjIterator iter =
	scfParent->GetIterator (csTextureWrapper::Type, false);
  	!iter.IsFinished () ; ++iter)
    {
      csTextureWrapper* csth = (csTextureWrapper*)iter.GetObj ();
      if (!csth->GetTextureHandle ())
        csth->Register (txtmgr);
    }
  }

  // Prepare all the textures.
  txtmgr->PrepareTextures ();

  // Then register all materials to the texture manager.
  {
    for (csObjIterator iter = 
	scfParent->GetIterator (csMaterialWrapper::Type, false);
  	!iter.IsFinished () ; ++iter)
    {
      csMaterialWrapper* csmh = (csMaterialWrapper*)iter.GetObj ();
      if (!csmh->GetMaterialHandle ())
        csmh->Register (txtmgr);
    }
  }

  // Prepare all the materials.
  txtmgr->PrepareMaterials ();
  return true;
}

bool csRegion::Region::PrepareSectors ()
{
  for (csObjIterator iter = scfParent->GetIterator (csSector::Type);
    !iter.IsFinished () ; ++iter)
  {
    csSector* s = (csSector*)iter.GetObj ();
    s->Prepare (s);
  }
  return true;
}

bool csRegion::Region::ShineLights ()
{
  scfParent->engine->ShineLights (scfParent);
  return true;
}

bool csRegion::Region::Prepare ()
{
  if (!PrepareTextures ()) return false;
  if (!PrepareSectors ()) return false;
  if (!ShineLights ()) return false;
  return true;
}

void csRegion::AddToRegion (csObject* obj)
{
  ObjAdd (obj);
}

void csRegion::ReleaseFromRegion (csObject* obj)
{
  ObjRelease (obj);
}

csObject* csRegion::FindObject (const char* iName, const csIdType& type,
      	bool derived)
{
  for (csObjIterator iter = GetIterator (type, derived);
  	!iter.IsFinished () ; ++iter)
  {
    csObject* o = iter.GetObj ();
    const char* on = o->GetName ();
    if (on && !strcmp (on, iName)) return o;
  }
  return NULL;
}

iSector* csRegion::Region::FindSector (const char *iName)
{
  csSector* obj = (csSector*)scfParent->FindObject(iName,csSector::Type,false);
  if (!obj) return NULL;
  return &obj->scfiSector;
}

iThing* csRegion::Region::FindThing (const char *iName)
{
  for (csObjIterator iter = scfParent->GetIterator (csThing::Type, false);
    !iter.IsFinished () ; ++iter)
  {
    csThing* o = (csThing*)iter.GetObj ();
    if (!o->IsSky () && !o->IsTemplate ())
    {
      // Only if not a sky and not a template.
      const char* on = o->GetName ();
      if (on && !strcmp (on, iName)) return &o->scfiThing;
    }
  }
  return NULL;
}

iThing* csRegion::Region::FindSky (const char *iName)
{
  for (csObjIterator iter = scfParent->GetIterator (csThing::Type, false);
    !iter.IsFinished () ; ++iter)
  {
    csThing* o = (csThing*)iter.GetObj ();
    if (o->IsSky () && !o->IsTemplate ())
    {
      // Only if a sky and not a template.
      const char* on = o->GetName ();
      if (on && !strcmp (on, iName)) return &o->scfiThing;
    }
  }
  return NULL;
}

iThing* csRegion::Region::FindThingTemplate (const char *iName)
{
  for (csObjIterator iter = scfParent->GetIterator (csThing::Type, false);
    !iter.IsFinished () ; ++iter)
  {
    csThing* o = (csThing*)iter.GetObj ();
    if (o->IsTemplate ())
    {
      // Only if a template.
      const char* on = o->GetName ();
      if (on && !strcmp (on, iName)) return &o->scfiThing;
    }
  }
  return NULL;
}

iSprite* csRegion::Region::FindSprite (const char *iName)
{
  csSprite* obj = (csSprite*)scfParent->FindObject(iName,csSprite::Type,true);
  if (!obj) return NULL;
  return &obj->scfiSprite;
}

iSpriteTemplate* csRegion::Region::FindSpriteTemplate (const char *iName)
{
  csSpriteTemplate* obj = (csSpriteTemplate*)
    scfParent->FindObject (iName, csSpriteTemplate::Type, false);
  if (!obj) return NULL;
  return &obj->scfiSpriteTemplate;
}

iTextureWrapper* csRegion::Region::FindTexture (const char *iName)
{
  csObject* obj = scfParent->FindObject (iName, csTextureWrapper::Type, false);
  if (!obj) return NULL;
  csTextureWrapper* wrapper = (csTextureWrapper*)obj;
  return &wrapper->scfiTextureWrapper;
}

iMaterialWrapper* csRegion::Region::FindMaterial (const char *iName)
{
  csObject* obj = scfParent->FindObject(iName,csMaterialWrapper::Type,false);
  if (!obj) return NULL;
  csMaterialWrapper* wrapper = (csMaterialWrapper*)obj;
  return &wrapper->scfiMaterialWrapper;
}

iCameraPosition* csRegion::Region::FindCameraPosition (const char *iName)
{
  csObject* obj = scfParent->FindObject (iName, csCameraPosition::Type, false);
  if (!obj) return NULL;
  csCameraPosition* campos = (csCameraPosition*)obj;
  return &campos->scfiCameraPosition;
}

bool csRegion::IsInRegion (iObject* iobj)
{
  csObject* obj = (csObject*)iobj;
  csObject* objpar = obj->GetObjectParent ();
  return objpar == (csObject*)this;
}

bool csRegion::Region::IsInRegion (iObject* iobj)
{
  return scfParent->IsInRegion (iobj);
}
