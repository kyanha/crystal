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

#include <stdlib.h>

#include "mapstd.h"
#include "map.h"
#include "cworld.h"
#include "wad3file.h"
#include "csutil/scf.h"
#include "csutil/cfgfile.h"
#include "cstool/initapp.h"
#include "iutil/objreg.h"

CS_IMPLEMENT_APPLICATION

void PrintSyntax()
{
  printf("map2cs is a utility program to convert a MAP file to a Crystal Space level\n\n");

  printf("syntax: map2cs <mapfile> <worldfile> [configfile]\n");

  printf("for example: map2cs sample.map data/sample.zip sample.cfg\n");
  printf("             to create the CS level called sample.zip from sample.map\n");
  printf("             using config data in sample.cfg\n");
}

int main( int argc, char *argv[ ])
{
  // this is required for the image loader
  csRef<iObjectRegistry> object_reg = csInitializer::CreateEnvironment (argc, argv);
  if (!object_reg) return false;

  printf("\nmap2cs version 0.97, Copyright (C) 1999-2000 by Thomas Hieber\n");
  printf("                     Copyright (C) 1999-2003 by Jorrit Tyberghein "
    "and others\n\n");
  printf("map2cs comes with ABSOLUTELY NO WARRANTY; for details see the file 'COPYING'\n");
  printf("This is free software, and you are welcome to redistribute it under certain\n");
  printf("conditions; see `COPYING' for details.\n\n");

  if (argc < 3 || argc > 4)
  {
    PrintSyntax();
    return 1;
  }
  
  const char* configfile;
  if (argc==4)
  {
    configfile = argv[3];
  }
  else
  {
    char* filename = new char[255];
    const char* crystal = getenv("CRYSTAL");
    if (!crystal)
    {
      printf ("Couldn't find Crystal Space directory (CRYSTAL var)! Using current dir!\n");
      strcpy (filename, "data/config/map2cs.cfg");
    }
    else
    {
      strcpy (filename, crystal);
      strcat (filename, "/data/config/map2cs.cfg");
    }
    configfile=filename;
  }
  const char* mapfile   = argv[1];
  const char* worldfile = argv[2];

  CMapFile Map;
  printf("Reading map '%s'\n", mapfile);
  Map.Read(mapfile, configfile);

  Map.CreatePolygons();
  printf("Writing world '%s'\n", worldfile);

  CCSWorld World;
  World.Write(worldfile, &Map, mapfile);

  printf("done.");

  csInitializer::DestroyApplication (object_reg);

  return 0;
}
