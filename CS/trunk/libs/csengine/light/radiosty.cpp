/*
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
#include "csgeom/polyaa.h"
#include "csengine/radiosty.h"
#include "csengine/engine.h"
#include "csengine/sector.h"
#include "csengine/light.h"
#include "csengine/lghtmap.h"
#include "csengine/polytext.h"
#include "csengine/cspmeter.h"
#include "csengine/csppulse.h"
#include "csengine/polytmap.h"
#include "csengine/texture.h"
#include "csengine/material.h"
#include "csengine/rview.h"
#include "csengine/thing.h"
#include "csengine/meshobj.h"
#include "csgeom/math3d.h"
#include "csgeom/vector3.h"
#include "csgeom/frustum.h"
#include "qsqrt.h"
#include "igraphic/image.h"
#include "iengine/lightmap.h"
#include "ivideo/texture.h"
#include "iengine/texture.h"
#include <math.h>
#include "qint.h"

//------------ Utility ------------------------------------
float FastPow2(float x, const int y)
// computes x ** (2 ** y), using only y multiplies.
{
  float res = x;
  for(int i=1; i<y; i++)
    res *= res;
  return res;
}

//--------------- csRadElement -----------------------------------

IMPLEMENT_CSOBJTYPE(csRadElement, csObject);

csRadElement::csRadElement()
{
  last_shoot_priority = 0.0f;
  num_repeats = 0;
  deltamap = new csRGBFloatLightMap();
  copy_lightmap = NULL;
}

csRadElement::~csRadElement()
{
  delete deltamap;
  delete copy_lightmap;
}

void csRadElement::ComputePriority()
{
  float red, green, blue;
  float max;
  CapDelta(0, width, height, 255.0f);
  GetDeltaSums(red, green, blue);

  // use maximum instead of average so very colourfully lighted
  // elements will be shot earlier - they are very brightly lit.
  // and should not colour the ambient light.
  max = red;
  if(green > max) max=green;
  if(blue > max) max=blue;


  // priority is the light we expect this polygon to deliver,
  // the noticeable change it will cause.
  // the avg_lumel_delta * the diffuse.
  total_unshot_light = GetDiffuse() * max / size;

  // to prevent loops, polygons with several repeats (at the same
  // priority) are ignored.
  if(num_repeats > 5)
    total_unshot_light = 0.0;

  //CsPrintf(MSG_STDOUT, "RP %s, pri %g, area %g, sum %d, meanval %g ",
  //  polygon->GetName(), total_unshot_light, area, sum, mean_lightval);
  //CsPrintf(MSG_STDOUT, "w %d, h %d, size %d\n", width, height, size);
}

bool csRadElement::DeltaIsZero(int suv, int w, int h)
{
  for(int y=0; y<h; y++, suv += width - w)
    for(int x=0; x<w; x++, suv ++)
      if(!DeltaIsZero(suv)) return false;
  return true;
}

void csRadElement::GetTextureColour(int suv, int w, int h, csColor &avg, 
                                    csRGBLightMap *texturemap)
{
  /// Note that the texturemap must be width * height in size
  avg.red = 0.0f;
  avg.green = 0.0f;
  avg.blue = 0.0f;
  for(int y=0; y<h; y++, suv += width - w)
    for(int x=0; x<w; x++, suv ++)
    {
      avg.red += texturemap->GetRed()[suv];
      avg.green += texturemap->GetGreen()[suv];
      avg.blue += texturemap->GetBlue()[suv];
    }
  avg *= 1.0f / float( w*h );
}

void csRadElement::CapDelta(int suv, int w, int h, float max)
{
  for(int y=0; y<h; y++, suv += width - w)
    for(int x=0; x<w; x++, suv ++)
    {
      if(deltamap->GetRed()[suv] > max)
        deltamap->GetRed()[suv] = max;
      if(deltamap->GetGreen()[suv] > max)
        deltamap->GetGreen()[suv] = max;
      if(deltamap->GetBlue()[suv] > max)
        deltamap->GetBlue()[suv] = max;
    }
}


void csRadElement::GetSummedDelta(int suv, int w, int h, csColor& sum)
{
  sum.red = 0.0f;
  sum.green = 0.0f;
  sum.blue = 0.0f;
  for(int y=0; y<h; y++, suv += width - w)
    for(int x=0; x<w; x++, suv ++)
    {
      sum.red += deltamap->GetRed()[suv];
      sum.green += deltamap->GetGreen()[suv];
      sum.blue += deltamap->GetBlue()[suv];
    }
}


void csRadElement::AddDelta(csRadElement *src, int suv, int ruv,
			    float fraction, const csColor& filtercolor)
{
  deltamap->GetRed()[ruv] +=
    (fraction * src->deltamap->GetRed()[suv] * filtercolor.red);
  deltamap->GetGreen()[ruv] +=
    (fraction * src->deltamap->GetGreen()[suv] * filtercolor.green);
  deltamap->GetBlue()[ruv] +=
    (fraction * src->deltamap->GetBlue()[suv] * filtercolor.blue);
}


void csRadElement::CopyAndClearDelta()
{
  int res;
  int uv;
  int total = size*3;
  for(uv=0; uv<total; uv++)
  {
    res = lightmap->GetMap()[uv] + QRound(deltamap->GetMap()[uv]);
    if (res > 255) 
    {
      res = 255;
    }
    else if (res < 0) 
    {
      res = 0;
    }

    lightmap->GetMap()[uv] = res;
    deltamap->GetMap()[uv] = 0.0;
  }

  total_unshot_light = 0.0;
}

void csRadElement::GetDeltaSums(float &red, float &green, float &blue)
{
  red = 0.0;
  green = 0.0;
  blue = 0.0;
  for(int uv=0; uv<size; uv++)
  {
    red += deltamap->GetRed()[uv];
    green += deltamap->GetGreen()[uv];
    blue += deltamap->GetBlue()[uv];
  }
}

void csRadElement::ApplyAmbient(int red, int green, int blue)
{
  float res;
  for(int uv=0; uv<size; uv++)
  {
    res = deltamap->GetRed()[uv] + red;
    if(res>255)res=255; else if(res<0) res=0;
    deltamap->GetRed()[uv] = res;

    res = deltamap->GetGreen()[uv] + green;
    if(res>255)res=255; else if(res<0) res=0;
    deltamap->GetGreen()[uv] = res;

    res = deltamap->GetBlue()[uv] + blue;
    if(res>255)res=255; else if(res<0) res=0;
    deltamap->GetBlue()[uv] = res;
  }
}

csRGBLightMap * csRadElement::ComputeTextureLumelSized()
{
  int uv;
  csRGBLightMap *map = new csRGBLightMap();
  map->Alloc(size);
  // fill map with flat color
  int flatr = QRound(GetFlatColor().red * 255.0);
  if(flatr > 255) flatr = 255; else if (flatr < 0) flatr = 0;
  int flatg = QRound(GetFlatColor().green * 255.0);
  if(flatg > 255) flatg = 255; else if (flatg < 0) flatg = 0;
  int flatb = QRound(GetFlatColor().blue * 255.0);
  if(flatb > 255) flatb = 255; else if (flatb < 0) flatb = 0;

  for(uv=0; uv<size; uv++)
  {
    map->GetRed()[uv] = flatr;
    map->GetGreen()[uv] = flatg;
    map->GetBlue()[uv] = flatb;
  }

  // get texture of element
  csMaterialWrapper* mathandle = GetMaterialWrapper ();

  if(mathandle == NULL) // no material: flatcol is enough.
    return map;
  
  int transr=0, transg=0, transb=0; // transparent color
  
  csMaterial* mat = (csMaterial*)mathandle->GetMaterial ();
  csTextureWrapper* txthandle = mat->GetTextureWrapper ();
  txthandle->GetKeyColor(transr, transg, transb);
  
  iImage *txtimage = txthandle->GetImageFile();
  iImage *rgbimage = txtimage->Clone(); 
  
  rgbimage->SetFormat(CS_IMGFMT_TRUECOLOR); // get rgb
  
  int txtw = rgbimage->GetWidth();
  int txth = rgbimage->GetHeight();
  
  csRGBpixel *rgb = (csRGBpixel *) rgbimage->GetImageData();

  int lightcell_size = csLightMap::lightcell_size;
  int lightcell_shift = csLightMap::lightcell_shift;

  // scale down texture
  // map each lumel to the texture and scan the lightcellsize x lightcellsize
  // region of the texture map.
  int texelsperlumel_shift = lightcell_shift * 2;
  int lumel_uv = 0;
  for(int lumel_y = 0; lumel_y < height; lumel_y ++)
    for(int lumel_x = 0; lumel_x < width; lumel_x++, lumel_uv++)
    {
      /// these ints are only large enough for a lightcellsize < 4096
      int sumr = 0;
      int sumg = 0;
      int sumb = 0;
      // in texture coords the lumel is:
      int txt_start_x = (lumel_x << lightcell_shift) % txtw;
      int txt_start_y = (lumel_y << lightcell_shift) % txth;
      for(int dy = 0; dy < lightcell_size; dy++)
        for(int dx = 0; dx < lightcell_size; dx++)
	{
	  int txt_x = (dx + txt_start_x) % txtw; // modulo to wrap around
	  int txt_y = (dy + txt_start_y) % txth; // and make texture tile
	  int txt_idx = txt_y * txtw + txt_x;
	  sumr += rgb[txt_idx].red;
	  sumg += rgb[txt_idx].green;
	  sumb += rgb[txt_idx].blue;
	}
       // store averages
       map->GetRed()[lumel_uv] = sumr >> texelsperlumel_shift;
       map->GetGreen()[lumel_uv] = sumg >> texelsperlumel_shift;
       map->GetBlue()[lumel_uv] = sumb >> texelsperlumel_shift;
    }

  /*
  printf("Map for %s, %s\n", polygon->GetName(), rgbimage->GetName());
  uv = 0;
  for(int y=0; y<height; y++)
  {
    for(int x=0; x<width; x++, uv++)
      printf("%2.2x%2.2x%2.2x ", map->GetRed()[uv], map->GetGreen()[uv],
        map->GetBlue()[uv]);
    printf("\n");
  }
  */

  // get rid of rgbimage
  rgbimage->DecRef();
  return map;
}

csVector3 csRadElement::GetAvgNormal() const
{
  /// @@@ This is definately not an accurate or effecient way to calculate
  /// @@@ the average normal value.  What is?
  
  csVector3 avg(0,0,0);
  
  // sum every normal in _uv2Normal
  for(int i=0; i < width; i++)
  {
    for(int j=0; j < height; j++)
    {
      avg += GetNormal(i,j);
    }
  }

  avg /= size;
  avg.Norm();

  return avg;
}


csRadElement* csRadElement::GetRadElement(csPolygon3D &object)
{ 
  // we are attached to the original polygon as a child.
  csObject *o = object.GetChild (csRadPoly::Type);

  if (o) 
  {
    return (csRadElement*) o;
  }

  return NULL;
}

csRadElement* csRadElement::GetRadElement(csCurve &object)
{ 
  // we are attached to the original curve as a child.
  csObject *o = object.GetChild (csRadCurve::Type);

  if (o) 
  {
    return (csRadElement*) o;
  }

  return NULL;
}

void csRadElement::ShowDeltaMap ()
{
  if (copy_lightmap) return;

  copy_lightmap = new csRGBLightMap ();
  copy_lightmap->Copy (*lightmap, lightmap->GetMaxSize ());

  deltamap->CopyTo (*lightmap, lightmap->GetMaxSize ());
}

void csRadElement::RestoreStaticMap ()
{
  if (!copy_lightmap) return;

  lightmap->Copy (*copy_lightmap, copy_lightmap->GetMaxSize ());

  delete copy_lightmap;
  copy_lightmap = NULL;
}

//--------------- csRadPoly --------------------------------------

IMPLEMENT_CSOBJTYPE(csRadPoly, csObject);

csRadPoly :: csRadPoly(csPolygon3D *original, csSector* sector)
: csRadElement()
{
  csRadPoly::sector = sector;
  polygon = original;
  polygon->ObjAdd(this); // attach to original
  area = original->GetArea();
  total_unshot_light = area;
  
  //lightmap = polygon->GetLightMapInfo()->GetLightMap(); // returns the
  // 'real' lightmap, which includes dynamic lights.
  // but we need the static lightmap
  csmap = polygon->GetLightMapInfo()->GetPolyTex()->GetCSLightMap();
  //  csmap->SetLastFrustumId(0); // id's start at zero again
  
  width = csmap->GetRealWidth();
  height = csmap->GetRealHeight();
  size = csmap->GetSize();
  lightmap = &csmap->GetStaticMap();

  // all light in static map is unshot now, add it to delta. clear lightmap.
  deltamap->Copy(*lightmap, size);
  memset( lightmap->GetMap(), 0, size*3);

  // Initialize some necessary values
  Setup();

  ComputePriority();
}

csRadPoly :: ~csRadPoly()
{
  polygon->ObjRelease(this); // detach from original
}

void csRadPoly::CalcLumel2World(csVector3& res, int x, int y)
{
  // Slow getting lumel to world coords

  // see polytext.cpp for more info.
  int ww=0, hh=0;
  polygon->GetMaterialHandle()->GetTexture()->GetMipMapDimensions(0, ww, hh);
  float invww = 1. / (float)ww;
  float invhh = 1. / (float)hh;

  csPolyTexture *polytext = polygon->GetLightMapInfo()->GetPolyTex();
  csPolyTxtPlane* txt_pl = polygon->GetLightMapInfo ()->GetTxtPlane ();
  csMatrix3 *m_world2tex;
  csVector3 *v_world2tex;
  txt_pl->GetWorldToTexture(m_world2tex, v_world2tex);
  csMatrix3 m_t2w = m_world2tex->GetInverse ();
  csVector3 vv = *v_world2tex;

  csVector3 v1(0), v2;
  int lightcell_shift = csLightMap::lightcell_shift;
  int ru = x << lightcell_shift;
  int rv = y << lightcell_shift;
  int Imin_u = polytext->GetIMinU();
  int Imin_v = polytext->GetIMinV();
  v1.x = (float)(ru + Imin_u) * invww;
  v1.y = (float)(rv + Imin_v) * invhh;
  v2 = vv + m_t2w * v1;

  res = v2;
}

void csRadPoly::Setup()
{
  // setup quick conversion to lumel space.
  CalcLumel2World(lumel_origin, 0, 0);
  CalcLumel2World(lumel_x_axis, 1, 0);
  CalcLumel2World(lumel_y_axis, 0, 1);
  one_lumel_area = ABS( csMath3::Area3(lumel_origin, lumel_x_axis, 
    lumel_y_axis) );
  printf("The one lumel area is %g in size\n", one_lumel_area);
  lumel_x_axis -= lumel_origin;
  lumel_y_axis -= lumel_origin;
}

void csRadPoly::Lumel2World(csVector3& res, int x, int y)
{
  // Quick getting lumel to world coords
  res = lumel_origin + x* lumel_x_axis + y * lumel_y_axis;
}

csColor csRadPoly::GetFlatColor() const
{
  csRGBpixel color;
  polygon->GetMaterialHandle ()->GetFlatColor (color);
  return csColor (color.red / 255.0, color.green / 255.0, color.blue / 255.0);
}

//--------------- csRadCurve -------------------------------------

IMPLEMENT_CSOBJTYPE(csRadCurve, csObject);

csRadCurve :: csRadCurve(csCurve *original, csSector* sector)
: csRadElement()
{
  csRadCurve::sector = sector;
  curve = original;
  curve->ObjAdd(this); // attach to original
  total_unshot_light = original->GetArea();
  
  //lightmap = polygon->GetLightMapInfo()->GetLightMap(); // returns the
  // 'real' lightmap, which includes dynamic lights.
  // but we need the static lightmap
  csmap = curve->GetLightMap();
  //  csmap->SetLastFrustumId(0); // id's start at zero again
  
  width = csmap->GetRealWidth();
  height = csmap->GetRealHeight();
  size = csmap->GetSize();
  lightmap = &csmap->GetStaticMap();

  // all light in static map is unshot now, add it to delta. clear lightmap.
  deltamap->Copy(*lightmap, size);
  memset( lightmap->GetMap(), 0, size*3);

  // Initialize some necessary values
  Setup();

  ComputePriority();
}

csRadCurve :: ~csRadCurve()
{
  curve->ObjRelease(this); // detach from original
}


void csRadCurve::Setup()
{
  one_lumel_area = curve->GetArea() / size;
}

void csRadCurve :: Lumel2World(csVector3& res, int x, int y)
{
  /// use the curves precalculated uv buffers
  res = curve->_uv2World[y*width + x];
}

const csVector3& csRadCurve::GetNormal(int x, int y) const
{
  /// use the curves precalculated uv buffers
  return curve->_uv2Normal[y*width + x];
}

//--------------- csRadTree --------------------------------------
/**
 *  Note: the right subtree contains all elements >= the current.
 *  The left subtree all elements < the current.
 */

void csRadTree::Insert(csRadElement *e)
{
  // find spot
  csRadTree *spot = this;
  csRadTree *parent = NULL;
  while(spot != NULL)
  {
    parent = spot;
    if(e->GetPriority() >= spot->GetPriority())
      spot = spot->right;
    else spot = spot->left;
  }
  // spot is 0, parent can insert new leaf now.
  if(e->GetPriority() >= parent->GetPriority())
    parent->right = new csRadTree(e, 0, 0);
  else parent->left = new csRadTree(e, 0, 0);
  // done.
}


void csRadTree :: DelNode()
{
  // deletes this node with both left & right subtrees
  // so left != NULL && right != NULL.
  // because tree has >= elements in its right subtree, we must
  // switch this element with the smallest el. of the right subtree.
  // Since that element may be equal in priority to this one.

  csRadTree *parent = 0;
  csRadTree *replacement = right->FindLeftMost(parent);
  // swap contents of replacement and this
  element = replacement->element;
  // get rid of replacment. replacement.left == NULL.
  if(parent==NULL)
  {
    // I am parent
    right = replacement->right;
    replacement->right = NULL;
    delete replacement;
    return;
  }
  parent->left = replacement->right;
  replacement->right = NULL;
  delete replacement;
}

csRadTree* csRadTree::Delete(csRadElement *e)
{
  // find spot containing p, and its parent
  csRadTree *spot = this;
  csRadTree *parent = NULL;

  while(spot != NULL && spot->element != e)
  {
    parent = spot;
    if(e->GetPriority() >= spot->GetPriority())
      spot = spot->right;
    else spot = spot->left;
  }
  
  if(spot==NULL) // no such element
    return this;
  
  // spot is the element containing p, parent is parent of spot.
  if(parent == NULL) // no parent, this el. contains things.
  {
    csRadTree *newroot = 0;
    if(left==NULL)
    {
      newroot = right;
      right = NULL;
      delete this;
      return newroot;
    }
    if(right==NULL)
    {
      newroot = left;
      left = NULL;
      delete this;
      return newroot;
    }
    // this node has a left & a right subtree
    DelNode();
    return this;
  }
  // spot has a parent
  if(spot->left!=NULL && spot->right!=NULL)
  {
    // spot has both subtrees
    spot->DelNode();
    return this;
  }
  // one subtree is missing, substitute the remaining one for spot.
  // remaining one may be NULL.
  csRadTree *replacement = 0;
  float spotpri = spot->GetPriority();
  if(spot->left == NULL)
  {
    replacement = spot->right;
    spot->right = NULL;
    delete spot;
  }
  else // else! do not do both.
    if(spot->right == NULL) 
  {
    replacement = spot->left;
    spot->left = NULL;
    delete spot;
  }
  if(spotpri >= parent->GetPriority() )
    parent->right = replacement;
  else parent->left = replacement;
  return this;
}


csRadTree* csRadTree::PopHighest(csRadElement*& e)
{
  csRadTree *parent = 0;
  csRadTree *spot = FindRightMost(parent);
  e = spot->element;
  if(parent)
  {
    parent->right = spot->left;
    spot->left = NULL;
    delete spot;
    return this;
  }
  else
  { // this node is deleted, and no right subtree
    csRadTree* newroot = spot->left;
    spot->left = NULL;
    delete spot;
    return newroot;
  }
}


csRadTree* csRadTree::FindLeftMost(csRadTree*& parent)
{
  parent = NULL;
  csRadTree *spot = this;
  while(spot->left != NULL)
  {
    parent = spot;
    spot = spot->left;
  }
  return spot;
}

csRadTree* csRadTree::FindRightMost(csRadTree*& parent)
{
  parent = NULL;
  csRadTree *spot = this;
  while(spot->right != NULL)
  {
    parent = spot;
    spot = spot->right;
  }
  return spot;
}

void csRadTree::TraverseInOrder( void (*func)( csRadElement * ) )
{
  if(left) left->TraverseInOrder( func );
  func(element);
  if(right) right->TraverseInOrder( func );
}

//--------------- csRadList --------------------------------------
csRadList :: csRadList()
{
  tree = 0;
  num = 0;
}

static void deletefunc (csRadElement* e)
{
  delete e;
}

csRadList :: ~csRadList()
{
  if(tree){
    tree->TraverseInOrder( deletefunc );
    delete tree;
  }
}


void csRadList :: InsertElement(csRadElement *e)
{
  if(!tree) 
  {
    tree = new csRadTree(e, 0, 0);
  }
  else 
  {
    tree->Insert(e);
  }

  num++;
}


void csRadList::DeleteElement(csRadElement *e)
{
  if(tree) 
  {
    tree = tree->Delete(e);
    num--;
  }
}


csRadElement* csRadList :: PopHighest()
{
  csRadElement *e;

  if(!tree) 
  {
    return NULL;
  }

  tree = tree->PopHighest(e);
  num--;
  
  return e;
}

static void print_func( csRadElement *e )
{
  CsPrintf(MSG_STDOUT, "csRadList: csRadElement %x, pri %f \n",
    (int)e, e->GetPriority() );
}

void csRadList :: Print()
{
  // for debug purposes
  CsPrintf(MSG_STDOUT, "csRadList Print().\n");

  if (tree) 
  {
    tree->TraverseInOrder( print_func );
  }
  else 
  {
    CsPrintf(MSG_STDOUT, "csRadList empty.\n");
  }

  CsPrintf(MSG_STDOUT, "csRadList Print() end.\n");
}

//--------------- csRadiosity --------------------------------------

// configuration defaults
bool  csRadiosity::do_static_specular = false;
float csRadiosity::static_specular_amount = 0.70;
int   csRadiosity::static_specular_tightness = 2;
float csRadiosity::colour_bleed = 1.0;
float csRadiosity::stop_priority = 0.1;
float csRadiosity::stop_improvement = 10000.0;
int   csRadiosity::stop_iterations = 1000;
int   csRadiosity::source_patch_size = 2;

csRadiosity :: csRadiosity(csEngine *current_engine)
{
  CsPrintf (MSG_INITIALIZATION, "\nPreparing radiosity...\n");
  iterations = 0;
  engine = current_engine;
  meter = new csProgressMeter(engine->System, 1000);
  meter->SetGranularity(1);
  pulse = new csProgressPulse(engine->System);
  // copy data needed, create list and all radpolys
  list = new csRadList();

  // fill list with polygons
  csPolyIt poly_it(engine);

  csPolygon3D* poly;
  while ( (poly = poly_it.Fetch()) != NULL)
  {
    if( (poly->GetUnsplitPolygon() == NULL) && // only for original polygons 
	   // in the list, not the split children also in list.
         poly->GetLightMapInfo() && // only for lightmapped polys
         poly->GetLightMapInfo()->GetPolyTex()->GetCSLightMap())
    {
      list->InsertElement(new csRadPoly(poly, poly_it.GetLastSector ()));
    }
  }

  // fill list with curves
  csCurveIt curve_it(engine);
  csCurve* curve;
  while ( (curve = curve_it.Fetch()) != NULL)
  {
    if(curve->GetLightMap()) // only for lightmapped curves
    {
      list->InsertElement(new csRadCurve(curve, curve_it.GetLastSector ()));
    }
  }
  
  /// remove ambient light from maps.
  RemoveAmbient();
  texturemap = 0;
  shadow_matrix = 0;

  showing_deltamaps = false;
}

csRadiosity :: ~csRadiosity()
{
  delete texturemap;
  delete shadow_matrix;
  delete pulse;
  delete meter;
  // remove data needed.
  delete list;
}

static void show_deltamap (csRadElement *p)
{
  p->ShowDeltaMap ();
}

static void restore_staticmap (csRadElement *p)
{
  p->RestoreStaticMap ();
}

void csRadiosity :: ToggleShowDeltaMaps ()
{
  if (showing_deltamaps)
  {
    RestoreStaticMaps ();
    return;
  }
  showing_deltamaps = true;
  list->Traverse (show_deltamap);
}

void csRadiosity :: RestoreStaticMaps ()
{
  if (!showing_deltamaps) return;
  showing_deltamaps = false;
  list->Traverse (restore_staticmap);
}

csPolygon3D* csRadiosity :: GetNextPolygon ()
{
  csRadElement *shoot;

  shoot = list->PopHighest();
  if(shoot)
  {
    start_priority = shoot->GetPriority();
    list->InsertElement(shoot);
    // @@@ Only for polygons for now!!!
    csRadPoly* radpoly = (csRadPoly*)shoot;
    return radpoly->GetPolygon3D ();
  }
  return NULL;
}

bool csRadiosity :: DoRadiosityStep (int steps)
{
  RestoreStaticMaps ();
  csRadElement *shoot;

  shoot = list->PopHighest();
  if(shoot) {
    start_priority = shoot->GetPriority();
    list->InsertElement(shoot);
  }

  // do the work
  // Take RadPoly with highest unshot light amount, and distribute
  // the light to other polys (incrementing their unshot amount).
  // until stability, in theory, in practice some stop-condition.

  while (steps > 0 && (shoot = FetchNext()) != NULL)
  {
    steps--;
    iterations++;
    // shoot the light off of RadPoly.
    CsPrintf(MSG_STDOUT, "(priority at %f).\n", shoot->GetPriority() );
    // prepare to shoot from source (visibility, precompute, etc)
    PrepareShootSource(shoot);

    // start the frustum calcs.
    StartFrustum();
    // have shot all from shootrad.
    shoot->CopyAndClearDelta();
    list->InsertElement(shoot); 
  }

  if (steps <= 0)
    return true;
  else
  {
    ApplyDeltaAndAmbient();
    return false;
  }
}

void csRadiosity :: DoRadiosity()
{
  csRadElement *shoot;

  CsPrintf (MSG_INITIALIZATION, "Calculating radiosity (%d lightmaps):\n",
    list->GetNumElements());
  CsPrintf(MSG_INITIALIZATION, "  ");
  meter->Restart();
  shoot = list->PopHighest();
  if(shoot) {
    start_priority = shoot->GetPriority();
    list->InsertElement(shoot);
  }

  // do the work
  // Take RadPoly with highest unshot light amount, and distribute
  // the light to other polys (incrementing their unshot amount).
  // until stability, in theory, in practice some stop-condition.

  while( (shoot = FetchNext()) != NULL)
  {
    iterations++;
    // shoot the light off of RadPoly.
    CsPrintf(MSG_STDOUT, "(priority at %f).\n", shoot->GetPriority() );
    pulse->Step();
    // prepare to shoot from source (visibility, precompute, etc)
    PrepareShootSource(shoot);

    // start the frustum calcs.
    StartFrustum();
    // have shot all from shootrad.
    shoot->CopyAndClearDelta();
    list->InsertElement(shoot); 
    pulse->Erase();
  }
  ApplyDeltaAndAmbient();
}


csRadElement* csRadiosity :: FetchNext()
{
  // you can define any stop moment you like. And stop here anytime you like
  // by returning NULL. The remaining unshot light will be added as
  // ambient light

  float stop_value = 0.1000; // the amount of unshot light, where we can stop.

  /// take first stop moment, do the least work expected.
  if(stop_priority > start_priority / stop_improvement)
  {
    stop_value = stop_priority;
  }
  else 
  {
    stop_value = start_priority / stop_improvement;
  }

  int max_repeats = 10; // after n loops, stop. Should never be necessary.
                        // due to loop detection in csRadPoly. For robustness.

  csRadElement* element = list->PopHighest();

  float nextpriority = element ? element->GetPriority() : 0.0;
  
  float val = (start_priority - nextpriority ) / start_priority;
  
  if (val < 0.0f)
  {
    val=0.0f;
  }

  val = pow(val, 2.0f) * 0.98f;

  int ticks_now = QRound( val * meter->GetTotal());
  
  //CsPrintf(MSG_STDOUT, "New value %g, ticks at %d / %d\n",
  //  val, ticks_now, meter->GetTotal());
  
  while(meter->GetCurrent() < ticks_now)
  {
    meter->Step();
  }

  // stop varibles
  bool stop_now = false;
  char reason[80];
 
  if (element == NULL)
  {
    stop_now = true;
    sprintf(reason, "no polygons to light");
  }
  else if (element->GetPriority() < stop_value)
  {
    stop_now = true;
    sprintf(reason, "priority down to %g", element->GetPriority());
  }
  else if (iterations > stop_iterations)
  {
    stop_now = true;
    sprintf(reason, "%d iterations reached", iterations);
  }
  else if(element->GetNumRepeats() > max_repeats)
  {
    stop_now = true;
    sprintf(reason, "loop detected");
  }
  /// more stop conditions can be put here.

  if(stop_now)
  {
    while(meter->GetCurrent() < meter->GetTotal())
    {
      meter->Step();
    }

    CsPrintf(MSG_INITIALIZATION, "\n");
    CsPrintf(MSG_INITIALIZATION, "Finished radiosity (%s).\n", reason);
    
    if(element) list->InsertElement(element); // to prevent memory leak.
    
    return NULL;
  }

  if (nextpriority == element->GetLastShootingPriority())
  {
    element->IncNumRepeats();
  }

  element->SetLastShootingPriority(nextpriority);
  
  return element;
}


void csRadiosity :: PrepareShootSource(csRadElement *src)
{
  shoot_src = src;

  src_normal = - shoot_src->GetAvgNormal();

  source_poly_patch_area = shoot_src->GetOneLumelArea();

  delete texturemap;

  texturemap = shoot_src->ComputeTextureLumelSized();
}


static void frustum_polygon_report_func (csObject *obj, csFrustumView* lview);
static void frustum_curve_report_func (csObject *obj, csFrustumView* lview);

void csRadiosity :: StartFrustum()
{
  csFrustumView *lview = new csFrustumView();
  csFrustumContext* ctxt = lview->GetFrustumContext ();
  csLightingInfo& linfo = ctxt->GetLightingInfo ();
  linfo.SetGouraudOnly (false);
  // the resulting color can be used as a filter
  linfo.SetColor (csColor (1, 1, 1));
  lview->SetUserData ((void*) this);
  lview->SetCurveFunction (frustum_curve_report_func);
  lview->SetPolygonFunction (frustum_polygon_report_func);
  lview->SetRadius (10000000.0); // should be enough
  lview->EnableThingShadows (true);
  lview->SetDynamic (false);
  lview->SetShadowMask (CS_ENTITY_NOSHADOWS, 0);
  lview->SetProcessMask (CS_ENTITY_NOLIGHTING, 0);

  // start from the center of the shooting poly.
  // this will lead to inaccuracy as each lumel of the shooting
  // poly is it's own center. But that is too slow.
  csVector3 center; 

  // And this leads to sharper shadows as well.
  shoot_src->Lumel2World(center, QInt (shoot_src->GetWidth()/2.0),
                         QInt (shoot_src->GetHeight()/2.0));

  center -= shoot_src->GetAvgNormal() * 0.1f;

  ctxt->SetLightFrustum (new csFrustum (center));
  ctxt->GetLightFrustum ()->MakeInfinite ();
  
  // add a backplane to frustum to clip to it... But which plane?
  //csPlane3 *src_plane = shoot_src->GetPolygon3D()->GetPolyPlane();
  //lview->GetLightFrustum ()->SetBackPlane(* src_plane);

  /// setup some vectors so we can test on plane location
  //plane_origin = shoot_src->GetPolygon3D()->Vwor(0);
  //plane_v1 = shoot_src->GetPolygon3D()->Vwor(1) - plane_origin;
  //plane_v2 = shoot_src->GetPolygon3D()->Vwor(2) - plane_origin;

  shoot_src->GetSector()->CheckFrustum ((iFrustumView*)lview);

  delete lview;
}


static void frustum_curve_report_func (csObject *obj, csFrustumView* lview)
{ 
  // obtain RadCurve
  csRadElement *dest = csRadElement::GetRadElement(*(csCurve*)obj);

  if(dest)
  {
    /// radiosity to this curve.
    csRadiosity *rad = (csRadiosity*)lview->GetUserData ();
    rad->ProcessDest (dest, lview);
  }
}

static void frustum_polygon_report_func (csObject *obj, csFrustumView* lview)
{
  csPlane3 poly_plane;
  // radiosity works with the base, unsplit polygon.
  csPolygon3D *destpoly3d = ((csPolygon3D*)obj)->GetBasePolygon();
  // obtain radpoly
  csRadElement *dest = csRadElement::GetRadElement(*destpoly3d);
  // if polygon not lightmapped/radiosity rendered, it can still be a portal.
  csFrustum* light_frustum = lview->GetFrustumContext ()->GetLightFrustum ();

  // check poly -- on right side of us?
  const csPlane3& wplane = destpoly3d->GetPlane ()->GetWorldPlane ();
  if (!csMath3::Visible (light_frustum->GetOrigin (), wplane))
    return;

  csFrustumContext* old_ctxt = lview->GetFrustumContext ();
  lview->CreateFrustumContext ();
  csFrustumContext* new_ctxt = lview->GetFrustumContext ();
  new_ctxt->SetLightFrustum (NULL);
  csVector3 poly[40];
  const csVector3& center = light_frustum->GetOrigin ();
  int num_vert = 4;
  int num_vertices = destpoly3d->GetVertices ().GetNumVertices ();
  int j;
  if(dest)
  {
    if( !destpoly3d->GetLightMapInfo()->GetPolyTex()->
        GetLightmapBounds(center, old_ctxt->IsMirrored (), poly) )
      /// empty intersection or lightmap has already been seen by frustum.
      goto stop;
  }
  else
  {
    /// clip to polygon instead of lightmap if no lightmap.
    /// @@@ hope that poly array is big enough
    if (old_ctxt->IsMirrored ())
      for (j = 0 ; j < num_vertices ; j++)
        poly[j] = destpoly3d->Vwor (num_vertices - j - 1) - center;
    else
      for (j = 0 ; j < num_vertices ; j++)
        poly[j] = destpoly3d->Vwor (j) - center;
    num_vert = num_vertices;
  }
  new_ctxt->SetLightFrustum (light_frustum->Intersect(poly, num_vert));
  // empty intersection, none covered (will be skipped)
  if(!new_ctxt->GetLightFrustum ()) goto stop;

  // uses polygon3d of *base* polygon...
  csPortal *po;
  po = destpoly3d->GetPortal();

  poly_plane = *destpoly3d->GetPolyPlane ();
  // First translate plane to center of frustum.
  poly_plane.DD += poly_plane.norm * center;
  poly_plane.Invert ();
  if (!destpoly3d->MarkRelevantShadowFrustums (*lview, poly_plane))
    goto stop;

  if(dest)
  {
    /// radiosity to this polygon.
    csRadiosity *rad = (csRadiosity*)lview->GetUserData ();
    rad->ProcessDest(dest, lview);
  }
  
  /// portal?
  if (!po) goto stop;
  /// @@@ hope that poly array is big enough
  if (old_ctxt->IsMirrored ())
    for (j = 0 ; j < num_vertices ; j++)
      poly[j] = destpoly3d->Vwor (num_vertices - j - 1) - center;
  else
    for (j = 0 ; j < num_vertices ; j++)
      poly[j] = destpoly3d->Vwor (j) - center;
 
  delete new_ctxt->GetLightFrustum ();
  new_ctxt->SetLightFrustum (old_ctxt->
  	GetLightFrustum ()-> Intersect(poly,num_vertices));
  if (!new_ctxt->GetLightFrustum ()) goto stop;

  po->CheckFrustum (*lview, destpoly3d->GetAlpha ());

stop:
  lview->RestoreFrustumContext (old_ctxt);
}


void csRadiosity :: ProcessDest(csRadElement *dest, csFrustumView *lview)
{
  if(shoot_src == dest) return; // different elements required. or we 
    			//might requeue 'shoot', and corrupt the list.

  // prepare to send/receive light.
  if(!PrepareShootDest(dest, lview))
  {
    delete shadow_matrix; 
    shadow_matrix = 0;
    return;
  }

  ShootRadiosityToElement(dest);
  delete shadow_matrix; 
  shadow_matrix = 0;
  list->DeleteElement(dest); // get out of tree
  dest->ComputePriority(); // recompute dest's unshot light
  list->InsertElement(dest); // and requeue dest
}


bool csRadiosity :: PrepareShootDest(csRadElement *dest, csFrustumView *lview)
{
  shoot_dest = dest;
  // compute the factor for the light getting through. The same for
  // every lumel. This factor is expanded to be the formfactor * areafraction

  // * diffusereflection.
  factor = source_poly_patch_area * shoot_dest->GetDiffuse() / (float)PI;

  // note that the Normal's must be UnitLength for this to work.
  // to do: factor *= cos(shootangle) * cos(destangle) * H(i,j) * src_area
  //                  ----------------------------------------------------
  //                      dist * dist

  // Note: when bumpmaps arrive, you can make radiosity take them
  // into account. Simply have GetNormal() return the normal vector
  // for the specific lumel-coords. This will shade according to
  // the bump-map.
  // The same sort of thing could be used to make curved surfaces look
  // smoother, i.e. compute the normal for the location again.
  // The Normal must have length 1, in order to work here.

  // @@@: this will be calculated elsewhere:
  // dest_normal = - shoot_dest->GetNormal();

  // use filter colour from lview
  csLightingInfo& linfo = lview->GetFrustumContext ()->GetLightingInfo ();
  trajectory_color = linfo.GetColor ();

  // use shadows and light from lview
  // gets coverage matrix from polytext.cpp, so the code is shared
  // between regular lighting and radiosity lighting, prevents bugs.
  shadow_matrix = new csCoverageMatrix(dest->GetWidth(), dest->GetHeight());
  dest->GetCoverageMatrix(lview, shadow_matrix);

  return true;
}


void csRadiosity :: ShootRadiosityToElement(csRadElement* dest)
{
  // shoot from each lumel, also a radiosity patch, to each lumel on other.
#if 1
  csRadPoly* rp_src = (csRadPoly*)shoot_src;
  csRadPoly* rp_dest = (csRadPoly*)dest;
  CsPrintf(MSG_STDOUT,
        "Shooting from RadPoly %x (%s in %s sz %d) to %x (%s in %s sz %d).\n",
  	(int)shoot_src, rp_src->GetPolygon3D()->GetName(), 
	rp_src->GetSector()->GetName(), 
	shoot_src->GetSize(), 
	(int)dest, rp_dest->GetPolygon3D()->GetName(), 
	rp_dest->GetSector()->GetName(), dest->GetSize());
#endif

  int sx, sy, rx, ry; // shoot x,y, receive x,y
  int suv = 0, ruv = 0; // shoot uv index, receive uv index.
  int suv_x_inc = source_patch_size;
  // increment x by patch size, but at the end of every line, we will be
  // patchsize - width % patchsize too far to be on the start of the next line.
  // unless width % patchsize == 0, when we will be right on the good spot
  // so for a (10x5 lightmap) and a 3x3 source patches, it will be scanned
  // at uv = 0, 3, 6, 9, and uv=12 at end, 3 - 10%3 too far.
  // then 20 - 2, 18 have to be skipped more to arrive at uv = 30, the
  // start of line 3 where we can scan the rest.
  // But when the patch_size equals 1, no y skip is required, since the
  // uv will continue nicely.
  int suv_y_inc = (source_patch_size-1) * shoot_src->GetWidth();
  if(shoot_src->GetWidth() % source_patch_size != 0)
    suv_y_inc -=
      (source_patch_size - shoot_src->GetWidth() % source_patch_size);

  for(sy=0; sy<shoot_src->GetHeight(); sy+=source_patch_size, suv+=suv_y_inc)
  {
   srcp_height = source_patch_size;
   if(srcp_height + sy > shoot_src->GetHeight())
     srcp_height = shoot_src->GetHeight() - sy;
   for(sx=0; sx<shoot_src->GetWidth(); sx+=source_patch_size, suv+=suv_x_inc)
   {
     srcp_width = source_patch_size;
     if(srcp_width + sx > shoot_src->GetWidth())
       srcp_width = shoot_src->GetWidth() - sx;
     // if source lumel delta == 0 or lumel not visible, skip.
     if(shoot_src->DeltaIsZero(suv, srcp_width, srcp_height)) continue;
     // get source lumel info
     PrepareShootSourceLumel(sx, sy, suv);
     ruv = 0;
     for(ry=0; ry<dest->GetHeight(); ry++)
       for(rx=0; rx<dest->GetWidth(); rx++, ruv++)
       {
         ShootPatch(rx, ry, ruv);
       }
    }
  }
}


void csRadiosity :: PrepareShootSourceLumel(int sx, int sy, int suv)
{
  src_uv = suv;
  src_x = sx;
  src_y = sy;
  shoot_src->Lumel2World(src_lumel, QInt (sx + srcp_width / 2.0),
    QInt (sy + srcp_height / 2.0));
  /// use the size of a lumel in the source poly *
  /// the amount of the lumel visible to compute area of sender.
  /// factor is included, which saves a lot of multiplications.
  /// does not include the patch size, the source patch width * source
  /// patch height. Simply because, the totaldelta is obtained by summing the
  /// lumel deltas, and thus is already srcp_width*srcp_height too large.
  /// and by not including the factor here, and not averaging delta, but
  /// taking summed delta unneeded computation is avoided.
  source_patch_area = factor;
  // color is texture color filtered by trajectory (i.e. half-transparent-
  // portals passed by from source to dest poly)
  shoot_src->GetTextureColour(suv, srcp_width, srcp_height, src_lumel_color,
    texturemap);
  src_lumel_color.red *= trajectory_color.red * colour_bleed / 255.0f ;
  src_lumel_color.green *= trajectory_color.green * colour_bleed / 255.0f ;
  src_lumel_color.blue *= trajectory_color.blue * colour_bleed / 255.0f ;
  // cap source delta to prevent explosions of light
  // now in ComputePriority to prevent the queue to be soiled with
  // exploded values.
  //float cap = 255.0f;
  //shoot_src->CapDelta(suv, srcp_width, srcp_height, cap);
  // get delta in colour
  shoot_src->GetSummedDelta(suv, srcp_width, srcp_height, delta_color);
  csColor colored_delta = delta_color; /// delta * src lumel color
  colored_delta.red *= src_lumel_color.red;
  colored_delta.green *= src_lumel_color.green;
  colored_delta.blue *= src_lumel_color.blue;
  /// now take a weighted average depending on colour_bleed
  delta_color += colored_delta; /// for 1.0 + colourbleed of delta
  delta_color *= 1.0 / (1.0 + colour_bleed);
}


void csRadiosity :: ShootPatch(int rx, int ry, int ruv)
{
  // check visibility
  float visibility = shadow_matrix->coverage[ruv];
  //if(visibility <= SMALL_EPSILON) return;

  // prepare dest lumel info
  shoot_dest->Lumel2World(dest_lumel, rx, ry);

  // compute formfactors.
  csVector3 path = dest_lumel - src_lumel;
  dest_normal = shoot_dest->GetNormal(rx, ry);
  csVector3 pathangle = path;
  pathangle.Normalize();

  // Since 'path' is not normalized the cosinus calculated below
  // is not really a cosinus. But it doesn't matter for our calculations.
  // If specular is enabled it matters and in that case we correct it.
  float cossrcangle;
  float cosdestangle;
  //// linear version
  //float distance = path.Norm ();
  //float sqdistance = distance * distance;
  //// squared version
  float distance;

  if( 0 && source_patch_size == 1)
  {
    /// although this is OK code, 
    /// taking only one normal at short distances is bad.
    /// an average works much better then.
    cossrcangle = src_normal * pathangle;
    cosdestangle = - (dest_normal * pathangle);
    distance = path.SquaredNorm ();
  } else
  {
    /// average over the source area
    /// take the 4 corners.
    csVector3 pos;
    cossrcangle = 0.0;
    cosdestangle = 0.0;
    distance = 0.0;

    shoot_src->Lumel2World(pos, src_x, src_y);
    cossrcangle += src_normal * (dest_lumel - pos).Unit();
    cosdestangle += - (dest_normal * (dest_lumel - pos).Unit());
    distance += (dest_lumel - pos).SquaredNorm();
    shoot_src->Lumel2World(pos, src_x + srcp_width, src_y);
    cossrcangle += src_normal * (dest_lumel - pos).Unit();
    cosdestangle += - (dest_normal * (dest_lumel - pos).Unit());
    distance += (dest_lumel - pos).SquaredNorm();
    shoot_src->Lumel2World(pos, src_x + srcp_width, src_y + srcp_height);
    cossrcangle += src_normal * (dest_lumel - pos).Unit();
    cosdestangle += - (dest_normal * (dest_lumel - pos).Unit());
    distance += (dest_lumel - pos).SquaredNorm();
    shoot_src->Lumel2World(pos, src_x, src_y + srcp_height);
    cossrcangle += src_normal * (dest_lumel - pos).Unit();
    cosdestangle += - (dest_normal * (dest_lumel - pos).Unit());
    distance += (dest_lumel - pos).SquaredNorm();

    cossrcangle /= 4.0; 
    cosdestangle /= 4.0; 
    distance /= 4.0; 
  }

  float sqdistance = distance;
  
  float totalfactor;
  //// avoid divide by zero
  if (sqdistance < 0.00001f) totalfactor = source_patch_area * visibility;
  else
    // This function calculates radiosity with squared light attenuation
    // what CS uses is linear attenuation in lighting.
    totalfactor = cossrcangle * cosdestangle * 
      source_patch_area * visibility / (sqdistance);
  
  //if(totalfactor > 10.0f) totalfactor = 10.0f;

  if(totalfactor < 0.0)
  {
    //CsPrintf(MSG_STDOUT, "totalfactor was %g\n", totalfactor);
    totalfactor = -totalfactor;
  }
#if 1
  //if(totalfactor > 10.0f)
  //if(totalfactor > 0.001f)
  if( (rand()%100000==0)
     ||(totalfactor > 10.0f) )
    CsPrintf(MSG_STDOUT, "totalfactor %g = "
  	"cosshoot %g * cosdest %g * area %g * vis %g / sqdis %g.  "
	"srclumelcolor (%g, %g, %g), deltacolor (%g, %g, %g)\n", 
  	totalfactor, cossrcangle, cosdestangle,
 	source_patch_area, visibility, sqdistance,
	src_lumel_color.red, src_lumel_color.green, src_lumel_color.blue,
	delta_color.red, delta_color.green, delta_color.blue);
#endif

  //shoot_dest->AddDelta(shoot_src, src_uv, ruv, totalfactor, src_lumel_color);
  //if(totalfactor > 0.000001) 
    shoot_dest->AddToDelta(ruv, delta_color * totalfactor * 1.);

  // specular gloss
  // direction of the 'light' on dest is -path
  // normal at this destlumel is dest_normal
  // viewing direction, below, equal to normal (look at polygon frontally)
  if(!csRadiosity::do_static_specular) 
  {
    return;
  }
 
  // Now we need to correct the cosinus factor so that it
  // really represents a cosinus.
  if(distance < 0.1f) return; // also prevents divide by zero
  float inv_distance = qisqrt (distance);
  path *= inv_distance;
  cosdestangle *= inv_distance;
  
  /// take angle as positive
  if(cosdestangle<0.0)cosdestangle=-cosdestangle;

  csVector3 viewdir = dest_normal;

  csVector3 reflectdir =
    (2.0f * dest_normal * (cosdestangle) - path) * dest_normal;
 
  double val = ( reflectdir * viewdir );
  
  if(val<0.0) val=-val;
  if(val<SMALL_EPSILON) return;
  
  if (val>1.0) 
    val=1.0;
  //alternative specular computation
  //csVector3 halfdir = (-path + viewdir);
  //halfdir.Normalize();
  //double val = ABS( halfdir * dest_normal );
  //float gloss = spec_amt * pow(val, spec_tightness);
  float gloss = static_specular_amount * 
    FastPow2(val, static_specular_tightness);
  
#if 0
    CsPrintf(MSG_STDOUT, "Gloss %g, val=%g, reflect %g,%g,%g\n",
      gloss, val, reflectdir.x, reflectdir.y, reflectdir.z);
#endif

  // add delta using both gloss and totalfactor
  //shoot_dest->AddDelta(shoot_src, src_uv, ruv, gloss*totalfactor,
  //  src_lumel_color);

  gloss *= source_patch_area * visibility / sqdistance;
  // add gloss seperately -- too much light this way
  //shoot_dest->AddDelta(shoot_src, src_uv, ruv, gloss, trajectory_color );
  shoot_dest->AddToDelta(ruv, gloss * delta_color);
}


static volatile float total_delta_color_red;
static volatile float total_delta_color_green;
static volatile float total_delta_color_blue;
static volatile float total_reflect = 0.0;
static volatile float total_area = 0.0;

static void calc_ambient_func(csRadElement *p)
{
  if(p->GetNumRepeats() > 5) // skip repeat-disabled polygons
    return; // they will be too bright anyway, don't spread the problem

  float this_area = p->GetOneLumelArea() * p->GetSize();
  // and not p->GetArea (which is only polygon, but whole lightmap will shine)

  total_area += this_area;
  total_reflect += p->GetDiffuse() * this_area;
  
  float red, green, blue;
  p->GetDeltaSums(red, green, blue);

#if 0
  CsPrintf(MSG_STDOUT, "added %x (%g) delta %g, %g, %g.\n",
   (int)p, p->GetPriority(),
   red/ p->GetSize(), green/ p->GetSize(), blue/ p->GetSize());
#endif

  total_delta_color_red += red * p->GetOneLumelArea() ;
  total_delta_color_green += green * p->GetOneLumelArea() ;
  total_delta_color_blue += blue * p->GetOneLumelArea() ;
}

/// add the ambient sec, without * diffuse it.
static void add_ambient_sec_func(csRadElement *p)
{
  p->ApplyAmbient(QRound((float)total_delta_color_red), 
    QRound((float)total_delta_color_green), 
    QRound((float)total_delta_color_blue));
}

static void apply_ambient_func(csRadElement *p)
{
  p->ApplyAmbient(QRound((float)total_delta_color_red * p->GetDiffuse()), 
    QRound((float)total_delta_color_green * p->GetDiffuse()), 
    QRound((float)total_delta_color_blue * p->GetDiffuse()));
}

static void add_delta_func(csRadElement *p)
{
  p->CopyAndClearDelta();
}

void csRadiosity :: RemoveAmbient()
{
  total_delta_color_red = -csLight::ambient_red;
  total_delta_color_green = -csLight::ambient_green;
  total_delta_color_blue = -csLight::ambient_blue;
  list->Traverse(add_ambient_sec_func);
}

void csRadiosity :: ApplyDeltaAndAmbient()
{
  /// first calculate the Ambient
  total_delta_color_red = 0.0;
  total_delta_color_green = 0.0;
  total_delta_color_blue = 0.0;
  total_reflect = 0.0;
  total_area = 0.0;

  list->Traverse(calc_ambient_func);

  total_reflect /= total_area;
  
  /// the usual formula - 1.0
  /// subtraction, as the deltamaps will already get added directly.
  /// thus the 1.0, immediate path to the polygon is already added.
  total_reflect = 1.0 / (1.0 - total_reflect) - 1.0;
  
  float mult = total_reflect / total_area;
  
  total_delta_color_red *= mult;
  total_delta_color_green *= mult;
  total_delta_color_blue *= mult;

  // add in the system ambient light settings.
  CsPrintf(MSG_INITIALIZATION, "Setting ambient (%g, %g, %g).\n",
   total_delta_color_red, total_delta_color_green, total_delta_color_blue);

  /// then apply ambient to deltamaps
  list->Traverse(apply_ambient_func);

  /// re-apply static ambient (was removed at start of radiosity)
  total_delta_color_blue = csLight::ambient_blue;
  total_delta_color_red = csLight::ambient_red;
  total_delta_color_green = csLight::ambient_green;
  list->Traverse(add_ambient_sec_func);

  /// add deltamaps
  list->Traverse(add_delta_func);
}
