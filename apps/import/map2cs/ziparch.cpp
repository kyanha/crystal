/*  
    Map2cs: a convertor to convert the frequently used MAP format, into
    something, that can be directly understood by Crystal Space.

    Copyright (C) 1999 Thomas Hieber (thieber@gmx.net)
 
    This program is free software; you can redistribute it and/or modify 
    it under the terms of the GNU General Public License as published by 
    the Free Software Foundation; either version 2 of the License, or 
    (at your option) any later version. 
 
    This program is distributed in the hope that it will be useful, 
    but WITHOUT ANY WARRANTY; without even the implied warranty of 
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
    GNU General Public License for more details. 
 
    You should have received a copy of the GNU General Public License 
    along with this program; if not, write to the Free Software 
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. 
*/

#include "cssysdef.h"
#include "mapstd.h"
#include "ziparch.h"
#include "texfile.h"
#include "zipfile.h"


CZipArchive::CZipArchive()
{
  m_pZipFile = NULL;
}

CZipArchive::~CZipArchive()
{
  delete m_pZipFile;
}


CTextureFile* CZipArchive::CreateTexture(const char* texturename)
{
  char texfilename[256];
  CTextureFile* pTexture = NULL;

  const char* Extensions[] = {"tga", "jpg", "jpeg", "bmp", "wal", "gif"};

  for (int i=0; i<int(sizeof(Extensions)/sizeof(Extensions[0])); i++)
  {
    sprintf (texfilename, "%s.%s", texturename, Extensions[i]);
    pTexture = ExtractTexture(texturename, texfilename);
    if (pTexture) return pTexture;
  }
  
  return NULL;
}

CTextureFile* CZipArchive::ExtractTexture(const char* texturename, const char* texfilename)
{
  CTextureFile* pTexture = NULL;
  CBinaryData   TextureData;

  if (m_pZipFile->ExtractData(&TextureData, texfilename))
  {
    pTexture = new CTextureFile;

    pTexture->SetTexturename (texturename);
    pTexture->SetFilename    (texfilename);
    pTexture->SetOriginalData(TextureData.GetData(), TextureData.GetSize());

    pTexture->SetOriginalSize(256, 256);
  }

  return pTexture;
}


bool CZipArchive::Open(const char *filename)
{
  m_pZipFile = new CZipFile(filename);
  return true;
}


