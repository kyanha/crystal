/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
   $Id$

    This file is part of Crystal Space Virtual Object System Abstract
    3D Layer plugin (csvosa3dl).

    Copyright (C) 2004 Peter Amstutz

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "cssysdef.h"

#include "csgfx/memimage.h"
#include "iutil/objreg.h"
#include "ivideo/graph3d.h"
#include "ivideo/material.h"
#include "ivideo/txtmgr.h"

#include "vosmaterial.h"
#include "vostexture.h"
#include "vosobject3d.h"


using namespace VOS;

class ConstructMaterialTask : public Task
{
public:
  vRef<csMetaTexture> base;
  std::vector<csMetaTexture*> layers;
  csTextureLayer* coords;
  vRef<csMetaMaterial> metamaterial;
  csRef<iObjectRegistry> object_reg;

  ConstructMaterialTask(csRef<iObjectRegistry> objreg, csMetaMaterial* mm);
  virtual ~ConstructMaterialTask();
  virtual void doTask();
};

ConstructMaterialTask::ConstructMaterialTask(csRef<iObjectRegistry> objreg, csMetaMaterial* mm)
  : coords(0), metamaterial(mm, true), object_reg(objreg)
{
}

ConstructMaterialTask::~ConstructMaterialTask()
{
}

void ConstructMaterialTask::doTask()
{
  csRef<iEngine> engine = CS_QUERY_REGISTRY (object_reg, iEngine);
  csRef<iGraphics3D> g3d = CS_QUERY_REGISTRY (object_reg, iGraphics3D);
  csRef<iTextureManager> txtmgr = g3d->GetTextureManager();

  csRef<iMaterial> imat;

  if(base.isValid()) {
    csRef<iTextureWrapper> basetw = base->getTextureWrapper();

    if(layers.size() > 0)
    {
      iTextureWrapper** layertws = (iTextureWrapper**)malloc(layers.size() * sizeof(iTextureWrapper*));
      for(unsigned int i = 0; i < layers.size(); i++)
      {
        layertws[i] = layers[i]->getTextureWrapper();
        layers[i]->release();
      }

      imat = engine->CreateBaseMaterial(basetw, layers.size(), layertws, coords);
      free(layertws);
      free(coords);
    }
    else
    {
      imat = engine->CreateBaseMaterial(basetw);
    }
  }
#if 0
  else
  {
    /*
      csRef<iMaterial> color  = engine->CreateBaseMaterial(0);
      int red = strtol(newproperty.substr(1, 2).c_str(), 0, 16);
      int green = strtol(newproperty.substr(3, 2).c_str(), 0, 16);
      int blue = strtol(newproperty.substr(5, 2).c_str(), 0, 16);
      color->SetFlatColor(csRGBcolor(red, green, blue));
      if(!material) {
      material = engine->GetMaterialList()->NewMaterial(color);
      material->QueryObject()->SetName(p.getURL().getString().c_str());
      }
      material->SetMaterial(color);
      material->Register(txtmgr);
      material->GetMaterialHandle()->Prepare ();
    */

    /*
      csRef<iMaterial> color = engine->CreateBaseMaterial(0);
      color->SetFlatColor(csRGBcolor((int)(r*255), (int)(g*255), (int)(b*255)));
      if(!material) {
      material = engine->GetMaterialList()->NewMaterial(color);
      material->QueryObject()->SetName(p.getURL().getString().c_str());
      }
      material->SetMaterial(color);
      material->Register(txtmgr);
      material->GetMaterialHandle()->Prepare ();
    */




    iTextureWrapper* txtwrap;
    txtwrap = engine->CreateBlackTexture(getURL().getString().c_str(), 2, 2, 0, CS_TEXTURE_3D);

    float r, g, b;
    try
    {
      getColor(r, g, b);
    }
    catch(exception& e)
    {
      r = g = b = 0;
    }
    csImageMemory* img = new csImageMemory(1, 1);
    csRGBpixel px((int)(r*255.0), (int)(g*255.0), (int)(b*255.0));
    img->Clear(px);
    txtwrap->SetImageFile(img);

    txtwrap->Register(txtmgr);
    txtwrap->GetTextureHandle()->Prepare ();

    imat = engine->CreateBaseMaterial(txtwrap);
  }
#endif

  csRef<iMaterialWrapper> material = engine->GetMaterialList()->NewMaterial(imat);
  if(!material)
  {
    return;
  }
  material->Register(txtmgr);
  material->GetMaterialHandle()->Prepare();

  metamaterial->materialwrapper = material;
}



csMetaMaterial::csMetaMaterial(VobjectBase* superobject)
  : A3DL::Material(superobject), alreadyLoaded(false)
{
}

csMetaMaterial::~csMetaMaterial()
{
}

void csMetaMaterial::setup(csVosA3DL* vosa3dl)
{
  LOG("csMetaMaterial", 2, "setting up material");
  ConstructMaterialTask* cmt = new ConstructMaterialTask(vosa3dl->GetObjectRegistry(), this);

  A3DL::TextureIterator txt = getTextureLayers();
  if(txt.hasMore())
  {
    vRef<csMetaTexture> base = meta_cast<csMetaTexture>(*txt);
    if(base.isValid())
    {
      base->setup(vosa3dl);
      cmt->base = base;
    }

    txt++;
    if(txt.hasMore())
    {
      cmt->coords = (csTextureLayer*)malloc(txt.remaining() * sizeof(csTextureLayer));
      float uscale = 1, vscale = 1, ushift = 0, vshift = 0;
      for(int i = 0; txt.hasMore(); txt++, i++)
      {
        vRef<csMetaTexture> mt = meta_cast<csMetaTexture>(*txt);
        if(mt.isValid()) {
          mt->setup(vosa3dl);
          mt->acquire();
          cmt->layers.push_back(&mt);
          try
          {
            mt->getUVScaleAndShift(uscale, vscale, ushift, vshift);
          }
          catch(std::runtime_error& e)
          {
            uscale = 1; vscale = 1; ushift = 0; vshift = 0;
          }
          cmt->coords[i].uscale = uscale;
          cmt->coords[i].vscale = vscale;
          cmt->coords[i].ushift = ushift;
          cmt->coords[i].vshift = vshift;
          switch(mt->getBlendMode()) {
          case A3DL::Material::BLEND_NORMAL:
            try
            {
              double transparency = mt->getTransparency();
              if(transparency == 1.0)
                cmt->coords[i].mode = CS_FX_TRANSPARENT;
              else if(transparency == 0.0)
                cmt->coords[i].mode = CS_FX_COPY;
              else
                cmt->coords[i].mode = CS_FX_SETALPHA(1.0-transparency);
            }
            catch(...)
            {
              cmt->coords[i].mode = CS_FX_COPY;
            }
            break;
          case A3DL::Material::BLEND_ADD:
            cmt->coords[i].mode = CS_FX_ADD;
            break;
          case A3DL::Material::BLEND_MULTIPLY:
            cmt->coords[i].mode = CS_FX_MULTIPLY;
            break;
          case A3DL::Material::BLEND_DOUBLE_MULTIPLY:
            cmt->coords[i].mode = CS_FX_MULTIPLY2;
            break;
          }
          try
          {
            if(mt->getShaded())
              cmt->coords[i].mode |= CS_FX_GOURAUD;
          }
          catch(...) { }
          cmt->coords[i].mode |= CS_FX_TILING;   // for software renderer :P
        }
      }
    }
  }

  vosa3dl->mainThreadTasks.push(cmt);

#if 0
  // now go through our parents and let them know that this material is ready
  const ParentSet& parents = getParents();
  for(ParentSet::const_iterator i = parents.begin(); i != parents.end(); i++) {
    Object3D* obj3d = meta_cast<Object3D*>((*i)->parent);
    if(obj3d)
      obj3d->setupMaterial();
  }
#endif

  return;
}

void csMetaMaterial::notifyPropertyChange(const PropertyEvent& event)
{
  try
  {
    vRef<ParentChildRelation> pcr = event.getProperty()->findParent(*this);
    if(pcr->getContextualName() == "a3dl:color")
    {
      // XXX
    }
  }
  catch(NoSuchObjectError) { }
  catch(AccessControlError) { }
  catch(RemoteError) { }
}

csRef<iMaterialWrapper> csMetaMaterial::getMaterialWrapper()
{
  return materialwrapper;
}

void csMetaMaterial::notifyChildInserted(VobjectEvent& event)
{
  if(event.getContextualName() == "a3dl:texture")
  {
    // TODO: can we just modify the material's texture list?
    //clearMaterial();
    //loadMaterial();
  }
  vRef<Property> p = meta_cast<Property>(event.getNewChild());
  if(p.isValid()) p->addPropertyListener(this);
}

void csMetaMaterial::notifyChildReplaced(VobjectEvent& event)
{
  notifyChildRemoved(event);
  notifyChildInserted(event);
}

void csMetaMaterial::notifyChildRemoved(VobjectEvent& event)
{
  if(event.getContextualName() == "a3dl:texture")
  {
    // TODO: can we just modify the material's texture list?
    //clearMaterial();
    //loadMaterial();
  }
  vRef<Property> p = meta_cast<Property>(event.getOldChild());
  if(p.isValid()) p->removePropertyListener(this);
}

MetaObject* csMetaMaterial::new_csMetaMaterial(VobjectBase* superobject, const std::string& type)
{
  return new csMetaMaterial(superobject);
}
