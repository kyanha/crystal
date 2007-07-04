/*
  Copyright (C) 2005-2006 by Marten Svanfeldt

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

#ifndef __SCENE_H__
#define __SCENE_H__

#include "object.h"
#include "kdtree.h"
#include "light.h"

namespace lighter
{
  class KDTree;
  class Scene;
  class Sector;

  class Portal : public csRefCount
  {
  public:
    Portal ()
    {}

    Sector* sourceSector;
    Sector* destSector;
    csReversibleTransform wrapTransform;
    Vector3DArray worldVertices;
    csPlane3 portalPlane;
  };
  typedef csRefArray<Portal> PortalRefArray;

  // Representation of sector in our local setup
  class Sector : public csRefCount
  {
  public:
    Sector (Scene* scene)
      : kdTree (0), scene (scene)
    {}

    ~Sector ()
    {
      delete kdTree;
    }

    // Initialize any extra data in the sector
    void Initialize (Statistics::Progress& progress);

    void PrepareLighting (Statistics::Progress& progress);

    // Build kd tree for Sector
    void BuildKDTree (Statistics::Progress& progress);

    // All objects in sector
    ObjectHash allObjects;

    // All light sources (no PD lights)
    LightRefArray allNonPDLights;

    // All PD light sources
    LightRefArray allPDLights;

    // All portals in sector
    PortalRefArray allPortals;

    // KD-tree of all primitives in sector
    KDTree *kdTree;

    // Sector-name
    csString sectorName;

    Scene* scene;
  };
  typedef csHash<csRef<Sector>, csString> SectorHash;

  class Scene
  {
  public:
    Scene ();
    ~Scene ();

    // Add a file for later loading
    void AddFile (const char* directory);

    // Load all files, and parse the loaded info
    bool LoadFiles (Statistics::Progress& progress);

    /* Save all files we've loaded. Will save any changed factory. */
    bool SaveWorldFactories (Statistics::Progress& progress);
    /* Save all files we've loaded. Will save any changed mesh. */
    bool SaveWorldMeshes (Statistics::Progress& progress);
    /* Save all files we've loaded. Writes out document to a temporary file. */
    bool FinishWorldSaving (Statistics::Progress& progress);

    // Copy the temporary file created in SaveWorld() over the actual world file.
    bool ApplyWorldChanges (Statistics::Progress& progress);
    // Write the generated lightmaps out.
    bool SaveLightmaps (Statistics::Progress& progress);
    // Save any mesh data that can only be saved after lighting.
    bool SaveMeshesPostLighting (Statistics::Progress& progress);

    // Data access
    inline ObjectFactoryHash& GetFactories () 
    { return radFactories; }

    inline const ObjectFactoryHash& GetFactories () const
    { return radFactories; }

    inline SectorHash& GetSectors () { return sectors; }

    inline const SectorHash& GetSectors () const { return sectors; }

    const LightmapPtrDelArray& GetLightmaps () const 
    { return lightmaps; }

    LightmapPtrDelArray& GetLightmaps () 
    { return lightmaps; }

    Lightmap* GetLightmap (uint lightmapID, Light* light);

    csArray<LightmapPtrDelArray*> GetAllLightmaps ();

    /**
     * Helper class to perform some lightmap postprocessing
     * (exposure + ambient term).
     */
    class LightingPostProcessor
    {
    private:
      friend class Scene;
      Scene* scene;

      LightingPostProcessor (Scene* scene);
    public:
      //@{
      /// Apply exposure function
      void ApplyExposure (Lightmap* lightmap);
      void ApplyExposure (csColor* colors, size_t numColors);
      //@}

      //@{
      /**
       * Apply ambient term.
       * Ambient may be a hack to approximate indirect lighting, but then, 
       * as long as that is not supported, or disabled by the user later on, 
       * ambient can still serve a purpose.
       */
      void ApplyAmbient (Lightmap* lightmap);
      void ApplyAmbient (csColor* colors, size_t numColors);
      //@}
    };
    LightingPostProcessor lightmapPostProc;
  protected:
    
    //  factories
    ObjectFactoryHash radFactories;
 
    // All sectors
    SectorHash sectors;
    typedef csHash<Sector*, csPtrKey<iSector> > SectorOrigSectorHash;
    SectorOrigSectorHash originalSectorHash;

    LightmapPtrDelArray lightmaps;
    typedef csHash<LightmapPtrDelArray*, csPtrKey<Light> > PDLightmapsHash;
    PDLightmapsHash pdLightmaps;

    struct LoadedFile
    {
      csRef<iDocumentNode> rootNode;
      csRef<iDocument> document;
      csString directory; //VFS name, full path
      csSet<csString> texturesToClean;
      csSet<csString> texFileNamesToDelete;
      csArray<Object*> fileObjects;
    };

    // All files loaded into scene
    csArray<LoadedFile> sceneFiles;

    // Save functions
    void CollectDeleteTextures (iDocumentNode* textureNode,
                                csSet<csString>& filesToDelete);
    void BuildLightmapTextureList (csStringArray& texturesToSave);
    void CleanOldLightmaps (LoadedFile* fileInfo);
    void SaveSceneFactoriesToDom (iDocumentNode* root, LoadedFile* fileInfo,
                                  Statistics::Progress& progress);
    void SaveSceneMeshesToDom (iDocumentNode* root, LoadedFile* fileInfo,
                               Statistics::Progress& progress);
    bool SaveSceneLibrary (csSet<csString>& savedFactories, 
                           const char* libFile, LoadedFile* fileInfo,
                           Statistics::Progress& progress);
    void HandleLibraryNode (csSet<csString>& savedFactories, 
                            iDocumentNode* node, LoadedFile* fileInfo,
                            Statistics::Progress& progress);
    void SaveMeshFactoryToDom (csSet<csString>& savedObjects, 
                               iDocumentNode* factNode, LoadedFile* fileInfo);
    void SaveSectorToDom (iDocumentNode* sectorNode, LoadedFile* fileInfo,
                          Statistics::Progress& progress);
    void SaveMeshObjectToDom (csSet<csString>& savedObjects, iDocumentNode *objNode, 
                              Sector* sect, LoadedFile* fileInfo);

    csStringHash solidColorFiles;
    const char* GetSolidColorFile (const csColor& col);
    void SaveLightmapsToDom (iDocumentNode* root, LoadedFile* fileInfo,
                             Statistics::Progress& progress);
    
    // Load functions
    bool ParseEngine (LoadedFile* fileInfo, Statistics::Progress& progress);
    void ParseSector (LoadedFile* fileInfo, iSector *sector, 
      Statistics::Progress& progress);
    void ParsePortals (iSector *srcSect, Sector* sector);
    enum MeshParseResult
    {
      Failure, Success, NotAGenMesh
    };
    MeshParseResult ParseMesh (LoadedFile* fileInfo, Sector *sector,  
      iMeshWrapper *mesh, csRef<Object>& obj);
    MeshParseResult ParseMeshFactory (iMeshFactoryWrapper *factory, 
      csRef<ObjectFactory>& radFact);
    void PropagateLight (Light* light, const csFrustum& lightFrustum);

  };
}

#endif
