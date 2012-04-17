/*
  Copyright (C) 2010-12 Christian Van Brussel, Institute of Information
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
#include "cstool/genmeshbuilder.h"
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
    g2d->DrawLineProjected (vxyz, vXyz, projection, color);
    g2d->DrawLineProjected (vXyz, vXYz, projection, color);
    g2d->DrawLineProjected (vXYz, vxYz, projection, color);
    g2d->DrawLineProjected (vxYz, vxyz, projection, color);
    g2d->DrawLineProjected (vxyZ, vXyZ, projection, color);
    g2d->DrawLineProjected (vXyZ, vXYZ, projection, color);
    g2d->DrawLineProjected (vXYZ, vxYZ, projection, color);
    g2d->DrawLineProjected (vxYZ, vxyZ, projection, color);
    g2d->DrawLineProjected (vxyz, vxyZ, projection, color);
    g2d->DrawLineProjected (vxYz, vxYZ, projection, color);
    g2d->DrawLineProjected (vXyz, vXyZ, projection, color);
    g2d->DrawLineProjected (vXYz, vXYZ, projection, color);
  }

  // --------------------------  Utility methods  --------------------------

  // Generate a random color from an index
  inline void GetRandomColor (int index, int& r, int& g, int& b)
  {
    int ii = (index + 1) * (index + 1);
    r = 255 - (ii % 255);
    g = 255 - ((ii * (index + 1)) % 255);
    b = 255 - ((ii * ii) % 255);
  }

  inline void GetRandomColor (int index, int& color, iGraphics2D* g2d)
  {
    int r, g, b;
    GetRandomColor (index, r, g, b);
    color = g2d->FindRGB (r, g, b);
  }

  inline void GetRandomColor (int index, csColor& color)
  {
    int r, g, b;
    GetRandomColor (index, r, g, b);
    // 0.003922f equals 1 / 255
    color.Set (r * 0.003922f, g * 0.003922f, b * 0.003922f);
  }

  // --------------------------  DebugNode  --------------------------

  CS_LEAKGUARD_IMPLEMENT(DebugNode);

  DebugNode::DebugNode (DebugNodeFactory* factory, CS::Animation::iSkeleton* skeleton)
    : scfImplementationType (this),
    CS::Animation::SkeletonAnimNodeSingle<DebugNodeFactory> (factory, skeleton),
    lastColor (1.0f, 0.0f, 1.0f)
  {
  }

  void DebugNode::Draw (iCamera* camera, csColor color)
  {
    lastColor = color;

    // Remove the bone meshes if we don't need it anymore
    if (!(factory->modes & CS::Animation::DEBUG_ELLIPSOIDS)
	&& boneData.GetSize ())
      RemoveBoneMeshes ();

    if (!factory->modes)
      return;

    csRef<iGraphics3D> g3d = csQueryRegistry<iGraphics3D>
      (factory->manager->GetObjectRegistry ());
    csRef<iGraphics2D> g2d = g3d->GetDriver2D ();
    CS_ASSERT(g3d && g2d);

    // Tell the 3D driver we're going to display 2D things.
    if (!g3d->BeginDraw (CSDRAW_2DGRAPHICS))
      return;

    CS::Animation::iSkeletonFactory* skeletonFactory = skeleton->GetFactory ();
    const CS::Math::Matrix4& projection (camera->GetProjectionMatrix ());
    csReversibleTransform object2camera =
      camera->GetTransform () / skeleton->GetSceneNode ()->GetMovable ()->GetFullTransform ();

    // Analyze the skeleton if not yet made
    if (childPositions.GetSize () != skeletonFactory->GetTopBoneID () + 1)
      AnalyzeSkeleton ();

    // Iterate on all bones and draw each 2D debug primitives
    const csArray<CS::Animation::BoneID> &bones =
      skeletonFactory->GetBoneOrderList ();
    for (size_t i = 0; i < bones.GetSize (); i++)
    {
      const CS::Animation::BoneID &boneID = bones.Get (i);

      // Check if the bone has to be displayed
      if ((factory->boneMaskUsed
	   && (factory->boneMask.GetSize () <= boneID
	       || !factory->boneMask.IsBitSet (boneID)))
	  || (!factory->leafBonesDisplayed && childCounts[boneID] == 0))
	continue;

      // Set the color of the bone and its bounding box
      int colorI;
      if (!factory->boneRandomColor)
	colorI = g2d->FindRGB ((int) (color[0] * 255.0f),
			       (int) (color[1] * 255.0f),
			       (int) (color[2] * 255.0f));
      else
	GetRandomColor (boneID, colorI, g2d);

      csQuaternion rotation;
      csVector3 position;
      skeleton->GetTransformAbsSpace (boneID, rotation, position);

      csVector3 bonePosition = object2camera * position;

      if (factory->modes & CS::Animation::DEBUG_IMAGES
	  && factory->image)
      {
	if (bonePosition.z < SMALL_Z)
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

	if (childCounts[boneID] > 0)
	  endLocal = childPositions[boneID] / childCounts[boneID];
	else
	{
	  endLocal = csVector3 (0.0f, 0.0f, 1.0f);

	  CS::Animation::BoneID parent = skeletonFactory->GetBoneParent (boneID);
	  if (parent != CS::Animation::InvalidBoneID)
	    endLocal *= (childPositions[parent] / childCounts[parent]).Norm ();
	}

	csVector3 endGlobal = position + rotation.Rotate (endLocal);
	csVector3 boneEnd = object2camera * endGlobal;

	g2d->DrawLineProjected (bonePosition, boneEnd, projection, colorI);
      }

      if (factory->modes & CS::Animation::DEBUG_SQUARES)
      {
	if (bonePosition.z < SMALL_Z)
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
	csBox3 bbox = animesh->GetBoneBoundingBox (boneID);

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

  void DebugNode::Stop ()
  {
    SkeletonAnimNodeSingleBase::Stop ();
    if (boneData.GetSize ())
      RemoveBoneMeshes ();
  }

  void DebugNode::BlendState (CS::Animation::AnimatedMeshState* state,
			      float baseWeight)
  {
    CS::Animation::SkeletonAnimNodeSingle<DebugNodeFactory>::BlendState
      (state, baseWeight);

    if (!skeleton) return;

    // Analyze the skeleton if not yet made
    if (childPositions.GetSize () != skeleton->GetFactory ()->GetTopBoneID () + 1)
      AnalyzeSkeleton ();

    // Create the bone meshes if not already made
    if (factory->modes & CS::Animation::DEBUG_ELLIPSOIDS
	&& !boneData.GetSize ())
    {
      csRef<iEngine> engine = csQueryRegistry<iEngine>
	(factory->manager->GetObjectRegistry ());

      iMaterialWrapper* defaultMaterial = nullptr;

      const csArray<CS::Animation::BoneID> &bones =
	skeleton->GetFactory ()->GetBoneOrderList ();
      for (size_t i = 0; i < bones.GetSize (); i++)
      {
	const CS::Animation::BoneID &boneID = bones.Get (i);

	// Check if the bone has to be displayed
	if ((factory->boneMaskUsed
	     && (factory->boneMask.GetSize () <= boneID
		 || !factory->boneMask.IsBitSet (boneID)))
	    || (!factory->leafBonesDisplayed && childCounts[boneID] == 0))
	continue;

	// Check that the bounding box is not empty
	const csBox3 &box = skeleton->GetAnimatedMesh ()->GetBoneBoundingBox (boneID);
	if (box.Empty ()) continue;

	BoneData data;
	data.boneID = boneID;

	// Create the bone mesh factory.
	csString name = "debug_bone";
	name += boneID;
	csRef<iMeshFactoryWrapper> meshFactory = engine->CreateMeshFactory (
	  "crystalspace.mesh.object.genmesh", name);
	if (!meshFactory)
	{
	  //ReportError ("Error creating mesh object factory!");
	  return;
	}

	csRef<iGeneralFactoryState> gmState = scfQueryInterface<iGeneralFactoryState>
	  (meshFactory->GetMeshObjectFactory ());
	csVector3 size = box.GetSize ();
	size *= 0.5f;
	csEllipsoid ellipsoid (box.GetCenter (), size);
	gmState->GenerateSphere (ellipsoid, 16);

	// Create the mesh.
	data.mesh = engine->CreateMeshWrapper (meshFactory, name);

	// Create the material
	if (!factory->boneRandomColor)
	{
	  // Create the default material if not already made
	  if (!defaultMaterial)
	  {
	    defaultMaterial = CS::Material::MaterialBuilder::CreateColorMaterial
	      (factory->manager->GetObjectRegistry (), "debug_node", lastColor);
	    materials.Push (defaultMaterial);
	  }

	  data.mesh->GetMeshObject ()->SetMaterialWrapper (defaultMaterial);
	}
	else
	{
	  // Create a new material for this bone
	  csColor color;
	  GetRandomColor (boneID, color);
	  iMaterialWrapper* material =
	    CS::Material::MaterialBuilder::CreateColorMaterial
	    (factory->manager->GetObjectRegistry (), name, color);
	  materials.Push (material);

	  data.mesh->GetMeshObject ()->SetMaterialWrapper (material);
	}

	// Set the mesh as a child of the skeleton's iMovable
	data.mesh->GetMovable ()->GetSceneNode ()->SetParent
	  (skeleton->GetSceneNode ());

	boneData.Push (data);
      }
    }

    // Update the position of the bone meshes
    csQuaternion rotation;
    csVector3 offset;
    for (size_t i = 0; i < boneData.GetSize (); i++)
    {
      BoneData& data = boneData.Get (i);
      skeleton->GetTransformAbsSpace (data.boneID, rotation, offset);
      csTransform transform (csMatrix3 (rotation.GetConjugate ()), offset);
      data.mesh->GetMovable ()->SetTransform (transform);
      data.mesh->GetMovable ()->UpdateMove ();
    }
  }

  void DebugNode::AnalyzeSkeleton ()
  {
    // Setup the "end" positions and child count of all bones
    CS::Animation::iSkeletonFactory* skeletonFactory = skeleton->GetFactory ();
    size_t count = skeletonFactory->GetTopBoneID () + 1;
    childPositions.DeleteAll ();
    childPositions.SetSize (count, csVector3 (0));
    childCounts.DeleteAll ();
    childCounts.SetSize (count, 0);

    const csArray<CS::Animation::BoneID> &bones =
      skeletonFactory->GetBoneOrderList ();
    for (size_t i = 0; i < bones.GetSize (); i++)
    {
      const CS::Animation::BoneID &boneID = bones.Get (i);

      CS::Animation::BoneID parent = skeletonFactory->GetBoneParent (boneID);
      if (parent != CS::Animation::InvalidBoneID)
      {
	csQuaternion q;
	csVector3 v;
	skeleton->GetTransformBoneSpace (boneID, q, v);

	childPositions[parent] += v;
	childCounts[parent] += 1;
      }
    }
  }

  void DebugNode::RemoveBoneMeshes ()
  {
    // Delete the meshes
    for (size_t i = 0; i < boneData.GetSize (); i++)
    {
      BoneData& data = boneData.Get (i);
      data.mesh->GetMovable ()->GetSceneNode ()->SetParent (nullptr);
    }
    boneData.DeleteAll ();

    // Delete the materials
    csRef<iEngine> engine = csQueryRegistry<iEngine>
      (factory->manager->GetObjectRegistry ());
    iMaterialList* materialList = engine->GetMaterialList ();
    for (size_t i = 0; i < materials.GetSize (); i++)
    {
      iMaterialWrapper* material = materials.Get (i);
      materialList->Remove (material);
    }
    materials.DeleteAll ();
  }

}
CS_PLUGIN_NAMESPACE_END(DebugNode)
