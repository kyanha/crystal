/*
    Copyright (C) 2004-2005 by Jorrit Tyberghein

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

#include "pvscalc.h"

CS_IMPLEMENT_APPLICATION

#undef DB
//#define DB(x) printf x
#define DB(x)

#undef DBA
#define DBA(x) printf x

//-----------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE (PVSMetaLoader)
  SCF_IMPLEMENTS_INTERFACE (iLoaderPlugin)
SCF_IMPLEMENT_IBASE_END

PVSMetaLoader::PVSMetaLoader (PVSCalc* parent)
{
  SCF_CONSTRUCT_IBASE (0);
  PVSMetaLoader::parent = parent;
  synserv = CS_QUERY_REGISTRY (parent->GetObjectRegistry (), iSyntaxService);
}

PVSMetaLoader::~PVSMetaLoader ()
{
  SCF_DESTRUCT_IBASE ();
}

csPtr<iBase> PVSMetaLoader::Parse (iDocumentNode* node,
	iLoaderContext* /*ldr_context*/, iBase* context)
{
  csRef<iSector> sector;
  if (context)
    sector = SCF_QUERY_INTERFACE (context, iSector);
  if (!sector)
  {
    synserv->ReportError ("crystalspace.application", node,
    	"PVSCalc meta information must be used from within a sector!");
    return 0;
  }

  parent->RegisterMetaInformation (sector, node);
  sector->IncRef ();
  return csPtr<iBase> ((iBase*)sector);
}

//-----------------------------------------------------------------------------

bool PVSPolygonNode::HitBeam (const csSegment3& seg)
{
  // First test if this beam hits the box.
  csVector3 isect;
  if (csIntersect3::BoxSegment (node_bbox, seg, isect) == -1)
  {
    return false;
  }

  // Test the polygons.
  size_t i;
  for (i = 0 ; i < polygons.Length () ; i++)
  {
    if (csIntersect3::IntersectPolygon (*polygons[i],
    	polygons[i]->GetPlane (), seg, isect))
      return true;
  }

  if (child1)
  {
    // There are children.
    if (child1->HitBeam (seg))
      return true;
    return child2->HitBeam (seg);
  }
  return false;
}

bool PVSCalcNode::HitBeam (const csSegment3& seg)
{
  // First test if this beam hits the box.
  csVector3 isect;
  if (csIntersect3::BoxSegment (node_bbox, seg, isect) == -1)
  {
    return false;
  }

  if (polygon_tree)
  {
    if (polygon_tree->HitBeam (seg))
      return true;
  }

  if (child1)
  {
    // There are children.
    if (child1->HitBeam (seg))
      return true;
    return child2->HitBeam (seg);
  }
  return false;
}

//-----------------------------------------------------------------------------

PVSCalcSector::PVSCalcSector (PVSCalc* parent, iSector* sector, iPVSCuller* pvs)
{
  PVSCalcSector::parent = parent;
  PVSCalcSector::sector = sector;
  PVSCalcSector::pvs = pvs;
  pvstree = pvs->GetPVSTree ();

  shadow_tree = 0;

  // @@@ Make dimension configurable?
  plane.covbuf = new csTiledCoverageBuffer (DIM_COVBUFFER, DIM_COVBUFFER);
  plane.covbuf_clipper = 0;

  // Minimum required polygon area.
  min_polygon_area = 2;

  // Minimum size for kdtree nodes.
  minsize_specified = false;

  init_token_table (xmltokens);
}

PVSCalcSector::~PVSCalcSector ()
{
  delete plane.covbuf;
  delete plane.covbuf_clipper;
  delete shadow_tree;
}

void PVSCalcSector::DistributeBoxes (int axis, float where,
	const csArray<csBox3>& boxlist,
	csArray<csBox3>& boxlist_left,
	csArray<csBox3>& boxlist_right)
{
  size_t i;
  for (i = 0 ; i < boxlist.Length () ; i++)
  {
    int split = boxlist[i].TestSplit (axis, where);
    if (split <= 0)
      boxlist_left.Push (boxlist[i]);
    if (split >= 0)
      boxlist_right.Push (boxlist[i]);
  }
}

void PVSCalcSector::DistributePolygons (int axis, float where,
	const csArray<csPoly3DBox*>& polylist,
	csArray<csPoly3DBox*>& polylist_left,
	csArray<csPoly3DBox*>& polylist_right)
{
  size_t i;
  for (i = 0 ; i < polylist.Length () ; i++)
  {
    int split = 0;
    switch (axis)
    {
      case 0: split = polylist[i]->ClassifyX (where); break;
      case 1: split = polylist[i]->ClassifyY (where); break;
      case 2: split = polylist[i]->ClassifyZ (where); break;
    }
    if (split == CS_POL_FRONT || split == CS_POL_SPLIT_NEEDED)
      polylist_left.Push (polylist[i]);
    if (split == CS_POL_BACK || split == CS_POL_SPLIT_NEEDED)
      polylist_right.Push (polylist[i]);
  }
}

float PVSCalcSector::FindBestSplitLocation (int axis, float& where,
	const csBox3& bbox1, const csBox3& bbox2)
{
  float max1 = bbox1.Max (axis);
  float min2 = bbox2.Min (axis);
  if (max1 < min2-.01)
  {
    where = max1 + (min2-max1) * 0.5;
    return min2-max1;
  }
  float min1 = bbox1.Min (axis);
  float max2 = bbox2.Max (axis);
  if (max2 < min1-.01)
  {
    where = max2 + (min1-max2) * 0.5;
    return min1-max2;
  }
  return -1.0;
}

float PVSCalcSector::FindBestSplitLocation (int axis, float& where,
	const csBox3& node_bbox, const csArray<csBox3>& boxlist)
{
  if (boxlist.Length () == 2)
  {
    return FindBestSplitLocation (axis, where, boxlist[0], boxlist[1]);
  }

  size_t i, j;

  // Calculate minimum and maximum value along the axis.
  float mina = boxlist[0].Min (axis);
  float maxa = boxlist[0].Max (axis);
  for (i = 1 ; i < boxlist.Length () ; i++)
  {
    const csBox3& bbox = boxlist[i];
    if (bbox.Min (axis) < mina) mina = bbox.Min (axis);
    if (bbox.Max (axis) > maxa) maxa = bbox.Max (axis);
  }
  // Make sure we don't go outside node_box.
  if (mina < node_bbox.Min (axis)) mina = node_bbox.Min (axis);
  if (maxa > node_bbox.Max (axis)) maxa = node_bbox.Max (axis);

  // Do 10 tests to find best split location. This should
  // probably be a configurable parameter.

  // @@@ Is the routine below very efficient?
# define FBSL_ATTEMPTS 50
  float a;
  float best_qual = -2.0;
  float inv_num_objects = 1.0 / float (boxlist.Length ());
  for (i = 0 ; i < FBSL_ATTEMPTS ; i++)
  {
    // Calculate a possible split location.
    a = mina + float (i+1)*(maxa-mina)/float (FBSL_ATTEMPTS+1.0);
    // Now count the number of objects that are completely
    // on the left and the number of objects completely on the right
    // side. The remaining objects are cut by this position.
    int left = 0;
    int right = 0;
    for (j = 0 ; j < boxlist.Length () ; j++)
    {
      const csBox3& bbox = boxlist[j];
      // The .0001 is for safety.
      if (bbox.Max (axis) < a-.0001) left++;
      else if (bbox.Min (axis) > a+.0001) right++;
    }
    int cut = boxlist.Length ()-left-right;
    // If we have no object on the left or right then this is a bad
    // split which we should never take.
    float qual;
    if (left == 0 || right == 0)
    {
      qual = -1.0;
    }
    else
    {
      float qual_cut = 1.0 - (float (cut) * inv_num_objects);
      float qual_balance = 1.0 - (float (ABS (left-right)) * inv_num_objects);
      qual = 6.0 * qual_cut + qual_balance;
    }
    if (qual > best_qual)
    {
      best_qual = qual;
      where = a;
    }
  }
# undef FBSL_ATTEMPTS
  return best_qual;
}

float PVSCalcSector::FindBestSplitLocation (int axis, float& where,
	const csBox3& node_bbox, const csArray<csPoly3DBox*>& polylist)
{
  size_t i, j;

  // Calculate minimum and maximum value along the axis.
  float mina = polylist[0]->GetBBox ().Min (axis);
  float maxa = polylist[0]->GetBBox ().Max (axis);
  for (i = 1 ; i < polylist.Length () ; i++)
  {
    const csBox3& bbox = polylist[i]->GetBBox ();
    if (bbox.Min (axis) < mina) mina = bbox.Min (axis);
    if (bbox.Max (axis) > maxa) maxa = bbox.Max (axis);
  }
  // Make sure we don't go outside node_box.
  if (mina < node_bbox.Min (axis)) mina = node_bbox.Min (axis);
  if (maxa > node_bbox.Max (axis)) maxa = node_bbox.Max (axis);

  // Do 10 tests to find best split location. This should
  // probably be a configurable parameter.

  // @@@ Is the routine below very efficient?
# define FBSL_ATTEMPTS 50
  float a;
  float best_qual = -2.0;
  float inv_num_objects = 1.0 / float (polylist.Length ());
  for (i = 0 ; i < FBSL_ATTEMPTS ; i++)
  {
    // Calculate a possible split location.
    a = mina + float (i+1)*(maxa-mina)/float (FBSL_ATTEMPTS+1.0);
    // Now count the number of objects that are completely
    // on the left and the number of objects completely on the right
    // side. The remaining objects are cut by this position.
    int left = 0;
    int right = 0;
    for (j = 0 ; j < polylist.Length () ; j++)
    {
      const csBox3& bbox = polylist[j]->GetBBox ();
      // The .0001 is for safety.
      if (bbox.Max (axis) < a-.0001) left++;
      else if (bbox.Min (axis) > a+.0001) right++;
    }
    int cut = polylist.Length ()-left-right;
    // If we have no object on the left or right then this is a bad
    // split which we should never take.
    float qual;
    if (left == 0 || right == 0)
    {
      qual = -1.0;
    }
    else
    {
      float qual_cut = 1.0 - (float (cut) * inv_num_objects);
      float qual_balance = 1.0 - (float (ABS (left-right)) * inv_num_objects);
      qual = 6.0 * qual_cut + qual_balance;
    }
    if (qual > best_qual)
    {
      best_qual = qual;
      where = a;
    }
  }
# undef FBSL_ATTEMPTS
  return best_qual;
}

void PVSCalcSector::BuildKDTree (void* node, const csArray<csBox3>& boxlist,
	const csBox3& bbox, bool minsize_only, int depth)
{
  if (depth > maxdepth) maxdepth = depth;

  int axis;
  float where;
  csVector3 bbox_size = bbox.Max () - bbox.Min ();
  if (minsize_only || boxlist.Length () <= 10)
  {
    // If we have 1 or less objects left then we continue splitting the
    // node so that leafs are smaller then 'minsize'.
    if (bbox_size.x > minsize.x)
    {
      axis = 0;
      where = bbox.MinX () + bbox_size.x / 2;
    }
    else if (bbox_size.y > minsize.y)
    {
      axis = 1;
      where = bbox.MinY () + bbox_size.y / 2;
    }
    else if (bbox_size.z > minsize.z)
    {
      axis = 2;
      where = bbox.MinZ () + bbox_size.z / 2;
    }
    else
    {
      return;
    }
  }
  else
  {
    float where0, where1, where2;
    float q0 = FindBestSplitLocation (0, where0, bbox, boxlist);
    float q1 = FindBestSplitLocation (1, where1, bbox, boxlist);
    float q2 = FindBestSplitLocation (2, where2, bbox, boxlist);
    if (q0 > q1 && q0 > q2)
    {
      axis = 0;
      where = where0;
    }
    else if (q1 > q0 && q1 > q2)
    {
      axis = 1;
      where = where1;
    }
    else if (q2 >= 0)
    {
      axis = 2;
      where = where2;
    }
    else
    {
      // All options are bad. Here we mark the traversal so that
      // we only continue to split for minsize.
      BuildKDTree (node, boxlist, bbox, true, depth);
      return;	// No good split location.
    }
  }

  void* child1;
  void* child2;
  csBox3 box1, box2;
  bbox.Split (axis, where, box1, box2);
  csArray<csBox3> boxlist_left;
  csArray<csBox3> boxlist_right;
  DistributeBoxes (axis, where, boxlist, boxlist_left, boxlist_right);
  pvstree->SplitNode (node, axis, where, child1, child2);
  countnodes += 2;
  BuildKDTree (child1, boxlist_left, box1, minsize_only, depth+1);
  BuildKDTree (child2, boxlist_right, box2, minsize_only, depth+1);
}

void PVSCalcSector::BuildKDTree ()
{
  iStaticPVSTree* pvstree = pvs->GetPVSTree ();
  pvstree->Clear ();
  void* root = pvstree->CreateRootNode ();
  const csBox3& bbox = pvstree->GetBoundingBox ();
  if (!minsize_specified)
  {
    minsize = (bbox.Max () - bbox.Min ()) / 3.0f;
  }
  countnodes = 1;
  maxdepth = 0;
  BuildKDTree (root, boxes, bbox, false, 0);
  totalnodes = countnodes;
}

PVSCalcNode* PVSCalcSector::BuildShadowTree (void* node)
{
  if (!node) return 0;
  PVSCalcNode* shadow_node = new PVSCalcNode (node);
  shadow_node->node_bbox = pvstree->GetNodeBBox (node);
  void* child1 = pvstree->GetFirstChild (node);
  int count_child1 = 0;
  if (child1)
  {
    PVSCalcNode* shadow_child1 = BuildShadowTree (child1);
    shadow_node->child1 = shadow_child1;
    count_child1 = shadow_child1->represented_nodes;
  }
  void* child2 = pvstree->GetSecondChild (node);
  int count_child2 = 0;
  if (child2)
  {
    PVSCalcNode* shadow_child2 = BuildShadowTree (child2);
    shadow_node->child2 = shadow_child2;
    count_child2 = shadow_child2->represented_nodes;
  }
  shadow_node->represented_nodes = 1 + count_child1 + count_child2;
  return shadow_node;
}

void PVSCalcSector::BuildShadowTreePolygons (PVSPolygonNode* node,
	const csBox3& bbox_node, const csArray<csPoly3DBox*>& polygons)
{
  node->node_bbox = bbox_node;

  if (polygons.Length () == 0) return;
  if (polygons.Length () <= 6)
  {
    node->polygons = polygons;
    return;
  }

  float where0, where1, where2;
  float q0 = FindBestSplitLocation (0, where0, bbox_node, polygons);
  float q1 = FindBestSplitLocation (1, where1, bbox_node, polygons);
  float q2 = FindBestSplitLocation (2, where2, bbox_node, polygons);
  if (q0 > q1 && q0 > q2)
  {
    node->axis = 0;
    node->where = where0;
  }
  else if (q1 > q0 && q1 > q2)
  {
    node->axis = 1;
    node->where = where1;
  }
  else if (q2 >= 0)
  {
    node->axis = 2;
    node->where = where2;
  }
  else
  {
    // All options are bad.
    node->polygons = polygons;
    return;	// No good split location.
  }

  csBox3 box1, box2;
  bbox_node.Split (node->axis, node->where, box1, box2);
  csArray<csPoly3DBox*> polygons_left;
  csArray<csPoly3DBox*> polygons_right;
  DistributePolygons (node->axis, node->where, polygons, polygons_left, polygons_right);
  node->child1 = new PVSPolygonNode ();
  node->child2 = new PVSPolygonNode ();
  BuildShadowTreePolygons (node->child1, box1, polygons_left);
  BuildShadowTreePolygons (node->child2, box2, polygons_right);
}

void PVSCalcSector::BuildShadowTreePolygons (PVSCalcNode* node,
	const csArray<csPoly3DBox*>& polygons)
{
  if (polygons.Length () == 0) return;

  if (node->child1)
  {
    // This node has children, that means we simply have to distribute
    // the polygons over the two children.
    int axis;
    float where;
    pvstree->GetAxisAndPosition (node->node, axis, where);
    csArray<csPoly3DBox*> polygons_left;
    csArray<csPoly3DBox*> polygons_right;
    DistributePolygons (axis, where, polygons, polygons_left, polygons_right);
    BuildShadowTreePolygons (node->child1, polygons_left);
    BuildShadowTreePolygons (node->child2, polygons_right);
  }
  else
  {
    // This node has no children. Here we have to create a new polygon
    // tree.
    node->polygon_tree = new PVSPolygonNode ();
    BuildShadowTreePolygons (node->polygon_tree, node->node_bbox, polygons);
  }
}

void csPoly3DBox::Calculate ()
{
  size_t i;
  bbox.StartBoundingBox ((*this)[0]);
  for (i = 1 ; i < GetVertexCount () ; i++)
    bbox.AddBoundingVertexSmart ((*this)[i]);
  area = GetArea ();
  plane = ComputePlane ();
}

void PVSCalcSector::CollectGeometry (iMeshWrapper* mesh,
  	csBox3& allbox, csBox3& staticbox,
	int& allcount, int& staticcount,
	int& allpcount, int& staticpcount)
{
  csBox3 mbox;
  mesh->GetWorldBoundingBox (mbox);
  iMeshObject* meshobj = mesh->GetMeshObject ();
  iObjectModel* objmodel = meshobj->GetObjectModel ();
  iPolygonMesh* polybase = objmodel->GetPolygonMeshViscull ();

  // Increase stats for all objects.
  allcount++;
  allbox += mbox;
  if (polybase) allpcount += polybase->GetPolygonCount ();

  // Register the box.
  boxes.Push (mbox);

  iMeshFactoryWrapper* fact = mesh->GetFactory ();
  bool staticshape_fact = fact
    	? fact->GetMeshObjectFactory ()
		->GetFlags ().Check (CS_FACTORY_STATICSHAPE)
	: false;
  const csFlags& mesh_flags = meshobj->GetFlags ();
  bool staticshape_mesh = mesh_flags.Check (CS_MESH_STATICSHAPE);
  if (polybase && polybase->GetPolygonCount () &&
	mesh_flags.Check (CS_MESH_STATICPOS) &&
    	(staticshape_mesh || staticshape_fact))
  {
    //bool closed = csPolygonMeshTools::IsMeshClosed (polybase);
    //bool convex = csPolygonMeshTools::IsMeshConvex (polybase);
    //if (convex || closed)
    //printf ("closed=%d convex=%d mesh=%s poly=%d %s\n",
    // closed, convex, mesh->QueryObject ()->GetName (),
    // polybase->GetPolygonCount (), (closed && convex) ?  "BOTH" : "");

    // Increase stats for static objects.
    staticcount++;
    staticbox += mbox;
    staticpcount += polybase->GetPolygonCount ();
    csReversibleTransform trans = mesh->GetMovable ()->GetFullTransform ();
    csVector3* vertices = polybase->GetVertices ();
    csMeshedPolygon* mp = polybase->GetPolygons ();
    int p, vt;
    for (p = 0 ; p < polybase->GetPolygonCount () ; p++)
    {
      const csMeshedPolygon& poly = mp[p];
      csPoly3DBox poly3d;
      for (vt = 0 ; vt < poly.num_vertices ; vt++)
      {
        csVector3 vwor = trans.This2Other (vertices[poly.vertices[vt]]);
        poly3d.AddVertex (vwor);
      }
      poly3d.Calculate ();
      polygons.Push (poly3d);
    }
  }

  int i;
  iMeshList* ml = mesh->GetChildren ();
  for (i = 0 ; i < ml->GetCount () ; i++)
  {
    iMeshWrapper* child = ml->Get (i);
    CollectGeometry (child, allbox, staticbox, allcount, staticcount,
    	allpcount, staticpcount);
  }
}

struct poly_with_area
{
  size_t idx;
  float area;
};

static int compare_polygons_on_size (const poly_with_area& p1,
	const poly_with_area& p2)
{
  if (p1.area > p2.area) return -1;
  else if (p1.area < p2.area) return 1;
  else return 0;
}

void PVSCalcSector::SortPolygonsOnSize ()
{
  csArray<poly_with_area> polygons_with_area;
  double total_area = 0.0;
  float min_area = 1000000000.0;
  float max_area = -1.0f;
  size_t i;
  for (i = 0 ; i < polygons.Length () ; i++)
  {
    poly_with_area pwa;
    pwa.idx = i;
    pwa.area = polygons[i].GetPrecalcArea ();
    polygons_with_area.Push (pwa);
    if (pwa.area < min_area) min_area = pwa.area;
    if (pwa.area > max_area) max_area = pwa.area;
    total_area += double (pwa.area);
  }
  polygons_with_area.Sort (compare_polygons_on_size);
  csArray<csPoly3DBox> sorted_polygons;
  for (i = 0 ; i < polygons_with_area.Length () ; i++)
  {
    poly_with_area& pwa = polygons_with_area[i];
    if (pwa.area < min_polygon_area) break;
    sorted_polygons.Push (polygons[pwa.idx]);
  }
  polygons = sorted_polygons;

  parent->ReportInfo ("Average polygon area %g, min %g, max %g\n",
  	float (total_area / double (polygons_with_area.Length ())),
	min_area, max_area);
}

bool PVSCalcSector::FindShadowPlane (const csBox3& source, const csBox3& dest,
  	int& axis, float& where, float& where_other)
{
  float sdx = dest.MinX () - source.MaxX ();
  float sdy = dest.MinY () - source.MaxY ();
  float sdz = dest.MinZ () - source.MaxZ ();
  float dsx = source.MinX () - dest.MaxX ();
  float dsy = source.MinY () - dest.MaxY ();
  float dsz = source.MinZ () - dest.MaxZ ();
  if (sdx > SMALL_EPSILON &&
  	sdx > sdy &&
	sdx > sdz &&
	sdx > dsx &&
	sdx > dsy &&
	sdx > dsz)
  {
    axis = 0;
    where = dest.MinX ();
    where_other = source.MaxX ();
    return true;
  }
  if (dsx > SMALL_EPSILON &&
  	dsx > sdx &&
	dsx > sdy &&
	dsx > sdz &&
	dsx > dsy &&
	dsx > dsz)
  {
    axis = 0;
    where = dest.MaxX ();
    where_other = source.MinX ();
    return true;
  }
  if (sdy > SMALL_EPSILON &&
  	sdy > sdx &&
	sdy > sdz &&
	sdy > dsx &&
	sdy > dsy &&
	sdy > dsz)
  {
    axis = 1;
    where = dest.MinY ();
    where_other = source.MaxY ();
    return true;
  }
  if (dsy > SMALL_EPSILON &&
  	dsy > sdx &&
	dsy > sdy &&
	dsy > sdz &&
	dsy > dsx &&
	dsy > dsz)
  {
    axis = 1;
    where = dest.MaxY ();
    where_other = source.MinY ();
    return true;
  }
  if (sdz > SMALL_EPSILON &&
  	sdz > sdx &&
	sdz > sdy &&
	sdz > dsx &&
	sdz > dsy &&
	sdz > dsz)
  {
    axis = 2;
    where = dest.MinZ ();
    where_other = source.MaxZ ();
    return true;
  }
  if (dsz > SMALL_EPSILON &&
  	dsz > sdx &&
	dsz > sdy &&
	dsz > sdz &&
	dsz > dsx &&
	dsz > dsy)
  {
    axis = 2;
    where = dest.MaxZ ();
    where_other = source.MinZ ();
    return true;
  }
  return false;
}

bool PVSCalcSector::SetupProjectionPlane (const csBox3& source,
	const csBox3& dest)
{
  float where_other;
  if (!FindShadowPlane (source, dest, plane.axis, plane.where, where_other))
    return false;

  //plane.where = (1.95 * plane.where + 0.05 * where_other) / 2.0;

  // We project the destination box on the given shadow plane as seen
  // from every corner of the source box. ProjectOutline() will add the
  // projected vertices to 'poly' so in the end we will have a collection
  // of points on the shadow plane. These points form the boundaries
  // where all the projected shadows will be relevant for visibility.
  csDirtyAccessArray<csVector2> poly;
  dest.ProjectOutline (source.GetCorner (CS_BOX_CORNER_xyz),
  	plane.axis, plane.where, poly);
  dest.ProjectOutline (source.GetCorner (CS_BOX_CORNER_Xyz),
  	plane.axis, plane.where, poly);
  dest.ProjectOutline (source.GetCorner (CS_BOX_CORNER_xYz),
  	plane.axis, plane.where, poly);
  dest.ProjectOutline (source.GetCorner (CS_BOX_CORNER_XYz),
  	plane.axis, plane.where, poly);
  dest.ProjectOutline (source.GetCorner (CS_BOX_CORNER_xyZ),
  	plane.axis, plane.where, poly);
  dest.ProjectOutline (source.GetCorner (CS_BOX_CORNER_XyZ),
  	plane.axis, plane.where, poly);
  dest.ProjectOutline (source.GetCorner (CS_BOX_CORNER_xYZ),
  	plane.axis, plane.where, poly);
  dest.ProjectOutline (source.GetCorner (CS_BOX_CORNER_XYZ),
  	plane.axis, plane.where, poly);

  // Now we calculate the convex hull of the projection points. This
  // convex hull will refine the boundary which is calculated above as
  // only projections that intersect with this hull are relevant for
  // visibility. This convex hull can later be inserted in the coverage
  // buffer.
  csChainHull2D::SortXY (poly.GetArray (), poly.Length ());
  csVector2* hull = new csVector2[poly.Length ()];
  int hull_points = csChainHull2D::CalculatePresorted (poly.GetArray (),
  	poly.Length (), hull);

  // Now we calculate the bounding 2D box of those points. That will
  // be used to calculate how we will transform projected 2D coordinates
  // to coordinates on the coverage buffer.
  csBox2 bbox (hull[0]);
  size_t i;
  for (i = 1 ; i < (size_t)hull_points ; i++)
    bbox.AddBoundingVertexSmart (hull[i]);
  plane.offset = bbox.Min ();
  plane.scale.x = float (DIM_COVBUFFER) / (bbox.MaxX ()-bbox.MinX ());
  plane.scale.y = float (DIM_COVBUFFER) / (bbox.MaxY ()-bbox.MinY ());
  DB(("  Hull box: (%g,%g)-(%g,%g) scale=%g,%g offset=%g,%g\n",
  	bbox.MinX (), bbox.MinY (), bbox.MaxX (), bbox.MaxY (),
	plane.scale.x, plane.scale.y, plane.offset.x, plane.offset.y));

  // Clear the coverage buffer.
  plane.covbuf->Initialize ();

  // Now insert our hull outline inverted in the coverage buffer. That
  // will basically mask out all vertices outside the relevant area.
  // Before we do that we have to transform the hull to coverage buffer
  // space.
  DB(("  Hull:\n"));
  for (i = 0 ; i < (size_t)hull_points ; i++)
  {
    DB(("    N:%d (%g,%g)\n", i, hull[i].x, hull[i].y));
    hull[i].x = (hull[i].x-plane.offset.x) * plane.scale.x;
    hull[i].y = (hull[i].y-plane.offset.y) * plane.scale.y;
    DB(("    C:%d (%g,%g)\n", i, hull[i].x, hull[i].y));
  }
  plane.covbuf->InsertPolygonInvertedNoDepth (hull, hull_points);

  //csRef<iString> str = plane.covbuf->Debug_Dump ();
  //printf ("%s\n", str->GetData ());

  // We no longer need the hull points here.
  delete[] hull;

  // Here we setup the box clipper that represents the boundaries of our
  // coverage buffer.
  plane.covbuf_clipper = new csBoxClipper (bbox);

  return true;
}

bool PVSCalcSector::CastAreaShadow (const csBox3& source,
	const csPoly3D& polygon)
{
  DB(("  Polygon:"));
  size_t j;
  for (j = 0 ; j < polygon.GetVertexCount () ; j++)
  {
    DB((" (%g,%g,%g)", polygon[j].x, polygon[j].y, polygon[j].z));
  }
  DB(("\n"));

  // First we calculate the projection of the polygon on the shadow plane
  // as seen from the first point on the source box. That will be the start
  // for calculating the intersection of all those projections.
  if (!polygon.ProjectAxisPlane (source.GetCorner (CS_BOX_CORNER_xyz),
  	plane.axis, plane.where, &poly_intersect))
    return false;

  // First we clip the projected polygon to the bounding box that
  // represents the coverage buffer. If this intersection is empty already
  // (returns false) then this is an easy way out.
  if (!poly_intersect.ClipAgainst (plane.covbuf_clipper))
    return false;

  // Now we continue calculating projections for the remaining 7 box
  // corners and we intersect those with poly_intersect.
  int i;
  for (i = 1 ; i < 8 ; i++)
  {
    if (!polygon.ProjectAxisPlane (source.GetCorner (i),
  	plane.axis, plane.where, &poly))
      return false;

    // Now we construct a clipper from this projected polygon.
    float area = poly.GetSignedArea ();
    csPolygonClipper clip (&poly, area > 0);

    // Time to clip our 'poly_intersect'.
    if (!poly_intersect.ClipAgainst (&clip))
      return false;
  }

  DB(("    -> has valid shadow\n"));

  // Now 'poly_intersect' contains the intersection of all projected
  // polygons on the shadow plane. This is the area shadow and we will now
  // insert that in the coverage buffer. But before we do that we first
  // have to scale the 2D coordinates so that they match coverage buffer
  // coordinates.
  csVector2* pi_verts = poly_intersect.GetVertices ();
  DB(("    -> covbuf poly:"));
  for (i = 0 ; i < poly_intersect.GetVertexCount () ; i++)
  {
    pi_verts[i].x = (pi_verts[i].x-plane.offset.x) * plane.scale.x;
    pi_verts[i].y = (pi_verts[i].y-plane.offset.y) * plane.scale.y;
    DB((" (%g,%g)", pi_verts[i].x, pi_verts[i].y));
  }
  DB(("\n"));
  int nummod = plane.covbuf->InsertPolygonNoDepth (
  	pi_verts, poly_intersect.GetVertexCount ());
  DB(("    -> nummod = %d\n", nummod));

  // If nummod > 0 then we modified the coverage buffer and we return
  // true then.
  return nummod > 0;
}

int PVSCalcSector::CastShadowsUntilFull (const csBox3& source)
{
  // Start casting shadows starting with the biggest polygons.
  size_t i;
  int status = -1;	// Coverage buffer is now empty.

  // Calculate this for CheckRelevantPolygon.
  float minsource = source.Min (plane.axis);
  float maxsource = source.Max (plane.axis);
  float mincheck, maxcheck;
  if (plane.where < minsource)
  {
    mincheck = plane.where;
    maxcheck = minsource;
  }
  else
  {
    mincheck = maxsource;
    maxcheck = plane.where;
  }

  DB(("  Cast Shadows between %g and %g (plane %d/%g)\n",
  	mincheck, maxcheck,
	plane.axis, plane.where));

  for (i = 0 ; i < polygons.Length () ; i++)
  {
    // First we test if the box of the polygon is enclosed within the
    // source and the shadow plane. We use 'mincheck' and 'maxcheck' for those.
    const csBox3& polygon_box = polygons[i].GetBBox ();
    if (polygon_box.Min (plane.axis) >= mincheck
    	&& polygon_box.Max (plane.axis) <= maxcheck)
    {
      if (CastAreaShadow (source, polygons[i]))
      {
        // The shadow modified the coverage buffer. Test if our buffer is
        // full now.
        status = plane.covbuf->StatusNoDepth ();
        if (status == 1)
        {
          // Yes! Coverage buffer is full. We can return early here.
	  DB(("  Coverage Buffer Full!\n"));
	  return 1;
        }
      }
    }
  }

  return status;
}

bool PVSCalcSector::NodesSurelyVisible (const csBox3& source,
	const csBox3& dest)
{
  // First we do a trivial test where we test if a few points of the boxes can
  // see each other. If we hit no polygon then we don't have to continue
  // testing with the coverage buffer.
  if (!shadow_tree->HitBeam (csSegment3 (
  	source.GetCenter (),
    	dest.GetCenter ())))
    return true;

  int i, j;
  for (i = 0 ; i < 8 ; i++)
    for (j = 0 ; j < 8 ; j++)
      if (!shadow_tree->HitBeam (csSegment3 (
  	    source.GetCorner (i), dest.GetCorner (j))))
        return true;

  return false;
}

void PVSCalcSector::RecurseDestNodes (PVSCalcNode* sourcenode,
	PVSCalcNode* destnode,
	csSet<PVSCalcNode*>& invisible_nodes)
{
  // If sourcenode is equal to node then visibility is obvious. In that
  // case we don't proceed since all children of node will also be visible
  // (since they are contained in sourcenode then).
  if (sourcenode == destnode) return;

  // If the destination node is already in the set of invisible nodes
  // then we don't have to proceed either. That means that the destination
  // node is invisible for one of the parents of the sourcenode.
  if (invisible_nodes.In (destnode)) return;

  // If the PVS of the destination node is already calculated and this node
  // was invisible for the destination node then we can use symmetry. i.e.
  // if A cannot see B then B cannot see A. With one-sided polygons there
  // are situations where this is not true but in most maps this situation
  // should not occur.
  // One could contemplate also doing symmetry for visibility but that has
  // the disadvantage of not being able to use the fact that calculating
  // invisibility from the other side may result in an invisible node. So it
  // is best to calculate invisibility from both A->B and B->A in that case.
  if (destnode->calculated_pvs)
  {
    if (destnode->IsInvisible (sourcenode))
    {
      // sourcenode is invisible for destnode. So we mark destnode
      // invisible for sourcenode.
      invisible_nodes.Add (destnode);
      pvstree->MarkInvisible (sourcenode->node, destnode->node);
      sourcenode->invisible_nodes.Add (destnode);
      int destrep = destnode->represented_nodes;
      total_invisnodes += destrep;
      DBA(("S%d ", destrep));
      DB(("S:Marked invisible\n"));

      // If the destination node is invisible that automatically implies
      // that the children are invisible too. No need to recurse further.
      return;
    }
  }

  // Here we have to do the actual shadow casting to see if 'sourcenode'
  // can see 'node'.
  const csBox3& source = sourcenode->node_bbox;
  const csBox3& dest = destnode->node_bbox;

  // If the source node is contained in the destination node then
  // we know the destination node is visible. However we do have to continue
  // traversing to the children so we only skip the testing part.
  if (!dest.Overlap (source))
  {
    //if ((source.MaxZ()<5 && dest.MinZ()>5) || (source.MinZ()>5 && dest.MaxZ()<5))
      //printf ("####################################################\n");
    //else
      //printf ("----------------------------------------------------\n");
    // First we do a trivial test to see if the nodes can surely see each
    // other.
    if (!NodesSurelyVisible (source, dest))
    {
      DB(("\nTEST (%g,%g,%g)-(%g,%g,%g) -> (%g,%g,%g)-(%g,%g,%g)\n",
	source.MinX (), source.MinY (), source.MinZ (),
	source.MaxX (), source.MaxY (), source.MaxZ (),
	dest.MinX (), dest.MinY (), dest.MinZ (),
	dest.MaxX (), dest.MaxY (), dest.MaxZ ()));

      // If the projection plane failed to set up we still have to test
      // children.
      if (SetupProjectionPlane (source, dest))
      {
        int level = CastShadowsUntilFull (source);
        if (level == 1)
        {
          // The coverage buffer is full! This means the destination node
          // cannot be seen from the source node.
          invisible_nodes.Add (destnode);
          pvstree->MarkInvisible (sourcenode->node, destnode->node);
	  sourcenode->invisible_nodes.Add (destnode);
	  int destrep = destnode->represented_nodes;
          total_invisnodes += destrep;
	  DBA(("%d ", destrep));
          DB(("Marked invisible (%g,%g,%g)-(%g,%g,%g) to (%g,%g,%g)-(%g,%g,%g)\n",
    	    source.MinX (), source.MinY (), source.MinZ (),
    	    source.MaxX (), source.MaxY (), source.MaxZ (),
    	    dest.MinX (), dest.MinY (), dest.MinZ (),
    	    dest.MaxX (), dest.MaxY (), dest.MaxZ ()));

	  // If visibility for the destination node was already calculated
	  // and we are here then that means that this node was considered visible
	  // for the destination node (otherwise we wouldn't have done these
	  // calculations because of the symmetry test in the beginning of this
	  // function).
	  // But now we know that the destination node is invisible for this
	  // node so we make use of symmetry and also mark this source node as
	  // invisible in the destination node.
	  if (destnode->calculated_pvs)
	  {
            pvstree->MarkInvisible (destnode->node, sourcenode->node);
	    destnode->invisible_nodes.Add (sourcenode);
	    int srcrep = sourcenode->represented_nodes;
            total_invisnodes += srcrep;
	    DBA(("I%d ", srcrep));
            DB(("Marked invisible symmetry\n"));
	  }

          // If the destination node is invisible that automatically implies
          // that the children are invisible too. No need to recurse further.
          return;
        }
      }
    }
  }

  total_visnodes++;

  // The destination node was visible and coverage buffer was partially
  // full. This means that we have to proceed to test visibility for the
  // children of the destination node.
  if (destnode->child1)
    RecurseDestNodes (sourcenode, destnode->child1, invisible_nodes);
  if (destnode->child2)
    RecurseDestNodes (sourcenode, destnode->child2, invisible_nodes);
}

void PVSCalcSector::RecurseSourceNodes (PVSCalcNode* sourcenode,
	csSet<PVSCalcNode*> invisible_nodes, int& nodecounter)
{
  nodecounter--;
  csTicks currenttime = csGetTicks ();
  csTicks totaltime = currenttime - starttime;
  float average = float (totaltime) / float (totalnodes-nodecounter);
  csTicks remaining = (csTicks)(average * nodecounter);
  float cull_quality = 100.0 * float (total_invisnodes)
  	/ float (1 + total_visnodes + total_invisnodes);
  printf ("\n#n=%d cnt=%d (tot=%ds rem=%ds avg=%gms) (cq=%g%%) ",
  	sourcenode->represented_nodes, nodecounter,
	totaltime / 1000, remaining / 1000, average,
	cull_quality);
  fflush (stdout);

  // We recurse through all destination nodes now. This will update the
  // set of invisible_nodes.
  RecurseDestNodes (sourcenode, shadow_tree, invisible_nodes);

  // We mark PVS for this node as being calculated.
  sourcenode->calculated_pvs = true;

  // Traverse to the children to calculate visibility from there.
  if (sourcenode->child1)
    RecurseSourceNodes (sourcenode->child1, invisible_nodes, nodecounter);
  if (sourcenode->child2)
    RecurseSourceNodes (sourcenode->child2, invisible_nodes, nodecounter);
}

void PVSCalcSector::Calculate ()
{
  parent->ReportInfo ("Calculating PVS for '%s'!",
  	sector->QueryObject ()->GetName ());

  const csBox3& bbox = pvstree->GetBoundingBox ();
  parent->ReportInfo ("Total box (from culler) %g,%g,%g  %g,%g,%g",
  	bbox.MinX (), bbox.MinY (), bbox.MinZ (),
  	bbox.MaxX (), bbox.MaxY (), bbox.MaxZ ());

  size_t i;
  csBox3 allbox, staticbox;
  allbox.StartBoundingBox ();
  staticbox.StartBoundingBox ();
  int allcount = 0, staticcount = 0;
  int allpcount = 0, staticpcount = 0;
  iMeshList* ml = sector->GetMeshes ();
  for (i = 0 ; i < (size_t)(ml->GetCount ()) ; i++)
  {
    iMeshWrapper* m = ml->Get (i);
    CollectGeometry (m, allbox, staticbox,
	allcount, staticcount,
	allpcount, staticpcount);
  }

  // Now we add all polygons that were added with meta information.
  for (i = 0 ; i < meta_polygons.Length () ; i++)
  {
    csPoly3DBox poly3d (meta_polygons[i]);
    poly3d.Calculate ();
    polygons.Push (poly3d);
  }

  // We sort polygons so that the polygons with the biggest area
  // are in front of the list. That way we will first try to fill
  // big polygons during visibility calculation.
  SortPolygonsOnSize ();

  parent->ReportInfo ("Total box (all geometry) %g,%g,%g  %g,%g,%g",
  	allbox.MinX (), allbox.MinY (), allbox.MinZ (),
  	allbox.MaxX (), allbox.MaxY (), allbox.MaxZ ());
  parent->ReportInfo ("Total box (static geometry) %g,%g,%g  %g,%g,%g",
  	staticbox.MinX (), staticbox.MinY (), staticbox.MinZ (),
  	staticbox.MaxX (), staticbox.MaxY (), staticbox.MaxZ ());
  parent->ReportInfo( "%d static and %d total objects",
  	staticcount, allcount);
  parent->ReportInfo ("%d static, %d meta, and %d total polygons",
  	staticpcount, meta_polygons.Length (), allpcount);
  parent->ReportInfo ("%d static polygons have area larger then %g",
  	polygons.Length (), min_polygon_area);
  fflush (stdout);

  // From all geometry (static and dynamic) we now build the KDtree
  // that is the base for visibility culling. We also build the shadow
  // tree that contains extra information we use during building.
  BuildKDTree ();
  pvstree->UpdateBoundingBoxes ();
  delete shadow_tree;
  shadow_tree = BuildShadowTree (pvstree->GetRootNode ());
  csArray<csPoly3DBox*> polygon_ptrs;
  for (i = 0 ; i < polygons.Length () ; i++)
    polygon_ptrs.Push (&polygons[i]);
  BuildShadowTreePolygons (shadow_tree, polygon_ptrs);
  parent->ReportInfo ("KDTree: Minimal node size %g,%g,%g",
  	minsize.x, minsize.y, minsize.z);
  parent->ReportInfo ("KDTree: max depth=%d, number of nodes=%d",
  	maxdepth, countnodes);

  // Now it is time to calculate (in)visibility between all nodes. This
  // happens in a recursive process where we first traverse source nodes
  // in the tree from the root up. And for every such source nodes we
  // again traverse the tree from the root up for the destination nodes.
  // During this traversal we maintain a set of invisible nodes. These
  // are the nodes that are certainly invisible for the parent of the source
  // node that we're currently considering. That means that those nodes
  // are automatically invisible for the children too and don't have
  // to be considered anymore.
  csSet<PVSCalcNode*> invisible_nodes;
  int nodecounter = countnodes;
  starttime = csGetTicks ();
  total_invisnodes = 0;
  total_visnodes = 0;
  RecurseSourceNodes (shadow_tree, invisible_nodes, nodecounter);

  // Write our KDTree with pvs.
  if (!pvstree->WriteOut ())
  {
    parent->ReportError ("Error writing out PVS cache!");
  }
}

bool PVSCalcSector::FeedMetaInformation (iDocumentNode* node)
{
  csRef<iSyntaxService> synserv = CS_QUERY_REGISTRY (
  	parent->GetObjectRegistry (), iSyntaxService);
  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    const char* value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    switch (id)
    {
      case XMLTOKEN_MINPOLYGONAREA:
        min_polygon_area = child->GetContentsValueAsFloat ();
        break;
      case XMLTOKEN_MINNODESIZE:
	if (!synserv->ParseVector (child, minsize))
	  return false;
	minsize_specified = true;
        break;
      case XMLTOKEN_POLYGON:
        {
	  csPoly3D polygon;
	  csRef<iDocumentNodeIterator> it2 = child->GetNodes ();
	  while (it2->HasNext ())
	  {
            csRef<iDocumentNode> child2 = it2->Next ();
            if (child2->GetType () != CS_NODE_ELEMENT) continue;
            const char* value2 = child2->GetValue ();
            csStringID id2 = xmltokens.Request (value2);
	    if (id2 == XMLTOKEN_V)
	    {
	      csVector3 v;
	      if (!synserv->ParseVector (child2, v))
	        return false;
	      polygon.AddVertex (v);
	    }
	    else
	    {
	      parent->ReportError (
		"Unknown token <%s> in <polygon> for PVSCalc meta information!",
		value2);
	      return false;
	    }
	  }
	  meta_polygons.Push (polygon);
	}
	break;
      default:
        parent->ReportError (
		"Unknown token <%s> for PVSCalc meta information!",
		value);
        return false;
    }
  }
  return true;
}

//-----------------------------------------------------------------------------

PVSCalc::PVSCalc ()
{
  SetApplicationName ("CrystalSpace.PVSCalcMap");
}

PVSCalc::~PVSCalc ()
{
}

bool PVSCalc::OnInitialize (int argc, char* argv[])
{
  // RequestPlugins() will load all plugins we specify. In addition
  // it will also check if there are plugins that need to be loaded
  // from the config system (both the application config and CS or
  // global configs). In addition it also supports specifying plugins
  // on the commandline.
  if (!csInitializer::RequestPlugins (GetObjectRegistry (),
      CS_REQUEST_VFS,
      CS_REQUEST_NULL3D,
      CS_REQUEST_FONTSERVER,
      CS_REQUEST_ENGINE,
      CS_REQUEST_IMAGELOADER,
      CS_REQUEST_LEVELLOADER,
      CS_REQUEST_REPORTER,
      CS_REQUEST_REPORTERLISTENER,
      CS_REQUEST_END))
    return ReportError ("Failed to initialize plugins!");

  // Change the standard reporter listener so that it reports info to
  // standard output.
  csRef<iStandardReporterListener> stdrep = CS_QUERY_REGISTRY (object_reg,
  	iStandardReporterListener);
  stdrep->SetMessageDestination (CS_REPORTER_SEVERITY_NOTIFY, true, false,
  	false, false, false, false);

  // Now we need to setup an event handler for our application.
  // Crystal Space is fully event-driven. Everything (except for this
  // initialization) happens in an event.
  if (!RegisterQueue (GetObjectRegistry ()))
    return ReportError ("Failed to set up event handler!");

  meta_loader.AttachNew (new PVSMetaLoader (this));
  GetObjectRegistry ()->Register ((iBase*)meta_loader, "crystalspace.pvscalc");

  return true;
}

void PVSCalc::OnExit ()
{
  if (meta_loader)
  {
    GetObjectRegistry ()->Unregister ((iBase*)meta_loader, "crystalspace.pvscalc");
  }
}

bool PVSCalc::Application ()
{
  // Open the main system. This will open all the previously loaded plug-ins.
  // i.e. all windows will be opened.
  if (!OpenApplication (GetObjectRegistry ()))
    return ReportError ("Error opening system!");

  // Now get the pointer to various modules we need. We fetch them
  // from the object registry. The RequestPlugins() call we did earlier
  // registered all loaded plugins with the object registry.
  // The virtual clock.
  g3d = CS_QUERY_REGISTRY (GetObjectRegistry (), iGraphics3D);
  if (!g3d) return ReportError ("Failed to locate 3D renderer!");

  engine = CS_QUERY_REGISTRY (GetObjectRegistry (), iEngine);
  if (!engine) return ReportError ("Failed to locate 3D engine!");

  vc = CS_QUERY_REGISTRY (GetObjectRegistry (), iVirtualClock);
  if (!vc) return ReportError ("Failed to locate Virtual Clock!");

  kbd = CS_QUERY_REGISTRY (GetObjectRegistry (), iKeyboardDriver);
  if (!kbd) return ReportError ("Failed to locate Keyboard Driver!");

  loader = CS_QUERY_REGISTRY (GetObjectRegistry (), iLoader);
  if (!loader) return ReportError ("Failed to locate Loader!");

  // Here we load our world from a map file.
  if (!LoadMap ()) return false;

  // Let the engine prepare all lightmaps for use and also free all images 
  // that were loaded for the texture manager.
  engine->Prepare ();

  // Calculate the PVS.
  CalculatePVS ();

  return true;
}

bool PVSCalc::SetMapDir (const char* map_dir)
{
  csStringArray paths;
  paths.Push ("/lev/");
  csRef<iVFS> VFS = CS_QUERY_REGISTRY (object_reg, iVFS);
  if (!VFS->ChDirAuto (map_dir, &paths, 0, "world"))
  {
    ReportError ("Error setting VFS directory '%s'!", map_dir);
    return false;
  }
  return true;
}

bool PVSCalc::LoadMap ()
{
  // Set VFS current directory to the level we want to load.
  csRef<iCommandLineParser> cmdline = CS_QUERY_REGISTRY (object_reg,
  	iCommandLineParser);
  const char* mapfile = cmdline->GetName (0);
  if (!mapfile)
  {
    ReportError ("Required parameters: <mapdir/zip> [ <sectorname> ]!");
    return false;
  }

  if (!SetMapDir (mapfile))
    return false;

  // Load the level file which is called 'world'.
  if (!loader->LoadMapFile ("world"))
  {
    ReportError ("Error couldn't load world file at '%s'!", mapfile);
    return false;
  }

  // Get optional sector name. If no sector is given then all sectors
  // that have PVS will be calculated.
  sectorname = cmdline->GetName (1);

  return true;
}

bool PVSCalc::CalculatePVS (iSector* sector, iPVSCuller* pvs)
{
  PVSCalcSector pvscalcsector (this, sector, pvs);
  size_t i;
  for (i = 0 ; i < meta_info.Length () ; i++)
  {
    if (meta_info[i].sector == sector)
      if (!pvscalcsector.FeedMetaInformation (meta_info[i].meta_node))
        return false;
  }
  pvscalcsector.Calculate ();
  return true;
}

void PVSCalc::CalculatePVS ()
{
  int i;
  iSectorList* sl = engine->GetSectors ();
  bool found = false;
  for (i = 0 ; i < sl->GetCount () ; i++)
  {
    iSector* sector = sl->Get (i);
    const char* sname = sector->QueryObject ()->GetName ();
    if (sectorname.IsEmpty () || (sname != 0 &&
    	strcmp ((const char*)sectorname, sname) == 0))
    {
      iVisibilityCuller* viscul = sector->GetVisibilityCuller ();
      if (viscul)
      {
	csRef<iPVSCuller> pvs = SCF_QUERY_INTERFACE (viscul, iPVSCuller);
        if (pvs)
	{
	  found = true;
	  if (!CalculatePVS (sector, pvs))
	    return;
	}
      }
    }
  }
  if (!found)
  {
    if (sectorname.IsEmpty ())
      ReportError ("Found no sectors that have a PVS visibility culler!");
    else
      ReportError ("Sector '%s' has no PVS visibility culler!",
      	(const char*)sectorname);
  }
}

void PVSCalc::RegisterMetaInformation (iSector* sector,
	iDocumentNode* meta_node)
{
  meta_info.Push (PVSMetaInfo (sector, meta_node));
}

/*---------------------------------------------------------------------*
 * Main function
 *---------------------------------------------------------------------*/
int main (int argc, char* argv[])
{
  return PVSCalc().Main(argc, argv);
}
