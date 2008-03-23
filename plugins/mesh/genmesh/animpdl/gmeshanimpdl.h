/*
    Copyright (C) 2007 by Jorrit Tyberghein
              (C) 2007 by Frank Richter

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

#ifndef __CS_GENMESHANIM_H__
#define __CS_GENMESHANIM_H__

#include "csgeom/transfrm.h"
#include "csgeom/vector3.h"
#include "csutil/array.h"
#include "csutil/cscolor.h"
#include "csutil/csstring.h"
#include "csutil/dirtyaccessarray.h"
#include "csutil/parray.h"
#include "csutil/strhash.h"
#include "csutil/stringarray.h"
#include "csutil/scf_implementation.h"
#include "csutil/weakref.h"
#include "iengine/light.h"
#include "imesh/genmesh.h"
#include "imesh/gmeshanim.h"
#include "imesh/lighting.h"
#include "iutil/comp.h"
#include "iutil/eventh.h"
#include "ivideo/rndbuf.h"

struct iCacheManager;

CS_PLUGIN_NAMESPACE_BEGIN(GMeshAnimPDL)
{

class GenmeshAnimationPDLType;
class GenmeshAnimationPDLFactory;

class GenmeshAnimationPDLFactory :
  public scfImplementation1<GenmeshAnimationPDLFactory,
    iGenMeshAnimationControlFactory>
{
private:
  friend class GenmeshAnimationPDL;

  csRef<GenmeshAnimationPDLType> type;

  csString parseError;
  csStringHash xmltokens;
#define CS_TOKEN_ITEM_FILE "plugins/mesh/genmesh/animpdl/gmeshanimpdl.tok"
#include "cstool/tokenlist.h"

  struct ColorBuffer
  {
    csStringID name;

    struct MappedLight
    {
      struct LightIdentity : public CS::Memory::CustomAllocated
      {
        csString sectorName, lightName;
        uint8 lightId[16];
      };
      LightIdentity* lightId;
      csRef<iRenderBuffer> colors;

      MappedLight() : lightId (0) {}
      MappedLight(const MappedLight& other)
      {
        if (other.lightId != 0)
        {
          lightId = new LightIdentity (*(other.lightId));
        }
        else
          lightId = 0;
        colors = other.colors;
      }
      ~MappedLight() { delete lightId; }

      void FreeLightId() { delete lightId; lightId = 0; }
    };
    csArray<MappedLight> lights;
    csRef<iRenderBuffer> staticColors;

    ColorBuffer () : name (0) {}
  };
  csArray<ColorBuffer> buffers;

  void Report (iSyntaxService* synsrv, int severity, iDocumentNode* node, 
    const char* msg, ...);
  bool HexToLightID (uint8* lightID, const char* lightIDHex);
  const char* ParseBuffer (iSyntaxService* synsrv,
    ColorBuffer& buffer, iDocumentNode* node);
  const char* ParseLight (ColorBuffer::MappedLight& light,
    iSyntaxService* synsrv, iDocumentNode* node);

  const char* ValidateBufferSizes(const ColorBuffer& buffer);
public:
  /// Constructor.
  GenmeshAnimationPDLFactory (GenmeshAnimationPDLType* type);
  /// Destructor.
  virtual ~GenmeshAnimationPDLFactory ();

  /**\name iGenMeshAnimationControlFactory implementation
   * @{ */
  virtual csPtr<iGenMeshAnimationControl> CreateAnimationControl (
  	iMeshObject *mesh);     
  virtual const char* Load (iDocumentNode* node);
  virtual const char* Save (iDocumentNode* parent);
  /** @} */
};

/**
 * Genmesh animation control.
 */
class GenmeshAnimationPDL :
  public scfImplementation3<GenmeshAnimationPDL,
    iGenMeshAnimationControl, 
    iGenMeshAnimationControl1_4, 
    iLightingInfo>
{
private:
  csRef<GenmeshAnimationPDLFactory> factory;
  iGeneralMeshState* genmesh;

  struct ColorBuffer
  {
    csStringID name;
    bool lightsDirty;
    uint32 lastMeshVersion;
    csRef<iRenderBuffer> rbuf;

    struct MappedLight
    {
      csWeakRef<iLight> light;
      csRef<iRenderBuffer> colors;
    };
    csSafeCopyArray<MappedLight> lights;
    csRef<iRenderBuffer> staticColors;
    csDirtyAccessArray<csColor4> combinedColors;

    ColorBuffer() : name (0), lightsDirty (true), lastMeshVersion (~0) {}
  };

  bool prepared;

  ColorBuffer colorsBuffer;
  csArray<ColorBuffer> buffers;

  void PrepareBuffer (iEngine* engine,
    const GenmeshAnimationPDLFactory::ColorBuffer& factoryBuf,
    ColorBuffer& buffer);

  void Prepare();
  void UpdateBuffer (ColorBuffer& buffer, csTicks current, 
    const csColor4* colors, int num_colors, uint32 version_id);
public:
  /// Constructor.
  GenmeshAnimationPDL (GenmeshAnimationPDLFactory* fact,
    iGeneralMeshState* genmesh);
  /// Destructor.
  virtual ~GenmeshAnimationPDL ();

  /**\name iGenMeshAnimationControl implementation
   * @{ */
  virtual bool AnimatesVertices () const { return false; }
  virtual bool AnimatesTexels () const { return false; }
  virtual bool AnimatesNormals () const { return false; }
  virtual bool AnimatesColors () const { return true; }
  virtual void Update(csTicks current) { }
  virtual void Update(csTicks current, int num_verts, uint32 version_id);
  virtual const csVector3* UpdateVertices (csTicks current,
  	const csVector3* verts, int num_verts, uint32 version_id);
  virtual const csVector2* UpdateTexels (csTicks current,
  	const csVector2* texels, int num_texels, uint32 version_id);
  virtual const csVector3* UpdateNormals (csTicks current,
  	const csVector3* normals, int num_normals, uint32 version_id);
  virtual const csColor4* UpdateColors (csTicks current,
  	const csColor4* colors, int num_colors, uint32 version_id);
  /** @} */

  /**\name iLightingInfo implementation
   * @{ */
  void DisconnectAllLights ()
  { 
    colorsBuffer.lightsDirty = true;
    colorsBuffer.lights.DeleteAll ();
    for (size_t i = 0; i < buffers.GetSize(); i++)
    {
      buffers[i].lightsDirty = true;
      buffers[i].lights.DeleteAll ();
    }
  }
  void InitializeDefault (bool /*clear*/) {}
  void LightChanged (iLight* /*light*/) 
  {
    colorsBuffer.lightsDirty = true;
    for (size_t i = 0; i < buffers.GetSize(); i++)
    {
      buffers[i].lightsDirty = true;
    }
  }
  void LightDisconnect (iLight* light);
  void PrepareLighting () {}
  bool ReadFromCache (iCacheManager* /*cache_mgr*/) { return true; }
  bool WriteToCache (iCacheManager* /*cache_mgr*/) { return true; }
  /** @} */
};

/**
 * Genmesh animation control type.
 */
class GenmeshAnimationPDLType :
  public scfImplementation2<GenmeshAnimationPDLType,
    iGenMeshAnimationControlType, iComponent>
{
private:
  friend class GenmeshAnimationPDLFactory;
  friend class GenmeshAnimationPDL;
  iObjectRegistry* object_reg;
public:
  csRef<iStringSet> strings;
  csStringID colorsID;

  /// Constructor.
  GenmeshAnimationPDLType (iBase*);
  /// Destructor.
  virtual ~GenmeshAnimationPDLType ();
  /// Initialize.
  bool Initialize (iObjectRegistry* object_reg);

  virtual csPtr<iGenMeshAnimationControlFactory> 
    CreateAnimationControlFactory ();

  void Report (int severity, const char* msg, ...);
};

}
CS_PLUGIN_NAMESPACE_END(GMeshAnimPDL)

#endif // __CS_GENMESHANIM_H__
