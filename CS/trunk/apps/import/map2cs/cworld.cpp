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
#include "map.h"
#include "entity.h"
#include "mpoly.h"
#include "cworld.h"
#include "csector.h"
#include "cthing.h"
#include "texplane.h"
#include "zipfile.h"
#include "vertbuf.h"
#include "mcurve.h"
#include "sparser.h"

#define TEMPWORLD "map2cs2.$$$"

CCSWorld::CCSWorld()
{
}

CCSWorld::~CCSWorld()
{
}

void CCSWorld::FindSectors()
{
  int i, j, SectorCounter = 0;

  //iterate all entities and brushes
  for (i=0; i<m_pMap->GetNumEntities(); i++)
  {
    CMapEntity* pEntity = m_pMap->GetEntity(i);
    if (strcmp(pEntity->GetClassname(), "cs_sector")==0)
    {
      //This entity is a sector!
      for (j=0; j<pEntity->GetNumBrushes(); j++)
      {
        SectorCounter++;
        CMapBrush* pBrush  = pEntity->GetBrush(j);
        CCSSector* pSector = new CCSSector(pBrush);
        m_Sectors.Push(pSector);
      } //for brush
    }
    else
    {
      //no sector. Remember the pointer to it.
      m_Entities.Push(pEntity);
    }
  } //for entity

  if (m_Sectors.Length() == 0)
  {
    GenerateDefaultsector();
  }
}

void CCSWorld::GenerateDefaultsector()
{
  if (m_pMap->GetPlaneCount()<=0)
  {
    printf("Map contains no data. Aborting!");
    exit(1);
  }

  //Find out the size of this sector
  CdVector3 Min, Max;
  m_pMap->GetMapSize(Min, Max);

  Min.x -= 10;
  Min.y -= 10;
  Min.z -= 10;

  Max.x += 10;
  Max.y += 10;
  Max.z += 10;

  CdVector3 v[8];
  v[0] = CdVector3(Min.x, Min.y, Min.z);
  v[1] = CdVector3(Max.x, Min.y, Min.z);
  v[2] = CdVector3(Max.x, Min.y, Max.z);
  v[3] = CdVector3(Min.x, Min.y, Max.z);
  v[4] = CdVector3(Min.x, Max.y, Min.z);
  v[5] = CdVector3(Max.x, Max.y, Min.z);
  v[6] = CdVector3(Max.x, Max.y, Max.z);
  v[7] = CdVector3(Min.x, Max.y, Max.z);

  static int Planes[6][4] =
  {
    {4,5,1,0},
    {5,6,2,1},
    {6,7,3,2},
    {7,4,0,3},
    {0,1,2,3},
    {7,6,5,4}
  };

  //Create the Brush for that sector;
  CMapBrush* pBrush = new CMapBrush(NULL);

  int i;
  for (i=0; i<6; i++)
  {
    //For every side of the default sector create a flatshaded
    //plane in black color.
    CMapTexturedPlane* pPlane = m_pMap->AddPlane(v[Planes[i][2]],
                                                 v[Planes[i][1]],
                                                 v[Planes[i][0]],
                                                 0,0,0); //black
    assert(pPlane);
    pBrush->AddPlane(pPlane);
  }

  pBrush->CreatePolygons();

  CCSSector* pSector = new CCSSector(pBrush);
  m_Sectors.Push(pSector);
}

bool CCSWorld::Write(const char* filename, CMapFile* pMap, const char * /*sourcename*/)
{
  if (!PrepareData(TEMPWORLD, pMap)) return false;

  AllocateSkytextures();

  fprintf(m_fd, "<world>\n");

  Indent();
  WriteIndent();

  fprintf(m_fd, "<settings><clearzbuf>yes</clearzbuf></settings>\n\n", m_ScaleFactor);
    
  WriteIndent();
  fprintf(m_fd, "<key name=\"map2cs_scaling\" value=\"%g\" />\n\n", m_ScaleFactor);

  WritePlayerStart();
  WriteTextures();

  fprintf(m_fd, 
    "\t<renderpriorities>\n"
    "\t\t<priority name=\"sky\"><level>1</level><sort>NONE</sort></priority>\n"
    "\t\t<priority name=\"wall\"><level>2</level><sort>NONE</sort></priority>\n"
    "\t\t<priority name=\"object\"><level>3</level><sort>NONE</sort></priority>\n"
    "\t\t<priority name=\"alpha\"><level>4</level><sort>BACK2FRONT</sort></priority>\n"
    "\t</renderpriorities>\n"
    "\n");  

  WritePlugins();
  WriteSounds();
  WriteSpritesTemplate();
  WriteScriptsTemplate();
  WritePlanes();

  WriteCurvetemplates();

  WriteSkysector();
  WriteSectors();

  Unindent();

  fprintf(m_fd, "</world>\n"); //End of "<world>"
  fclose(m_fd);
  m_fd = NULL;

  remove(filename);

  CZipFile Zip(filename);
  Zip.AddFile(TEMPWORLD, "world");

  if (!m_pMap->GetTextureManager()->AddAllTexturesToZip(&Zip))
  {
    printf("Can't add all textures to the woldfile Aborting!\n");
    remove(TEMPWORLD);
    return false;
  }

  if (!Zip.WriteArchive())
  {
    printf("Can't create Archive '%s'! Aborting!\n", filename);
    return false;
  }

  remove(TEMPWORLD);
  return true;
}

CMapEntity* CCSWorld::GetWorldspawn()
{
  int i, NumEntities = GetNumEntities();
  for (i=0; i<NumEntities; i++)
  {
    CMapEntity* pEntity = GetEntity(i);
    assert(pEntity);
    if (strcasecmp(pEntity->GetClassname(), "worldspawn") == 0)
    {
      return pEntity;
    }
  }

  return NULL;
}

bool CCSWorld::NeedSkysector()
{
  // in future, this should be more elaborate: it should check all polygons, and
  // return only true, if there is at least one portal to the skysector.
  return true;
}

void CCSWorld::AllocateSkytextures()
{
  CMapEntity* pEntity = GetWorldspawn();
  if (!pEntity) return;

  CTextureManager* pTexMan  = m_pMap->GetTextureManager();

  if (pEntity->GetValueOfKey("skydome"))
  {
    pTexMan->GetTexture(pEntity->GetValueOfKey("skydome", "sky"));
  }
  else if (pEntity->GetValueOfKey("skybox"))
  {
    static const char ext[] = "frblud";
    const char* basename    = pEntity->GetValueOfKey("skybox", "sky");

	int i;
    for (i=0; i<6; i++)
    {
      char name[255];
      sprintf(name, "%s_%c", basename, ext[i]);
      CTextureFile* pTexture = pTexMan->GetTexture(name);
      CS_ASSERT(pTexture);

      pTexture->SetMipmapped(false);
    }
  }
}

void CCSWorld::WriteSkysector()
{
  CMapEntity* pEntity = GetWorldspawn();
  if (!pEntity) return;

  // Only write a skydome, if there has been specified a skysector in the mapfile.
  // There will likely be problems, if you are using sky-portals, but didn't specify
  // a skysector. (to handle this correctly, you should check if there is a skysector,
  // when writing portals to it.)
  if (pEntity->GetValueOfKey("skybox") ||
      pEntity->GetValueOfKey("skydome"))
  {
    WriteIndent();
    fprintf(m_fd, "<sector name=\"cs_skysector\">\n");
    Indent();

    WriteIndent();
    fprintf(m_fd, "<meshobj name=\"sky\">\n");
    Indent();

    WriteIndent();
    fprintf(m_fd, "<plugin>thing</plugin>\n");
    WriteIndent();
    fprintf(m_fd, "<zuse />\n");
    WriteIndent();
    fprintf(m_fd, "<priority>sky</priority>\n");

    WriteIndent();
    fprintf(m_fd, "<camera />\n");

    WriteIndent();
    fprintf(m_fd, "<params>\n");
    Indent();

    /*WriteIndent();
    fprintf(m_fd, "PART 'p1' (\n");
    Indent();*/

    WriteSky();

    /*Unindent();
    WriteIndent();
    fprintf(m_fd, ")\n");*/

    Unindent();
    WriteIndent();
    fprintf(m_fd, "</params>\n"); // END params

    Unindent();
    WriteIndent();
    fprintf(m_fd, "</meshobj>\n"); // END meshobj

    Unindent();
    WriteIndent();
    fprintf(m_fd, "</sector>\n\n"); // END sector
  }
}

void CCSWorld::WriteSky()
{
  CMapEntity* pEntity = GetWorldspawn();
  if (!pEntity) return;

  if (pEntity->GetValueOfKey("skybox"))
  {
    WriteSkybox();
    WriteIndent();
    //fprintf(m_fd, "<moveable />\n");
  }
  else if (pEntity->GetValueOfKey("skydome"))
  {
    WriteSkydome();
    WriteIndent();
    //fprintf(m_fd, "<moveable />\n");
  }
}

void CCSWorld::WriteSkydome()
{
  CMapEntity* pEntity = GetWorldspawn();
  if (!pEntity) return;

  const char* DomeName   = pEntity->GetValueOfKey   ("skydome",      "sky");
  double      DomeRadius = pEntity->GetNumValueOfKey("skydomeradius", 800);

  CTextureManager* pTexMan  = m_pMap->GetTextureManager();
  CTextureFile*    pTexture = pTexMan->GetTexture(DomeName);

  WriteIndent();
  //fprintf(m_fd, "CIRCLE (0,0,0:%g,0,%g,-12)\n", DomeRadius, DomeRadius);
  fprintf(m_fd, "<circle>0,0,0:%g,0,%g,-12</circle>\n", DomeRadius, DomeRadius);
  WriteIndent();
  fprintf(m_fd, "<material>%s</material><texlen>1</texlen>\n", pTexture->GetTexturename());
  WriteIndent();
  // appears to be unsupported now...
  fprintf(m_fd, "<skydome name=\"up\"><lighting>NO</lighting><radius>%g</radius> "
                "<vertices>0,1,2,3,4,5,6,7,8,9,10,11</vertices></skydome>\n", DomeRadius);
  WriteIndent();
  fprintf(m_fd, "<skydome name=\"down\"><lighting>NO</lighting><radius>%g</radius> "
                "<vertices>0,11,10,9,8,7,6,5,4,3,2,1</vertices></skydome>\n", -DomeRadius);
}

void CCSWorld::WriteSkybox()
{
  CMapEntity* pEntity = GetWorldspawn();
  if (!pEntity) return;

  CTextureManager* pTexMan  = m_pMap->GetTextureManager();
  const char* BoxName = pEntity->GetValueOfKey   ("skybox",    "sky");
  double      BoxSize = pEntity->GetNumValueOfKey("skyboxsize", 1600) / 2;

  //The following code is pretty ugly, because to create a goodlooking
  //skybox, we create a flatcolored box, that contains another (smaller)
  //box with slightly overlapping sides.
  //This is because CS has some precision problems on the sides, and so
  //we will end up with lots of cracks between the boxes sides, which just
  //looks ugly. So for now we accept significant overdraw, to avoid cracks.
#if 0
  static const struct {int x; int y; int z;} Vertices [] =
  {{-1,-1,-1},
   {-1,-1, 1},
   {-1, 1,-1},
   {-1, 1, 1},
   { 1,-1,-1},
   { 1,-1, 1},
   { 1, 1,-1},
   { 1, 1, 1}};

  static struct
  {
    const char*   ext;
    const char*   vertices;
    CTextureFile* pTex;
  }

  ThingSides[] =
  {
    {"f", " 3, 7, 5, 1", NULL},
    {"r", "15,14,12,13", NULL},
    {"b", " 6, 2, 0, 4", NULL},
    {"l", "10,11, 9, 8", NULL},
    {"u", "18,22,23,19", NULL},
    {"d", "17,21,20,16", NULL},
  };

  //assign texture pointers to the sides of the skybox
  int s,v;
  for (s=0; s<int(sizeof(ThingSides)/sizeof(ThingSides[0])); s++)
  {
    char name[255];
    sprintf(name, "%s_%s", BoxName, ThingSides[s].ext);
    ThingSides[s].pTex = pTexMan->GetTexture(name);
  }


  //Write the skyboxes vertices

  double TweakFactor = 1.005;
  for (v = 0; v<int(sizeof(Vertices)/sizeof(Vertices[0])); v++)
  {
    WriteIndent();
    fprintf(m_fd, "<v x=\"%g\" y=\"%g\" z=\"%g\" />\n",
                  BoxSize*Vertices[v].x*TweakFactor,
                  BoxSize*Vertices[v].y*TweakFactor,
                  BoxSize*Vertices[v].z);
  }
  for (v = 0; v<int(sizeof(Vertices)/sizeof(Vertices[0])); v++)
  {
    WriteIndent();
    fprintf(m_fd, "<v x=\"%g\" y=\"%g\" z=\"%g\" />\n",
                  BoxSize*Vertices[v].x,
                  BoxSize*Vertices[v].y*TweakFactor,
                  BoxSize*Vertices[v].z*TweakFactor);
  }
  for (v = 0; v<int(sizeof(Vertices)/sizeof(Vertices[0])); v++)
  {
    WriteIndent();
    fprintf(m_fd, "<v x=\"%g\" y=\"%g\" z=\"%g\" />\n",
                  BoxSize*Vertices[v].x*TweakFactor,
                  BoxSize*Vertices[v].y,
                  BoxSize*Vertices[v].z*TweakFactor);
  }

  //write the skyboxes polygons
  for (s=0; s<int(sizeof(ThingSides)/sizeof(ThingSides[0])); s++)
  {
    WriteIndent();
    fprintf(m_fd, "POLYGON '' (VERTICES (%s) MATERIAL ('%s') "
                  "TEXTURE (LEN (%g)) LIGHTING (NO))\n",
                  ThingSides[s].vertices,
                  ThingSides[s].pTex->GetTexturename(),
                  BoxSize * 2 * TweakFactor);
  }
#endif

  //Matze: This is my own version, I can't realize crackles so I'm using the
  // simple version here... If it's ok in all cases you can remove the above
  static const struct {int x; int y; int z;} Vertices [] =
  {{-1,-1, 1}, { 1,-1, 1},
   {-1,-1,-1}, { 1,-1,-1},
   {-1, 1, 1}, { 1, 1, 1},
   {-1, 1,-1}, { 1, 1,-1}};

  static struct
  {
    const char*   ext;
    const char*   vertices;
    CTextureFile* pTex;
  }

  ThingSides[] =
  {
    {"f", "<v>4</v><v>5</v><v>1</v><v>0</v>", NULL},
    {"r", "<v>5</v><v>7</v><v>3</v><v>1</v>", NULL},
    {"b", "<v>7</v><v>6</v><v>2</v><v>3</v>", NULL},
    {"l", "<v>6</v><v>4</v><v>0</v><v>2</v>", NULL},
    {"u", "<v>6</v><v>7</v><v>5</v><v>4</v>", NULL},
    {"d", "<v>3</v><v>2</v><v>0</v><v>1</v>", NULL},
  };

  //assign texture pointers to the sides of the skybox
  int s,v;
  for (s=0; s<int(sizeof(ThingSides)/sizeof(ThingSides[0])); s++)
  {
    char name[255];
    sprintf(name, "%s_%s", BoxName, ThingSides[s].ext);
    ThingSides[s].pTex = pTexMan->GetTexture(name);
  }
 
  for (v = 0; v<int(sizeof(Vertices)/sizeof(Vertices[0])); v++)
  {
    WriteIndent();
    fprintf(m_fd, "<v x=\"%g\" y=\"%g\" z=\"%g\" />\n",  
                  BoxSize*Vertices[v].x,
                  BoxSize*Vertices[v].y,
                  BoxSize*Vertices[v].z);
  }

  //write the skyboxes polygons
  for (s=0; s<int(sizeof(ThingSides)/sizeof(ThingSides[0])); s++)
  {
    WriteIndent();
    fprintf(m_fd, "<p>%s" 
      "<texmap>"
      "<uv idx=\"0\" u=\"0.005\" v=\"0.005\" />"
      "<uv idx=\"1\" u=\"0.995\" v=\"0.005\" />"
      "<uv idx=\"2\" u=\"0.995\" v=\"0.955\" />"
      "</texmap>"
      "<material>%s</material><lighting>NO</lighting></p>\n",
                  ThingSides[s].vertices,
                  ThingSides[s].pTex->GetTexturename());
  }
}

void CCSWorld::FindAdditionalTextures()
{
  // Thomas Hieber, 16.05.2001:
  // Please don't ask me about this function. I didn't create it, nor do I know the
  // deeper truth about it. All it did was to move the code to a separate function
  // for better maintainablility. (And I have made the textures really register with
  // the texture manager, which should really help to get the textures in the zip
  // file, if map2cs can find them. Also it makes handling of materials much easier,
  // because I don't have to skip through all entities twice, and it avoids adding
  // some textures multiple times with the same name.)

  int j=0;

  // Look for cs_sprite entities
  for (j=0; j<m_pMap->GetNumEntities(); j++)
  {
    CMapEntity* pEntity = m_pMap->GetEntity(j);
    if (strcmp(pEntity->GetClassname(), "cs_sprite")==0)
    {
      if (pEntity->GetValueOfKey("texture"))
      {
        const char* basename = pEntity->GetValueOfKey("texture", "white");

        // the texture manager will create a new texture for us, if the given
        // texture is missing
        CTextureFile* pTexture = m_pMap->GetTextureManager()->GetTexture(basename);
        assert(pTexture);

        const char* keycolor = pEntity->GetValueOfKey("keycolor", "");
        if (*keycolor)
        {
          //if the string is not empty
          float r = 0.0f;
          float g = 0.0f;
          float b = 0.0f;
          if (sscanf(keycolor, "%f %f %f", &r, &g, &b) == 3)
          {
            pTexture->SetKeyColor(r, g, b);
          }
        }
      }
    }
  }

  //texture for 3D sprites (cs_model)
  //requires models subdir under data. also, need to be added to vfs
  int NumEntities = GetNumEntities();
  for (j=0; j<NumEntities; j++)
  {
    CMapEntity* pEntity = GetEntity(j);
    assert(pEntity);
    if (strcmp(pEntity->GetClassname(), "cs_model") == 0)
    {
      const char* csnamevalue = pEntity->GetValueOfKey("cs_name");
      if (csnamevalue)
      {
        const char* modelnamevalue = pEntity->GetValueOfKey("modelname");
        if (modelnamevalue)
        {
          if (pEntity->GetValueOfKey("texture"))
          {
            CTextureFile* pTexture = m_pMap->GetTextureManager()->GetTexture(modelnamevalue);
            assert(pTexture);

            char name[255];
            const char* basename = pEntity->GetValueOfKey("texture", "white");
            sprintf(name, "/lib/models/%s", basename);

            pTexture->SetFilename(name);
          }
        }
      }
    }
  }
}

bool CCSWorld::WriteTextures()
{
  CMapEntity* pEntity = GetWorldspawn();

  FindAdditionalTextures();

  WriteIndent();
  fprintf(m_fd, "<textures>\n");
  Indent();

  int i=0;
  for (i=0; i<m_pMap->GetTextureManager()->GetTextureCount(); i++)
  {
    CTextureFile* pTexture = m_pMap->GetTextureManager()->GetTexture(i);
    assert(pTexture);

    WriteIndent();

    char replacename[255];
    const char *newtexfile;
    sprintf(replacename, "filename_%s", pTexture->GetTexturename());
    if ((newtexfile = pEntity->GetValueOfKey(replacename)))
      pTexture->SetStored (false);
      
    fprintf(m_fd, "<texture name=\"%s\"><file>%s</file>",
      pTexture->GetTexturename(),
      newtexfile?newtexfile:pTexture->GetFilename());

    if (pTexture->IsColorKeyed())
    {
      float r, g, b;
      pTexture->GetKeyColor(r, g, b);

      fprintf(m_fd, " <transparent red=\"%g\" green=\"%g\" blue=\"%g\" />", r, g, b);
    }

    if (!pTexture->IsMipmapped())
    {
      fprintf(m_fd, " <mipmap>no</mipmap>");
    }

    fprintf(m_fd, "</texture>\n"); //End of TEXTURE (single line statement)
  }

  Unindent();
  WriteIndent();
  fprintf(m_fd, "</textures>\n\n"); //End of "TEXTURES ("

  //-------------------------------------------------------------------

  WriteIndent();
  fprintf(m_fd, "<materials>\n");
  Indent();

  for (i=0; i<m_pMap->GetTextureManager()->GetTextureCount(); i++)
  {
    CTextureFile* pTexture = m_pMap->GetTextureManager()->GetTexture(i);
    assert(pTexture);

    WriteIndent();
    fprintf(m_fd, "<material name=\"%s\"><texture>%s</texture></material>\n",
                  pTexture->GetTexturename(),
                  pTexture->GetTexturename());
  }

  Unindent();
  WriteIndent();
  fprintf(m_fd, "</materials>\n\n"); //End of "MATERIALS ("

  return true;
}

bool CCSWorld::WritePlugins()
{
  WriteIndent();
  fprintf(m_fd, "<plugins>\n");
  Indent();

  WriteIndent(); fprintf(m_fd, "<plugin name=\"thing\">crystalspace.mesh.loader.thing</plugin>\n");
  WriteIndent(); fprintf(m_fd, "<plugin name=\"thingFact\">crystalspace.mesh.loader.factory.thing</plugin>\n");
  WriteIndent(); fprintf(m_fd, "<plugin name=\"plane\">crystalspace.mesh.loader.thing.plane</plugin>\n");
  WriteIndent(); fprintf(m_fd, "<plugin name=\"bezier\">crystalspace.mesh.loader.thing.bezier</plugin>\n");

  Unindent();
  WriteIndent();
  fprintf(m_fd, "</plugins>\n\n"); //End of "PLUGINS ("

  return true;
}


bool CCSWorld::WritePlayerStart()
{
  int i;

  //iterate all entities, brushes, polygons and vertices:
  for (i=0; i<m_pMap->GetNumEntities(); i++)
  {
    CMapEntity* pEntity = m_pMap->GetEntity(i);
    assert(pEntity);

    const char* classname = pEntity->GetClassname();
    assert(classname);

    if (strcmp(classname, "info_player_start")==0 ||
        strcmp(classname, "info_player_deathmatch")==0)
    {
      double x, y, z;
      if (pEntity->GetTripleNumValueOfKey("origin", x,y,z))
      {
        CdVector3 vec(x,y,z);
        CISector* pSector = FindSectorForPoint(vec);
        if (pSector)
        {
          //The strings format matched
          WriteIndent();
	  CdVector3 forw(0,0,1);
	  // rotate angle
	  double angle = 0.0;
	  if (pEntity->GetNumValueOfKey("angle", angle))
	  {
	    angle = PI * angle/180;
	    forw = CdMatrix3 (cos(angle), 0, -sin(angle),
			       0,          1,           0,
			       sin(angle), 0, cos(angle)) * forw;
	  }
	  
          fprintf(m_fd, "<start><sector>%s</sector><position x=\"%g\" y=\"%g\" z=\"%g\" />"
			"<up x=\"0\" y=\"1\" z=\"0\" /><forward x=\"%g\" y=\"%g\" z=\"%g\" />"
			"</start>\n\n",
                  pSector->GetName(),
                  x*m_ScaleFactor,
                  z*m_ScaleFactor,
                  y*m_ScaleFactor,
		  forw.x, forw.y, forw.z
		  );
          return true;
        }
        else
        {
          printf("warning: info_player_start not inside a valid sector.\n");
          printf("         no startinfo is written!\n");
        }
      }
      else
      {
        printf("warning: info_player_start has no origin.\n");
        printf("         no startinfo is written!\n");
      }
    }
  }
  return true;
}

bool CCSWorld::WritePlanes()
{
  int i;
  for (i=0; i<m_pMap->GetPlaneCount(); i++)
  {
    CMapTexturedPlane* pPlane = m_pMap->GetPlane(i);

    char TexName[200];
    sprintf(TexName, "Plane%d", i);
    pPlane->SetName(TexName);

    //Check if the plane has a texture. Otherwise, we will not write
    //thi info to the map file.
    if (pPlane->GetTexture())
    {
      CdVector3 Origin = pPlane->GetTextureCoordinates(0);
      CdVector3 First  = pPlane->GetTextureCoordinates(1);
      CdVector3 Second = pPlane->GetTextureCoordinates(2);

      WriteIndent();
      fprintf(m_fd, "<addon>\n");
      Indent ();
      WriteIndent();
      fprintf(m_fd, "<plugin>plane</plugin>\n");
      WriteIndent();
      fprintf(m_fd, "<params>\n");
      Indent ();
      WriteIndent();
      fprintf(m_fd, "<name>%s</name>\n", TexName);
      WriteVector("orig", Origin);
      WriteVector("first", First);
      WriteVector("second", Second);
      WriteIndent();
      fprintf(m_fd, "<firstlen>%g</firstlen>\n",
              (First-Origin).Norm()*m_ScaleFactor);
      WriteIndent();
      fprintf(m_fd, "<secondlen>%g</secondlen>\n",
              (Second-Origin).Norm()*m_ScaleFactor);
      Unindent ();
      WriteIndent();
      fprintf(m_fd, "</params>\n");
      Unindent ();
      WriteIndent();
      fprintf(m_fd, "</addon>\n"); //End of "PLANE ("
    }
  }
  fprintf(m_fd, "\n"); //End of planes section
  return true;
}

bool CCSWorld::WriteSectors()
{
  int i;
  for (i=0; i<m_Sectors.Length(); i++)
  {
    if (!m_Sectors[i]->Write(this)) return false;
  }
  return true;
}

bool CCSWorld::WriteVertex(double x, double y, double z)
{
  WriteVector("v", x,y,z);
  return true;
}

bool CCSWorld::WriteVector(const char* name, double x, double y, double z)
{
  WriteIndent();
  fprintf(m_fd, "<%s x=\"%g\" y=\"%g\" z=\"%g\" />\n",
	  name,
          x*m_ScaleFactor,
          z*m_ScaleFactor,
          y*m_ScaleFactor);
  return true;
}

bool CCSWorld::WriteVector(const char* name, const CdVector3& v)
{
  return WriteVector(name, v.x, v.y, v.z);
}


bool CCSWorld::WriteCurvetemplates()
{
  int i, c;
  for (i=0; i<m_pMap->GetNumEntities(); i++)
  {
    CMapEntity* pEntity = m_pMap->GetEntity(i);
    for (c=0; c<pEntity->GetCurveCount(); c++)
    {
      CMapCurve*    pCurve   = pEntity->GetCurve(c);
      CTextureFile* pTexture = pCurve->GetTexture();
      if (pTexture->IsVisible())
      {
        pCurve->Write(this);
      }
    }
  }

  return true;
}

bool CCSWorld::WriteKeys(CIWorld* pWorld, CMapEntity* pEntity)
{
  if (pEntity)
  {
	int i;
    for (i=0; i<pEntity->GetNumberOfKeyValuePairs(); i++)
    {
      CMapKeyValuePair* pKV = pEntity->GetKeyValuePair(i);
      pWorld->WriteIndent();
      fprintf(pWorld->GetFile(), "<key name=\"%s\" value=\"%s\" />\n",
              pKV->GetKey(), pKV->GetValue());
    }

    fprintf(pWorld->GetFile(), "\n");
  }
  return true;
}

bool CCSWorld::WritePolygon(CMapPolygon* pPolygon, CCSSector* pSector,
                            bool SectorPolygon, const CVertexBuffer& Vb)
{
  const CMapTexturedPlane* pPlane   = pPolygon->GetBaseplane();
  CMapEntity*              pEntity  = pPolygon->GetEntity();
  int l;

  WriteIndent();
  fprintf(m_fd, "<p>\n");
  
  Indent();
  if (SectorPolygon)
  {
    //Because for a sector we draw the _inside_ of the brush, we spit out the
    //vertices in reverse order, so they will have proper orientation for
    //backface culling in the engine.
    for (l=pPolygon->GetVertexCount()-1; l>=0; l--)
    {
      WriteIndent();
      fprintf(m_fd, "<v>%d</v>\n", Vb.GetIndex(pPolygon->GetVertex(l)));
    }
  }
  else
  {
    //regular vertex order
    for (l=0; l<pPolygon->GetVertexCount(); l++)
    {
      WriteIndent();
      fprintf(m_fd, "<v>%d</v>\n", Vb.GetIndex(pPolygon->GetVertex(l)));
    }
  }

  CTextureFile* pTexture = pPlane->GetTexture();
  if (!pTexture)
  {
    int r, g, b;
    pPlane->GetColor(r,g,b);
    fprintf(m_fd, "<flatcol red=\"%g\" green=\"%g\" blue=\"%g\" />",
                r/(float)255, g/(float)255, b/(float)255);
  }
  else
  {
    //print textureinfo
    WriteIndent();
    fprintf(m_fd, "<material>%s</material>\n", pTexture->GetTexturename());
    WriteIndent();
    fprintf(m_fd, "<texmap><plane>%s</plane></texmap>\n", pPolygon->GetBaseplane()->GetName());
  }

  if (pEntity)
  {
    bool Sky = pEntity->GetBoolValueOfKey("sky", false);
    if (Sky)
    {
      fprintf(m_fd, "<portal><sector>cs_skysector</sector><clip /></portal>");
    }
    else
    {
      //support for special rendering flags. Ideally, these properties would be
      //assigned to surfaces instead of entities, but the Quake/ Half-Life map
      //format doesn't have a really portable way to do so.
      double Alpha  = pEntity->GetNumValueOfKey ("alpha",  100);
      bool   Solid  = pEntity->GetBoolValueOfKey("solid",  true);
      bool   Mirror = pEntity->GetBoolValueOfKey("mirror", false);
      if (!pEntity->GetBoolValueOfKey("lighting", true))
      {
        //if the polygon is not supposed to be lighted...
        fprintf(m_fd, "<lighting>no</lighting>");
      }

      if (Alpha < 100)
      {
        fprintf(m_fd, "<alpha>%g</alpha>", Alpha);
      }

      if (Mirror || Alpha<100 || !Solid)
      {
        //We have a special case, where we need to turn this polygon into a portal.
        const char* targetsector =
          pEntity->GetValueOfKey("targetsector", pSector->GetName());

	WriteIndent();
        fprintf(m_fd, "<portal><sector>%s</sector>", targetsector);

        if ( Mirror )
        {
          fprintf(m_fd, "<mirror /><clip />");
        }
        else
        {
          if (Solid)
          {
            fprintf(m_fd, " <clip />");
          }
        }
	fprintf(m_fd, "</portal>\n");
      } //if polygon is a portal of some special kind
    } // if entity classname ==
  } // if there is a entity to define some special flags.

  Unindent();
      WriteIndent();
  fprintf(m_fd, "</p>\n"); //End of Polygon
  return true;
}

CIThing* CCSWorld::CreateNewThing(CMapEntity* pEntity)
{
  return new CCSThing(pEntity);
}

CISector* CCSWorld::CreateNewSector(CMapBrush* pBrush)
{
  return new CCSSector(pBrush);
}

void CCSWorld::WriteSpritesTemplate()
{
  int i;
  char   mdlname[99] = "none";
  char   action[99] = "none";

  //iterate all entities, brushes, polygons and vertices:
  for (i=0; i<m_pMap->GetNumEntities(); i++)
  {
    CMapEntity* pEntity = m_pMap->GetEntity(i);
    assert(pEntity);

    CTextureManager* pTexMan  = m_pMap->GetTextureManager();

    if (pEntity->GetValueOfKey("skydome"))
    {
      pTexMan->GetTexture(pEntity->GetValueOfKey("skydome", "sky"));
    }

    const char* classname = pEntity->GetClassname();
    assert(classname);

    if (strcmp(classname, "cs_model")==0)
    {
      const char* csnamevalue = pEntity->GetValueOfKey("cs_name");

      if (!csnamevalue)
      {
        return;
      }

      const char* modelnamevalue = pEntity->GetValueOfKey("modelname");
      if (modelnamevalue)
      {
        char dummy;
        if (sscanf(modelnamevalue, "%s%c",mdlname, &dummy) == 1)
        {
          WriteIndent();
          fprintf(m_fd, "<meshfact name=\"s\">\n", csnamevalue);
          Indent();
          WriteIndent();
          fprintf(m_fd, "<plugin>spr3d</plugin>\n", csnamevalue);
          WriteIndent();
          //TODO: see if we can initalize .3ds files
          fprintf(m_fd, "<file>/lib/models/%s.mdl</file>\n", mdlname);
          WriteIndent();
          fprintf(m_fd, "<material>%s</material>\n", csnamevalue);

          //TODO: add loop for multiple actions, action2, action3, ...
          //search for all frames from base name would be better??
          const char* actionvalue = pEntity->GetValueOfKey("action");

          if (sscanf(actionvalue, "%s%c",action, &dummy) == 1)
          {
            WriteIndent();
            fprintf(m_fd, "<action name=\"%s\">\n", actionvalue);
            WriteIndent();
            fprintf(m_fd, "(\n");
            Indent();

            int i = 1;
            char frameaction[100];
            sprintf(frameaction, "action%d", i);
            while (pEntity->GetValueOfKey(frameaction))
            {
              WriteIndent();
              actionvalue = pEntity->GetValueOfKey(frameaction);
              if (sscanf(actionvalue, "%s%c",frameaction, &dummy) == 1)
              {
                fprintf(m_fd, "<f name=\"%s\" delay=\"100\" />\n", frameaction); //hardcode 100 for now
              }
              i++;
              sprintf(frameaction, "action%d", i);
            }

            Unindent();
            WriteIndent();
            fprintf(m_fd, "</action>\n");
          }

          Unindent();
          WriteIndent();
          fprintf(m_fd, ") \n\n");
        }
      }
    }

  } //for entity
  return;
}

void CCSWorld::WriteScriptsTemplate()
{
  int i;

  //iterate all entities, brushes, polygons and vertices:
/*  for (i=0; i<m_pMap->GetNumEntities(); i++)
  {
    CMapEntity* pEntity = m_pMap->GetEntity(i);
    assert(pEntity);

    const char* classname = pEntity->GetClassname();
    assert(classname);

    if (strcmp(classname, "cs_script")==0)
    {
      const char* cs_scriptname = pEntity->GetValueOfKey("cs_name");
      assert(cs_scriptname);
      const char* scriptfilename = pEntity->GetValueOfKey("script_cmdfile");
      assert(scriptfilename);

      if (!cs_scriptname || strcmp(scriptfilename, "none")==0 )
      {
        return;
      }

      //if scriptfile does not exist
      CScriptParser parser;
      if (!parser.Open(scriptfilename))
      {
        return;
      }
      //exit

      int found = 0;
      char Buffer[1000] = "";
      char spritecode[1000] = "";

      while (found == 0)
      {
        parser.GetTextToken(Buffer);
        if (strcmp(cs_scriptname, Buffer) == 0)
        {
          found = 1;
        }
      }

      //else look for script if script does not exist
      if (found == 0)
      {
        return;
      }
      else
      {
        int count = 1;

        parser.GetLineToken(Buffer);
        strcpy(spritecode, Buffer);
        while (count != 0 )
        {
          parser.GetLineToken(Buffer);

          if ('(' == Buffer[0])
          {
            count++;
            strcat(spritecode, Buffer);
          }
          else if (')' == Buffer[0])
          {
            count--;
            if(count != 0)
            {
              strcat(spritecode, Buffer);
            }
          }
          else
          {
            strcat(spritecode, Buffer);
          }
        }
        if (found == 1)
        {
          WriteIndent();
          fprintf(m_fd, "SCRIPT '%s'\n", cs_scriptname);
          Indent();
          WriteIndent();
          fprintf(m_fd, "%s", spritecode);
          WriteIndent();
          fprintf(m_fd, ") \n\n");
          Unindent();
        }
      }
    }//if classname
  } //for entity*/
  return;
}

//Sounds template section
    //SOUNDS (
    //  SOUND 'track4' (FILE (track4.wav))
    //)
//add filename.zip to vfs in lib/sounds
void CCSWorld::WriteSounds()
{
  int i;
  char sfname[99] = "none";
  int found = 0;
  //iterate all entities, brushes, polygons and vertices:
  for (i=0; i<m_pMap->GetNumEntities(); i++)
  {
    CMapEntity* pEntity = m_pMap->GetEntity(i);
    assert(pEntity);

    const char* classname = pEntity->GetClassname();
    assert(classname);

    if (strcmp(classname, "cs_sound")==0)
    {
      found = 1;
      break;
    }
  }

  if (found == 1)
  {
    WriteIndent();
    fprintf(m_fd, "<sounds>\n");
    Indent();

    for (i=0; i<m_pMap->GetNumEntities(); i++)
    {
      CMapEntity* pEntity = m_pMap->GetEntity(i);
      assert(pEntity);

      const char* classname = pEntity->GetClassname();
      assert(classname);

      if (strcmp(classname, "cs_sound")==0)
      {
        const char* soundfileval = pEntity->GetValueOfKey("soundfile");
        if (soundfileval)
        {
          char dummy;
          if (sscanf(soundfileval, "%s%c",sfname, &dummy) == 1)
          {
            WriteIndent();
            fprintf(m_fd, "<sound name=\"%s\"><file>/lib/sounds/%s</file></sound>\n", sfname, sfname);
          }
        }
      }
    }//end for
    Unindent();
    WriteIndent();
    fprintf(m_fd, "</sounds>\n");
  }
}//end sounds
