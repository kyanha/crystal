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

#include "csextern.h"

#include "csutil/refcount.h"
#include "csutil/strhash.h"
#include "iutil/string.h"
#include "csgeom/vector2.h"
#include "csgeom/vector3.h"
#include "csgeom/vector4.h"
#include "csgfx/rgbpixel.h"
#include "iengine/texture.h"
#include "ivideo/texture.h"
#include "csutil/refarr.h"
#include "ivideo/rndbuf.h"

struct iTextureHandle;
struct iTextureWrapper;
struct csShaderVariableWrapper;

class csShaderVariable;

SCF_VERSION (iShaderVariableAccessor, 0, 0, 1);
/**
* Interface to an accessorcallback for shadervariables.
* This is used when we know the object providing the value of a single
* variable, but the exact value cannot be predetermined.
*/
struct iShaderVariableAccessor : public iBase
{
  /// Called before the value of the attached SV is returned
  virtual void PreGetValue (csShaderVariable *variable) = 0;
};


/**
 * Storage class for inheritable variables in the shader system.
 */
class CS_CSGFX_EXPORT csShaderVariable : public csRefCount
{
public:
  /// Data types able to be stored
  enum VariableType
  {
    INT = 1,
    FLOAT,
    COLOR,
    TEXTURE,
    RENDERBUFFER,
    VECTOR2,
    VECTOR3,
    VECTOR4,
    MATRIX
  };

private:

  VariableType Type;

  int Int;
  csRef<iTextureHandle> TextureHandValue;
  csRef<iTextureWrapper> TextureWrapValue;
  csRef<iRenderBuffer> RenderBuffer;
  csVector4 VectorValue;
  csMatrix3* MatrixValuePtr;

  csRef<iShaderVariableAccessor> accessor;

public:
  csStringID Name;

  /// Constructor
  csShaderVariable (csStringID name);
  virtual ~csShaderVariable ()
  {
    delete MatrixValuePtr;
  }

  csShaderVariable& operator= (csShaderVariable& copyFrom);

  /// Get type of data stored
  VariableType GetType() const { return Type; }
  /// Set type (calling this after SetValue will cause undefined behaviour)
  void SetType (VariableType t) { Type = t; }

  /// Set an accessor to use when getting the value
  void SetAccessor (iShaderVariableAccessor* a) { accessor = a;}

  /// Get the name of the variable
  csStringID GetName () const { return Name; }

  /// Retireve an int
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
    value.red = (char) VectorValue.x;
    value.green = (char) VectorValue.y;
    value.blue = (char) VectorValue.z;
    value.alpha = (char) VectorValue.w;
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

  /// Retireve a csVector2
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

  /// Retrieve a csVector4
  bool GetValue (csVector4& value)
  { 
    if (accessor) accessor->PreGetValue (this);
    value = VectorValue; 
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
    VectorValue.x = (float) value.red;
    VectorValue.y = (float) value.green;
    VectorValue.z = (float) value.blue;
    VectorValue.w = (float) value.alpha;
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
    VectorValue.Set (value.x, value.y, 1.0f, 1.0f);
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

  /// Store a csVector4
  bool SetValue (const csVector4 &value)
  { 
    Type = VECTOR4; 
    VectorValue.Set (value.x, value.y, value.z, value.w);
    Int = (int)value.x;
    return true; 
  }

  /// Store a csMatrix3
  bool SetValue (const csMatrix3 &value)
  {
    Type = MATRIX;
    delete MatrixValuePtr;
    MatrixValuePtr = new csMatrix3 (value);
    return true;
  }
};

#endif
