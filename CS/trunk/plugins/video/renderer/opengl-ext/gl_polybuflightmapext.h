#ifndef POLYBUFLIGHTMAPEXT_H_INCLUDED
#define POLYBUFLIGHTMAPEXT_H_INCLUDED

#include "csgeom/vector2.h"
#include "csgeom/vector3.h"
#include "csutil/garray.h"
#include "plugins/video/renderer/common/vbufmgr.h"
#include "ivideo/graph3d.h"

struct csPolyBufMatExtPolygon
{
  iPolygonTexture *m_lmhandle;
  int             *m_indices;
  int              m_index_count;
};

struct csRealVertexBuffer
{
  CS_DECLARE_GROWING_ARRAY(m_vertices, csVector3);
  CS_DECLARE_GROWING_ARRAY(m_texcoords, csVector2);
  CS_DECLARE_GROWING_ARRAY(m_lmcoords, csVector2);
};

class csPolyBufMatLightmap
{
public:
  csPolyBufMatLightmap(csRealVertexBuffer &vbuf);
  csPolyBufMatLightmap(iGraphics3D &g3d, csRealVertexBuffer &vbuf);
  csPolyBufMatLightmap(int width, int height, csRealVertexBuffer &vbuf);
  ~csPolyBufMatLightmap();

  bool AddPolygon(csVector3 *origverts,
      int* verts, int num_verts,
      const csPlane3& poly_normal,
      const csMatrix3& m_obj2tex, const csVector3& v_obj2tex,
      iPolygonTexture* poly_texture);

  void MarkLightmapsDirty();


  CS_DECLARE_GROWING_ARRAY(m_polygons, csPolyBufMatExtPolygon);
  csRealVertexBuffer &vbuf;

};



#endif