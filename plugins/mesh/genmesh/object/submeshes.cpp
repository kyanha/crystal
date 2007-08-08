/*
  Copyright (C) 2006 by Jorrit Tyberghein
            (C) 2006 by Frank Richter

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

#include "cstool/rbuflock.h"

#include "submeshes.h"
#include "genmesh.h"

CS_PLUGIN_NAMESPACE_BEGIN(Genmesh)
{
  static int SubmeshSubmeshCompare (SubMesh* const& A, 
                                    SubMesh* const& B)
  {
    const char* a = A->GetName();
    const char* b = B->GetName();
    if (a == 0) return (b == 0) ? 0 : 1;
    if (b == 0) return -1;
    return strcmp (a, b);
  }

  iGeneralMeshSubMesh* SubMeshesContainer::AddSubMesh (
    iRenderBuffer* indices, iMaterialWrapper *material, const char* name, 
    uint mixmode)
  {
    csRef<SubMesh> subMesh;
    subMesh.AttachNew (new SubMesh ());
    subMesh->material = material;
    subMesh->MixMode = mixmode;
    subMesh->index_buffer = indices;
    subMesh->name = name;

    subMeshes.InsertSorted (subMesh, SubmeshSubmeshCompare);
    /* @@@ FIXME: Prolly do some error checking, like sanity of
     * indices */
    changeNum++;
    return subMesh;
  }

  static int SubmeshStringCompare (SubMesh* const& A, const char* const& b)
  {
    const char* a = A->GetName();
    if (a == 0) return (b == 0) ? 0 : 1;
    if (b == 0) return -1;
    return strcmp (a, b);
  }

  SubMesh* SubMeshesContainer::FindSubMesh (const char* name) const
  {
    size_t idx = subMeshes.FindSortedKey (
      csArrayCmp<SubMesh*, const char*> (name, &SubmeshStringCompare));
    if (idx == csArrayItemNotFound) return 0;
    return subMeshes[idx];
  }

  void SubMeshesContainer::DeleteSubMesh (iGeneralMeshSubMesh* mesh)
  {
    SubMesh* subMesh = static_cast<SubMesh*> (mesh);
    size_t idx = subMeshes.FindSortedKey (
      csArrayCmp<SubMesh*, SubMesh*> (subMesh, &SubmeshSubmeshCompare));
    if (idx == csArrayItemNotFound) return;
    subMeshes.DeleteIndex (idx);
  }

  //-------------------------------------------------------------------------

  csRenderBufferHolder* SubMeshProxy::GetBufferHolder()
  {
    if (!bufferHolder.IsValid())
    {
      bufferHolder.AttachNew (new csRenderBufferHolder);
      bufferHolder->SetRenderBuffer(CS_BUFFER_INDEX, GetIndices ());
    }
    return bufferHolder;
  }

  //-------------------------------------------------------------------------

  static int SubmeshProxySubmeshProxyCompare (SubMeshProxy* const& A, 
                                              SubMeshProxy* const& B)
  {
    const char* a = A->GetName();
    const char* b = B->GetName();
    if (a == 0) return (b == 0) ? 0 : 1;
    if (b == 0) return -1;
    return strcmp (a, b);
  }

  void SubMeshProxiesContainer::AddSubMesh (SubMeshProxy* subMesh)
  {
    subMeshes.InsertSorted (subMesh, SubmeshProxySubmeshProxyCompare);
  }

  static int SubmeshProxyStringCompare (SubMeshProxy* const& A, const char* const& b)
  {
    const char* a = A->GetName();
    if (a == 0) return (b == 0) ? 0 : 1;
    if (b == 0) return -1;
    return strcmp (a, b);
  }

  SubMeshProxy* SubMeshProxiesContainer::FindSubMesh (const char* name) const
  {
    size_t idx = subMeshes.FindSortedKey (
      csArrayCmp<SubMeshProxy*, const char*> (name, &SubmeshProxyStringCompare));
    if (idx == csArrayItemNotFound) return 0;
    return subMeshes[idx];
  }

  //-------------------------------------------------------------------------

  void SubMeshesTriMesh::CacheTriangles ()
  {
    if (triChangeNum == subMeshes.GetChangeNum()) return;

    triangleCache.Empty();
    for (size_t s = 0; s < subMeshes.GetSubMeshCount(); s++)
    {
      SubMesh* subMesh = subMeshes.GetSubMesh (s);
      iRenderBuffer* buffer = subMesh->GetIndices();
      size_t offs = triangleCache.GetSize();
      size_t bufferTris = buffer->GetElementCount() / 3;
      triangleCache.SetSize (offs + bufferTris);
      void* tris = buffer->Lock (CS_BUF_LOCK_READ);
      memcpy (triangleCache.GetArray() + offs, tris, 
        bufferTris * sizeof (csTriangle));
      buffer->Release();
    }
    triangleCache.ShrinkBestFit();

    triChangeNum = subMeshes.GetChangeNum();
  }

  size_t SubMeshesTriMesh::GetVertexCount ()
  {
    if (!factory) return 0;
    return factory->GetVertexCount ();
  }
  csVector3* SubMeshesTriMesh::GetVertices ()
  {
    if (!factory) return 0;
    return factory->GetVertices ();
  }
  size_t SubMeshesTriMesh::GetTriangleCount ()
  {
    if (!factory) return 0;
    CacheTriangles ();
    return triangleCache.GetSize ();
  }
  csTriangle* SubMeshesTriMesh::GetTriangles ()
  {
    if (!factory) return 0;
    CacheTriangles ();
    return triangleCache.GetArray ();
  }

}
CS_PLUGIN_NAMESPACE_END(Genmesh)
