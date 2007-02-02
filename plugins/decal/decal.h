/*
    Copyright (C) 2006 by Andrew Robberts

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

#ifndef __CS_DECAL_H__
#define __CS_DECAL_H__

#include "iutil/comp.h"
#include "igeom/decal.h"
#include "csutil/scf_implementation.h"
#include "csgeom/vector3.h"
#include "iengine/sector.h"
#include "iengine/engine.h"
#include "csgeom/poly3d.h"
#include "csgeom/tri.h"
#include "ivaria/collider.h"
#include "csgfx/renderbuffer.h"
#include "ivideo/rendermesh.h"
#include "iutil/array.h"
#include "iutil/eventh.h"
#include "csutil/eventnames.h"
#include "iutil/virtclk.h"

#define CS_DECAL_CLIP_DECAL
#define CS_DECAL_MAX_TRIS_PER_DECAL         64 
#define CS_DECAL_MAX_VERTS_PER_DECAL        128 

struct csDecalRenderMeshInfo
{
  csRenderMesh* pRenderMesh;
  csRef<iMeshWrapper>	mesh;
};

class csDecal : public iDecal, iDecalBuilder
{
private:
  iObjectRegistry*              objectReg;
  csRef<iEngine>                engine;

  // buffers and rendermesh to hold the rendering data
  csRef<csRenderBuffer>         	vertexBuffer;
  csRef<csRenderBuffer>         	texCoordBuffer;
  csRef<csRenderBuffer>         	normalBuffer;
  csRef<csRenderBuffer>         	indexBuffer;
  csRef<csRenderBufferHolder>   	bufferHolder;
  csArray<csRenderMesh*>        	renderMeshes;
  csArray<csDecalRenderMeshInfo>	renderMeshInfos;

  csRef<iDecalTemplate>         	decalTemplate;

  float					life;

  // used to keep track of the next open slot in our buffers and what to render
  size_t                        indexCount;
  size_t                        vertexCount;

  // some worldspace values defining this decal
  csVector3                     normal;
  csVector3                     right;
  csVector3                     up;
  csVector3                     pos;

  // dimensions of the polygon
  float                         width;
  float                         height;
  float				invWidth;
  float				invHeight;

  // radius is the length from the center of the decal to a corner
  float                         radius;

  // some settings for the current mesh
  iMeshWrapper*                 currMesh;
  size_t                        firstIndex;
  csVector3                     localNormal;
  csVector3                     localUp;
  csVector3                     localRight;
  csVector3                     vertOffset;
  csVector3                     relPos;
  csPlane3                      clipPlanes[6];
  
public:
  csDecal(iObjectRegistry * objectReg, iDecalManager * pManager);
  virtual ~csDecal();

  void Initialize(iDecalTemplate * decalTemplate, 
      const csVector3 & normal, const csVector3 & pos, const csVector3& up, 
      const csVector3 & right, float width, float height);
  
  void BeginMesh(iMeshWrapper * mesh);
  virtual void AddStaticPoly(const csPoly3D & p);
  void EndMesh();

  bool Age(csTicks ticks);
};

#endif // __CS_DECAL_H__
