/*
    Copyright (C) 2001 by Martin Geisse <mgeisse@gmx.net>

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
#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "iutil/objreg.h"
#include "imesh/mdlconv.h"
#include "cstool/mdldata.h"
#include "csutil/datastrm.h"
#include "csutil/csstring.h"
#include "csutil/nobjvec.h"
#include "csutil/garray.h"

// all int's in an MDL file are little endian
#include "cssys/csendian.h"

// upper bound onsize of biggest data element (vertex, polygon) in an MDL file
static int const MAX_DATAELEMENT_SIZE = 8192;

// size of various MDL elements
static int const SIZEOF_MDLSHORT = 2;
static int const SIZEOF_MDLLONG = 4;
static int const SIZEOF_MDLFLOAT = 4;
static int const SIZEOF_MDLSKINNAME = 64;
static int const SIZEOF_MDLFRAMENAME = 16;
static int const SIZEOF_MDLHEADER = 11 * SIZEOF_MDLFLOAT + 8 * SIZEOF_MDLLONG;

struct csMDLHeader
{
  // translation and scale
  csVector3 Scale, Origin;
  // radius of the bounding sphere
  float BoundingSphereRadius;
  // position of the player camera for this model
  csVector3 CameraPosition;
  // number of skins, width and height of the skin texture
  long SkinCount, SkinWidth, SkinHeight;
  // number of vertices, triangles and actions
  long VertexCount, TriangleCount, ActionCount;
  // ???
  long SyncType, Flags;
  // average triangle size (?)
  float AverageTriangleSize;
};

typedef csGrowingArray<csVector2> csVector2Array;

// ---------------------------------------------------------------------------

class csModelConverterMDL : iModelConverter
{
private:
  csModelConverterFormat FormatInfo;

public:
  SCF_DECLARE_IBASE;

  /// constructor
  csModelConverterMDL (iBase *pBase);

  /// destructor
  virtual ~csModelConverterMDL ();

  bool Initialize (iObjectRegistry *object_reg);
  virtual int GetFormatCount();
  virtual const csModelConverterFormat *GetFormat( int idx );
  virtual csPtr<iModelData> Load( uint8* Buffer, uint32 size );
  virtual csPtr<iDataBuffer> Save( iModelData*, const char *format );

  struct Component : public iComponent
  {
    SCF_DECLARE_EMBEDDED_IBASE (csModelConverterMDL);
    virtual bool Initialize (iObjectRegistry *object_reg)
    {
      return scfParent->Initialize (object_reg);
    }
  } scfiComponent;
};

SCF_IMPLEMENT_IBASE (csModelConverterMDL)
  SCF_IMPLEMENTS_INTERFACE (iModelConverter)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csModelConverterMDL::Component)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_FACTORY (csModelConverterMDL)

SCF_EXPORT_CLASS_TABLE (mdlie)
  SCF_EXPORT_CLASS (csModelConverterMDL,
    "crystalspace.modelconverter.mdl",
    "MDL Model Converter")
SCF_EXPORT_CLASS_TABLE_END

CS_IMPLEMENT_PLUGIN

csModelConverterMDL::csModelConverterMDL (iBase *pBase)
{
  SCF_CONSTRUCT_IBASE (pBase);
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiComponent);

  FormatInfo.Name = "mdl";
  FormatInfo.CanLoad = true;
  FormatInfo.CanSave = false;
}

csModelConverterMDL::~csModelConverterMDL ()
{
}

bool csModelConverterMDL::Initialize (iObjectRegistry *)
{
  return true;
}

int csModelConverterMDL::GetFormatCount ()
{
  return 1;
}

const csModelConverterFormat *csModelConverterMDL::GetFormat (int idx)
{
  return (idx == 0) ? &FormatInfo : 0;
}

/*
  Purpose:

    csModelConverterMDL::Load() reads a Quake2 MDL model file.

  Examples:

  Modified:

    13 July 1999 Gary Haussmann

  Author:

    John Burkardt

  Modified by Martin Geisse to work with the new converter system.
*/

static bool CheckMDLVersion (csDataStream &in)
{
  // Read in header and check for a correct file.  The
  // header consists of two longs, containing
  // the magic identifiter 'IDP2' as the first long,
  // followed by a version number (8)
  uint32 FileID, FileVersion;
  in.ReadUInt32 (FileID);
  in.ReadUInt32 (FileVersion);
  FileID = little_endian_long (FileID);
  FileVersion = little_endian_long (FileVersion);

  if (FileID != ( ('O'<<24)+('P'<<16)+('D'<<8)+'I' ) )
    return false;

  if (FileVersion != 6)
    return false;

  return true;
}

#define CS_MDL_READ_LONG(name)						\
  hdr->name = get_le_long (buf + Position);				\
  Position += SIZEOF_MDLLONG;

#define CS_MDL_READ_FLOAT(name)						\
  hdr->name = convert_endian(*((float*)(buf + Position)));		\
  Position += SIZEOF_MDLFLOAT;

static void ReadMDLHeader (csMDLHeader *hdr, csDataStream *in)
{
  char buf [SIZEOF_MDLHEADER];
  in->Read (buf, SIZEOF_MDLHEADER);
  int Position = 0;

  CS_MDL_READ_FLOAT (Scale.x);
  CS_MDL_READ_FLOAT (Scale.y);
  CS_MDL_READ_FLOAT (Scale.z);
  CS_MDL_READ_FLOAT (Origin.x);
  CS_MDL_READ_FLOAT (Origin.y);
  CS_MDL_READ_FLOAT (Origin.z);
  CS_MDL_READ_FLOAT (BoundingSphereRadius);
  CS_MDL_READ_FLOAT (CameraPosition.x);
  CS_MDL_READ_FLOAT (CameraPosition.y);
  CS_MDL_READ_FLOAT (CameraPosition.z);
  CS_MDL_READ_LONG (SkinCount);
  CS_MDL_READ_LONG (SkinWidth);
  CS_MDL_READ_LONG (SkinHeight);
  CS_MDL_READ_LONG (VertexCount);
  CS_MDL_READ_LONG (TriangleCount);
  CS_MDL_READ_LONG (ActionCount);
  CS_MDL_READ_LONG (SyncType);
  CS_MDL_READ_LONG (Flags);
  CS_MDL_READ_FLOAT (AverageTriangleSize);
}
#undef CS_MDL_READ_LONG
#undef CS_MDL_READ_FLOAT

csPtr<iModelData> csModelConverterMDL::Load (uint8 *Buffer, uint32 Size)
{
  // prepare input buffer
  csDataStream in (Buffer, Size, false);
  unsigned char Readbuffer[MAX_DATAELEMENT_SIZE];
  int i,j,k;

  // check for the correct version
  if (!CheckMDLVersion (in))
    return 0;

  // build the object framework
  iModelData *Scene = new csModelData ();
  iModelDataObject *Object = new csModelDataObject ();
  Scene->QueryObject ()->ObjAdd (Object->QueryObject ());

  // read MDL header
  csMDLHeader Header;
  ReadMDLHeader (&Header, &in);

  // skip skin data
  for (i=0; i<Header.SkinCount; i++)
  {
    in.Read (Readbuffer, SIZEOF_MDLLONG);
    int Type = get_le_long (Readbuffer);
    if (Type == 0) {
      // single skin
      in.Skip (Header.SkinWidth * Header.SkinHeight);
    } else if (Type == 1) {
      in.Read (Readbuffer, SIZEOF_MDLLONG);
      int MultiskinCount = get_le_long (Readbuffer);
      for (j=0; j<MultiskinCount; j++)
        in.Skip (Header.SkinWidth * Header.SkinHeight);
    } else {
      Scene->DecRef ();
      Object->DecRef ();
      return 0;
    }
  }

  /*
   * read texel and seam information. After this is done, the following
   * information is available:
   *   Texels [0..VertexCount]: Base texels
   *   Texels [VertexCount..VertexCount+n] Seam texels (using n s-texels)
   *   SeamTexels [0..VertexCount]: Maps vertex indices to seam texel
   *     indices in the (VertexCount..VertexCount+n) range, or -1 for
   *     seamless vertices.
   */
  csVector2Array Texels;
  Texels.SetLength (Header.VertexCount);
  int *SeamTexels = new int [Header.VertexCount];

  for (i=0; i<Header.VertexCount; i++)
  {
    in.Read (Readbuffer, SIZEOF_MDLLONG * 3);
    int IsSeamVertex = get_le_long (Readbuffer);
    Texels [i].Set (
      get_le_long(Readbuffer + SIZEOF_MDLLONG) / (float)Header.SkinWidth,
      get_le_long(Readbuffer + 2*SIZEOF_MDLLONG) / (float)Header.SkinHeight);
    if (IsSeamVertex) {
      csVector2 SeamTexel = Texels [i] + csVector2 (0.5, 0);
      SeamTexels [i] = Texels.Push (SeamTexel);
    } else SeamTexels [i] = -1;
  }

  // read triangle data
  for (i=0; i<Header.TriangleCount; i++)
  {
    iModelDataPolygon *Polygon = new csModelDataPolygon ();
    Object->QueryObject ()->ObjAdd (Polygon->QueryObject ());

    in.Read (Readbuffer, SIZEOF_MDLLONG * 4);
    int FrontPolygon = get_le_long (Readbuffer);

    for (j=2; j>=0; j--) {
      int VertexIndex = get_le_long (Readbuffer + (j+1) * SIZEOF_MDLLONG);
      if ((SeamTexels [VertexIndex] != -1) && !FrontPolygon) {
        Polygon->AddVertex (VertexIndex, 0, 0, SeamTexels [VertexIndex]);
      } else {
        Polygon->AddVertex (VertexIndex, 0, 0, VertexIndex);
      }
    }
    Polygon->DecRef ();
  }

  // read action information
  csVector3 *Vertices = new csVector3 [Header.VertexCount];
  iModelDataVertices *DefaultVertices = 0;

  for (i=0; i<Header.ActionCount; i++)
  {
    int FrameCount;
    in.Read (Readbuffer, SIZEOF_MDLLONG);
    int ActionType = get_le_long (Readbuffer);
    if (ActionType == 0)
      FrameCount = 1;
    else
    {
      in.Read (Readbuffer, SIZEOF_MDLLONG);
      FrameCount = get_le_long (Readbuffer);

      // skip some stuff--general min/max, frame timings
      in.Skip (SIZEOF_MDLLONG * (2 + FrameCount));
    }

    iModelDataAction *Action = new csModelDataAction ();
    char buf [20];
    sprintf (buf, "action%d", i);
    Action->QueryObject ()->SetName (buf);
    Object->QueryObject ()->ObjAdd (Action->QueryObject ());

    for (j=0; j<FrameCount; j++)
    {
      iModelDataVertices *Vertices = new csModelDataVertices ();
      if (DefaultVertices) DefaultVertices = Vertices;
      // @@@ use correct timing
      Action->AddFrame (100*j, Vertices->QueryObject ());

      Vertices->AddNormal (csVector3 (1, 0, 0));
      Vertices->AddColor (csColor (1, 1, 1));

      // skip min/max info
      in.Skip (SIZEOF_MDLLONG * 2);

      // get frame name
      in.Read (buf, 16);
      buf [16] = 0;
      Vertices->QueryObject ()->SetName (buf);

      // read vertex positions
      in.Read (Readbuffer, 4 * Header.VertexCount);

      // store vertex positions
      for (k=0; k<Header.VertexCount; k++) {
        csVector3 ver (
	  Readbuffer [4*k + 0] * Header.Scale.x + Header.Origin.x,
	  Readbuffer [4*k + 1] * Header.Scale.y + Header.Origin.y,
	  Readbuffer [4*k + 2] * Header.Scale.z + Header.Origin.z);
        Vertices->AddVertex (ver);
      }

      // store texel positions
      for (k=0; k<Texels.Length (); k++)
        Vertices->AddTexel (Texels [k]);

      Vertices->DecRef ();
    }

    Action->DecRef ();
  }
  delete[] Vertices;

  delete[] SeamTexels;
  Object->SetDefaultVertices (DefaultVertices);
  Object->DecRef ();
  return csPtr<iModelData> (Scene);
}

csPtr<iDataBuffer> csModelConverterMDL::Save (iModelData *, const char *Format)
{
  if (strcasecmp (Format, "mdl"))
    return 0;

  return 0;
}
