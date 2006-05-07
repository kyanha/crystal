/*
    Copyright (C) 2000-2001 by Jorrit Tyberghein
    Copyright (C) 1998-2000 by Ivan Avramovic <ivan@avramovic.com>

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

/* This file contains functions to load textures from image files. They do
 * not handle parsing of texture statements in any way.
 */

#include "cssysdef.h"

#include "ivideo/graph3d.h"

#include "csgfx/imagecubemapmaker.h"
#include "csgfx/imagevolumemaker.h"
#include "csgfx/xorpat.h"
#include "csutil/cscolor.h"
#include "iengine/engine.h"
#include "iengine/material.h"
#include "iengine/texture.h"
#include "iengine/region.h"
#include "igraphic/image.h"
#include "igraphic/imageio.h"
#include "imap/reader.h"
#include "iutil/databuff.h"
#include "iutil/document.h"
#include "iutil/object.h"
#include "iutil/objreg.h"
#include "iutil/vfs.h"
#include "ivideo/material.h"
#include "ivideo/texture.h"

#include "csloader.h"
#include "loadtex.h"

csPtr<iImage> csLoader::LoadImage (iDataBuffer* buf, const char* fname,
	int Format)
{
  if (!ImageLoader)
     return 0;

  if (Format & CS_IMGFMT_INVALID)
  {
    if (Engine)
      Format = Engine->GetTextureFormat ();
    else if (G3D)
      Format = G3D->GetTextureManager()->GetTextureFormat();
    else
      Format = CS_IMGFMT_TRUECOLOR;
  }

  if (!buf || !buf->GetSize ())
  {
    ReportWarning (
	"crystalspace.maploader.parse.image",
    	"Could not open image file '%s' on VFS!", fname ? fname : "<unknown>");
    return 0;
  }

  // we don't use csRef because we need to return an Increfed object later
  csRef<iImage> image (ImageLoader->Load (buf, Format));
  if (!image)
  {
    ReportWarning (
	"crystalspace.maploader.parse.image",
	"Could not load image '%s'. Unknown format!",
	fname ? fname : "<unknown>");
    return 0;
  }

  if (fname)
  {
    csRef<iDataBuffer> xname = VFS->ExpandPath (fname);
    image->SetName (**xname);
  }

  return csPtr<iImage> (image);
}

csPtr<iImage> csLoader::LoadImage (iDataBuffer* buf, int Format)
{
  return LoadImage (buf, 0, Format);
}

csPtr<iTextureHandle> csLoader::LoadTexture (iDataBuffer* buf, int Flags,
	iTextureManager *tm, csRef<iImage>* img)
{
  if (!tm && G3D) tm = G3D->GetTextureManager();
  int Format;
  if (tm)
    Format = tm->GetTextureFormat ();
  else
    Format = CS_IMGFMT_TRUECOLOR;

  csRef<iImage> Image = LoadImage (buf, Format);
  if (!Image)
  {
    ReportWarning (
	"crystalspace.maploader.parse.texture",
	"Couldn't load image. Using checkerboard instead!");
    Image = csCreateXORPatternImage (32, 32, 5);
    if (!Image)
      return 0;
  }

  if(img) *img=Image;

  if (!tm)
    return 0;
  
  csRef<iTextureHandle> TexHandle = tm->RegisterTexture (Image, Flags);
  if (!TexHandle)
  {
    ReportError (
	"crystalspace.maploader.parse.texture",
	"Cannot create texture!");
    return 0;
  }

  return csPtr<iTextureHandle> (TexHandle);
}

iTextureWrapper* csLoader::LoadTexture (const char *name,
	iDataBuffer* buf, int Flags, iTextureManager *tm, bool reg,
	bool create_material, bool free_image)
{
  if (!Engine)
    return 0;

  csRef<iImage> img;
  if (!tm && G3D) tm = G3D->GetTextureManager();
  csRef<iTextureHandle> TexHandle = LoadTexture (buf, Flags, tm, &img);
  if (!TexHandle)
    return 0;

  iTextureWrapper *TexWrapper =
	Engine->GetTextureList ()->NewTexture(TexHandle);
  TexWrapper->QueryObject ()->SetName (name);
  TexWrapper->SetImageFile(img);

  iMaterialWrapper* matwrap = 0;
  if (create_material)
  {
    csRef<iMaterial> material (Engine->CreateBaseMaterial (TexWrapper));
    matwrap = Engine->GetMaterialList ()->NewMaterial (material, name);
  }

  if (reg && tm)
  {
    // If we already have a texture handle then we don't register again.
    if (!TexWrapper->GetTextureHandle ())
      TexWrapper->Register (tm);
    if (free_image)
      TexWrapper->SetImageFile (0);
  }

  return TexWrapper;
}

csPtr<iImage> csLoader::LoadImage (const char* fname, int Format)
{
  csRef<iDataBuffer> buf = VFS->ReadFile (fname, false);
  return LoadImage (buf, fname, Format);
}

csPtr<iTextureHandle> csLoader::LoadTexture (const char *fname, int Flags,
	iTextureManager *tm, csRef<iImage>* img)
{
  if (!tm && G3D) tm = G3D->GetTextureManager();
  int Format;
  if (tm)
    Format = tm->GetTextureFormat ();
  else
    Format = CS_IMGFMT_TRUECOLOR;

  csRef<iImage> Image = LoadImage (fname, Format);
  if (!Image)
  {
    ReportWarning (
	"crystalspace.maploader.parse.texture",
	"Couldn't load image '%s', using checkerboard instead!",
	fname);
    Image = csCreateXORPatternImage (32, 32, 5);
    if (!Image)
      return 0;
  }

  if(img) *img=Image;

  if (!tm)
    return 0;
  
  csRef<iTextureHandle> TexHandle (tm->RegisterTexture (Image, Flags));
  if (!TexHandle)
  {
    ReportError (
	"crystalspace.maploader.parse.texture",
	"Cannot create texture from '%s'!", fname);
    return 0;
  }

  return csPtr<iTextureHandle> (TexHandle);
}

iTextureWrapper* csLoader::LoadTexture (const char *name,
	const char *fname, int Flags, iTextureManager *tm, bool reg,
	bool create_material, bool free_image, iRegion* region)
{
  if (!Engine)
    return 0;

  csRef<iImage> img;
  if (!tm && G3D) tm = G3D->GetTextureManager();
  csRef<iTextureHandle> TexHandle = LoadTexture (fname, Flags, tm, &img);
  if (!TexHandle)
    return 0;

  iTextureWrapper *TexWrapper =
	Engine->GetTextureList ()->NewTexture(TexHandle);
  TexWrapper->QueryObject ()->SetName (name);
  TexWrapper->SetImageFile(img);
  if (region) region->QueryObject ()->ObjAdd (TexWrapper->QueryObject ());

  iMaterialWrapper* matwrap = 0;
  if (create_material)
  {
    csRef<iMaterial> material (Engine->CreateBaseMaterial (TexWrapper));
    matwrap = Engine->GetMaterialList ()->NewMaterial (material, name);
    if (region) region->QueryObject ()->ObjAdd (matwrap->QueryObject ());
  }

  if (reg && tm)
  {
    // If we already have a texture handle then we don't register again.
    if (!TexWrapper->GetTextureHandle ())
      TexWrapper->Register (tm);
    if (free_image)
      TexWrapper->SetImageFile (0);
  }

  return TexWrapper;
}

//----------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE(TextureLoaderContext);
  SCF_IMPLEMENTS_INTERFACE(iTextureLoaderContext);
SCF_IMPLEMENT_IBASE_END

TextureLoaderContext::TextureLoaderContext (const char* texname)
{
  SCF_CONSTRUCT_IBASE (0);

  has_flags = false;
  flags = CS_TEXTURE_3D;

  has_image = false;
  image = 0;

  has_size = false;
  width = height = 128;

  TextureLoaderContext::texname = texname;
}

TextureLoaderContext::~TextureLoaderContext ()
{
  SCF_DESTRUCT_IBASE();
}

void TextureLoaderContext::SetFlags (int Flags)
{
  flags = Flags;
  has_flags = true;
}

bool TextureLoaderContext::HasFlags ()
{
  return has_flags;
}

int TextureLoaderContext::GetFlags ()
{
  return flags;
}
  
void TextureLoaderContext::SetImage (iImage* Image)
{
  image = Image;
  has_image = true;
}

bool TextureLoaderContext::HasImage ()
{
  return has_image;
}

iImage* TextureLoaderContext::GetImage()
{
  return image;
}

void TextureLoaderContext::SetSize (int w, int h)
{
  has_size = true;
  width = w; height = h;
}

bool TextureLoaderContext::HasSize ()
{
  return has_size;
}

void TextureLoaderContext::GetSize (int& w, int& h)
{
  w = width; h = height;
}

const char* TextureLoaderContext::GetName ()
{
  return texname;
}

void TextureLoaderContext::SetClass (const char* className)
{
  texClass = className;
}

const char* TextureLoaderContext::GetClass ()
{
  return texClass;
}

//----------------------------------------------------------------------------

SCF_IMPLEMENT_IBASE(csBaseTextureLoader);
  SCF_IMPLEMENTS_INTERFACE(iLoaderPlugin);
  SCF_IMPLEMENTS_INTERFACE(iComponent);
SCF_IMPLEMENT_IBASE_END

csBaseTextureLoader::csBaseTextureLoader (iBase *p)
{
  SCF_CONSTRUCT_IBASE (p);
}

csBaseTextureLoader::~csBaseTextureLoader ()
{
  SCF_DESTRUCT_IBASE();
}

bool csBaseTextureLoader::Initialize(iObjectRegistry *object_reg)
{
  csBaseTextureLoader::object_reg = object_reg;
  return true;
}

//----------------------------------------------------------------------------

SCF_IMPLEMENT_FACTORY(csImageTextureLoader)

csImageTextureLoader::csImageTextureLoader (iBase *p) : 
  csBaseTextureLoader(p)
{
}

csPtr<iBase> csImageTextureLoader::Parse (iDocumentNode* /*node*/, 
					  iStreamSource*,
					  iLoaderContext* /*ldr_context*/, 	
					  iBase* context)
{
  if (!context) return 0;
  csRef<iTextureLoaderContext> ctx = csPtr<iTextureLoaderContext>
    (SCF_QUERY_INTERFACE (context, iTextureLoaderContext));
  if (!ctx) return 0;
  if (!ctx->HasImage() || !ctx->GetImage())
    return 0;

  csRef<iGraphics3D> G3D = CS_QUERY_REGISTRY (object_reg, iGraphics3D);
  if (!G3D) return 0;
  csRef<iTextureManager> tm = G3D->GetTextureManager();
  if (!tm) return 0;
  csRef<iEngine> Engine = CS_QUERY_REGISTRY (object_reg, iEngine);
  if (!Engine)
    return 0;

  csRef<iTextureHandle> TexHandle (tm->RegisterTexture (ctx->GetImage(), 
    ctx->HasFlags() ? ctx->GetFlags() : CS_TEXTURE_3D));
  if (!TexHandle) return 0;

  csRef<iTextureWrapper> TexWrapper =
	Engine->GetTextureList ()->NewTexture(TexHandle);
  TexWrapper->SetImageFile(ctx->GetImage());

  return csPtr<iBase> (TexWrapper);
}

//----------------------------------------------------------------------------

SCF_IMPLEMENT_FACTORY(csCheckerTextureLoader)

csCheckerTextureLoader::csCheckerTextureLoader (iBase *p) : 
  csBaseTextureLoader(p)
{
}

csPtr<iBase> csCheckerTextureLoader::Parse (iDocumentNode* node, 
					    iStreamSource*,
					    iLoaderContext* /*ldr_context*/,
					    iBase* context)
{
  int w = 64, h = 64, depth = 6;
  csColor color (1.0f, 1.0f, 1.0f);
  csRef<iTextureLoaderContext> ctx;
  if (context)
  {
    ctx = SCF_QUERY_INTERFACE (context, iTextureLoaderContext);
    if (ctx)
    {
      if (ctx->HasSize())
      {
	ctx->GetSize (w, h);
	int a = csLog2 (w), b = csLog2 (h);
	depth = MIN (a, b);
	depth = MIN (depth, 8);
      }
    }
  }
  if (node)
  {
    csRef<iDocumentNode> depthNode = node->GetNode ("depth");
    if (depthNode)
    {
      depth = depthNode->GetContentsValueAsInt ();
    }
    csRef<iDocumentNode> colorNode = node->GetNode ("color");
    if (colorNode)
    {
      csRef<iSyntaxService> synserv = 
	CS_QUERY_REGISTRY (object_reg, iSyntaxService);
      if (synserv)
      {
	synserv->ParseColor (colorNode, color);
      }
    }
  }

  csRef<iImage> Image = csCreateXORPatternImage (w, h, depth, color.red,
  	color.green, color.blue);

  csRef<iGraphics3D> G3D = CS_QUERY_REGISTRY (object_reg, iGraphics3D);
  if (!G3D) return 0;
  csRef<iTextureManager> tm = G3D->GetTextureManager();
  if (!tm) return 0;
  csRef<iEngine> Engine = CS_QUERY_REGISTRY (object_reg, iEngine);
  if (!Engine)
    return 0;

  csRef<iTextureHandle> TexHandle (tm->RegisterTexture (Image, 
    (ctx && ctx->HasFlags()) ? ctx->GetFlags() : CS_TEXTURE_3D));
  if (!TexHandle) return 0;

  csRef<iTextureWrapper> TexWrapper =
	Engine->GetTextureList ()->NewTexture(TexHandle);
  TexWrapper->SetImageFile (Image);

  TexWrapper->IncRef ();
  return csPtr<iBase> ((iBase*)TexWrapper);
}

//----------------------------------------------------------------------------

SCF_IMPLEMENT_FACTORY(csCubemapTextureLoader)

csCubemapTextureLoader::csCubemapTextureLoader (iBase *p) : 
  csBaseTextureLoader(p)
{
  InitTokenTable (xmltokens);
}

csPtr<iBase> csCubemapTextureLoader::Parse (iDocumentNode* node, 
					    iStreamSource*,
					    iLoaderContext* /*ldr_context*/,
					    iBase* context)
{
  if (!context) return 0;
  csRef<iTextureLoaderContext> ctx = csPtr<iTextureLoaderContext>
    (SCF_QUERY_INTERFACE (context, iTextureLoaderContext));
  if (!ctx) return 0;
  
  csRef<iEngine> Engine = CS_QUERY_REGISTRY (object_reg, iEngine);
  csRef<iGraphics3D> G3D = CS_QUERY_REGISTRY (object_reg, iGraphics3D);
  csRef<iTextureManager> tm = G3D->GetTextureManager();
  csRef<iLoader> loader = CS_QUERY_REGISTRY (object_reg, iLoader);
  csRef<iSyntaxService> SyntaxService = 
    CS_QUERY_REGISTRY (object_reg, iSyntaxService);

  csRef<csImageCubeMapMaker> cube;
  cube.AttachNew (new csImageCubeMapMaker (ctx->GetImage()));
  if (!Engine->GetSaveableFlag()) cube->SetName (0);

  int Format = tm->GetTextureFormat ();
  const char* fname;

  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    const char* value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    switch (id)
    {
      case XMLTOKEN_POSZ:
      case XMLTOKEN_NORTH:
        fname = child->GetContentsValue ();
	if (!fname)
	{
	  SyntaxService->ReportError (
	       PLUGIN_TEXTURELOADER_CUBEMAP,
	       child, "Expected VFS filename for 'file'!");
	  return 0;
	}
      
	cube->SetSubImage (4, csRef<iImage>(loader->LoadImage (fname, Format)));
        break;
    
      case XMLTOKEN_NEGZ:
      case XMLTOKEN_SOUTH:
        fname = child->GetContentsValue ();
	if (!fname)
	{
	  SyntaxService->ReportError (
	       PLUGIN_TEXTURELOADER_CUBEMAP,
	       child, "Expected VFS filename for 'file'!");
	  return 0;
	}
      
	cube->SetSubImage (5, csRef<iImage>(loader->LoadImage (fname, Format)));
        break;
    
      case XMLTOKEN_POSX:
      case XMLTOKEN_EAST:
        fname = child->GetContentsValue ();
	if (!fname)
	{
	  SyntaxService->ReportError (
	       PLUGIN_TEXTURELOADER_CUBEMAP,
	       child, "Expected VFS filename for 'file'!");
	  return 0;
	}
      
	cube->SetSubImage (0, csRef<iImage>(loader->LoadImage (fname, Format)));
        break;
    
      case XMLTOKEN_NEGX:
      case XMLTOKEN_WEST:
        fname = child->GetContentsValue ();
	if (!fname)
	{
	  SyntaxService->ReportError (
	       PLUGIN_TEXTURELOADER_CUBEMAP,
	       child, "Expected VFS filename for 'file'!");
	  return 0;
	}
      
	cube->SetSubImage (1, csRef<iImage>(loader->LoadImage (fname, Format)));
        break;
    
      case XMLTOKEN_POSY:
      case XMLTOKEN_TOP:
        fname = child->GetContentsValue ();
	if (!fname)
	{
	  SyntaxService->ReportError (
	       PLUGIN_TEXTURELOADER_CUBEMAP,
	       child, "Expected VFS filename for 'file'!");
	  return 0;
	}
      
	cube->SetSubImage (2, csRef<iImage>(loader->LoadImage (fname, Format)));
        break;
    
      case XMLTOKEN_NEGY:
      case XMLTOKEN_BOTTOM:
        fname = child->GetContentsValue ();
	if (!fname)
	{
	  SyntaxService->ReportError (
	       PLUGIN_TEXTURELOADER_CUBEMAP,
	       child, "Expected VFS filename for 'file'!");
	  return 0;
	}
      
	cube->SetSubImage (3, csRef<iImage>(loader->LoadImage (fname, Format)));
        break;
    }
  }


  csRef<iTextureHandle> TexHandle (tm->RegisterTexture (cube, 
    ctx->HasFlags() ? ctx->GetFlags() : CS_TEXTURE_3D));
  if (!TexHandle) return 0;

  csRef<iTextureWrapper> TexWrapper =
	Engine->GetTextureList ()->NewTexture(TexHandle);
  TexWrapper->SetImageFile (cube);

  TexWrapper->IncRef ();
  return csPtr<iBase> (TexWrapper);
}

//----------------------------------------------------------------------------

SCF_IMPLEMENT_FACTORY(csTexture3DLoader)

csTexture3DLoader::csTexture3DLoader (iBase *p) : csBaseTextureLoader(p)
{
  InitTokenTable (xmltokens);
}

csPtr<iBase> csTexture3DLoader::Parse (iDocumentNode* node, 
				       iStreamSource*,
				       iLoaderContext* /*ldr_context*/,
				       iBase* context)
{
  if (!context) return 0;
  csRef<iTextureLoaderContext> ctx = csPtr<iTextureLoaderContext>
    (SCF_QUERY_INTERFACE (context, iTextureLoaderContext));
  if (!ctx) return 0;
  
  csRef<iEngine> Engine = CS_QUERY_REGISTRY (object_reg, iEngine);
  csRef<iGraphics3D> G3D = CS_QUERY_REGISTRY (object_reg, iGraphics3D);
  csRef<iTextureManager> tm = G3D->GetTextureManager();
  csRef<iLoader> loader = CS_QUERY_REGISTRY (object_reg, iLoader);
  csRef<iSyntaxService> SyntaxService = 
    CS_QUERY_REGISTRY (object_reg, iSyntaxService);

  int Format = tm->GetTextureFormat ();
  csRef<csImageVolumeMaker> vol;
  int w = -1, h = -1;
  if (ctx->HasSize())
  {
    ctx->GetSize (w, h);
    vol.AttachNew (new csImageVolumeMaker (Format, w, h));
  }
  else if (ctx->HasImage())
  {
    vol.AttachNew (new csImageVolumeMaker (ctx->GetImage()));
  }
  else
    vol.AttachNew (new csImageVolumeMaker (Format));

  if (!Engine->GetSaveableFlag()) vol->SetName (0);

  const char* fname;

  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    const char* value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    switch (id)
    {
      case XMLTOKEN_LAYER:
        fname = child->GetContentsValue ();
	if (!fname)
	{
	  SyntaxService->ReportError (
	    PLUGIN_TEXTURELOADER_TEX3D,
	       child, "Expected VFS filename for 'file'!");
	  return 0;
	}
      
	vol->AddImage (csRef<iImage>(loader->LoadImage (fname, Format)));
        break;
    }
  }


  csRef<iTextureHandle> TexHandle (tm->RegisterTexture (vol, 
    ctx->HasFlags() ? ctx->GetFlags() : CS_TEXTURE_3D));
  if (!TexHandle) return 0;

  csRef<iTextureWrapper> TexWrapper =
	Engine->GetTextureList ()->NewTexture(TexHandle);
  TexWrapper->SetImageFile (vol);

  TexWrapper->IncRef ();
  return csPtr<iBase> (TexWrapper);
}
