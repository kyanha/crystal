/*
    Copyright (C) 2006 by Kapoulkine Arseny

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

#ifndef __CS_TERRAIN_LOADER_H
#define __CS_TERRAIN_LOADER_H

#include "imap/reader.h"
#include "imap/writer.h"
#include "iutil/comp.h"
#include "csutil/strhash.h"
#include "csutil/csstring.h"

struct iObjectRegistry;
struct iSyntaxService;
struct iVFS;

CS_PLUGIN_NAMESPACE_BEGIN(Terrain2Loader)
{

/**
 *
 */
class csTerrain2FactoryLoader :
  public scfImplementation2<csTerrain2FactoryLoader,
                            iLoaderPlugin,
                            iComponent>
{
public:
  /// Constructor
  csTerrain2FactoryLoader (iBase*);
  /// Destructor
  virtual ~csTerrain2FactoryLoader ();

  /// Setup the plugin with the system driver
  bool Initialize (iObjectRegistry *objreg);

  /// Parse the given node block and build the terrain factory
  csPtr<iBase> Parse (iDocumentNode *node,
    iStreamSource*, iLoaderContext *ldr_context,
    iBase* context);	
private:
  struct ParamPair
  {
    csString name, value;
  };

  typedef csArray<ParamPair> ParamPairArray;

  struct DefaultCellValues
  {
    DefaultCellValues ()
      : size (128.0f, 32.0f, 128.0f), gridWidth (128), gridHeight (128),
      materialmapWidth (128), materialmapHeight (128), materialmapPersist (false)
    {}

    ParamPairArray renderParams, collParams, feederParams;
    csRefArray<csShaderVariable> svs;
    csVector3 size;
    unsigned int gridWidth, gridHeight, materialmapWidth, materialmapHeight;
    bool materialmapPersist;
    csRef<iMaterialWrapper> baseMaterial;
  };

  iObjectRegistry* object_reg;
  csRef<iSyntaxService> synldr;
  csRef<iReporter> reporter;

  csStringHash xmltokens;
#define CS_TOKEN_ITEM_FILE "plugins/mesh/terrain/persist/terrain2/loader.tok"
#include "cstool/tokenlist.h"
#undef CS_TOKEN_ITEM_FILE 

  bool ParseCell (iDocumentNode* node, iLoaderContext* ldr_ctx,
    iTerrainFactory* fact, const DefaultCellValues& defaults);

  bool ParseDefaultCell (iDocumentNode* node, iLoaderContext* ldr_ctx,
    DefaultCellValues& defaults);

  bool ParseParams (csArray<ParamPair>& pairs, iDocumentNode* node);

  bool ParseRenderParams (csArray<ParamPair>& pairs, csRefArray<csShaderVariable>& svs,
    iLoaderContext* ldr_context, iDocumentNode* node);
};

/**
 *
 */
class csTerrain2ObjectLoader :
  public scfImplementation2<csTerrain2ObjectLoader,
                            iLoaderPlugin,
                            iComponent>
{
private:
  iObjectRegistry* object_reg;
  csRef<iSyntaxService> synldr;
  csRef<iReporter> reporter;

  csStringHash xmltokens;
#define CS_TOKEN_ITEM_FILE "plugins/mesh/terrain/persist/terrain2/loader.tok"
#include "cstool/tokenlist.h"
#undef CS_TOKEN_ITEM_FILE 

public:
  /// Constructor
  csTerrain2ObjectLoader (iBase*);

  /// Destructor
  virtual ~csTerrain2ObjectLoader ();

  /// Register plugin with system driver
  bool Initialize (iObjectRegistry *objreg);

  /// Parse the given block to create a new Terrain object
  csPtr<iBase> Parse (iDocumentNode* node,
    iStreamSource*, iLoaderContext* ldr_context,
    iBase *context);
};

}
CS_PLUGIN_NAMESPACE_END(Terrain2Loader)

#endif // __CS_CHUNKLDR_H
