/*
  Copyright (C) 2005 by Marten Svanfeldt
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

#ifndef __CONVERTER_H__
#define __CONVERTER_H__

namespace genmeshify
{
  class App;

  class Converter
  {
    App* app;
    csStringID idTexLightmap;
    csRef<iLoaderContext> context;
    csRef<iRegion> region;

    csRef<iLoaderPlugin> thingFactLoader;
    csRef<iLoaderPlugin> thingObjLoader;
    csRef<iSaverPlugin> gmfactSaver;
    csRef<iSaverPlugin> gmSaver;

    csRandomGen rng;

    struct PolyVertex
    {
      csVector3 pos;
      csVector3 normal;
      csVector2 tc;
      csVector2 tclm;
    };
    struct Poly
    {
      csArray<PolyVertex> vertices;
      int orgIndex;
    };
    struct LMLayout
    {
      struct Dim
      { 
        int w, h; 
        Dim() : w(0), h(0) {}
        void GrowTo (int newW, int newH)
        { 
          w = csMax (csFindNearestPowerOf2 (newW), w); 
          h = csMax (csFindNearestPowerOf2 (newH), h); 
        }
      };
      csArray<Dim> slmDimensions;
      csStringArray slmNames;
      struct Lightmap
      {
        bool hasLM;
        size_t slm;
        csRect rectOnSLM;
      };
      csArray<Lightmap> polyLightmaps;
      struct SubMesh
      {
        csString name;
        size_t slm;
      };
      csArray<SubMesh> subMeshes;
    };

    bool CopyThingToGM (iThingFactoryState* from, iGeneralFactoryState* to,
      const char* name, LMLayout& layout);
    bool ExtractPortals (iMeshWrapper* mesh, iDocumentNode* to);
    bool ExtractLightmaps (const LMLayout& layout, iThingState* object, 
      iDocumentNode* textures);
  public:
    Converter (App* app, iLoaderContext* context, iRegion* region);
  
    bool ConvertMeshFact (iDocumentNode* from, iDocumentNode* to);
    bool ConvertMeshObj (iSector* sector, const char* meshName, 
      iDocumentNode* from, iDocumentNode* to, iDocumentNode* sectorNode,
      iDocumentNode* textures);
  };
}

#endif // __CONVERTER_H__
