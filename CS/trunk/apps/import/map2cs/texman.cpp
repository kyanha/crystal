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
#include "texman.h"
#include "map.h"
#include "texfile.h"
#include "wad3file.h"
#include "pk3file.h"

#include "iutil/vfs.h"

static const char* Q3Extensions[] =
{
 "_hell",
 "_trans",
 "_1k",
 "_750",
 "flat_400"
};

CTextureManager::CTextureManager()
{
  m_pMap = NULL;
}

CTextureManager::~CTextureManager()
{
  DELETE_VECTOR_MEMBERS(m_StoredTextures);
  DELETE_VECTOR_MEMBERS(m_TextureArchives);
}

void CTextureManager::LoadTextureArchives(CMapFile* pMap)
{
  assert(pMap);
  m_pMap = pMap;

  enum                          {wad, pk3, zip, MaxType};
  const char* KeynameFormat[] = {
    "Map2CS.TextureSettings.wad%d",
    "Map2CS.TextureSettings.pk%d",
    "Map2CS.TextureSettings.zip%d"};

  int type;
  for (type=0; type<MaxType; type++)
  {
    int nr = 1;
    do
    {
      char keyname[200];
      char filename[300];
      sprintf(keyname, KeynameFormat[type], nr);
      strcpy(filename, pMap->GetConfigStr(keyname, ""));
      if (filename[0])
      {
        nr++;
        CTextureArchive* pArchive = NULL;
        switch (type)
        {
          case wad: pArchive = new CWad3File;   break;
          case pk3: pArchive = new CPk3File;    break;
          case zip: pArchive = new CZipArchive; break;
        }
        if (pArchive->Open(filename))
        {
          m_TextureArchives.Push(pArchive);
        }
        else
        {
          delete pArchive;
        }
      }
      else
      {
        nr = -1;
      }
    }
    while (nr>0);
  }
}

CTextureFile* CTextureManager::GetTexture(const char* TextureName)
{
  assert(m_pMap);

  char CleanedUpTextureName[200];
  strcpy(CleanedUpTextureName, TextureName);
  CleanupTexturename(CleanedUpTextureName);

  char InternalName[200];
  strncpy (InternalName, CleanedUpTextureName, sizeof(InternalName));

  // clean out some chars like '/' etc.
  int i=0;
  for (i=0; i<strlen(InternalName); i++)
  {
    if ((!isalnum(InternalName[i])) && (InternalName[i] != '_') && 
      (InternalName[i] != '{') && (InternalName[i] != '~'))
    {
      InternalName[i] = '_';
    }
  }

  //First, we search in the array of already stored textures.
  for (i=0; i<m_StoredTextures.Length(); i++)
  {
    CTextureFile* pTexture = m_StoredTextures[i];
    assert(pTexture);
    if (strcasecmp(pTexture->GetTexturename(), InternalName)==0)
    {
      return pTexture;
    }
  }

  //If we didn't find it there, we look in all known texture archives.
  for (i=0; i<m_TextureArchives.Length(); i++)
  {
    CTextureArchive* pTexArchive = m_TextureArchives[i];
    assert(pTexArchive);
    CTextureFile* pTexture = pTexArchive->CreateTexture(CleanedUpTextureName);
    if (pTexture)
    {
      pTexture->SetTexturename(InternalName);
      m_StoredTextures.Push(pTexture);
      return pTexture;
    }
  }

  //That texture is not available. Now we will use the default texture
  //instead, if we are not already looking for the defaulttexture
  char defaultname[200];
  strcpy(defaultname, m_pMap->GetConfigStr(
        "Map2CS.TextureSettings.DefaultTexture", ""));

  //Now we create a default texture for the missing texture.
  for (i=0; i<m_TextureArchives.Length(); i++)
  {
    CTextureArchive* pTexArchive = m_TextureArchives[i];
    assert(pTexArchive);
    CTextureFile* pTexture = pTexArchive->CreateTexture(defaultname);
    if (pTexture)
    {
      printf("Warning: texture '%s'('%s') is missing.\n         Using '%s' instead!\n",
             TextureName, InternalName, defaultname);
      pTexture->SetTexturename(InternalName);
      m_StoredTextures.Push(pTexture);
      return pTexture;
    }
  }

  CTextureFile* pTexture = NULL;

  printf("Warning: texture '%s'('%s') is missing.\n        Making a new null texture.\n",
    TextureName, CleanedUpTextureName);
  pTexture = new CTextureFile;
  pTexture->SetTexturename (InternalName);
  pTexture->SetFilename    (CleanedUpTextureName);
  pTexture->SetOriginalData(NULL,0);
  pTexture->SetOriginalSize(256, 256);
  m_StoredTextures.Push(pTexture);
  return pTexture;
}

bool CTextureManager::AddAllTexturesToVFS(csRef<iVFS> VFS, const char* path)
{
  bool ok = true;
  int i;
  for (i=0; i<m_StoredTextures.Length(); i++)
  {
    CTextureFile* pTexture = m_StoredTextures[i];
    assert(pTexture);

    //Only add visible textures to the ZIP. This avoids adding textures
    //that shouldn't bee seen in a map. (Like brushes that are used for
    //visblocking and that are using some reserved names.).
    if (pTexture->IsVisible() && pTexture->IsStored())
    {
      if (!pTexture->AddToVFS(VFS, path))
      {
        ok = false;
      }
    }
  }
  return ok;
}

void CTextureManager::CleanupTexturename(char* Name)
{
  int i;

  for (i=0; i<int(sizeof(Q3Extensions)/sizeof(Q3Extensions[0])); i++)
  {
    int ExtensionLen = strlen(Q3Extensions[i]);
    int NameLen      = strlen(Name);
    if (NameLen>ExtensionLen)
    {
      if (strcmp(Name+NameLen-ExtensionLen, Q3Extensions[i])==0)
      {
        Name[NameLen-ExtensionLen] = 0;
        return;
      }
    }
  }
}
