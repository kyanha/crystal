/*
    Copyright (C) 2003-2005 by Frank Richter

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
#include "csgeom/subrec.h"
#include "csutil/blockallocator.h"

#if 0//defined(CS_DEBUG)
#define DUMP_TO_IMAGES
#endif

#ifdef DUMP_TO_IMAGES
// csSubRectangles::Dump () writes an image, stuff needed for that
#include "csqint.h"
#include "csutil/csstring.h"
#include "csutil/ref.h"
#include "csutil/array.h"
#include "csgfx/rgbpixel.h"
#include "csgfx/memimage.h"
#include "iutil/objreg.h"
#include "iutil/vfs.h"
#include "ivaria/reporter.h"
#include "igraphic/imageio.h"
#endif

/*
  How it works:
    It's based on a BSP. However, a partition isn't split when 
    a rectangle is allocated, but upon the next allocation. That
    means that the space is used more efficiently.

    Consider:
    +-----+-----+ 
    |     |     | 
    |  1  |     | 
    |     |     | 
    +-----+     + 
    |           | 
    |           | 
    +-----------+ 

    Now, if the rectangle would be split from top to bottom when 
    [1] is allocated, a request for an allocation of a rectangle
    with the size of the lower part would fail, although there would
    be space for it. However, when the  split is delayed until that 
    rect is actually requested, the allocation request can be satisfied.
 */

// --------------------------------------------------------------------------

csSubRectangles::SubRect::SubRect ()
{
  splitType = SPLIT_UNSPLIT;
  splitPos = 0;
  children[0] = 0;
  children[1] = 0;
  parent = 0;
}

csSubRectangles::SubRect& csSubRectangles::SubRect::operator= (
  const csSubRectangles::SubRect& other)
{
  rect = other.rect;
  allocedRect = other.allocedRect;
  splitType = other.splitType;
  splitPos = other.splitPos;

  if (children[0] != 0)
  {
    superrect->FreeSubrect (children[0]);
    children[0] = 0;
  }
  if (other.children[0] != 0)
  {
    children[0] = superrect->AllocSubrect ();
    children[0]->parent = this;
    children[0]->superrect = superrect;
    *(children[0]) = *(other.children[0]);
  }
  if (children[1] != 0)
  {
    superrect->FreeSubrect (children[1]);
    children[1] = 0;
  }
  if (other.children[1] != 0)
  {
    children[1] = superrect->AllocSubrect ();
    children[1]->parent = this;
    children[1]->superrect = superrect;
    *(children[1]) = *(other.children[1]);
  }
  return *this;
}

void csSubRectangles::SubRect::TestAlloc (int w, int h, AllocInfo& ai)
{
  int rW = rect.Width ();
  if (w > rW) return;
  int rH = rect.Height ();
  if (h > rH) return;

  CS_ASSERT (splitType == SPLIT_UNSPLIT);
  // leaf is not split yet.
  int d = 0x7fffffff;

  if (allocedRect.IsEmpty ())
  {
    // empty leaf.
    int dw = rW - w;
    int dh = rH - h;

    if (dw < dh)
    {
      ai.d = dw;
    }
    else
    {
      ai.d = dh;
    }
    ai.allocPos = ALLOC_NEW;
    ai.node = this;
    ai.res = true;
  }
  else
  {
    // a part of the leaf is already allocated.
    int arW = allocedRect.Width ();
    int arH = allocedRect.Height ();

    int dw = rW - arW;
    int dh = rH - arH;

    // Test whether a good position is below the 
    // already allocated rect.
    if (dh >= h)
    {
      if (((d = (arW - w)) >= 0))
      {
	// below
	if (d < ai.d)
	{
	  ai.d = d;
	  ai.allocPos = ALLOC_BELOW;
	  ai.node = this;
	  ai.res = true;
	}
      }
      else
      {
	d = (dh - h);
	// below
	if (d < ai.d)
	{
	  ai.d = d;
	  ai.allocPos = ALLOC_BELOW;
	  ai.node = this;
	  ai.res = true;
	}
      }
    }
    // Test whether a good position is to right of the 
    // already allocated rect.
    if ((d != 0) && (dw >= w))
    {
      if (((d = (arH - h)) >= 0))
      {
	// right
	if (d < ai.d)
	{
	  ai.d = d;
	  ai.allocPos = ALLOC_RIGHT;
	  ai.node = this;
	  ai.res = true;
	}
      }
      else
      {
	d = (dw - w);
	// right
	if (d < ai.d)
	{
	  ai.d = d;
	  ai.allocPos = ALLOC_RIGHT;
	  ai.node = this;
	  ai.res = true;
	}
      }
    }
  }
}

void csSubRectangles::SubRect::DecideBestSplit (const csRect& rect, int splitX, int splitY,
				  csSubRectangles::SubRect::SplitType& splitType)
{
  int rW = rect.Width ();
  int rH = rect.Height ();
  int vsSize = rW - (splitX - rect.xmin);
  int hsSize = rH - (splitY - rect.ymin);

  if ((vsSize <= 0) || (hsSize <= 0))
  {
    splitType = (vsSize <= 0) ? SPLIT_H : SPLIT_V;
    return;
  }

  if (vsSize != hsSize)
  {
    if (vsSize > hsSize)
    {
      splitType = SPLIT_V;
    }
    else
    {
      splitType = SPLIT_H;
    }
  }
}

csSubRectangles::SubRect* csSubRectangles::SubRect::Alloc (int w, int h, const AllocInfo& ai, csRect& r)
{
  CS_ASSERT (splitType == SPLIT_UNSPLIT);

  switch (ai.allocPos)
  {
    case ALLOC_BELOW:
      {
	// below
	r.Set (allocedRect.xmin, allocedRect.ymax, allocedRect.xmin + w, 
	  allocedRect.ymax + h);
	splitType = SPLIT_H;
      }
      break;
    case ALLOC_RIGHT:
      {
	// right
	r.Set (allocedRect.xmax, allocedRect.ymin, allocedRect.xmax + w, 
	  allocedRect.ymin + h);

	splitType = SPLIT_V;
      }
      break;
    case ALLOC_NEW:
      {
	allocedRect.Set (rect.xmin, rect.ymin, rect.xmin + w, rect.ymin + h);
	r = allocedRect;
	return this;
      }
      break;
    default:
      break;
  }
  if (splitType != SPLIT_UNSPLIT)
  {
    superrect->RemoveLeaf (this);
    csSubRectangles::SubRect* ret = 0;

    int splitX, splitY;
    splitX = allocedRect.xmax;
    splitY = allocedRect.ymax;

    if ((ai.allocPos == ALLOC_RIGHT && r.Height() <= allocedRect.Height()) ||
	(ai.allocPos == ALLOC_BELOW && r.Width () <= allocedRect.Width ()))
    {
      DecideBestSplit (rect, splitX, splitY, splitType);
    }

    // Now, split that leaf.
    // Obviously, care has to be taken that a split doesn't go through an
    // allocated rect. That would cause overlapping -> bad.
    if (splitType == SPLIT_V)
    {
      splitPos = splitX - rect.xmin;

      children[0] = superrect->AllocSubrect ();
      children[0]->parent = this;
      children[0]->superrect = superrect;
      children[0]->rect.Set (rect.xmin, rect.ymin, 
	splitX, rect.ymax);
      if (ai.allocPos == ALLOC_RIGHT)
      {
	children[0]->allocedRect = allocedRect;
	superrect->AddLeaf (children[0]);
      }
      else
      {
	csSubRectangles::SubRect* subChild0 = superrect->AllocSubrect ();
	subChild0->parent = children[0];
	subChild0->superrect = superrect;
	subChild0->rect.Set (rect.xmin, rect.ymin, splitX, allocedRect.ymax);
	subChild0->allocedRect = allocedRect;
	superrect->AddLeaf (subChild0);

	csSubRectangles::SubRect* subChild1 = superrect->AllocSubrect ();
	subChild1->parent = children[0];
	subChild1->superrect = superrect;
	subChild1->rect.Set (rect.xmin, allocedRect.ymax, splitX, rect.ymax);
	subChild1->allocedRect = r;
	ret = subChild1;
	superrect->AddLeaf (subChild1);

	children[0]->splitType = SPLIT_H;
	children[0]->splitPos = allocedRect.Height ();
	children[0]->allocedRect = children[0]->rect;
	children[0]->children[0] = subChild0;
	children[0]->children[1] = subChild1;
      }

      children[1] = superrect->AllocSubrect ();
      children[1]->parent = this;
      children[1]->superrect = superrect;
      children[1]->rect.Set (splitX, rect.ymin, 
	rect.xmax, rect.ymax);
      if (ai.allocPos == ALLOC_RIGHT)
      {
	children[1]->allocedRect = r;
	ret = children[1];
      }
      superrect->AddLeaf (children[1]);
    }
    else
    {
      splitPos = splitY - rect.ymin;

      children[0] = superrect->AllocSubrect ();
      children[0]->parent = this;
      children[0]->superrect = superrect;
      children[0]->rect.Set (rect.xmin, rect.ymin, 
	rect.xmax, splitY);
      if (ai.allocPos == ALLOC_BELOW)
      {
	children[0]->allocedRect = allocedRect;
	superrect->AddLeaf (children[0]);
      }
      else
      {
	csSubRectangles::SubRect* subChild0 = superrect->AllocSubrect ();
	subChild0->parent = children[0];
	subChild0->superrect = superrect;
	subChild0->rect.Set (rect.xmin, rect.ymin, allocedRect.xmax, splitY);
	subChild0->allocedRect = allocedRect;
	superrect->AddLeaf (subChild0);

	csSubRectangles::SubRect* subChild1 = superrect->AllocSubrect ();
	subChild1->parent = children[0];
	subChild1->superrect = superrect;
	subChild1->rect.Set (allocedRect.xmax, rect.ymin, rect.xmax, splitY);
	subChild1->allocedRect = r;
	ret = subChild1;
	superrect->AddLeaf (subChild1);

	children[0]->splitType = SPLIT_V;
	children[0]->splitPos = allocedRect.Width ();
	children[0]->allocedRect = children[0]->rect;
	children[0]->children[0] = subChild0;
	children[0]->children[1] = subChild1;
      }

      children[1] = superrect->AllocSubrect ();
      children[1]->parent = this;
      children[1]->superrect = superrect;
      children[1]->rect.Set (rect.xmin, splitY, 
	rect.xmax, rect.ymax);
      if (ai.allocPos == ALLOC_BELOW)
      {
	children[1]->allocedRect = r;
	ret = children[1];
      }
      superrect->AddLeaf (children[1]);
    }
    CS_ASSERT (ret != 0);
    return ret;
  }
  return 0;
}

void csSubRectangles::SubRect::Reclaim ()
{
  // @@@ This could be improved.
  if (splitType == SPLIT_UNSPLIT)
  {
    allocedRect.MakeEmpty ();
    if (parent != 0) parent->TestCollapse ();
  }
  else
  {
    CS_ASSERT (children[0]);
    children[0]->Reclaim ();
  }
}

void csSubRectangles::SubRect::TestCollapse ()
{
  // If both children are "empty space" we can revert the status
  // of this sub-rectangle to "unsplit" and free the children.
  if (((children[0] != 0) && (children[0]->allocedRect.IsEmpty ())) && 
    ((children[1] != 0) && (children[1]->allocedRect.IsEmpty ())))
  {
    splitType = SPLIT_UNSPLIT;
    allocedRect.MakeEmpty ();
    superrect->RemoveLeaf (children[0]);
    superrect->RemoveLeaf (children[1]);
    superrect->FreeSubrect (children[0]); children[0] = 0;
    superrect->FreeSubrect (children[1]); children[1] = 0;
    superrect->AddLeaf (this);
    if (parent != 0) parent->TestCollapse ();
  }
}

// --------------------------------------------------------------------------

csSubRectangles::csSubRectangles (const csRect &region) : 
  region(region), root (0), alloc (4096 / sizeof(SubRect))
{
  Clear ();
}

csSubRectangles::csSubRectangles (const csSubRectangles& other) : 
  region(region), root (0), alloc (4096 / sizeof(SubRect))
{
  Clear();
  *root = *(other.root);
}

csSubRectangles::~csSubRectangles ()
{
  // No need to free the rects, the block allocator will take care
}

void csSubRectangles::FreeSubrect (SubRect* sr)
{
  if (sr == 0) return;
  FreeSubrect (sr->children[0]);
  FreeSubrect (sr->children[1]);
  alloc.Free (sr);
}

void csSubRectangles::Clear ()
{
  alloc.Free (root);
  leaves.DeleteAll();

  root = alloc.Alloc ();
  root->rect = region;
  root->superrect = this;
  leaves.Push (root);
}

csSubRectangles::SubRect* csSubRectangles::Alloc (int w, int h, csRect &rect)
{
  csSubRectangles::SubRect::AllocInfo ai;

  for (size_t i = 0; i < leaves.Length(); i++)
  {
    leaves[i]->TestAlloc (w, h, ai);
    if (ai.res && (ai.d == 0)) break;
  }

  if (ai.res)
  {
    csSubRectangles::SubRect* sr = ai.node->Alloc (w, h, ai, rect);
    return sr;
  }

  return 0;
}

void csSubRectangles::Reclaim (csSubRectangles::SubRect* subrect)
{
  if (subrect) subrect->Reclaim ();
}

void csSubRectangles::Grow (csSubRectangles::SubRect* sr, int ow, int oh, int nw, int nh)
{
  if (sr == 0) return;

  if (sr->rect.xmax == ow) sr->rect.xmax = nw;
  if (sr->rect.ymax == oh) sr->rect.ymax = nh;

  if (sr->splitType != csSubRectangles::SubRect::SPLIT_UNSPLIT)
  {
    Grow (sr->children[0], ow, oh, nw, nh);
    Grow (sr->children[1], ow, oh, nw, nh);
  }
}

bool csSubRectangles::Grow (int newWidth, int newHeight)
{
  if ((newWidth < region.Width ()) || (newHeight < region.Height ()))
    return false;

  Grow (root, region.Width (), region.Height (), newWidth, newHeight);
  region.SetSize (newWidth, newHeight);
  return true;
}

#if defined(DUMP_TO_IMAGES)
static void FillImgRect (uint8* data, uint8 color, int imgW, int imgH, 
			 const csRect& r)
{
  int x, y;
  uint8* p = data + (r.ymin * imgW) + r.xmin;
  for (y = r.ymin; y < r.ymax; y++)
  {
    for (x = r.xmin; x < r.xmax; x++)
    {
      *p++ = color;
    }
    p += (imgW - r.Width ());
  }
}

static void IncImgRect (uint8* data, int imgW, int imgH, 
			const csRect& r)
{
  int x, y;
  uint8* p = data + (r.ymin * imgW) + r.xmin;
  for (y = r.ymin; y < r.ymax; y++)
  {
    for (x = r.xmin; x < r.xmax; x++)
    {
      (*p++)++;
    }
    p += (imgW - r.Width ());
  }
}
#endif

// Debug dump: write some rect distribution into some images.
void csSubRectangles::Dump ()
{
#if defined(DUMP_TO_IMAGES)
  if (!iSCF::SCF->object_reg) return;

  csRef<iImageIO> imgsaver =
    CS_QUERY_REGISTRY (iSCF::SCF->object_reg, iImageIO);
  csRef<iVFS> vfs =
    CS_QUERY_REGISTRY (iSCF::SCF->object_reg, iVFS);

  if (!imgsaver || !vfs) return;
  
  csRGBpixel pal[256];
  int i;
  for (i = 0; i < 256; i++)
  {
    pal[i].red = csQint ((((i & 0x01) << 2) + ((i & 0x08) >> 2) +
      ((i & 0x40) >> 6)) * (255.0f / 7.0f));
    pal[i].green = csQint ((((i & 0x02) << 1) + ((i & 0x10) >> 3) +
      ((i & 0x80) >> 7)) * (255.0f / 7.0f));
    pal[i].blue = csQint ((((i & 0x04) >> 1) + ((i & 0x20) >> 5)) *
      (255.0f / 3.0f));
  }

  int w = region.Width (), h = region.Height ();

  csRGBpixel* newpal = new csRGBpixel[256];
  memcpy (newpal, pal, sizeof (pal));

  csImageMemory* img = 
    new csImageMemory (w, h, (new uint8[w * h]),
    true, CS_IMGFMT_PALETTED8, newpal);

  uint8* data = (uint8*)img->GetImageData ();
  memset (data, 0, w * h);

  newpal = new csRGBpixel[256];
  memcpy (newpal, pal, sizeof (pal));

  csImageMemory* img2 = 
    new csImageMemory (w, h, (new uint8[w * h]),
    true, CS_IMGFMT_PALETTED8, newpal);

  uint8* data2 = (uint8*)img2->GetImageData ();
  memset (data2, 0, w * h);

  newpal = new csRGBpixel[256];
  memcpy (newpal, pal, sizeof (pal));

  csImageMemory* img3 = 
    new csImageMemory (w, h, (new uint8[w * h]),
    true, CS_IMGFMT_PALETTED8, newpal);

  uint8* data3 = (uint8*)img3->GetImageData ();
  memset (data3, 0, w * h);

  int c = 0;
  csArray<csSubRectangles::SubRect*> nodes;
  nodes.Push (root);
  
  while (nodes.Length ())
  {
    csSubRectangles::SubRect* node = nodes[0];
    nodes.DeleteIndex (0);

    FillImgRect (data, c + 1, w, h, node->rect);
    FillImgRect (data2, c + 1, w, h, node->allocedRect);
    // Overlap information. The whole image should be just one color.
    IncImgRect (data3, w, h, node->rect);

    if (node->children[0] != 0) nodes.Push (node->children[0]);
    if (node->children[1] != 0) nodes.Push (node->children[1]);
    c = (c + 1) % 255;
  }

  {
    csRef<iDataBuffer> buf = imgsaver->Save (img, "image/png");
    csString outfn;
    outfn.Format ("/tmp/csSubRectangles_dump_%p_r.png", this);
    if (vfs->WriteFile (outfn, (char*)buf->GetInt8 (), buf->GetSize ()))
    {
      csReport (iSCF::SCF->object_reg, CS_REPORTER_SEVERITY_NOTIFY,
	"crystalspace.geom.subrects", "Successfully dumped to %s",
	outfn.GetData ());
    }
    else
    {
      csReport (iSCF::SCF->object_reg, CS_REPORTER_SEVERITY_NOTIFY,
	"crystalspace.geom.subrects", "Error dumping to %s",
	outfn.GetData ());
    }

    buf = imgsaver->Save (img2, "image/png");
    outfn.Format ("/tmp/csSubRectangles_dump_%p_ar.png", this);
    if (vfs->WriteFile (outfn, (char*)buf->GetInt8 (), buf->GetSize ()))
    {
      csReport (iSCF::SCF->object_reg, CS_REPORTER_SEVERITY_NOTIFY,
	"crystalspace.geom.subrects", "Successfully dumped to %s",
	outfn.GetData ());
    }
    else
    {
      csReport (iSCF::SCF->object_reg, CS_REPORTER_SEVERITY_NOTIFY,
	"crystalspace.geom.subrects", "Error dumping to %s",
	outfn.GetData ());
    }

    buf = imgsaver->Save (img3, "image/png");
    outfn.Format ("/tmp/csSubRectangles_dump_%p_ov.png", this);
    if (vfs->WriteFile (outfn, (char*)buf->GetInt8 (), buf->GetSize ()))
    {
      csReport (iSCF::SCF->object_reg, CS_REPORTER_SEVERITY_NOTIFY,
	"crystalspace.geom.subrects", "Successfully dumped to %s",
	outfn.GetData ());
    }
    else
    {
      csReport (iSCF::SCF->object_reg, CS_REPORTER_SEVERITY_NOTIFY,
	"crystalspace.geom.subrects", "Error dumping to %s",
	outfn.GetData ());
    }
  }
#endif
}
