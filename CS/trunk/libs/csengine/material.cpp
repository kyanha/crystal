/*
    Copyright (C) 2001 by Jorrit Tyberghein
    Copyright (C) 2000 by W.C.A. Wijngaards

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
#include "csengine/material.h"
#include "csengine/engine.h"
#include "csutil/debug.h"
#include "ivideo/txtmgr.h"
#include "ivideo/texture.h"
#include "iengine/texture.h"

SCF_IMPLEMENT_IBASE(csMaterial)
  SCF_IMPLEMENTS_INTERFACE(iMaterial)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iMaterialEngine)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csMaterial::MaterialEngine)
  SCF_IMPLEMENTS_INTERFACE(iMaterialEngine)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csMaterial::csMaterial () :
  texture(0),
  num_texture_layers(0),
  diffuse(CS_DEFMAT_DIFFUSE),
  ambient(CS_DEFMAT_AMBIENT),
  reflection(CS_DEFMAT_REFLECTION),
  effect(0)
{
  SCF_CONSTRUCT_IBASE (0);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiMaterialEngine);
  flat_color.Set (255, 255, 255); // Default state is white, flat-shaded.

#ifdef CS_USE_NEW_RENDERER
  shaders = new csHashMap();
#endif // CS_USE_NEW_RENDERER
}

csMaterial::csMaterial (iTextureWrapper *w) :
  num_texture_layers(0),
  diffuse(CS_DEFMAT_DIFFUSE),
  ambient(CS_DEFMAT_AMBIENT),
  reflection(CS_DEFMAT_REFLECTION),
  effect(0)
{
  SCF_CONSTRUCT_IBASE (0);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiMaterialEngine);
  texture = w;
  flat_color.Set (255, 255, 255); // Default state is white, flat-shaded.

#ifdef CS_USE_NEW_RENDERER
  shaders = new csHashMap();
#endif // CS_USE_NEW_RENDERER
}

csMaterial::~csMaterial ()
{
#ifdef CS_USE_NEW_RENDERER
  csGlobalHashIterator cIter (shaders);

  while(cIter.HasNext() )
  {
    iShader* i = (iShader*)cIter.Next();
    i->DecRef();
  }
  shaders->DeleteAll ();
  delete shaders;
#endif // CS_USE_NEW_RENDERER
}

void csMaterial::SetTextureWrapper (iTextureWrapper *tex)
{
  texture = tex;
}

void csMaterial::AddTextureLayer (
  iTextureWrapper *txtwrap,
  uint mode,
  float uscale,
  float vscale,
  float ushift,
  float vshift)
{
  if (num_texture_layers >= 4) return ;
  texture_layer_wrappers[num_texture_layers] = txtwrap;
  texture_layers[num_texture_layers].mode = mode;
  texture_layers[num_texture_layers].uscale = uscale;
  texture_layers[num_texture_layers].vscale = vscale;
  texture_layers[num_texture_layers].ushift = ushift;
  texture_layers[num_texture_layers].vshift = vshift;
  num_texture_layers++;
}

void csMaterial::SetEffect (iEffectDefinition *ed)
{
  effect = ed;
}

iEffectDefinition *csMaterial::GetEffect ()
{
  return effect;
}

#ifdef CS_USE_NEW_RENDERER
void csMaterial::SetShader (csStringID type, iShaderWrapper* shd)
{
  shd->IncRef ();
  AddChild (shd);
  shaders->Put (type, shd);
}

iShaderWrapper* csMaterial::GetShader(csStringID type)
{
  return (iShaderWrapper *) shaders->Get (type);
}

#endif

iTextureHandle *csMaterial::GetTexture ()
{
  return texture ? texture->GetTextureHandle () : 0;
}

int csMaterial::GetTextureLayerCount ()
{
  return num_texture_layers;
}

csTextureLayer *csMaterial::GetTextureLayer (int idx)
{
  if (idx >= 0 && idx < num_texture_layers)
  {
    texture_layers[idx].txt_handle = texture_layer_wrappers[idx]->GetTextureHandle ();
    return &texture_layers[idx];
  }
  else
    return 0;
}

void csMaterial::GetFlatColor (csRGBpixel &oColor, bool useTextureMean)
{
  oColor = flat_color;
  if (texture && useTextureMean)
  {
    iTextureHandle *th = texture->GetTextureHandle ();
    if (th) th->GetMeanColor (oColor.red, oColor.green, oColor.blue);
  }
}

void csMaterial::GetReflection (
  float &oDiffuse,
  float &oAmbient,
  float &oReflection)
{
  oDiffuse = diffuse;
  oAmbient = ambient;
  oReflection = reflection;
}

//---------------------------------------------------------------------------
SCF_IMPLEMENT_IBASE_EXT(csMaterialWrapper)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iMaterialWrapper)
  SCF_IMPLEMENTS_INTERFACE(csMaterialWrapper)
SCF_IMPLEMENT_IBASE_EXT_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csMaterialWrapper::MaterialWrapper)
  SCF_IMPLEMENTS_INTERFACE(iMaterialWrapper)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csMaterialWrapper::csMaterialWrapper (iMaterial *m) : csObject()
{
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiMaterialWrapper);
  DG_TYPE (this, "csMaterialWrapper");
  material = m;
}

csMaterialWrapper::csMaterialWrapper (iMaterialHandle *ith) : csObject()
{
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiMaterialWrapper);
  DG_TYPE (this, "csMaterialWrapper");

  handle = ith;
  DG_LINK (this, handle);
}

csMaterialWrapper::csMaterialWrapper (csMaterialWrapper &w) : csObject(w)
{
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiMaterialWrapper);
  DG_TYPE (this, "csMaterialWrapper");

  material = w.material;
  handle = w.handle;
  DG_LINK (this, handle);
}

csMaterialWrapper::~csMaterialWrapper ()
{
  if (handle)
  {
    DG_UNLINK (this, handle);
  }
}

void csMaterialWrapper::SetMaterial (iMaterial *m)
{
  material = m;
}

void csMaterialWrapper::SetMaterialHandle (iMaterialHandle *m)
{
  material = 0;

  if (handle)
  {
    DG_UNLINK (this, handle);
  }

  handle = m;
  DG_LINK (this, handle);
}

void csMaterialWrapper::Register (iTextureManager *txtmgr)
{
  if (handle)
  {
    DG_UNLINK (this, handle);
  }

  handle = txtmgr->RegisterMaterial (material);
  DG_LINK (this, handle);
}

void csMaterialWrapper::Visit ()
{
  // @@@ This is not very clean! We shouldn't cast from iMaterial to csMaterial.
  // @@@ This is also not up-to-date because it doesn't deal with layers
  csMaterial *mat = (csMaterial *)(iMaterial*)material;
  if (mat && mat->GetTextureWrapper ()) mat->GetTextureWrapper ()->Visit ();
}

//------------------------------------------------------ csMaterialList -----//
SCF_IMPLEMENT_IBASE(csMaterialList)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iMaterialList)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csMaterialList::MaterialList)
  SCF_IMPLEMENTS_INTERFACE(iMaterialList)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csMaterialList::csMaterialList () :
  csRefArrayObject<iMaterialWrapper> (16, 16)
{
  SCF_CONSTRUCT_IBASE (0);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiMaterialList);
}

iMaterialWrapper *csMaterialList::NewMaterial (iMaterial *material)
{
  iMaterialWrapper *tm = &
    (new csMaterialWrapper (material))->scfiMaterialWrapper;
  Push (tm);
  tm->DecRef ();
  return tm;
}

iMaterialWrapper *csMaterialList::NewMaterial (iMaterialHandle *ith)
{
  iMaterialWrapper *tm = &(new csMaterialWrapper (ith))->scfiMaterialWrapper;
  Push (tm);
  tm->DecRef ();
  return tm;
}

iMaterialWrapper *csMaterialList::MaterialList::NewMaterial (
  iMaterial *material)
{
  return scfParent->NewMaterial (material);
}

iMaterialWrapper *csMaterialList::MaterialList::NewMaterial (
  iMaterialHandle *ith)
{
  return scfParent->NewMaterial (ith);
}

int csMaterialList::MaterialList::GetCount () const
{
  return scfParent->Length ();
}

iMaterialWrapper *csMaterialList::MaterialList::Get (int n) const
{
  return scfParent->Get (n);
}

int csMaterialList::MaterialList::Add (iMaterialWrapper *obj)
{
  return scfParent->Push (obj);
}

bool csMaterialList::MaterialList::Remove (iMaterialWrapper *obj)
{
  return scfParent->Delete (obj);
}

bool csMaterialList::MaterialList::Remove (int n)
{
  return scfParent->DeleteIndex (n);
}

void csMaterialList::MaterialList::RemoveAll ()
{
  scfParent->DeleteAll ();
}

int csMaterialList::MaterialList::Find (iMaterialWrapper *obj) const
{
  return scfParent->Find (obj);
}

iMaterialWrapper *csMaterialList::MaterialList::FindByName (
  const char *Name) const
{
  return scfParent->FindByName (Name);
}
