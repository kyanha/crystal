/*
    Copyright (C) 2003 by Jorrit Tyberghein, John Harger, Daniel Duhprey

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

#include "iutil/objreg.h"
#include "iutil/plugin.h"

#include "iengine/camera.h"
#include "iengine/movable.h"
#include "iengine/rview.h"

#include "imesh/particles.h"

#include "ivideo/material.h"
#include "ivideo/rndbuf.h"
#include "ivideo/texture.h"
#include "ivideo/txtmgr.h"

#include "ivaria/reporter.h"

#include "csutil/randomgen.h"
#include "csutil/util.h"

#include "qsqrt.h"
#include "qint.h"

#include "particles.h"
#include <limits.h>

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_IBASE (csParticlesType)
  SCF_IMPLEMENTS_INTERFACE (iMeshObjectType)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csParticlesType::eiComponent)
  SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_FACTORY (csParticlesType)

csParticlesType::csParticlesType (iBase* p) : parent(p)
{
  SCF_CONSTRUCT_IBASE (parent)
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiComponent)
}

csParticlesType::~csParticlesType ()
{
  SCF_DESTRUCT_EMBEDDED_IBASE (scfiComponent);
  SCF_DESTRUCT_IBASE ();
}

csPtr<iMeshObjectFactory> csParticlesType::NewFactory ()
{
  return csPtr<iMeshObjectFactory>
	(new csParticlesFactory (this, object_reg));
}

SCF_IMPLEMENT_IBASE (csParticlesFactory)
  SCF_IMPLEMENTS_INTERFACE (iMeshObjectFactory)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iParticlesFactoryState)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csParticlesFactory::eiParticlesFactoryState)
  SCF_IMPLEMENTS_INTERFACE (iParticlesFactoryState)
  SCF_IMPLEMENTS_INTERFACE (iParticlesStateBase)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csParticlesFactory::csParticlesFactory (csParticlesType* p,
	iObjectRegistry* objreg) : parent (p), object_reg (objreg)
{
  SCF_CONSTRUCT_IBASE (p)
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiParticlesFactoryState)

  g3d = CS_QUERY_REGISTRY (object_reg, iGraphics3D);
  shmgr = CS_QUERY_REGISTRY (object_reg, iShaderManager);

  emit_type = CS_PART_EMIT_SPHERE;
  emit_size_1 = 0.0015f;
  emit_size_2 = 0.001f;
  emit_size_3 = 0.0f;

  force_type = CS_PART_FORCE_RADIAL;

  force_direction = csVector3(0.0f, 0.0f, 0.0f);
  force_range = 1.0f;
  force_falloff = CS_PART_FALLOFF_LINEAR;
  force_cone_radius = 0.0f;
  force_cone_radius_falloff = CS_PART_FALLOFF_CONSTANT;

  force_amount = 1.0f;

  particles_per_second = 100;
  initial_particles = 100;

  gravity = csVector3(0.0f, 0.0f, 0.0f);

  emit_time = 1.0f;
  time_to_live = 1.0f;
  time_variation = 0.0f;

  diffusion = 0.0f;

  particle_radius = 0.05f;
  particle_mass = 1.0f;
  mass_variation = 0.0f;
  dampener = 0.01f;

  autostart = true;

  transform_mode = false;

  physics_plugin = "crystalspace.particles.physics.simple";

  color_method = CS_PART_COLOR_CONSTANT;
  constant_color = csColor(1,1,1);
}

csParticlesFactory::~csParticlesFactory ()
{
}

csPtr<iMeshObject> csParticlesFactory::NewInstance ()
{
  return csPtr<iMeshObject>(new csParticlesObject (this));
}

SCF_IMPLEMENT_IBASE (csParticlesObject)
  SCF_IMPLEMENTS_INTERFACE (iMeshObject)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iParticlesObjectState)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iObjectModel)
  SCF_IMPLEMENTS_EMBEDDED_INTERFACE (iShaderVariableAccessor)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csParticlesObject::eiParticlesObjectState)
  SCF_IMPLEMENTS_INTERFACE (iParticlesObjectState)
  SCF_IMPLEMENTS_INTERFACE (iParticlesStateBase)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csParticlesObject::eiObjectModel)
  SCF_IMPLEMENTS_INTERFACE (iObjectModel)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csParticlesObject::eiShaderVariableAccessor)
  SCF_IMPLEMENTS_INTERFACE (iShaderVariableAccessor)
SCF_IMPLEMENT_EMBEDDED_IBASE_END

csParticlesObject::csParticlesObject (csParticlesFactory* p)
  : logparent (p), pFactory (p)
{
  SCF_CONSTRUCT_IBASE (p)
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiParticlesObjectState)
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiObjectModel)
  SCF_CONSTRUCT_EMBEDDED_IBASE (scfiShaderVariableAccessor)

  mesh = 0;
  meshpp = 0;
  meshppsize = 0;

  csRef<iStringSet> strings =
    CS_QUERY_REGISTRY_TAG_INTERFACE (p->object_reg,
	  "crystalspace.shared.stringset", iStringSet);

  vertex_name = strings->Request ("vertices");
  color_name = strings->Request ("colors");
  texcoord_name = strings->Request ("texture coordinates");
  index_name = strings->Request ("indices");
  radius_name = strings->Request ("point radius");
  scale_name = strings->Request ("point scale");

  matwrap = p->material;

  camera_fov = -1;

  emit_type = p->emit_type;
  emit_size_1 = p->emit_size_1;
  emit_size_2 = p->emit_size_2;
  emit_size_3 = p->emit_size_3;

  force_type = p->force_type;

  force_direction = p->force_direction;
  force_range = p->force_range;
  force_falloff = p->force_falloff;
  force_cone_radius = p->force_cone_radius;
  force_cone_radius_falloff = p->force_cone_radius_falloff;

  force_amount = p->force_amount;
  particle_mass = p->particle_mass;
  mass_variation = p->mass_variation;
  dampener = p->dampener;

  particles_per_second = p->particles_per_second;
  initial_particles = p->initial_particles;

  gravity = p->gravity;

  emit_time = p->emit_time;
  time_to_live = p->time_to_live;
  time_variation = p->time_variation;

  diffusion = p->diffusion;

  autostart = p->autostart;

  particle_radius = p->particle_radius;

  gradient_colors = p->gradient_colors;

  color_method = p->color_method;
  constant_color = p->constant_color;
  base_heat = p->base_heat;
  loop_time = p->loop_time;
  color_callback = p->color_callback;

  transform_mode = p->transform_mode;

  point_data = NULL; // This gets created by the physics plugin

  emitter = csVector3(0.0f, 0.0f, 0.0f);
  radius = 1.0f;
  //dead_particles = 0;
  point_sprites = p->g3d->GetCaps ()->SupportsPointSprites;

  running = false;

  svcontext = new csShaderVariableContext ();

  csShaderVariable* sv;
  sv = svcontext->GetVariableAdd (vertex_name);
  sv->SetAccessor (&scfiShaderVariableAccessor);
  sv = svcontext->GetVariableAdd (color_name);
  sv->SetAccessor (&scfiShaderVariableAccessor);
  sv = svcontext->GetVariableAdd (texcoord_name);
  sv->SetAccessor (&scfiShaderVariableAccessor);
  sv = svcontext->GetVariableAdd (index_name);
  sv->SetAccessor (&scfiShaderVariableAccessor);
  sv = svcontext->GetVariableAdd (radius_name);
  sv->SetValue (particle_radius);
  sv = svcontext->GetVariableAdd (scale_name);
  sv->SetValue (1.0f);

  LoadPhysicsPlugin (p->physics_plugin);

  if(autostart) Start();
}

csParticlesObject::~csParticlesObject ()
{
  SCF_DESTRUCT_EMBEDDED_IBASE (scfiShaderVariableAccessor)
  SCF_DESTRUCT_EMBEDDED_IBASE (scfiObjectModel)
  SCF_DESTRUCT_EMBEDDED_IBASE (scfiParticlesObjectState)
  SCF_DESTRUCT_IBASE ()
  
  if (meshpp)
    delete [] meshpp;
  if (mesh)
    delete mesh;
}

bool csParticlesObject::LoadPhysicsPlugin (const char *plugin_id)
{
  csRef<iPluginManager> plugin_mgr = CS_QUERY_REGISTRY (
    pFactory->object_reg, iPluginManager);

  if(physics)
  {
    physics->RemoveParticles (&scfiParticlesObjectState);
  }

  physics = CS_QUERY_PLUGIN_CLASS (plugin_mgr, plugin_id, iParticlesPhysics);
  if (!physics)
  {
    physics = CS_LOAD_PLUGIN (plugin_mgr, plugin_id, iParticlesPhysics);
  }
  if (!physics)
  {
    csReport (pFactory->object_reg, CS_REPORTER_SEVERITY_ERROR,
      "crystalspace.mesh.object.particles",
      "Could not load the particles physics plugin '%s'!", plugin_id);
    return false;
  }
  point_data = physics->RegisterParticles (&scfiParticlesObjectState);
  return true;
}

void csParticlesObject::SetParticleRadius (float rad)
{
  particle_radius = rad;
  if (svcontext)
  {
    csShaderVariable* sv = svcontext->GetVariableAdd (radius_name);
    if (sv) sv->SetValue (particle_radius);
  }
}

bool csParticlesObject::DrawTest (iRenderView* rview, iMovable* movable)
{
  iCamera* cam = rview->GetCamera ();

  tr_o2c = cam->GetTransform ();
  if(!transform_mode)
  {
    emitter = movable->GetFullPosition();
    csReversibleTransform trans;
    trans.Identity();
    trans.SetOrigin (emitter);
    tr_o2c /= trans;
  }
  else
  {
    tr_o2c /= movable->GetFullTransform ();
  }
  rotation_matrix = movable->GetTransform ().GetT2O ();

  int vertnum = 0;
  float new_radius = 0.0f;

  for(int i=0;i<point_data->Length ();i++)
  {
    const csParticlesData &point = point_data->Get(i);
    if(point.time_to_live < 0.0f) break;

    vertnum ++;

    // For calculating radius
    csVector3 dist_vect = point.position - emitter;
    if (dist_vect.SquaredNorm() > new_radius)
    {
      new_radius = dist_vect.SquaredNorm();
    }
  }
  //dead_particles = point_data->Length () - vertnum;

  if(vertnum>0) 
  {
    radius = qsqrt(new_radius);
    running = true;
  }
  else
  {
    radius = 0.0f;
    running = false;
  }

  int clip_portal, clip_plane, clip_z_plane;
  csSphere s(csVector3(0,0,0), radius);
  if (!rview->ClipBSphere (tr_o2c, s, clip_portal, clip_plane, clip_z_plane))
    return false;

  if (!point_sprites)
  {
    vertnum *= 6;
    csMatrix3 m = tr_o2c.GetT2O();
    corners[0] = m * csVector3(-particle_radius, particle_radius, 0.0f);
    corners[1] = m * csVector3(particle_radius, particle_radius, 0.0f);
    corners[2] = m * csVector3(particle_radius, -particle_radius, 0.0f);
    corners[3] = m * csVector3(-particle_radius, -particle_radius, 0.0f);
  }
  else
  {
    int fov = QInt (cam->GetFOVAngle ());
    int fov_pixels = cam->GetFOV ();
    if (camera_fov != fov || camera_pixels != fov_pixels)
    {
      camera_fov = fov;
      camera_pixels = fov_pixels;
      float lambda = (float)(fov_pixels / (2.0 * tan (fov / 360.0 * PI)));
      csShaderVariable* sv = svcontext->GetVariable (scale_name);
      sv->SetValue (1.0f / (lambda * particle_radius * 3));
    }
  }

  if (!mesh)
    mesh = new csRenderMesh;

  mesh->z_buf_mode = CS_ZBUF_USE;
  mesh->mixmode = CS_FX_ALPHA;
  mesh->clip_plane = clip_plane;
  mesh->clip_portal = clip_portal;
  mesh->clip_z_plane = clip_z_plane;
  mesh->do_mirror = rview->GetCamera()->IsMirrored();
  matwrap->Visit ();
  mesh->material = matwrap;
  mesh->object2camera = tr_o2c;
  mesh->indexstart = 0;
  mesh->indexend = vertnum;
  mesh->variablecontext = 
    (iShaderVariableContext*)svcontext; // Cast for gcc 2.95.x.
  if (point_sprites)
    mesh->meshtype = CS_MESHTYPE_POINT_SPRITES;
  else
    mesh->meshtype = CS_MESHTYPE_TRIANGLES;

  return true;
}

void csParticlesObject::PreGetShaderVariableValue (csShaderVariable* var)
{
  var->SetValue(GetRenderBuffer(var->Name));
}

iRenderBuffer *csParticlesObject::GetRenderBuffer (csStringID name)
{
  if (!vertex_buffer || buffer_length != point_data->Length ())
  {
    buffer_length = point_data->Length ();
    csArray<iRenderBuffer*> buffers;
    int bufsize = (point_sprites ? buffer_length : buffer_length * 4);
    pFactory->g3d->CreateInterleavedRenderBuffers (bufsize
      * sizeof(csParticlesData), CS_BUF_DYNAMIC, 2, buffers);
    vertex_buffer = buffers.Get(0);
    vertex_buffer->SetOffset (0);
    vertex_buffer->SetComponentType (CS_BUFCOMP_FLOAT);
    vertex_buffer->SetComponentCount (3);
    color_buffer = buffers.Get (1);
    color_buffer->SetOffset (sizeof(csVector3));
    color_buffer->SetComponentType (CS_BUFCOMP_FLOAT);
    color_buffer->SetComponentCount (4);
    texcoord_buffer = pFactory->g3d->CreateRenderBuffer (
      sizeof (csVector2) * bufsize, CS_BUF_DYNAMIC,
      CS_BUFCOMP_FLOAT, 2, false);
    csVector2 *texcoords = new csVector2 [bufsize];
    if (point_sprites)
    {
      vertex_buffer->SetStride (sizeof(i_vertex));
      color_buffer->SetStride (sizeof(i_vertex));

      unsigned int *indices = new unsigned int[buffer_length];
      for (int i = 0; i < buffer_length; i++)
      {
        indices[i] = i;
      }
      index_buffer = pFactory->g3d->CreateRenderBuffer (
        sizeof (unsigned int) * buffer_length, CS_BUF_STATIC,
        CS_BUFCOMP_UNSIGNED_INT, 1, true);
      index_buffer->CopyToBuffer(indices, sizeof(unsigned int)*buffer_length);
      delete [] indices;
    }
    else
    {
      vertex_buffer->SetStride (sizeof(i_vertex));
      color_buffer->SetStride (sizeof(i_vertex));
      int i;
      for (i = 0; i < bufsize - 4; i += 4)
      {
        texcoords[i].x = 0;
        texcoords[i].y = 0;
        texcoords[i+1].x = 0;
        texcoords[i+1].y = 1;
        texcoords[i+2].x = 1;
        texcoords[i+2].y = 1;
        texcoords[i+3].x = 1;
        texcoords[i+3].y = 0;
      }
      unsigned int *indices = new unsigned int[buffer_length * 6];
      int j;
      for (i = 0, j = 0; i < bufsize-4; i += 4, j += 6)
      {
        // First triangle
        indices[j] = i;
        indices[j+1] = i + 1;
        indices[j+2] = i + 2;
        // Second triangle
        indices[j+3] = i;
        indices[j+4] = i + 2;
        indices[j+5] = i + 3;
      }
      index_buffer = pFactory->g3d->CreateRenderBuffer (
        sizeof (unsigned int) * buffer_length * 6, CS_BUF_STATIC,
        CS_BUFCOMP_UNSIGNED_INT, 1, true);
      index_buffer->CopyToBuffer(
        indices, sizeof(unsigned int) * buffer_length * 6);
      delete [] indices;
    }
    texcoord_buffer->CopyToBuffer (texcoords, sizeof(csVector2) * bufsize);
    delete [] texcoords;
  }
  if (name == vertex_name)
  {
    void *data;
    int size;
    if (point_sprites)
    {
      int len = point_data->Length ();
      vertex_data.SetLength (len);
      for (int i = 0; i < len - 1; i++)
      {
        const csParticlesData &point = point_data->Get(i);
        i_vertex vertex;
        vertex.position = point.position - emitter;
        vertex.color = point.color;
        vertex_data.Put (i, vertex);
      }

      data = vertex_data.GetArray ();
      size = vertex_data.Length () * sizeof(i_vertex);
    }
    else
    {
      int len = point_data->Length ();
      vertex_data.SetLength (len * 4);
      int i,j;
      for (i = 0, j = 0; i < len - 1; i++, j += 4)
      {
        const csParticlesData &point = point_data->Get(i);
        i_vertex vertex;
        vertex.position = (point.position - emitter) + corners[0];
        vertex.color = point.color;
        vertex_data.Put (j, vertex);
        vertex.position = (point.position - emitter) + corners[1];
        vertex.color = point.color;
        vertex_data.Put (j+1, vertex);
        vertex.position = (point.position - emitter) + corners[2];
        vertex.color = point.color;
        vertex_data.Put (j+2, vertex);
        vertex.position = (point.position - emitter) + corners[3];
        vertex.color = point.color;
        vertex_data.Put (j+3, vertex);
      }

      data = vertex_data.GetArray ();
      size = vertex_data.Length () * sizeof(i_vertex);
    }
    // Vertex buffer is an interleaved buffer, so copy all the data into it
    vertex_buffer->CopyToBuffer (data, size);
    return vertex_buffer;
  }
  else if (name == color_name)
  {
    return color_buffer;
  }
  else if (name == texcoord_name)
  {
    return texcoord_buffer;
  }
  else if (name == index_name)
  {
    return index_buffer;
  }
  return 0;
}

csRenderMesh** csParticlesObject::GetRenderMeshes (int &n)
{
  n = 1;
  if (!meshpp)
  {
    meshpp = new csRenderMesh*[1];
    meshppsize = 1;
  }

  meshpp[0] = mesh;

  return meshpp;
}

bool csParticlesObject::HitBeamOutline (const csVector3& start,
	const csVector3& end, csVector3& isect, float* pr)
{
  return false;
}

bool csParticlesObject::HitBeamObject (const csVector3& start,
	const csVector3& end, csVector3& isect, float* pr,
	int* polygon_idx)
{
  if (polygon_idx) *polygon_idx = -1;
  return false;
}

void csParticlesObject::GetObjectBoundingBox (csBox3& bbox, int type)
{
  bbox.StartBoundingBox ();
  switch (type)
  {
  case CS_BBOX_NORMAL:
    bbox.AddBoundingVertex (-radius, -radius, -radius);
    bbox.AddBoundingVertex (radius, radius, radius);
    break;
  case CS_BBOX_ACCURATE:
    // TODO: Vertices, not just points (and radius)
    //for (int i = 0; i < max_particles; i++)
    //  bbox.AddBoundingVertex (positions[i]);
    bbox.AddBoundingVertex (-radius, -radius, -radius);
    bbox.AddBoundingVertex (radius, radius, radius);
    break;
  case CS_BBOX_MAX:
    // TODO: Come up with a better estimated maximum
    bbox.AddBoundingVertex (-100000.0f, -100000.0f, -100000.0f);
    bbox.AddBoundingVertex (100000.0f, 100000.0f, 100000.0f);
    break;
  }
}

void csParticlesObject::GetRadius(csVector3 &rad, csVector3 &c)
{
  rad = csVector3(radius,radius,radius);
  c = emitter;
}

void csParticlesObject::Start ()
{
  if(point_data->Length () < 1) {
    buffer_length = 0;
  }
  physics->Start (&scfiParticlesObjectState);
  running = true;
}

void csParticlesObject::Stop ()
{
  physics->Stop (&scfiParticlesObjectState);
}

void csParticlesObject::NextFrame (csTicks, const csVector3 &)
{
}
