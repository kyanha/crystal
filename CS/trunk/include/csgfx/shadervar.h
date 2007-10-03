/*
    Copyright (C) 2003 by Mat Sutcliffe <oktal@gmx.co.uk>
                          Marten Svanfeldt

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

#ifndef __CS_GFX_SHADERVAR_H__
#define __CS_GFX_SHADERVAR_H__

/**\file
 * Shader variable.
 */
 
#include "csextern.h"

#include "csgeom/math.h"
#include "csgeom/quaternion.h"
#include "csgeom/transfrm.h"
#include "csgeom/vector2.h"
#include "csgeom/vector3.h"
#include "csgeom/vector4.h"
#include "csgfx/rgbpixel.h"
#include "csutil/cscolor.h"
#include "csutil/leakguard.h"
#include "csutil/refarr.h"
#include "csutil/refcount.h"
#include "csutil/strset.h"

#include "iengine/texture.h"
#include "ivideo/texture.h"
#include "ivideo/rndbuf.h"

struct iTextureHandle;
struct iTextureWrapper;
struct csShaderVariableWrapper;

class csShaderVariable;

/**\addtogroup gfx3d
 * @{ */

/**
 * Interface to an accessorcallback for shadervariables.
 * This is used when we know the object providing the value of a single
 * variable, but the exact value cannot be predetermined.
 */
struct iShaderVariableAccessor : public virtual iBase
{
  SCF_INTERFACE (iShaderVariableAccessor, 2, 0, 0);

  /// Called before the value of the attached SV is returned
  virtual void PreGetValue (csShaderVariable *variable) = 0;
};

/**
 * Storage class for "shader vars", inheritable variables in the shader 
 * system. Shader vars are a primary system to transport information from
 * the engine/meshes/etc. to the renderer.
 */
class CS_CRYSTALSPACE_EXPORT csShaderVariable : public csRefCount
{
public:
  /**
   * Data types that can be stored.
   * Data storage and retrieval is not strict - data stored as INT, FLOAT 
   * or any VECTORx data can also be retrieved as any other of those.
   */
  enum VariableType
  {
    /// No value was yet set, hence the type is unknown.
    UNKNOWN = 0,
    /// Integer
    INT = 1,
    /// Float
    FLOAT,
    /// Texture
    TEXTURE,
    /// Renderbuffer
    RENDERBUFFER,
    /// Vector with 2 components
    VECTOR2,
    /// Vector with 3 components
    VECTOR3,
    /// Vector with 4 components
    VECTOR4,
    /// Matrix
    MATRIX,
    /// Transform
    TRANSFORM,
    /// Array
    ARRAY,
    
    /**
     * Color
     * \deprecated Same as VECTOR4.
     */
    COLOR = VECTOR4
  };

  //CS_LEAKGUARD_DECLARE (csShaderVariable);
private:

  VariableType Type;

  csRef<iTextureHandle> TextureHandValue;
  csRef<iTextureWrapper> TextureWrapValue;
  csRef<iRenderBuffer> RenderBuffer;
  csVector4 VectorValue;

  int Int;
  csMatrix3* MatrixValuePtr;
  csReversibleTransform* TransformPtr;

  csRef<iShaderVariableAccessor> accessor;

  csRefArray<csShaderVariable> *array;

  csStringID Name;
public:

  /**
   * Construct without a name. SetName() must be called before the variable
   * can be used.
   */
  csShaderVariable ();
  /// Construct with name.
  csShaderVariable (csStringID name);
  csShaderVariable (const csShaderVariable& other) : csRefCount(),
    MatrixValuePtr(0), TransformPtr (0), array(0) { *this = other; }
  virtual ~csShaderVariable ()
  {
    delete MatrixValuePtr;
    delete TransformPtr;
    delete array;
  }

  csShaderVariable& operator= (const csShaderVariable& copyFrom);

  /// Get type of data stored
  VariableType GetType() 
  { 
    /* The accessor should be called at least once so the var has a proper
     * type set */
    if ((Type == UNKNOWN) && accessor) accessor->PreGetValue (this);
    return Type; 
  }
  /// Set type (calling this after SetValue will cause undefined behaviour)
  void SetType (VariableType t) { Type = t; }

  /// Set an accessor to use when getting the value
  void SetAccessor (iShaderVariableAccessor* a) { accessor = a;}

  /**
   * Set the name of the variable
   * \warning Changing the name of a variable while it's in use can cause 
   *    inexpected behaviour.
   */
  void SetName (csStringID newName) { Name = newName; }
  
  /// Get the name of the variable
  csStringID GetName () const { return Name; }

  /// Retrieve an int
  bool GetValue (int& value)
  { 
    if (accessor) accessor->PreGetValue (this);
    value = Int; 
    return true; 
  }

  /// Retrieve a float
  bool GetValue (float& value)
  { 
    if (accessor) accessor->PreGetValue (this);
    value = VectorValue.x; 
    return true; 
  }

  /// Retrieve a color
  bool GetValue (csRGBpixel& value)
  {
    if (accessor) accessor->PreGetValue (this);
    value.red = 
      (unsigned char) csClamp (int (VectorValue.x * 255.0f), 255, 0);
    value.green = 
      (unsigned char) csClamp (int (VectorValue.y * 255.0f), 255, 0);
    value.blue = 
      (unsigned char) csClamp (int (VectorValue.z * 255.0f), 255, 0);
    value.alpha = 
      (unsigned char) csClamp (int (VectorValue.w * 255.0f), 255, 0);;
    return true;
  }

  /// Retrieve a texture handle
  bool GetValue (iTextureHandle*& value)
  {
    if (accessor) accessor->PreGetValue (this);
    value = TextureHandValue;
    if (!value && TextureWrapValue)
      value = TextureHandValue = TextureWrapValue->GetTextureHandle ();
    return true;
  }

  /// Retrieve a texture wrapper
  bool GetValue (iTextureWrapper*& value)
  {
    if (accessor) accessor->PreGetValue (this);
    value = TextureWrapValue;
    return true;
  }

  /// Retrieve a iRenderBuffer
  bool GetValue (iRenderBuffer*& value)
  {
    if (accessor) accessor->PreGetValue (this);
    value = RenderBuffer;
    return true;
  }

  /// Retrieve a csVector2
  bool GetValue (csVector2& value)
  {
    if (accessor) accessor->PreGetValue (this);
    value.Set (VectorValue.x, VectorValue.y);
    return true;
  }

  /// Retrieve a csVector3
  bool GetValue (csVector3& value)
  { 
    if (accessor) accessor->PreGetValue (this);
    value.Set (VectorValue.x, VectorValue.y, VectorValue.z);
    return true; 
  }

  /// Retrieve a csColor
  bool GetValue (csColor& value)
  { 
    if (accessor) accessor->PreGetValue (this);
    value.Set (VectorValue.x, VectorValue.y, VectorValue.z);
    return true; 
  }

  /// Retrieve a csVector4
  bool GetValue (csVector4& value)
  { 
    if (accessor) accessor->PreGetValue (this);
    value = VectorValue; 
    return true; 
  }

  /// Retrieve a csQuaternion
  bool GetValue (csQuaternion& value)
  { 
    if (accessor) accessor->PreGetValue (this);
    value.Set (VectorValue.x, VectorValue.y, VectorValue.z, VectorValue.w);
    return true; 
  }

  /// Retrieve a csMatrix3
  bool GetValue (csMatrix3& value)
  {
    if (accessor) accessor->PreGetValue (this);
    if (MatrixValuePtr)
    {
      value = *MatrixValuePtr;
      return true;
    }
    else
    {
      value = csMatrix3();
    }
    return false;
  }

  /// Retrieve a csReversibleTransform
  bool GetValue (csReversibleTransform& value)
  {
    if (accessor) accessor->PreGetValue (this);
    if (TransformPtr)
    {
      value = *TransformPtr;
      return true;
    }
    else
    {
      value = csReversibleTransform();
    }
    return false;
  }


  /// Store an int
  bool SetValue (int value) 
  { 
    Type = INT; 
    Int = value; 
    float f = (float)value;
    VectorValue.Set (f, f, f, f);
    return true; 
  }

  /// Store a float
  bool SetValue (float value)
  { 
    Type = FLOAT; 
    Int = (int)value;
    VectorValue.Set (value, value, value, value);
    return true; 
  }

  /// Store a color
  bool SetValue (const csRGBpixel &value)
  {
    Type = COLOR;
    VectorValue.x = (float)value.red / 255.0f;
    VectorValue.y = (float)value.green / 255.0f;
    VectorValue.z = (float)value.blue / 255.0f;
    VectorValue.w = (float)value.alpha / 255.0f;
    return true;
  }

  /// Store a texture handle
  bool SetValue (iTextureHandle* value)
  {
    Type = TEXTURE;
    TextureHandValue = value;
    return true;
  }

  /// Store a texture wrapper
  bool SetValue (iTextureWrapper* value)
  {
    Type = TEXTURE;
    TextureWrapValue = value;
    return true;
  }

  /// Store a render buffer
  bool SetValue (iRenderBuffer* value)
  {
    Type = RENDERBUFFER;
    RenderBuffer = value;
    return true;
  }

  /// Store a csVector2
  bool SetValue (const csVector2 &value)
  {
    Type = VECTOR2;
    VectorValue.Set (value.x, value.y, 0.0f, 1.0f);
    Int = (int)value.x;
    return true;
  }

  /// Store a csVector3
  bool SetValue (const csVector3 &value)
  { 
    Type = VECTOR3; 
    VectorValue.Set (value.x, value.y, value.z, 1.0f);
    Int = (int)value.x;
    return true; 
  }

  /// Store a csColor
  bool SetValue (const csColor& value)
  { 
    Type = VECTOR3; 
    VectorValue.Set (value.red, value.green, value.blue, 1.0f);
    Int = (int)value.red;
    return true; 
  }

  /// Store a csVector4
  bool SetValue (const csVector4 &value)
  { 
    Type = VECTOR4; 
    VectorValue.Set (value.x, value.y, value.z, value.w);
    Int = (int)value.x;
    return true; 
  }

  bool SetValue (const csQuaternion& value)
  {
    Type = VECTOR4;
    VectorValue.Set (value.v.x, value.v.y, value.v.z, value.w);
    return true;
  }

  /// Store a csMatrix3
  bool SetValue (const csMatrix3 &value)
  {
    Type = MATRIX;
    if (MatrixValuePtr)
    {
      *MatrixValuePtr = value;
    }
    else
    {
      MatrixValuePtr = new csMatrix3 (value);
    }
    return true;
  }

  /// Store a csReversibleTransform
  bool SetValue (const csReversibleTransform &value)
  {
    Type = TRANSFORM;
    if (TransformPtr)
    {
      *TransformPtr = value;
    }
    else
    {
      TransformPtr = new csReversibleTransform (value);
    }
    return true;
  }

  void AddVariableToArray (csShaderVariable *variable)
  {
    if (array) array->Push (variable);
  }

  void RemoveFromArray (size_t element)
  {
    if (array) array->DeleteIndex (element);
  }
#include "csutil/custom_new_disable.h"
  /// Set the number of elements in an array variable
  void SetArraySize (size_t size)
  {
    if (array == 0)
    {
      array = new csRefArray<csShaderVariable>;
    }
    array->SetSize (size);
  }
#include "csutil/custom_new_enable.h"

  /// Get the number of elements in an array variable
  size_t GetArraySize ()
  {
    if (array == 0)
      return 0;
    else
      return array->GetSize ();
  }

  /**
   * Get a specific element in an array variable
   * Do not hold on to this for long, since it might change if
   * the array size changes.
   */
  csShaderVariable *GetArrayElement (size_t element)
  {
    if (array != 0 && element<array->GetSize ())
    {
      return array->Get (element);
    }
    return 0;
  }

  /**
   * Set a specific element in an array variable
   */
  void SetArrayElement (size_t element, csShaderVariable *variable)
  {
    array->Put (element, variable);
  }
};

namespace CS
{
  /// Helper class to obtain an ID for a shader variable.
  struct ShaderVarName
  {
    csStringID name;
    
    ShaderVarName() : name (csInvalidStringID) {}
    ShaderVarName (iStringSet* strings, const char* name) 
    { this->name = strings->Request (name); }
    
    operator csStringID () const { return name; }
  };
  
} // namespace CS

/** @} */

#endif
