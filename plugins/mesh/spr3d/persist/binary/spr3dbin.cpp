/*
    Copyright (C) 2002 by David M. Asbell

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

/**
 *
 * These classes Save and Load Sprites with a binary representation
 *
 * TODO: Add support for skeletons
 * TODO: Add support for SMOOTH once it becomes available to these routines
 *
 */

#include "cssysdef.h"
#include "cssys/sysfunc.h"
#include "cssys/csendian.h"
#include "csgeom/math3d.h"
#include "csgeom/matrix3.h"
#include "csgeom/quaterni.h"
#include "csgeom/transfrm.h"
#include "csutil/parser.h"
#include "csutil/scanstr.h"
#include "imesh/object.h"
#include "iengine/mesh.h"
#include "iengine/engine.h"
#include "iutil/plugin.h"
#include "imesh/sprite3d.h"
#include "imesh/skeleton.h"
#include "ivideo/graph3d.h"
#include "qint.h"
#include "iutil/vfs.h"
#include "csutil/csstring.h"
#include "iutil/object.h"
#include "iengine/material.h"
#include "iengine/motion.h"
#include "ivaria/reporter.h"
#include "iutil/objreg.h"
#include "iutil/eventh.h"
#include "iutil/comp.h"
#include "imap/ldrctxt.h"
#include "spr3dbin.h"

CS_IMPLEMENT_PLUGIN

/**
 * Reports errors
 */
static void ReportError (iReporter* reporter, const char* id,
	const char* description, ...)
{
  va_list arg;
  va_start (arg, description);

  if (reporter)
  {
    reporter->ReportV (CS_REPORTER_SEVERITY_ERROR, id, description, arg);
  }
  else
  {
    char buf[1024];
    vsprintf (buf, description, arg);
    csPrintf ("Error ID: %s\n", id);
    csPrintf ("Description: %s\n", buf);
  }
  va_end (arg);
}

SCF_IMPLEMENT_IBASE (csSprite3DBinFactoryLoader)
  SCF_IMPLEMENTS_INTERFACE (iBinaryLoaderPlugin)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSprite3DBinFactoryLoader::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_IBASE (csSprite3DBinFactorySaver)
  SCF_IMPLEMENTS_INTERFACE (iSaverPlugin)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csSprite3DBinFactorySaver::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_FACTORY (csSprite3DBinFactoryLoader)
SCF_IMPLEMENT_FACTORY (csSprite3DBinFactorySaver)

SCF_EXPORT_CLASS_TABLE (spr3dbin)
  SCF_EXPORT_CLASS (csSprite3DBinFactoryLoader,
  	"crystalspace.mesh.loader.factory.sprite.3d.binary",
	"Crystal Space Sprite3DBin Mesh Factory Loader")
  SCF_EXPORT_CLASS (csSprite3DBinFactorySaver,
  	"crystalspace.mesh.saver.factory.sprite.3d.binary",
	"Crystal Space Sprite3DBin Mesh Factory Saver")
SCF_EXPORT_CLASS_TABLE_END

/**
 * Creates a new csSprite3DBinFactoryLoader
 */
csSprite3DBinFactoryLoader::csSprite3DBinFactoryLoader (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
}

/**
 * Destroys a csSprite3DBinFactoryLoader
 */
csSprite3DBinFactoryLoader::~csSprite3DBinFactoryLoader ()
{
}

/**
 * Initializes a csSprite3DBinFactoryLoader
 */
bool csSprite3DBinFactoryLoader::Initialize (iObjectRegistry* object_reg)
{
  csSprite3DBinFactoryLoader::object_reg = object_reg;
  reporter = CS_QUERY_REGISTRY (object_reg, iReporter);
  synldr = CS_QUERY_REGISTRY (object_reg, iSyntaxService);
  return true;
}


/**
 * Loads a csSprite3DBinFactoryLoader
 */
csPtr<iBase> csSprite3DBinFactoryLoader::Parse (void* data,
				       iLoaderContext* ldr_context,
				       iBase* context)
{
  csRef<iPluginManager> plugin_mgr (CS_QUERY_REGISTRY (object_reg,
  	iPluginManager));
  csRef<iMeshObjectType> type (CS_QUERY_PLUGIN_CLASS(plugin_mgr,
	"crystalspace.mesh.object.sprite.3d", iMeshObjectType));
  if (!type)
  {
    type = CS_LOAD_PLUGIN (plugin_mgr, "crystalspace.mesh.object.sprite.3d",
    	iMeshObjectType);
  }
  if (!type)
  {
    ReportError (reporter,
		"crystalspace.sprite3dbinfactoryloader.setup.objecttype",
		"Could not load the sprite.3d mesh object plugin!");
    return NULL;
  }

  // @@@ Temporary fix to allow to set actions for objects loaded
  // with impexp. Once those loaders move to another plugin this code
  // below should be removed.
  csRef<iMeshObjectFactory> fact;
  if (context)
  {
    fact = SCF_QUERY_INTERFACE (context, iMeshObjectFactory);
    // DecRef of fact will be handled later.
  }

  // If there was no factory we create a new one.
  if (!fact)
    fact = type->NewFactory ();

  csRef<iSprite3DFactoryState> spr3dLook (SCF_QUERY_INTERFACE (fact,
  	iSprite3DFactoryState));

  char* p = (char*)data;

  // Read the magic number so we can ID the file
  char magic[4] = {'5','1', '5','0'};
  if (memcmp(magic, p, 4) != 0)
  {
    ReportError (reporter,
	"crystalspace.sprite3dbinfactoryloader.setup.objecttype",
	"Input was not binary sprite data!");
  }
  p += 4;

  // Read the version number so we can ID the file
  char ver[2] = {0x01,0x00};
  if (memcmp(ver, p, 2) != 0)
  {
    ReportError (reporter,
	"crystalspace.sprite3dbinfactoryloader.setup.objecttype",
	"Input was not the expected version. Expected version 1.0!");
  }
  p += 2;


  // Read and set the (one for now) material
  char mat_name[255];
  strcpy(mat_name, p);
  iMaterialWrapper* mat = ldr_context->FindMaterial (mat_name);
  if (!mat)
  {
    ReportError (reporter,
	"crystalspace.sprite3dbinfactoryloader.parse.unknownmaterial",
	"Couldn't find material named '%s'", mat_name);
    return NULL;
  }
  spr3dLook->SetMaterialWrapper (mat);
  p += strlen(mat_name) + 1;

  // Read the number of frames
  int frame_count = convert_endian(*((int32 *)p)); p += sizeof(int);

  // Read all the frames
  int i;
  for (i=0; i<frame_count; i++)
  {
    iSpriteFrame* fr = spr3dLook->AddFrame ();

    char frame_name[255];
    strcpy(frame_name, p);
    p += strlen(frame_name) + 1;

    fr->SetName (frame_name);

    int anm_idx = fr->GetAnmIndex ();
    int tex_idx = fr->GetTexIndex ();
    float x, y, z, u, v;

    // Read the number of vertecies
    int vertex_count = convert_endian(*((int32 *)p));
    p += sizeof(int);

    int j;
    for (j = 0; j < vertex_count; j++)
    {
      x = convert_endian(long2float(*((long *)p))); p += sizeof(float);
      y = convert_endian(long2float(*((long *)p))); p += sizeof(float);
      z = convert_endian(long2float(*((long *)p))); p += sizeof(float);
      u = convert_endian(long2float(*((long *)p))); p += sizeof(float);
      v = convert_endian(long2float(*((long *)p))); p += sizeof(float);

      // check if it's the first frame
      if (spr3dLook->GetFrameCount () == 1)
      {
	spr3dLook->AddVertices (1);
      }
      else if (i >= spr3dLook->GetVertexCount ())
      {
	ReportError (reporter,
	    "crystalspace.sprite3dbinfactoryloader.parse.frame.vertices",
	    "Trying to add too many vertices to frame '%s'!",
	    fr->GetName ());
	return NULL;
      }
      spr3dLook->SetVertex (anm_idx, j, csVector3 (x, y, z));
      spr3dLook->SetTexel  (tex_idx, j, csVector2 (u, v));
    }

    if (j < spr3dLook->GetVertexCount ())
    {
      ReportError (reporter,
	"crystalspace.sprite3dbinfactoryloader.parse.frame.vertices",
	"Too few vertices in frame '%s'!",
	fr->GetName ());
      return NULL;
    }
  }

  // Read the number of actions
  int action_count = convert_endian(*((int32 *)p)); p += sizeof(int);

  // Write out each action
  for (i=0; i<action_count; i++)
  {
    iSpriteAction* act = spr3dLook->AddAction ();

    char action_name[255];
    strcpy(action_name, p);
    p += strlen(action_name) + 1;

    act->SetName (action_name);

    int as = convert_endian(*((int32 *)p)); p += sizeof(int);

    int j;
    for (j = 0; j < as; j++)
    {
      char fn[64];
      strcpy(fn, p);
      p += strlen(fn) + 1;

      iSpriteFrame* ff = spr3dLook->FindFrame (fn);
      if (!ff)
      {
	ReportError (reporter,
	  "crystalspace.sprite3dbinfactoryloader.parse.action.badframe",
	  "Trying to add unknown frame '%s' to action '%s'!",
	  fn, act->GetName ());
	return NULL;
      }

      // Read the delay
      int delay = convert_endian(*((int32 *)p)); p += sizeof(int);

      act->AddFrame (ff, delay);
    }
  }

  // Read the number of actions
  int tri_count = convert_endian(*((int32 *)p)); p += sizeof(int);

  for(i=0; i<tri_count; i++)
  {
    int a = convert_endian(*((int32 *)p)); p += sizeof(int);
    int b = convert_endian(*((int32 *)p)); p += sizeof(int);
    int c = convert_endian(*((int32 *)p)); p += sizeof(int);

    spr3dLook->AddTriangle (a, b, c);
  }

  // Read the number of sockets
  int socket_count = convert_endian(*((int32 *)p)); p += sizeof(int);

  for(i=0; i<socket_count; i++)
  {
    char name[64];
    strcpy(name, p);
    p += strlen(name) + 1;
 
    int a = convert_endian(*((int32 *)p)); p += sizeof(int);
 
    iSpriteSocket* socket = spr3dLook->AddSocket();
    socket->SetName(name);
    socket->SetTriangleIndex(a);
  }

  /// @@@ Cannot retrieve smoothing information.
  /// SMOOTH()
  /// SMOOTH(baseframenr)
  /// or a list of SMOOTH(basenr, framenr);


  // Read the 1 byte tween value
  spr3dLook->EnableTweening (*p++);

  return csPtr<iBase> (fact);
}

//---------------------------------------------------------------------------

/**
 * Creates a csSprite3DBinFactorySaver
 */
csSprite3DBinFactorySaver::csSprite3DBinFactorySaver (iBase* pParent)
{
  SCF_CONSTRUCT_IBASE (pParent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
  reporter = NULL;
}

/**
 * Destroys a csSprite3DBinFactorySaver
 */
csSprite3DBinFactorySaver::~csSprite3DBinFactorySaver ()
{
}

/**
 * Initializes a csSprite3DBinFactorySaver
 */
bool csSprite3DBinFactorySaver::Initialize (iObjectRegistry* object_reg)
{
  csSprite3DBinFactorySaver::object_reg = object_reg;
  reporter = CS_QUERY_REGISTRY (object_reg, iReporter);
  return true;
}


/**
 * Saves a csSprite3DBinFactorySaver
 */
void csSprite3DBinFactorySaver::WriteDown (iBase* obj, iFile * file)
{
  const char * name = 0;

  csRef<iSprite3DFactoryState> state (
    SCF_QUERY_INTERFACE (obj, iSprite3DFactoryState));

  // Write a magic number so we can ID the file
  char magic[4] = {'5','1', '5','0'};
  file->Write (magic, 4);

  // Write a version
  char ver[2] = {0x01,0x00};
  file->Write (ver, 2);

  // Write out the material... This can easily expanded to multiple
  // materials later.
  name = state->GetMaterialWrapper()->QueryObject ()->GetName();
  file->Write (name, strlen(name) + 1);

  // Write the number of frames
  int frame_count = state->GetFrameCount();
  int fc = convert_endian((int32)frame_count);
  file->Write ((char *)&fc, 4);

  // Write all the frames
  int i,j;
  for(i=0; i<state->GetFrameCount(); i++)
  {
    iSpriteFrame* frame = state->GetFrame(i);

    // Write out the frame name
    name = frame->GetName();
    file->Write (name, strlen(name) + 1);

    // Write the number of verts
    int vertex_count = state->GetVertexCount();
    int vc = convert_endian((int32)vertex_count);
    file->Write ((char *)&vc, 4);

    // Get the animation and textures indicies for the frame
    int anm_idx = frame->GetAnmIndex ();
    int tex_idx = frame->GetTexIndex ();

    // Write out each vertex and texcel coord
    for (j=0; j<vertex_count; j++)
    {
      long v;
      v = convert_endian(float2long(state->GetVertex(anm_idx, j).x));
      file->Write ((char *)&v, 4);
      v = convert_endian(float2long(state->GetVertex(anm_idx, j).y));
      file->Write ((char *)&v, 4);
      v = convert_endian(float2long(state->GetVertex(anm_idx, j).z));
      file->Write ((char *)&v, 4);
      v = convert_endian(float2long(state->GetTexel(tex_idx, j).x));
      file->Write ((char *)&v, 4);
      v = convert_endian(float2long(state->GetTexel(tex_idx, j).y));
      file->Write ((char *)&v, 4);
    }
  }

  // Write out the number of actions
  int action_count = state->GetActionCount();
  int ac = convert_endian((int32)action_count);
  file->Write ((char *)&ac, 4);

  // Write out each action
  for(i=0; i<action_count; i++)
  {
    iSpriteAction* action = state->GetAction(i);

    // Write the name of the action
    name = action->GetName();
    file->Write (name, strlen(name) + 1);

    // Write out the number of triangles
    int action_frame_count = action->GetFrameCount();
    int afc = convert_endian((int32)action_frame_count);
    file->Write ((char *)&afc, 4);

    // Write the frame name, delay tuples
    for (j=0; j<action_frame_count; j++)
    {
      name = action->GetFrame(j)->GetName();
      file->Write (name, strlen(name) + 1);

      int frame_delay = action->GetFrameDelay(j);
      int fd = convert_endian((int32)frame_delay);
      file->Write ((char *)&fd, 4);
    }
  }

  // Write out the number of triangles
  int tri_count = state->GetTriangleCount();
  int tc = convert_endian((int32)tri_count);
  file->Write ((char *)&tc, 4);

  for (i=0; i<tri_count; i++)
  {
    int idx;
    idx = convert_endian((int32)state->GetTriangle(i).a);
    file->Write ((char *)&idx, 4);
    idx = convert_endian((int32)state->GetTriangle(i).b);
    file->Write ((char *)&idx, 4);
    idx = convert_endian((int32)state->GetTriangle(i).c);
    file->Write ((char *)&idx, 4);
  }

  // Write out the number of sockets
  int socket_count = state->GetSocketCount();
  int sc = convert_endian((int32)socket_count);
  file->Write ((char *)&sc, 4);

  for (i=0; i<socket_count; i++)
  {
    name = state->GetSocket(i)->GetName();
    file->Write (name, strlen(name) + 1);

    int idx;
    idx = convert_endian((int32)state->GetSocket(i)->GetTriangleIndex());
    file->Write ((char *)&idx, 4);
  }
  /// @@@ Cannot retrieve smoothing information.
  /// SMOOTH()
  /// SMOOTH(baseframenr)
  /// or a list of SMOOTH(basenr, framenr);

  // Write out TWEEN state
  char buf[1];
  buf[0] = state->IsTweeningEnabled() ? 0x00: 0x01;
  file->Write(buf, 1);
}

