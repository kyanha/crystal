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

#ifndef __CS_MATERIAL_H__
#define __CS_MATERIAL_H__

#include "csgfx/rgbpixel.h"
#include "csutil/scf_implementation.h"
#include "csutil/csobject.h"
#include "csutil/nobjvec.h"
#include "csutil/hash.h"
#include "csutil/leakguard.h"
#include "ivideo/material.h"
#include "iengine/material.h"

#include "plugins/engine/3d/engine.h"

struct iTextureWrapper;
struct iTextureManager;

/**
 * A material class.
 */
class csMaterial : public iMaterial
{
private:
  friend class csEngine;

  /// flat shading color
  csRGBcolor flat_color;

  /// Shader associated with material
  csHash<csRef<iShader>, csStringID> shaders;
  csEngine* engine;
  csShaderVariable* GetVar (csStringID name, bool create = false);


  static csStringID nameDiffuseParam;
  static csStringID nameAmbientParam;
  static csStringID nameReflectParam;
  static csStringID nameFlatColorParam;
  static csStringID nameDiffuseTexture;

  csShaderVariableContext svcontext;

public:
  CS_LEAKGUARD_DECLARE (csMaterial);

  /**
   * create an empty material
   */
  csMaterial (csEngine* engine);
  /**
   * create a material with only the texture given.
   */
  csMaterial (csEngine* engine,
    iTextureWrapper *txt);

  /**
   * destroy material
   */
  virtual ~csMaterial ();

  /// Get the flat shading color
  csRGBcolor& GetFlatColor ();

  /// Get diffuse reflection constant for the material
  float GetDiffuse ();
  /// Set diffuse reflection constant for the material
  void SetDiffuse (float val);

  /// Get ambient lighting for the material
  float GetAmbient ();
  /// Set ambient lighting for the material
  void SetAmbient (float val);

  /// Get reflection of the material
  float GetReflection ();
  /// Set reflection of the material
  void SetReflection (float val);

  /// Get the base diffuse texture (if none 0 is returned)
  iTextureWrapper* GetTextureWrapper ()
  {
    return GetTextureWrapper (nameDiffuseTexture);
  }

  /// Set the base diffuse texture (pass 0 to set no texture)
  void SetTextureWrapper (iTextureWrapper* tex);

  /// Set a texture (pass 0 to set no texture)
  void SetTextureWrapper (csStringID name, iTextureWrapper* tex);

  /// Get a texture (if none 0 is returned)
  iTextureWrapper* GetTextureWrapper (csStringID name);

  //--------------------- iMaterial implementation ---------------------

  /// Associate a shader with a shader type
  virtual void SetShader (csStringID type, iShader* shader);
  /// Get shader associated with a shader type
  virtual iShader* GetShader (csStringID type);

  /// Get all Shaders
  const csHash<csRef<iShader>, csStringID>& GetShaders() const
  { return shaders; }

  /// Get texture.
  virtual iTextureHandle* GetTexture ();
  /**
   * Get a texture from the material.
   */
  virtual iTextureHandle *GetTexture (csStringID name);

  /// Get flat color.
  virtual void GetFlatColor (csRGBpixel &oColor, bool useTextureMean = true);
  /// Set the flat shading color
  virtual void SetFlatColor (const csRGBcolor& col);
  /// Get reflection values (diffuse, ambient, reflection).
  virtual void GetReflection (float &oDiffuse, float &oAmbient,
    float &oReflection);
  /// Set reflection values (diffuse, ambient, reflection).
  virtual void SetReflection (float oDiffuse, float oAmbient,
    float oReflection);

  /**
   * Visit all textures.
   */
  void Visit ();
  bool IsVisitRequired () const;

  SCF_DECLARE_IBASE;

  /// iMaterialEngine implementation
  struct MaterialEngine : public iMaterialEngine
  {
    SCF_DECLARE_EMBEDDED_IBASE (csMaterial);
    virtual iTextureWrapper *GetTextureWrapper ()
    {
      return scfParent->GetTextureWrapper ();
    }
    virtual iTextureWrapper* GetTextureWrapper (csStringID name)
    {
      return scfParent->GetTextureWrapper (name);
    }
    virtual void Visit ()
    {
      scfParent->Visit ();
    }
    virtual bool IsVisitRequired () const
    {
      return scfParent->IsVisitRequired ();
    }
  } scfiMaterialEngine;
  friend struct MaterialEngine;


  //=================== iShaderVariableContext ================//

  /// Add a variable to this context
  void AddVariable (csShaderVariable *variable)
  { svcontext.AddVariable (variable); }

  /// Get a named variable from this context
  csShaderVariable* GetVariable (csStringID name) const
  { return svcontext.GetVariable (name); }

  /// Get Array of all ShaderVariables
  const csRefArray<csShaderVariable>& GetShaderVariables () const
  { return svcontext.GetShaderVariables (); }

  /**
  * Push the variables of this context onto the variable stacks
  * supplied in the "stacks" argument
  */
  void PushVariables (csShaderVarStack &stacks) const
  { svcontext.PushVariables (stacks); }

  bool IsEmpty () const 
  { return svcontext.IsEmpty(); }
  
  void ReplaceVariable (csShaderVariable *variable)
  { svcontext.ReplaceVariable (variable); }
  void Clear () { svcontext.Clear(); }
};

/**
 * csMaterialWrapper represents a texture and its link
 * to the iMaterialHandle as returned by iTextureManager.
 */
class csMaterialWrapper : public csObject
{
private:
  /// The corresponding iMaterial.
  csRef<iMaterial> material;
  /// The corresponding iMaterialEngine.
  csRef<iMaterialEngine> matEngine;

private:
  /// Release material handle
  virtual ~csMaterialWrapper ();

public:
  CS_LEAKGUARD_DECLARE (csMaterialWrapper);

  /// Construct a material handle given a material.
  csMaterialWrapper (iMaterial* Image);
  /// Copy constructor
  csMaterialWrapper (csMaterialWrapper &);

  /**
   * Change the base material. Note: The changes will not be visible until
   * you re-register the material.
   */
  void SetMaterial (iMaterial* material);
  /// Get the original material.
  iMaterial* GetMaterial () { return material; }

  /**
   * Visit this material. This should be called by the engine right
   * before using the material. It will call Visit() on all textures
   * that are used.
   */
  void Visit ();
  bool IsVisitRequired () const;

  SCF_DECLARE_IBASE_EXT (csObject);

  //------------------- iMaterialWrapper implementation -----------------------
  struct MaterialWrapper : public iMaterialWrapper
  {
    SCF_DECLARE_EMBEDDED_IBASE (csMaterialWrapper);
    virtual iObject *QueryObject()
    { return scfParent; }
    virtual iMaterialWrapper *Clone () const
    { return &(new csMaterialWrapper (*scfParent))->scfiMaterialWrapper; }
    virtual void SetMaterial (iMaterial* material)
    { scfParent->SetMaterial (material); }
    virtual iMaterial* GetMaterial ()
    { return scfParent->GetMaterial (); }
    virtual void Visit ()
    { scfParent->Visit (); }
    virtual bool IsVisitRequired () const
    {
      return scfParent->IsVisitRequired ();
    }
  } scfiMaterialWrapper;
  friend struct MaterialWrapper;
};

/**
 * This class is used to hold a list of materials.
 */
class csMaterialList : public iMaterialList
{
private:
  csRefArrayObject<iMaterialWrapper> list;
  csHash<iMaterialWrapper*,csStrKey> mat_hash;

  class NameChangeListener : public scfImplementation1<NameChangeListener,
  	iObjectNameChangeListener>
  {
  private:
    csWeakRef<csMaterialList> list;

  public:
    NameChangeListener (csMaterialList* list) : scfImplementationType (this),
  	  list (list)
    {
    }
    virtual ~NameChangeListener () { }

    virtual void NameChanged (iObject* obj, const char* oldname,
  	  const char* newname)
    {
      if (list)
        list->NameChanged (obj, oldname, newname);
    }
  };
  csRef<NameChangeListener> listener;

public:
  /// Initialize the array
  csMaterialList ();
  virtual ~csMaterialList ();

  SCF_DECLARE_IBASE;

  void NameChanged (iObject* object, const char* oldname,
  	const char* newname);

  virtual iMaterialWrapper* NewMaterial (iMaterial* material,
  	const char* name);
  virtual int GetCount () const { return (int)list.Length (); }
  virtual iMaterialWrapper *Get (int n) const { return list[n]; }
  virtual int Add (iMaterialWrapper *obj);
  virtual bool Remove (iMaterialWrapper *obj);
  virtual bool Remove (int n);
  virtual void RemoveAll ();
  virtual int Find (iMaterialWrapper *obj) const;
  virtual iMaterialWrapper *FindByName (const char *Name) const;
};

#endif // __CS_MATERIAL_H__
