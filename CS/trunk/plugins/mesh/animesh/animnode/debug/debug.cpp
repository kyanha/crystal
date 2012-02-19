/*
  Copyright (C) 2010 Christian Van Brussel, Institute of Information
      and Communication Technologies, Electronics and Applied Mathematics
      at Universite catholique de Louvain, Belgium
      http://www.uclouvain.be/en-icteam.html

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public
  License along with this library; if not, write to the Free
  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "cssysdef.h"
#include "csutil/scf.h"

#include "debug.h"
#include "csqint.h"
#include "cstool/cspixmap.h"
#include "iutil/objreg.h"
#include "iengine/camera.h"
#include "iengine/engine.h"
#include "iengine/movable.h"
#include "iengine/mesh.h"
#include "iengine/scenenode.h"
#include "imesh/object.h"
#include "imesh/objmodel.h"
#include "imesh/animesh.h"
#include "imesh/genmesh.h"
#include "ivaria/reporter.h"
#include "ivideo/graph2d.h"
#include "ivideo/graph3d.h"
#include "cstool/materialbuilder.h"

CS_PLUGIN_NAMESPACE_BEGIN(DebugNode)
{
  SCF_IMPLEMENT_FACTORY(DebugNodeManager);

  // --------------------------  DebugNodeFactory  --------------------------

  CS_LEAKGUARD_IMPLEMENT(DebugNodeFactory);

  DebugNodeFactory::DebugNodeFactory (DebugNodeManager* manager, const char *name)
    : scfImplementationType (this), CS::Animation::SkeletonAnimNodeFactorySingle (name),
    manager (manager), modes (CS::Animation::DEBUG_SQUARES), image (nullptr), boneMaskUsed (false),
    leafBonesDisplayed (true), boneRandomColor (false)
  {
  }

  void DebugNodeFactory::SetDebugModes (CS::Animation::SkeletonDebugMode modes)
  {
    this->modes = modes;
  }

  CS::Animation::SkeletonDebugMode DebugNodeFactory::GetDebugModes ()
  {
    return modes;
  }

  void DebugNodeFactory::SetDebugImage (csPixmap* image)
  {
    this->image = image;
  }

  void DebugNodeFactory::SetBoneMask (csBitArray& boneMask)
  {
    boneMaskUsed = true;
    this->boneMask = boneMask;
  }

  void DebugNodeFactory::UnsetBoneMask ()
  {
    boneMaskUsed = false;
  }

  void DebugNodeFactory::SetLeafBonesDisplayed (bool displayed)
  {
    leafBonesDisplayed = displayed;
  }

  csPtr<CS::Animation::SkeletonAnimNodeSingleBase> DebugNodeFactory::ActualCreateInstance (
    CS::Animation::iSkeletonAnimPacket* packet,
    CS::Animation::iSkeleton* skeleton)
  {
    return csPtr<CS::Animation::SkeletonAnimNodeSingleBase> (new DebugNode (this, skeleton));
  }

  // --------------------------  DrawBox3D  --------------------------

  static inline void DrawBox3D (iGraphics3D* g3d,
				const csBox3& box,
				const csReversibleTransform& tr,
				int color)
  {
    csVector3 vxyz = tr * box.GetCorner (CS_BOX_CORNER_xyz);
    csVector3 vXyz = tr * box.GetCorner (CS_BOX_CORNER_Xyz);
    csVector3 vxYz = tr * box.GetCorner (CS_BOX_CORNER_xYz);
    csVector3 vxyZ = tr * box.GetCorner (CS_BOX_CORNER_xyZ);
    csVector3 vXYz = tr * box.GetCorner (CS_BOX_CORNER_XYz);
    csVector3 vXyZ = tr * box.GetCorner (CS_BOX_CORNER_XyZ);
    csVector3 vxYZ = tr * box.GetCorner (CS_BOX_CORNER_xYZ);
    csVector3 vXYZ = tr * box.GetCorner (CS_BOX_CORNER_XYZ);
    const CS::Math::Matrix4& projection (g3d->GetProjectionMatrix ());
    iGraphics2D* g2d = g3d->GetDriver2D ();
    g2d->Draw3DLine (vxyz, vXyz, projection, color);
    g2d->Draw3DLine (vXyz, vXYz, projection, color);
    g2d->Draw3DLine (vXYz, vxYz, projection, color);
    g2d->Draw3DLine (vxYz, vxyz, projection, color);
    g2d->Draw3DLine (vxyZ, vXyZ, projection, color);
    g2d->Draw3DLine (vXyZ, vXYZ, projection, color);
    g2d->Draw3DLine (vXYZ, vxYZ, projection, color);
    g2d->Draw3DLine (vxYZ, vxyZ, projection, color);
    g2d->Draw3DLine (vxyz, vxyZ, projection, color);
    g2d->Draw3DLine (vxYz, vxYZ, projection, color);
    g2d->Draw3DLine (vXyz, vXyZ, projection, color);
    g2d->Draw3DLine (vXYz, vXYZ, projection, color);
  }

  // --------------------------  DebugNode  --------------------------

  CS_LEAKGUARD_IMPLEMENT(DebugNode);

  DebugNode::DebugNode (DebugNodeFactory* factory, CS::Animation::iSkeleton* skeleton)
    : scfImplementationType (this),
    CS::Animation::SkeletonAnimNodeSingle<DebugNodeFactory> (factory, skeleton)
  {
  }

  void DebugNode::Draw (iCamera* camera, csColor color)
  {
    if (!factory->modes)
      return;

    csRef<iGraphics3D> g3d = csQueryRegistry<iGraphics3D> (factory->manager->GetObjectRegistry());
    csRef<iGraphics2D> g2d = g3d->GetDriver2D ();
    CS_ASSERT(g3d && g2d);

    // Tell the 3D driver we're going to display 2D things.
    if (!g3d->BeginDraw (CSDRAW_2DGRAPHICS))
      return;

    CS::Animation::iSkeletonFactory* fact = skeleton->GetFactory ();
    const CS::Math::Matrix4& projection (camera->GetProjectionMatrix ());
    csReversibleTransform object2camera =
      camera->GetTransform () / skeleton->GetSceneNode ()->GetMovable ()->GetFullTransform ();

    // Setup the "end" positions and child count of all bones
    const CS::Animation::BoneID lastId = fact->GetTopBoneID ();
    csArray<csVector3> childPos;
    csArray<int> numChild;

    childPos.SetSize (lastId+1, csVector3 (0));
    numChild.SetSize (lastId+1, 0);

    for (CS::Animation::BoneID i = 0; i < lastId+1; ++i)
    {
      if (!fact->HasBone (i))
	continue;

      CS::Animation::BoneID parent = fact->GetBoneParent (i);
      if (parent != CS::Animation::InvalidBoneID)
      {
	csQuaternion q;
	csVector3 v;
	skeleton->GetTransformBoneSpace (i, q, v);

	childPos[parent] += v;
	numChild[parent] += 1;
      }
    }

    // Now draw the bones and their bounding boxes
    for (CS::Animation::BoneID i = 0; i <= lastId; i++)
    {
      if (!fact->HasBone (i)
	  || (factory->boneMaskUsed
	      && (factory->boneMask.GetSize () <= i
		  || !factory->boneMask.IsBitSet (i)))
	  || (!factory->leafBonesDisplayed && numChild[i] == 0))
	continue;

      // Set the color of the bone and its bounding box
      int colorI;
      if (!factory->boneRandomColor) 
	colorI = g2d->FindRGB (((int) color[0] * 255.0f),
			       ((int) color[1] * 255.0f),
			       ((int) color[2] * 255.0f));
      else
      {
	int ii = (i+1)*(i+1);
	colorI = g2d->FindRGB (255 - (ii % 255),
			       255 - ((ii*(i+1)) % 255),
			       255 - ((ii*ii) % 255));
      }

      csQuaternion rotation;
      csVector3 position;
      skeleton->GetTransformAbsSpace (i, rotation, position);

      csVector3 bonePosition = object2camera * position;

      if (factory->modes & CS::Animation::DEBUG_IMAGES
	  && factory->image)
      {
	if (bonePosition < SMALL_Z)
	  continue;

	csVector4 v1p (projection * csVector4 (bonePosition));
	v1p /= v1p.w;

	int px1 = csQint ((v1p.x + 1) * (g2d->GetWidth() / 2));
	int py1 = g2d->GetHeight () - 1 - csQint ((v1p.y + 1) * (g2d->GetHeight () / 2));
 
	// TODO: images are not drawn correctly

	factory->image->Draw (g3d, px1 - factory->image->Width () / 2,
			      py1 - factory->image->Height () / 2);
      }

      if (factory->modes & CS::Animation::DEBUG_2DLINES)
      {
	csVector3 endLocal;

	if (numChild[i] > 0)
	  endLocal = childPos[i] / numChild[i];
	else
	{
	  endLocal = csVector3 (0.0f, 0.0f, 1.0f);

	  CS::Animation::BoneID parent = fact->GetBoneParent (i);
	  if (parent != CS::Animation::InvalidBoneID)
	    endLocal *= (childPos[parent] / numChild[parent]).Norm ();
	}

	csVector3 endGlobal = position + rotation.Rotate (endLocal);
	csVector3 boneEnd = object2camera * endGlobal;

	g2d->Draw3DLine (bonePosition, boneEnd, projection, colorI);
      }

      if (factory->modes & CS::Animation::DEBUG_SQUARES)
      {
	if (bonePosition < SMALL_Z)
	  continue;

	csVector4 v1p (projection * csVector4 (bonePosition));
	v1p /= v1p.w;

	int px1 = csQint ((v1p.x + 1) * (g2d->GetWidth() / 2));
	int py1 = g2d->GetHeight () - 1 - csQint ((v1p.y + 1) * (g2d->GetHeight () / 2));
 
	size_t size = 5;
	for (size_t i = 0; i < size; i++)
	  for (size_t j = 0; j < size; j++)
	    g2d->DrawPixel (px1 - size / 2 + i, py1 - size / 2 + j, colorI);
      }

      // Draw the bounding boxes
      if (factory->modes & CS::Animation::DEBUG_BBOXES)
      {
	csRef<CS::Mesh::iAnimatedMesh> animesh = skeleton->GetAnimatedMesh ();
	csBox3 bbox = animesh->GetBoneBoundingBox (i);

	// Bone to object space transform
	csReversibleTransform object2bone (csMatrix3 (rotation.GetConjugate ()), position); 
	csReversibleTransform bone2object = object2bone.GetInverse ();

	// Bone to camera space transform
	csReversibleTransform bone2camera = object2camera * bone2object;

	// Draw the bounding box of the bone
	if (!bbox.Empty ())
	  DrawBox3D (g3d, bbox, bone2camera, colorI);
      }
    }

    // Draw the object bounding box
    if (factory->modes & CS::Animation::DEBUG_BBOXES)
    {
      csRef<iObjectModel> objectModel = scfQueryInterface<iObjectModel> (skeleton->GetAnimatedMesh ());
      csBox3 objectBbox = objectModel->GetObjectBoundingBox ();
      int bbox_color = g2d->FindRGB (255, 255, 0);
      DrawBox3D (g3d, objectBbox, object2camera, bbox_color);
    }
  }
}
CS_PLUGIN_NAMESPACE_END(DebugNode)
