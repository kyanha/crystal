/*
  Copyright (C) 2003-2006 by Marten Svanfeldt
		2005-2006 by Frank Richter

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public
  License along with this library; if not, write to the Free
  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "cssysdef.h"
#include <ctype.h>

#include "imap/services.h"
#include "iutil/hiercache.h"
#include "iutil/vfs.h"
#include "ivaria/reporter.h"
#include "ivideo/rendermesh.h"

#include "csplugincommon/shader/shadercachehelper.h"
#include "csutil/base64.h"
#include "csutil/csendian.h"
#include "csutil/cspmeter.h"
#include "csutil/databuf.h"
#include "csutil/documenthelper.h"
#include "csutil/parasiticdatabuffer.h"
#include "csutil/scfarray.h"
#include "csutil/xmltiny.h"
#include "cstool/vfsdirchange.h"

#include "shader.h"
#include "xmlshader.h"

CS_PLUGIN_NAMESPACE_BEGIN(XMLShader)
{

  CS_LEAKGUARD_IMPLEMENT (csXMLShader);

  //---------------------------------------------------------------------------

  csShaderConditionResolver::csShaderConditionResolver (
    csConditionEvaluator& evaluator)
    : rootNode (0), nextVariant (0),
    evaluator (evaluator)
  {
    SetEvalParams (0, 0);
  }

  csShaderConditionResolver::~csShaderConditionResolver ()
  {
    delete rootNode;
  }

  const char* csShaderConditionResolver::SetLastError (const char* msg, ...)
  {
    va_list args;
    va_start (args, msg);
    lastError.FormatV (msg, args);
    va_end (args);
    return lastError;
  }

  const char* csShaderConditionResolver::ParseCondition (const char* str, 
    size_t len, 
    csConditionID& result)
  {
    CondOperation op;
    const char* err = ParseCondition (str, len, op);
    if (err) return err;
    result = evaluator.FindOptimizedCondition (op);
    return 0;
  }

  const char* csShaderConditionResolver::ParseCondition (const char* str, 
    size_t len, 
    CondOperation& result)
  {
    csExpressionTokenList tokens;
    const char* err = tokenizer.Tokenize (str, len, tokens);
    if (err)
      return SetLastError ("Tokenization: %s", err);
    csExpression* newExpression;
    err = parser.Parse (tokens, newExpression);
    if (err)
    {
      delete newExpression;
      return SetLastError ("Parsing: %s", err);
    }

    err = evaluator.ProcessExpression (newExpression, result);
    delete newExpression;
    if (err)
      return SetLastError ("Processing: %s", err);

    return 0;
  }

  bool csShaderConditionResolver::Evaluate (csConditionID condition)
  {
    const csRenderMeshModes* modes = csShaderConditionResolver::modes;
    const csShaderVariableStack* stack = csShaderConditionResolver::stack;

    return evaluator.Evaluate (condition, modes ? *modes : csRenderMeshModes(),
      stack);
  }

  csConditionNode* csShaderConditionResolver::NewNode (csConditionNode* parent)
  {
    csConditionNode* newNode = new csConditionNode (parent);
    return newNode;
  }

  csConditionNode* csShaderConditionResolver::GetRoot ()
  {
    if (rootNode == 0)
      rootNode = NewNode (0);
    return rootNode;
  }

  size_t csShaderConditionResolver::GetVariant (csConditionNode* node)
  {
    MyBitArrayTemp bits (evaluator.GetNumConditions ());
    node->FillConditionArray (bits);
    size_t* var = variantIDs.GetElementPointer (bits);
    if (var)
      return *var;
    else
    {
      variantIDs.Put (bits, nextVariant);
      return nextVariant++;
    }
  }

  void csShaderConditionResolver::AddNode (csConditionNode* parent,
    csConditionID condition, 
    csConditionNode*& trueNode,
    csConditionNode*& falseNode,
    const MyBitArrayTemp& conditionResultsTrue,
    const MyBitArrayTemp& conditionResultsFalse)
  {
    if (rootNode == 0)
    {
      // This is the first condition, new node gets root
      CS_ASSERT_MSG ("No root but parent? Weird.", parent == 0);
      parent = GetRoot ();

      parent->condition = condition;

      parent->trueNode = trueNode = NewNode (parent);
      trueNode->variant = GetVariant (trueNode);
      variantConditions.PutUnique (trueNode->variant, conditionResultsTrue);

      parent->falseNode = falseNode = NewNode (parent);
      falseNode->variant = GetVariant (falseNode);
      variantConditions.PutUnique (falseNode->variant, conditionResultsFalse);
    }
    else
    {
      if (parent == 0)
        parent = GetRoot ();

      parent->trueNode = trueNode = NewNode (parent);
      parent->falseNode = falseNode = NewNode (parent);

      CS_ASSERT(parent->variant != csArrayItemNotFound);
      MyBitArrayTemp bits (evaluator.GetNumConditions ());
      parent->condition = condition;

      trueNode->variant = GetVariant (trueNode);
      variantConditions.PutUnique (trueNode->variant, conditionResultsTrue);

      falseNode->variant = parent->variant;
      falseNode->FillConditionArray (bits);
      variantIDs.PutUnique (bits, falseNode->variant);
      variantConditions.PutUnique (falseNode->variant, conditionResultsFalse);

      parent->variant = csArrayItemNotFound;
    }
  }

  void csShaderConditionResolver::FinishAdding ()
  {
    variantIDs.Empty();
  }

  void csShaderConditionResolver::SetEvalParams (const csRenderMeshModes* modes,
    const csShaderVariableStack* stack)
  {
    csShaderConditionResolver::modes = modes;
    csShaderConditionResolver::stack = stack;
  }

  size_t csShaderConditionResolver::GetVariant ()
  {
    const csRenderMeshModes& modes = *csShaderConditionResolver::modes;
    const csShaderVariableStack* stack = csShaderConditionResolver::stack;

    if (rootNode == 0)
    {
      return 0;
    }
    else
    {
      csConditionNode* currentRoot = 0;
      csConditionNode* nextRoot = rootNode;

      while (nextRoot != 0)
      {
        currentRoot = nextRoot;
        if (evaluator.Evaluate (currentRoot->condition, modes, stack))
        {
          nextRoot = currentRoot->trueNode;
        }
        else
        {
          nextRoot = currentRoot->falseNode;
        }
      }
      CS_ASSERT (currentRoot != 0);
      return currentRoot->variant;
    }
  }

  void csShaderConditionResolver::SetVariant (size_t variant)
  {
    if (rootNode == 0) return;

    csBitArray conditionResults (evaluator.GetNumConditions());
    csBitArray conditionSet (evaluator.GetNumConditions());
    MyBitArrayTemp* conditionResultsPtr = variantConditions.GetElementPointer (
      variant);
    CS_ASSERT(conditionResultsPtr);

    size_t i = 0;
    for (; i < conditionResultsPtr->GetSize(); i++)
    {
      conditionResults.Set (i, (*conditionResultsPtr)[i]);
      conditionSet.SetBit (i);
    }
    evaluator.ForceConditionResults (conditionSet, conditionResults);
  }

  void csShaderConditionResolver::DumpConditionTree (csString& out)
  {
    if (rootNode == 0)
      return;

    out += "\n";
    DumpConditionNode (out, rootNode, 0);
  }

  static void Indent (csString& out, int n)
  {
    while (n-- > 0)
      out += "| ";
  }

  void csShaderConditionResolver::DumpConditionNode (csString& out,
    csConditionNode* node, 
    int level)
  {
    if (node == 0)
    {
      Indent (out, level);
      out.Append ("<none>\n");
    }
    else
    {
      if (node->variant != csArrayItemNotFound)
      {
        out.AppendFmt ("variant: %zu", node->variant);
      }
      else
      {
        out.Append ("\n");
        Indent (out, level);
        out.AppendFmt ("condition %zu = true: ", node->condition);
        DumpConditionNode (out, node->trueNode, level + 1);
        out.Append ("\n");
        Indent (out, level);
        out.AppendFmt ("condition %zu = false: ", node->condition);
        DumpConditionNode (out, node->falseNode, level + 1);
      }
    }
  }

  void csShaderConditionResolver::CollectUsedConditions (csConditionNode* node,
    ConditionsWriter& condWrite)
  {
    if (node == 0) return;
    if (node->condition == csCondAlwaysTrue) return;
    if (node->condition == csCondAlwaysFalse) return;

    csFIFO<csConditionID> condStack;
    condStack.Push (node->condition);
    while (condStack.GetSize() > 0)
    {
      csConditionID cond = condStack.PopTop();
      condWrite.GetDiskID (cond);
      const CondOperation& op = evaluator.GetCondition (cond);
      if (op.left.type == operandOperation) condStack.Push (op.left.operation);
      if (op.right.type == operandOperation) condStack.Push (op.right.operation);
    }
    CollectUsedConditions (node->falseNode, condWrite);
    CollectUsedConditions (node->trueNode, condWrite);
  }

  bool csShaderConditionResolver::ReadFromCache (iFile* cacheFile,
    ConditionsReader& condReader)
  {
    uint32 nextVarLE;
    if (cacheFile->Read ((char*)&nextVarLE, sizeof (nextVarLE))
      != sizeof (nextVarLE))
      return false;
    nextVariant = csLittleEndian::UInt32 (nextVarLE);

    if (!ReadNode (cacheFile, condReader, 0, rootNode))
    {
      delete rootNode; rootNode = 0;
      nextVariant = 0;
      return false;
    }

    return true;
  }

  bool csShaderConditionResolver::WriteToCache (iFile* cacheFile,
    ConditionsWriter& condWriter)
  {
    uint32 nextVarLE = nextVariant;
    nextVarLE = csLittleEndian::UInt32 (nextVarLE);
    if (cacheFile->Write ((char*)&nextVarLE, sizeof (nextVarLE))
      != sizeof (nextVarLE))
      return false;

    if (rootNode == 0)
    {
      // Special case
      uint32 varLE = 0xffffffff;
      if (cacheFile->Write ((char*)&varLE, sizeof (varLE)) != sizeof (varLE))
        return false;
    }
    else
    {
      if (!WriteNode (cacheFile, rootNode, condWriter)) return false;
    }

    return true;
  }

  bool csShaderConditionResolver::ReadNode (iFile* cacheFile, 
    const ConditionsReader& condRead, csConditionNode* parent,
    csConditionNode*& node)
  {
    uint32 condLE;
    if (cacheFile->Read ((char*)&condLE, sizeof (condLE)) != sizeof (condLE))
      return false;
    if (condLE == 0xffffffff)
    {
      // Special case 'no root'
      node = 0;
      return true;
    }
    node = NewNode (parent);
    condLE = csLittleEndian::UInt32 (condLE);
    if (condLE & 0x80000000)
    {
      // Leaf, ie variant
      node->variant = condLE & 0x7fffffff;
      return true;
    }
    else
    {
      // Node, ie condition
      node->condition = condRead.GetConditionID (condLE);
      if (node->condition == (csConditionID)~0) return false;
      return ReadNode (cacheFile, condRead, node, node->trueNode)
        && ReadNode (cacheFile, condRead, node, node->falseNode);
    }
  }

  bool csShaderConditionResolver::WriteNode (iFile* cacheFile, csConditionNode* node,
    const ConditionsWriter& condWrite)
  {
    if (node->variant == csArrayItemNotFound)
    {
      CS_ASSERT(node->condition != csCondAlwaysTrue);
      CS_ASSERT(node->condition != csCondAlwaysFalse);

      uint32 condLE = condWrite.GetDiskID (node->condition);
      condLE = csLittleEndian::UInt32 (condLE);
      if (cacheFile->Write ((char*)&condLE, sizeof (condLE)) != sizeof (condLE))
        return false;
      return WriteNode (cacheFile, node->trueNode, condWrite)
        && WriteNode (cacheFile, node->falseNode, condWrite);
    }
    else
    {
      uint32 varLE = node->variant;
      varLE |= 0x80000000;
      varLE = csLittleEndian::UInt32 (varLE);
      return cacheFile->Write ((char*)&varLE, sizeof (varLE)) == sizeof (varLE);
    }
  }

  //---------------------------------------------------------------------------

  csXMLShader::csXMLShader (csXMLShaderCompiler* compiler, 
    iLoaderContext* ldr_context,
    iDocumentNode* source,
    int forcepriority)
    : scfImplementationType (this), techsResolver (0),
    sharedEvaluator (compiler->sharedEvaluator),
    fallbackTried (false)
  {
    InitTokenTable (xmltokens);

    csXMLShader::ldr_context = ldr_context;

    activeTech = 0;
    filename = 0;
    csXMLShader::compiler = compiler;
    g3d = compiler->g3d;
    csXMLShader::forcepriority = forcepriority;
    useFallbackContext = false;

    allShaderMeta.numberOfLights = source->GetAttributeValueAsInt ("lights");

    shadermgr = csQueryRegistry<iShaderManager> (compiler->objectreg);
    CS_ASSERT (shadermgr); // Should be present - loads us, after all

    vfsStartDir = CS::StrDup (compiler->vfs->GetCwd ());
    shaderCache = shadermgr->GetShaderCache();
    Load (source, false);
  }

  csXMLShader::csXMLShader (csXMLShaderCompiler* compiler)
    : scfImplementationType (this), techsResolver (0),
    sharedEvaluator (compiler->sharedEvaluator),
    fallbackTried (false)
  {
    InitTokenTable (xmltokens);

    csXMLShader::ldr_context = ldr_context;

    activeTech = 0;
    filename = 0;
    csXMLShader::compiler = compiler;
    g3d = compiler->g3d;
    csXMLShader::forcepriority = -1;
    useFallbackContext = false;

    shadermgr = csQueryRegistry<iShaderManager> (compiler->objectreg);
    CS_ASSERT (shadermgr); // Should be present - loads us, after all

    vfsStartDir = CS::StrDup (compiler->vfs->GetCwd ());
  }

  csXMLShader::~csXMLShader ()
  {
    for (size_t i = 0; i < techniques.GetSize(); i++)
    {
      techniques[i].Free();
    }

    cs_free (filename);
    delete techsResolver;
    cs_free (vfsStartDir);
    cs_free (const_cast<char*> (allShaderMeta.description));
  }

  csPtr<iDocumentNode> csXMLShader::StripShaderRoot (iDocumentNode* shaderRoot)
  {
    csRef<iDocument> doc = compiler->xmlDocSys->CreateDocument();
    csRef<iDocumentNode> docRoot = doc->CreateRoot ();
    csRef<iDocumentNode> topNode =
      docRoot->CreateNodeBefore (shaderRoot->GetType());
    topNode->SetValue (shaderRoot->GetValue());
    CS::DocSystem::CloneAttributes (shaderRoot, topNode);
    
    csRef<iDocumentNodeIterator> shaderNodesIter = shaderRoot->GetNodes();
    while (shaderNodesIter->HasNext())
    {
      csRef<iDocumentNode> child = shaderNodesIter->Next();
      csStringID id = xmltokens.Request (child->GetValue());
      if ((id != csXMLShaderCompiler::XMLTOKEN_SHADERVARS)
          && (id != csXMLShaderCompiler::XMLTOKEN_FALLBACKSHADER))
        continue;
      csRef<iDocumentNode> newNode = topNode->CreateNodeBefore (child->GetType());
      CS::DocSystem::CloneNode (child, newNode);
    }
    return csPtr<iDocumentNode> (topNode);
  }


  /* Magic value for cache file.
  * The most significant byte serves as a "version", increase when the
  * cache file format changes. */
  static const uint32 cacheFileMagic = 0x07737863;

  void csXMLShader::Load (iDocumentNode* source, bool forPrecache)
  {
    originalShaderDoc = source;
    csRef<iDocumentNode> shaderRoot;
    techsResolver = new csShaderConditionResolver (*sharedEvaluator);

    CS::PluginCommon::ShaderCacheHelper::ShaderDocHasher hasher (
      compiler->objectreg, source);

    csString cacheType = source->GetAttributeValue ("name");
    cacheTag = source->GetAttributeValue ("_cachetag");
    csString cacheID_header;
    {
      csString cacheID_base (source->GetAttributeValue ("_cacheid"));
      if (cacheID_base.IsEmpty())
      {
        csMD5::Digest sourceDigest (csMD5::Encode (CS::DocSystem::FlattenNode (source)));
        cacheID_base = sourceDigest.HexString();
      }
      cacheID_header.Format ("%sXH", cacheID_base.GetData());
      cacheScope_tech.Format ("%sXT", cacheID_base.GetData());
    }
    if (shaderCache.IsValid ()) shaderCache = shaderCache->GetRootedCache (
      csString().Format ("/%s", cacheType.GetData()));
    bool cacheValid = (shaderCache != 0) && !cacheType.IsEmpty()
      && !cacheID_header.IsEmpty();
    if (!cacheValid) shaderCache.Invalidate();
    bool readFromCache = cacheValid && !forPrecache;

    csRef<iFile> cacheFile;
    if (cacheValid)
    {
      csRef<iDataBuffer> cacheData;
      if (readFromCache)
        cacheData = shaderCache->ReadCache (csString().Format ("/%s", cacheID_header.GetData()));
      if (cacheData.IsValid())
      {
        cacheFile.AttachNew (new csMemFile (cacheData, true));
      }
      else
        readFromCache = false;
      if (cacheFile.IsValid())
      {
        do
        {
          // Read magic header
          uint32 diskMagic;
          size_t read = cacheFile->Read ((char*)&diskMagic, sizeof (diskMagic));
          if (read != sizeof (diskMagic)) break;
          if (csLittleEndian::UInt32 (diskMagic) != cacheFileMagic) break;

          csString cacheFileTag =
            CS::PluginCommon::ShaderCacheHelper::ReadString (cacheFile);
          if (cacheFileTag != cacheTag) break;

          // Extract hash stream
          csRef<iDataBuffer> hashStream = 
            CS::PluginCommon::ShaderCacheHelper::ReadDataBuffer (cacheFile);
          if (!hashStream.IsValid()) break;

          readFromCache = hasher.ValidateHashStream (hashStream);
        }
        while (false);
      }
      if (!readFromCache)
      {
        // Getting from cache failed, so prep for writing to cache
        cacheFile.AttachNew (new csMemFile ());
        // Write magic header
        uint32 diskMagic = csLittleEndian::UInt32 (cacheFileMagic);
        cacheFile->Write ((char*)&diskMagic, sizeof (diskMagic));
        CS::PluginCommon::ShaderCacheHelper::WriteString (cacheFile, cacheTag);
        // Write hash stream
        csRef<iDataBuffer> hashStream = hasher.GetHashStream ();
        if (!CS::PluginCommon::ShaderCacheHelper::WriteDataBuffer (
          cacheFile, hashStream))
          cacheFile.Invalidate();
      }
    }

    if (cacheTag.IsEmpty())
    {
      csRef<iDataBuffer> hashStream = hasher.GetHashStream ();
      if (hashStream.IsValid())
      {
	// Hash hash stream (to get a smaller ID)
	csMD5::Digest hashDigest (csMD5::Encode (hashStream->GetData(),
	  hashStream->GetSize()));
	/* In theory, anything would work as long as (a) it changes when
	some file the shader uses changes (b) the tag is reasonably
	unique (also over multiple program runs).
	E.g. a UUID, recomputed when the shader is 'touched',
	could do as well. */
	
	cacheTag = CS::Utility::EncodeBase64 (&hashDigest, sizeof (hashDigest));
      }
    }

    ConditionsReader* condReader = 0;
    if (readFromCache)
    {
      do
      {
        readFromCache = false;
        csRef<iDataBuffer> conditionsBuf =
          CS::PluginCommon::ShaderCacheHelper::ReadDataBuffer (cacheFile);
        if (!conditionsBuf.IsValid()) break;;
        condReader = new ConditionsReader (*sharedEvaluator, conditionsBuf);
        readFromCache = true;
      }
      while (false);

      if (readFromCache)
      {
        // Read condition tree from cache
        readFromCache = techsResolver->ReadFromCache (cacheFile, *condReader);
        if (readFromCache && compiler->doDumpConds)
        {
          csString tree;
          tree.SetGrowsBy (0);
          techsResolver->DumpConditionTree (tree);
          csString filename;
          filename.Format ("/tmp/shader/cond_%s_techs.txt", source->GetAttributeValue ("name"));
          compiler->vfs->WriteFile (filename, tree.GetData(), tree.Length ());
        }
      }
      
      if (readFromCache)
      {
	size_t tvc = techsResolver->GetVariantCount();
	if (tvc == 0) tvc = 1;
	
	for (size_t tvi = 0; (tvi < tvc) && readFromCache; tvi++)
	{
	  csRef<iDataBuffer> bitsBuf =
	    CS::PluginCommon::ShaderCacheHelper::ReadDataBuffer (cacheFile);
	    
	  if (!bitsBuf.IsValid())
	  {
	    readFromCache = false;
	    break;
	  }
	  ShaderTechVariant newVar;
	  newVar.activeTechniques = csBitArray::Unserialize (
	    bitsBuf->GetUint8(), bitsBuf->GetSize());
	  techVariants.Push (newVar);
	}
      }
      
      size_t numTechniques = 0;
      if (readFromCache)
      {
        uint32 diskTechNum;
        if (cacheFile->Read ((char*)&diskTechNum, sizeof (diskTechNum))
	    != sizeof (diskTechNum))
	  readFromCache = false;
	else
	  numTechniques = csLittleEndian::UInt32 (diskTechNum);
      }
	  
      csRef<iHierarchicalCache> techCache;
      for (size_t t = 0; (t < numTechniques) && readFromCache; t++)
      {
        Technique newTech;
        
	int32 diskPriority;
        readFromCache = cacheFile->Read ((char*)&diskPriority, sizeof (diskPriority))
	    == sizeof (diskPriority);
	if (!readFromCache) break;
	newTech.priority = csLittleEndian::Int32 (diskPriority);
	  
	int32 diskMinLights;
        readFromCache = cacheFile->Read ((char*)&diskMinLights, sizeof (diskMinLights))
	    == sizeof (diskMinLights);
	if (!readFromCache) break;
	newTech.minLights = csLittleEndian::Int32 (diskMinLights);
	  
	techCache = shaderCache->GetRootedCache (
	  csString().Format ("/%s/%zu", cacheScope_tech.GetData(),
	    t));
        readFromCache = LoadTechniqueFromCache (newTech, techCache);
        // Discard variations data from cache as well
        if (!readFromCache) techCache->ClearCache ("/");
        techniques.Push (newTech);
      }

      if (readFromCache)
      {
	csRef<iDataBuffer> docBuf =
	  CS::PluginCommon::ShaderCacheHelper::ReadDataBuffer (cacheFile);
	if (docBuf.IsValid ())
	{
	  if (compiler->binDocSys.IsValid())
	  {
	    csRef<iDocument> doc = compiler->binDocSys->CreateDocument();
	    if (doc->Parse (docBuf) == 0)
	    {
	      csRef<iDocumentNodeIterator> nodes = doc->GetRoot()->GetNodes();
	      if (nodes->HasNext())
		shaderRootStripped = nodes->Next();
	    }
	  }
	  if (!shaderRootStripped.IsValid())
	  {
	    csRef<iDocument> doc = compiler->xmlDocSys->CreateDocument();
	    if (doc->Parse (docBuf) == 0)
	    {
	      csRef<iDocumentNodeIterator> nodes = doc->GetRoot()->GetNodes();
	      if (nodes->HasNext())
		shaderRootStripped = nodes->Next();
	    }
	  }
	  readFromCache = shaderRootStripped.IsValid();
	}
	else
	 readFromCache = false;
      }
      if (!readFromCache)
      {
        // Make sure shader is pristine
        delete techsResolver;
        techsResolver = new csShaderConditionResolver (*sharedEvaluator);
        techVariants.Empty();
        techniques.Empty();
      }
    }

    if (!readFromCache)
    {
      // Scan techniques on node w/ expanded templates
      {
	csRef<csWrappedDocumentNode> wrappedNode;
        wrappedNode.AttachNew (
          compiler->wrapperFact->CreateWrapperStatic (source, 
          0, 0, wdnfpoExpandTemplates));
        shaderRoot = wrappedNode;
      }
      shaderRootStripped = StripShaderRoot (shaderRoot);
    
      csArray<TechniqueKeeper> techniquesTmp;
      ScanForTechniques (shaderRoot, techniquesTmp, forcepriority);

      csRef<iHierarchicalCache> techCache;
	
      /* Find a suitable technique
       * (Note that a wrapper is created for each technique node individually,
       * not the whole shader) */
      csArray<TechniqueKeeper>::Iterator techIt = techniquesTmp.GetIterator ();
      while (techIt.HasNext ())
      {
        const TechniqueKeeper& tk = techIt.Next();
        Technique newTech;
        newTech.priority = tk.priority;
        newTech.minLights = tk.node->GetAttributeValueAsInt ("minlights");

        size_t techIndex = techniques.GetSize();
	techCache = shaderCache->GetRootedCache (
	  csString().Format ("/%s/%zu", cacheScope_tech.GetData(),
	    techIndex));
        LoadTechnique (newTech, tk.node, techIndex, techCache);
        techniques.Push (newTech);
      }
    
      csRefArray<iDocumentNode> extraNodes;
      csRef<csWrappedDocumentNode> wrappedNode;
      if (compiler->doDumpConds)
      {
        csString tree;
        tree.SetGrowsBy (0);
        wrappedNode.AttachNew (compiler->wrapperFact->CreateWrapper (shaderRoot, 
          techsResolver, techsResolver->evaluator, extraNodes, &tree, 
          wdnfpoHandleConditions | wdnfpoOnlyOneLevelConditions
          | wdnfpoExpandTemplates));
        techsResolver->DumpConditionTree (tree);
        csString filename;
        filename.Format ("/tmp/shader/cond_%s_techs.txt",
          source->GetAttributeValue ("name"));
        compiler->vfs->WriteFile (filename, tree.GetData(), tree.Length ());
      }
      else
        wrappedNode.AttachNew (compiler->wrapperFact->CreateWrapper (shaderRoot, 
          techsResolver, techsResolver->evaluator, extraNodes, 0,
          wdnfpoHandleConditions | wdnfpoOnlyOneLevelConditions
          | wdnfpoExpandTemplates));
      shaderRoot = wrappedNode;
      
      PrepareTechVars (shaderRoot, forcepriority);
      
      if (cacheValid)
      {
        bool cacheState;
        ConditionsWriter condWriter (*sharedEvaluator);
        techsResolver->CollectUsedConditions (condWriter);
        {
          csRef<iDataBuffer> conditionsBuf = condWriter.GetPersistentData();
          cacheState = CS::PluginCommon::ShaderCacheHelper::WriteDataBuffer (
            cacheFile, conditionsBuf);
        }

        if (cacheState)
          cacheState = techsResolver->WriteToCache (cacheFile, condWriter);
	for (size_t i = 0; (i < techVariants.GetSize()) && cacheState; i++)
	{
	  const ShaderTechVariant& tv = techVariants[i];
	  size_t bitsSerSize;
	  uint8* bitsSer = tv.activeTechniques.Serialize (bitsSerSize);
	  CS::DataBuffer<> bitsBuffer ((char*)bitsSer, bitsSerSize);
	  cacheState = CS::PluginCommon::ShaderCacheHelper::WriteDataBuffer (
	    cacheFile, &bitsBuffer);
	}
        if (cacheState)
        {
	  uint32 diskTechNum = csLittleEndian::UInt32 (techniques.GetSize());
	  cacheState = cacheFile->Write ((char*)&diskTechNum, sizeof (diskTechNum))
	    == sizeof (diskTechNum);
	    
	  for (size_t t = 0; (t < techniques.GetSize()) && cacheState; t++)
	  {
	    Technique& tech = techniques[t];
	    int32 diskPriority = csLittleEndian::Int32 (tech.priority);
	    cacheState = cacheFile->Write ((char*)&diskPriority, sizeof (diskPriority))
	      == sizeof (diskPriority);
	    if (!cacheState) break;
	    int32 diskMinLights = csLittleEndian::Int32 (tech.minLights);
	    cacheState = cacheFile->Write ((char*)&diskMinLights, sizeof (diskMinLights))
	      == sizeof (diskMinLights);
	  }
        }
        if (cacheState)
        {
          csRef<iDocumentSystem> docSys = compiler->binDocSys.IsValid()
            ? compiler->binDocSys : compiler->xmlDocSys;
	  csRef<iDocument> doc = docSys->CreateDocument();
	  csRef<iDocumentNode> docRoot = doc->CreateRoot();
	  csRef<iDocumentNode> topNode = docRoot->CreateNodeBefore (shaderRootStripped->GetType());
	  CS::DocSystem::CloneNode (shaderRootStripped, topNode);
	  csMemFile docDataFile;
	  if (doc->Write (&docDataFile) != 0)
	    cacheState = false;
	  else
	  {
	    csRef<iDataBuffer> docData = docDataFile.GetAllData();
	    cacheState = CS::PluginCommon::ShaderCacheHelper::WriteDataBuffer (
	      cacheFile, docData);
	  }
	}

        if (cacheState)
        {
          csRef<iDataBuffer> allCacheData = cacheFile->GetAllData();
          shaderCache->CacheData (allCacheData->GetData(),
            allCacheData->GetSize(), 
            csString().Format ("/%s", cacheID_header.GetData()));
        }
      }
    }

    //Load global shadervars block
    csRef<iDocumentNode> varNode = shaderRootStripped->GetNode(
      xmltokens.Request (csXMLShaderCompiler::XMLTOKEN_SHADERVARS));
    if (varNode)
      ParseGlobalSVs (ldr_context, varNode);

    delete condReader;
  }

  bool csXMLShader::Precache (iDocumentNode* source, iHierarchicalCache* cacheTo)
  {
    shaderCache = cacheTo;
    Load (source, true);
    if (!shaderCache.IsValid()) return false;

    bool result = true;
    size_t tvc = techsResolver->GetVariantCount();
    if (tvc == 0) tvc = 1;

    size_t totalTechs = 0;

    for (size_t t = 0; t < techniques.GetSize(); t++)
    {
      Technique& tech = techniques[t];

      size_t vc = tech.resolver->GetVariantCount();
      if (vc == 0) vc = 1;
      
      if (compiler->do_verbose)
	compiler->Report (CS_REPORTER_SEVERITY_NOTIFY,
	"Shader '%s': priority %d: %zu variations",
	GetName(), tech.priority, vc);

      totalTechs += vc;
    }

    size_t techsHandled = 0;
    csTicks startTime = csGetTicks();
    csTextProgressMeter* progress = 0;
    for (size_t t = 0; t < techniques.GetSize(); t++)
    {
      Technique& tech = techniques[t];

      csRef<iHierarchicalCache> techCache;
      techCache = shaderCache->GetRootedCache (
	csString().Format ("/%s/%zu", cacheScope_tech.GetData(), t));

      size_t vc = tech.resolver->GetVariantCount();
      if (vc == 0) vc = 1;
      for (size_t vi = 0; vi < vc; vi++)
      {
	tech.resolver->SetVariant (vi);

	size_t ticket = vi * (techniques.GetSize()+1) + (t+1);
	//((vi*techVar.techniques.GetSize() + t) * (tvc+1) + (tvi+1));

	if (compiler->doDumpXML)
	{
	  csRef<iDocumentSystem> docsys;
	  docsys.AttachNew (new csTinyDocumentSystem);
	  csRef<iDocument> newdoc = docsys->CreateDocument();
	  CS::DocSystem::CloneNode (tech.techNode, newdoc->CreateRoot());
	  newdoc->Write (compiler->vfs, csString().Format ("/tmp/shader/%s_%zu_%zu.xml",
	    GetName(), t, vi));
	}

	csRef<iHierarchicalCache> varCache;
	varCache.AttachNew (
	  new CS::PluginCommon::ShaderCacheHelper::MicroArchiveCache (
	  techCache, csString().Format ("/%zu", vi)));

	// So external files are found correctly
	csVfsDirectoryChanger dirChange (compiler->vfs);
	dirChange.ChangeTo (vfsStartDir);

        sharedEvaluator->EnterEvaluation();
	csXMLShaderTech* xmltech = new csXMLShaderTech (this);
	bool result = xmltech->Precache (tech.techNode, ticket, varCache);
        sharedEvaluator->LeaveEvaluation();
	if (!result)
	{
	  if (compiler->do_verbose)
	  {
	    compiler->Report (CS_REPORTER_SEVERITY_NOTIFY,
	      "Shader '%s'<%zu/%zu>: Technique with priority %d fails. Reason: %s.",
	      GetName(), vi, tech.priority,
	      xmltech->GetFailReason());
	  }
	  result = false;
	}
	delete xmltech;
	techsHandled++;
	if (progress)
	  progress->Step (1);
	else if (csGetTicks() - startTime > 1000)
	{
	  progress = new csTextProgressMeter (0, totalTechs);
	  progress->Step (techsHandled);
	}
      }
    }

    /* If the "fallback" node lacks a 'file' attribute it's probably an inline
    * shader, precache as well */
    csRef<iDocumentNodeIterator> fallbackNodes = source->GetNodes ("fallbackshader");
    while (fallbackNodes->HasNext())
    {
      csRef<iDocumentNode> fallbackNode (fallbackNodes->Next());

      // So external files are found correctly
      csVfsDirectoryChanger chdir (compiler->vfs);
      chdir.ChangeTo (vfsStartDir);

      const char* fileStr = fallbackNode->GetAttributeValue ("file");
      if (fileStr == 0)
      {
        const char* type = fallbackNode->GetAttributeValue ("compiler");
        if (type == 0)
          type = fallbackNode->GetAttributeValue ("type");
        if (type != 0)
        {
          csRef<iShaderCompiler> shcom = shadermgr->GetCompiler (type);
          if (shcom.IsValid()) 
          {
            result &= shcom->PrecacheShader (fallbackNode, cacheTo);
          }
        }
      }
    }
    delete progress;
    return result;
  }

  void csXMLShader::SelfDestruct ()
  {
    if (shadermgr)
      shadermgr->UnregisterShader ((iShader*)this);
  }

  int csXMLShader::CompareTechniqueKeeper (
    TechniqueKeeper const& t1, TechniqueKeeper const& t2)
  {
    int v = t2.priority - t1.priority;
    if (v == 0) v = t2.tagPriority - t1.tagPriority;
    return v;
  }

  void csXMLShader::ScanForTechniques (iDocumentNode* templ,
    csArray<TechniqueKeeper>& techniquesTmp,
    int forcepriority)
  {
    csRef<iDocumentNodeIterator> it = templ->GetNodes();

    // Read in the techniques.
    while (it->HasNext ())
    {
      csRef<iDocumentNode> child = it->Next ();
      if (child->GetType () == CS_NODE_ELEMENT &&
        xmltokens.Request (child->GetValue ()) == XMLTOKEN_TECHNIQUE)
      {
        //save it
        int p = child->GetAttributeValueAsInt ("priority");
        if ((forcepriority != -1) && (p != forcepriority)) continue;
        TechniqueKeeper keeper (child, p);
        // Compute the tag's priorities.
        csRef<iDocumentNodeIterator> tagIt = child->GetNodes ("tag");
        while (tagIt->HasNext ())
        {
          csRef<iDocumentNode> tag = tagIt->Next ();
          csStringID tagID = compiler->strings->Request (tag->GetContentsValue ());

          csShaderTagPresence presence;
          int priority;
          shadermgr->GetTagOptions (tagID, presence, priority);
          if (presence == TagNeutral)
          {
            keeper.tagPriority += priority;
          }
        }
        techniquesTmp.Push (keeper);
      }
    }

    techniquesTmp.Sort (&CompareTechniqueKeeper);
  }

  iShader* csXMLShader::GetFallbackShader()
  {
    if (!fallbackTried)
    {
      // So external files are found correctly
      csVfsDirectoryChanger chdir (compiler->vfs);
      chdir.ChangeTo (vfsStartDir);

      csRef<iDocumentNode> fallbackNode = shaderRootStripped->GetNode ("fallbackshader");
      if (fallbackNode.IsValid())
      {
        if (fallbackNode->GetAttribute ("file").IsValid())
          fallbackShader = compiler->synldr->ParseShaderRef (ldr_context,
          fallbackNode);
        else
          fallbackShader = compiler->synldr->ParseShader (ldr_context,
          fallbackNode);
      }

      fallbackTried = true;
    }
    return fallbackShader;
  }

  /**
  * A wrapped class to have a csShaderVarStack synced to an
  * iShaderVariableContext*.
  */
  class SVCWrapper : public scfImplementation1<SVCWrapper, 
    iShaderVariableContext>
  {
    csShaderVariableContext& wrappedSVC;
  public:
    csShaderVariableStack svStack;

    SVCWrapper (csShaderVariableContext& wrappedSVC, size_t maxSVs) : 
    scfImplementationType (this), wrappedSVC (wrappedSVC)
    {     
      svStack.Setup (maxSVs);
      wrappedSVC.PushVariables (svStack);
    }
    virtual ~SVCWrapper () { }
    virtual void AddVariable (csShaderVariable *variable)
    {
      wrappedSVC.AddVariable (variable);
    }
    virtual csShaderVariable* GetVariable (CS::ShaderVarStringID name) const
    { return wrappedSVC.GetVariable (name); }
    virtual const csRefArray<csShaderVariable>& GetShaderVariables () const
    { return wrappedSVC.GetShaderVariables (); }  
    virtual void PushVariables (csShaderVariableStack& stacks) const
    { wrappedSVC.PushVariables (stacks); }
    virtual bool IsEmpty() const
    { return wrappedSVC.IsEmpty(); }
    void ReplaceVariable (csShaderVariable *variable)
    { wrappedSVC.ReplaceVariable (variable); }
    void Clear ()
    { wrappedSVC.Clear(); }
    bool RemoveVariable (csShaderVariable *variable)
    { return wrappedSVC.RemoveVariable (variable); }
    bool RemoveVariable (CS::ShaderVarStringID name)
    { return wrappedSVC.RemoveVariable (name); }
  };

  void csXMLShader::ParseGlobalSVs (iLoaderContext* ldr_context,
    iDocumentNode* node)
  {
    SVCWrapper wrapper (globalSVContext, shadermgr->GetSVNameStringset ()->GetSize ());
    sharedEvaluator->EnterEvaluation();
    techsResolver->SetEvalParams (0, &wrapper.svStack);
    compiler->LoadSVBlock (ldr_context, node, &wrapper);
    techsResolver->SetEvalParams (0, 0);
    sharedEvaluator->LeaveEvaluation();
  }

  size_t csXMLShader::GetTicket (const csRenderMeshModes& modes, 
    const csShaderVariableStack& stack)
  {
    size_t ticket = csArrayItemNotFound;
    //sharedEvaluator->ResetEvaluationCache();
    csConditionEvaluator::ScopedEvaluation scope (*sharedEvaluator);
    techsResolver->SetEvalParams (&modes, &stack);

    int lightCount = 0;
    if (stack.GetSize() > compiler->stringLightCount)
    {
      csShaderVariable* svLightCount = stack[compiler->stringLightCount];
      if (svLightCount != 0)
        svLightCount->GetValue (lightCount);
    }

    size_t tvc = techsResolver->GetVariantCount();
    if (tvc == 0) tvc = 1;

    size_t tvi = techsResolver->GetVariant ();
    if (tvi != csArrayItemNotFound)
    {
      // Get the techniques variant
      ShaderTechVariant& techVar = techVariants.GetExtend (tvi);

      csXMLShaderTech* usedTech = 0;
      for (size_t t = 0; t < techniques.GetSize(); t++)
      {
        if (!techVar.activeTechniques.IsBitSet (t)) continue;
        Technique& tech = techniques[t];
        if (lightCount < tech.minLights) continue;

        csRef<iHierarchicalCache> techCache;
        tech.resolver->SetEvalParams (&modes, &stack);

        size_t vi = tech.resolver->GetVariant ();
        if (vi != csArrayItemNotFound)
        {
          csXMLShaderTech*& var = tech.variants.GetExtend (vi);
          tech.variantsPrepared.SetSize (csMax (tech.variantsPrepared.GetSize(),
            vi+1));
          //ticket = ((vi*techVar.techniques.GetSize() + t) * (tvc+1) + (tvi+1));
          ticket = vi * (techniques.GetSize()+1) + (t+1);

          csRef<iHierarchicalCache> varCache;
          if (!tech.variantsPrepared[vi])
          {
            if (!techCache.IsValid() && shaderCache.IsValid())
            {
              techCache = shaderCache->GetRootedCache (
                csString().Format ("/%s/%zu", cacheScope_tech.GetData(), t));
            }

            if (techCache.IsValid())
            {
              varCache.AttachNew (
                new CS::PluginCommon::ShaderCacheHelper::MicroArchiveCache (
                techCache, csString().Format ("/%zu", vi)));
            }

            if (compiler->doDumpXML)
            {
              csRef<iDocumentSystem> docsys;
              docsys.AttachNew (new csTinyDocumentSystem);
              csRef<iDocument> newdoc = docsys->CreateDocument();
              CS::DocSystem::CloneNode (tech.techNode, newdoc->CreateRoot());
              newdoc->Write (compiler->vfs, csString().Format ("/tmp/shader/%s_%zu_%zu.xml",
                GetName(), t, vi));
            }

            iShaderProgram::CacheLoadResult loadResult = iShaderProgram::loadFail;
            var = 0;
            if (techCache.IsValid())
            {
              var = new csXMLShaderTech (this);
              loadResult = var->LoadFromCache (ldr_context, tech.techNode,
                varCache, shaderRootStripped, ticket);
              if (compiler->do_verbose)
              {
                switch (loadResult)
                {
                case iShaderProgram::loadFail:
                  {
                    compiler->Report (CS_REPORTER_SEVERITY_NOTIFY,
                      "Shader '%s'<%zu/%zu>: Technique with priority %d fails (from cache). Reason: %s.",
                      GetName(), tvi, vi, tech.priority, var->GetFailReason());
                  }
                  break;
                case iShaderProgram::loadSuccessShaderInvalid:
                  {
                    compiler->Report (CS_REPORTER_SEVERITY_NOTIFY,
                      "Shader '%s'<%zu/%zu>: Technique with priority %d succeeds (from cache) but shader is invalid.",
                      GetName(), tvi, vi, tech.priority);
                  }
                  break;
                case iShaderProgram::loadSuccessShaderValid:
                  {
                    compiler->Report (CS_REPORTER_SEVERITY_NOTIFY,
                      "Shader '%s'<%zu/%zu>: Technique with priority %d succeeds (from cache).",
                      GetName(), tvi, vi, tech.priority);
                  }
                  break;
                }
              }
              if (loadResult != iShaderProgram::loadSuccessShaderValid)
              {
                delete var; var = 0;
              }
            }

            if ((var == 0)
              && (loadResult == iShaderProgram::loadFail))
            {
              // So external files are found correctly
              csVfsDirectoryChanger dirChange (compiler->vfs);
              dirChange.ChangeTo (vfsStartDir);

              var = new csXMLShaderTech (this);
              if (var->Load (ldr_context, tech.techNode, shaderRootStripped, ticket,
                varCache))
              {
                if (compiler->do_verbose)
                  compiler->Report (CS_REPORTER_SEVERITY_NOTIFY,
                  "Shader '%s'<%zu/%zu>: Technique with priority %d succeeds!",
                  GetName(), tvi, vi, tech.priority);
              }
              else
              {
                if (compiler->do_verbose)
                {
                  compiler->Report (CS_REPORTER_SEVERITY_NOTIFY,
                    "Shader '%s'<%zu/%zu>: Technique with priority %d fails. Reason: %s.",
                    GetName(), tvi, vi, tech.priority, var->GetFailReason());
                }
                delete var; var = 0;
              }
            }

            tech.variantsPrepared[vi] = true;
          }
          if (var != 0)
          {
            tech.resolver->SetEvalParams (0, 0);
            usedTech = var;
            break;
          }
        }
        tech.resolver->SetEvalParams (0, 0);
      }

      if (usedTech == 0)
      {
        if (GetFallbackShader ())
        {
          if (compiler->do_verbose && !techVar.shownError)
          {
            compiler->Report (CS_REPORTER_SEVERITY_NOTIFY,
              "No technique validated for shader '%s' TV %zu: using fallback", 
              GetName(), tvi);
          }
          size_t fallbackTicket = GetFallbackShader()->GetTicket (modes, stack);
          if (fallbackTicket != csArrayItemNotFound)
          {
            ticket = fallbackTicket * (techniques.GetSize()+1);
          }
          else
            ticket = csArrayItemNotFound;
        }
        else if (!techVar.shownError && compiler->do_verbose)
          compiler->Report (CS_REPORTER_SEVERITY_WARNING,
          "No technique validated for shader '%s' TV %zu", GetName(), tvi);
      }
      techVar.shownError = true;
    }

    techsResolver->SetEvalParams (0, 0);

    return ticket;
  }

  void csXMLShader::PrepareTechVars (iDocumentNode* shaderRoot, int forcepriority)
  {
    size_t tvc = techsResolver->GetVariantCount();
    if (tvc == 0) tvc = 1;
    
    for (size_t tvi = 0; tvi < tvc; tvi++)
    {
      // Make sure evaluation cache is cleared after each loop
      csConditionEvaluator::ScopedEvaluation scope (*sharedEvaluator);
      
      techsResolver->SetVariant (tvi);
      ShaderTechVariant& techVar = techVariants.GetExtend (tvi);
      
      techVar.activeTechniques.SetSize (techniques.GetSize ());
    
      csArray<TechniqueKeeper> techniquesTmp;
      ScanForTechniques (shaderRoot, techniquesTmp, forcepriority);

      csArray<TechniqueKeeper>::Iterator techIt = techniquesTmp.GetIterator ();
      while (techIt.HasNext ())
      {
        const TechniqueKeeper& tk = techIt.Next();
        csWrappedDocumentNode* wrapperNode =
          static_cast<csWrappedDocumentNode*> ((iDocumentNode*)tk.node);
        
        for (size_t t = 0; t < techniques.GetSize(); t++)
        {
          iDocumentNode* srcNode = techniques[t].techNode->GetWrappedNode();
          if (srcNode->Equals (wrapperNode->GetWrappedNode()))
          {
            techVar.activeTechniques.SetBit (t);
            break;
          }
        }
      }
    }
  }

  bool csXMLShader::LoadTechniqueFromCache (Technique& tech,
    iHierarchicalCache* cache)
  {
    csRef<iFile> cacheFile;
    csRef<iDataBuffer> cacheData (cache->ReadCache ("/doc"));
    if (cacheData.IsValid())
      cacheFile.AttachNew (new csMemFile (cacheData, true));
    if (!cacheFile.IsValid()) return false;

    uint32 diskMagic;
    size_t read = cacheFile->Read ((char*)&diskMagic, sizeof (diskMagic));
    if (read != sizeof (diskMagic)) return false;
    if (csLittleEndian::UInt32 (diskMagic) != cacheFileMagic) return false;

    csString cachedTag =
      CS::PluginCommon::ShaderCacheHelper::ReadString (cacheFile);
    if (cachedTag != cacheTag) return false;

    csRef<iDataBuffer> conditionsBuf =
      CS::PluginCommon::ShaderCacheHelper::ReadDataBuffer (cacheFile);
    if (!conditionsBuf.IsValid()) return false;

    ConditionsReader condReader (*sharedEvaluator, conditionsBuf);

    tech.resolver = new csShaderConditionResolver (*sharedEvaluator);
    if (!tech.resolver->ReadFromCache (cacheFile, condReader))
    {
      delete tech.resolver;
      return false;
    }

    csRef<csWrappedDocumentNode> wrappedNode;
    wrappedNode.AttachNew (compiler->wrapperFact->CreateWrapperFromCache (cacheFile,
      tech.resolver, *sharedEvaluator, condReader));
    if (!wrappedNode.IsValid()) return false;
    tech.techNode = wrappedNode;

    return true;
  }

  void csXMLShader::LoadTechnique (Technique& tech, iDocumentNode* srcNode,
    size_t techIndex, iHierarchicalCache* cacheTo, bool forPrecache)
  {
    tech.resolver = new csShaderConditionResolver (*sharedEvaluator);

    csRefArray<iDocumentNode> extraNodes;
    static const char* const extraNodeNames[] = { "vp", "fp", "vproc", 0 };
    csRef<iDocumentNodeIterator> passNodes = srcNode->GetNodes ("pass");
    while (passNodes->HasNext())
    {
      csRef<iDocumentNode> passNode = passNodes->Next();
      const char* const* extraName = extraNodeNames;
      while (*extraName)
      {
        csRef<iDocumentNode> node = passNode->GetNode (*extraName);
        if (node.IsValid())
        {
          const char* filename = node->GetAttributeValue ("file");
          if (filename != 0)
          {
            csRef<iDocumentNode> extraNode = OpenDocFile (filename);
            if (extraNode.IsValid()) extraNodes.Push (extraNode);
          }
        }
        extraName++;
      }
    }

    // So external files are found correctly
    csVfsDirectoryChanger dirChange (compiler->vfs);
    dirChange.ChangeTo (vfsStartDir);

    /* @@@ TODO: Some SV values are fixed from the tech determination;
    * treat them as constant in the technique */
    csRef<csWrappedDocumentNode> wrappedNode;

    if (compiler->doDumpConds)
    {
      csString tree;
      tree.SetGrowsBy (0);

      wrappedNode.AttachNew (compiler->wrapperFact->CreateWrapper (srcNode, 
        tech.resolver, tech.resolver->evaluator, extraNodes, &tree,
        wdnfpoHandleConditions));

      tech.resolver->DumpConditionTree (tree);
      csString filename;
      filename.Format ("/tmp/shader/cond_%s_%zu.txt",
        shaderRootStripped->GetAttributeValue ("name"),
        techIndex);
      compiler->vfs->WriteFile (filename, tree.GetData(), tree.Length ());
    }
    else
      wrappedNode.AttachNew (compiler->wrapperFact->CreateWrapper (srcNode, 
        tech.resolver, tech.resolver->evaluator, extraNodes, 0,
        wdnfpoHandleConditions));

    tech.techNode = wrappedNode;

    if (cacheTo)
    {
      csRef<csMemFile> cacheFile;
      cacheFile.AttachNew (new csMemFile);
      uint32 diskMagic = csLittleEndian::UInt32 (cacheFileMagic);
      if (cacheFile->Write ((char*)&diskMagic, sizeof (diskMagic))
        != sizeof (diskMagic))
        cacheFile.Invalidate();

      if (cacheFile.IsValid()
        && !CS::PluginCommon::ShaderCacheHelper::WriteString (
        cacheFile, cacheTag))
        cacheFile.Invalidate();

      if (cacheFile.IsValid())
      {
        ConditionsWriter condWriter (*sharedEvaluator);
        tech.resolver->CollectUsedConditions (condWriter);
        wrappedNode->CollectUsedConditions (condWriter);
        {
          csRef<iDataBuffer> conditionsBuf = condWriter.GetPersistentData();
          if (!CS::PluginCommon::ShaderCacheHelper::WriteDataBuffer (
            cacheFile, conditionsBuf))
            cacheFile.Invalidate();
        }

        if (cacheFile.IsValid() && !tech.resolver->WriteToCache (cacheFile,
          condWriter))
          cacheFile.Invalidate();
        if (cacheFile.IsValid() && !wrappedNode->StoreToCache (cacheFile,
          condWriter))
          cacheFile.Invalidate();
        if (cacheFile.IsValid())
        {
          csRef<iDataBuffer> cacheData (cacheFile->GetAllData());
          cacheTo->CacheData (cacheData->GetData(), cacheData->GetSize(),
            "/doc");
        }
      }
    }
  }

  bool csXMLShader::ActivatePass (size_t ticket, size_t number)
  { 
    if (IsFallbackTicket (ticket))
    {
      useFallbackContext = true;
      return GetFallbackShader()->ActivatePass (GetFallbackTicket (ticket), number);
    }

    CS_ASSERT_MSG ("ActivatePass() has already been called.",
      activeTech == 0);
    activeTech = (ticket != csArrayItemNotFound) ? TechForTicket (ticket) : 0;
    return activeTech ? activeTech->ActivatePass (number) : false;
  }

  bool csXMLShader::DeactivatePass (size_t ticket)
  { 
    if (IsFallbackTicket (ticket))
    {
      useFallbackContext = false;
      return GetFallbackShader()->DeactivatePass (GetFallbackTicket (ticket));
    }

    bool ret = activeTech ? activeTech->DeactivatePass() : false; 
    activeTech = 0;
    return ret;
  }

  void csXMLShader::DumpStats (csString& str)
  {
    if (techsResolver->GetVariantCount () == 0)
      str.Replace ("unvarying techs");
    else
      str.Format ("%zu tech variations", techsResolver->GetVariantCount ());
  }

  csRef<iDocumentNode> csXMLShader::OpenDocFile (const char* filename)
  {
    csRef<iVFS> VFS =  
      csQueryRegistry<iVFS> (compiler->objectreg);
    csRef<iFile> file = VFS->Open (filename, VFS_FILE_READ);
    if (!file)
    {
      compiler->Report (CS_REPORTER_SEVERITY_ERROR,
        "Unable to open shader program file '%s'", filename);
      return 0;
    }
    csRef<iDocumentSystem> docsys (
      csQueryRegistry<iDocumentSystem> (compiler->objectreg));
    if (docsys == 0)
      docsys.AttachNew (new csTinyDocumentSystem ());

    csRef<iDocument> doc = docsys->CreateDocument ();
    const char* err = doc->Parse (file, true);
    if (err != 0)
    {
      compiler->Report (CS_REPORTER_SEVERITY_ERROR,
        "Unable to parse shader program file '%s': %s", filename, err);
      return 0;
    }

    return doc->GetRoot ();
  }

  csRef<iDocumentNode> csXMLShader::LoadProgramFile (const char* filename, 
    size_t variant)
  {
    csRef<iDocumentNode> programRoot = OpenDocFile (filename);
    if (!programRoot.IsValid()) return 0;

    csString dumpFN;
    if (compiler->doDumpConds || compiler->doDumpXML)
    {
      csString filenameClean (filename);
      for (size_t p = 0; p < filenameClean.Length (); p++)
      {
        if (!isalnum (filenameClean[p])) filenameClean[p] = '_';
      }
      dumpFN.Format ("%s_%s",
        GetName(), filenameClean.GetData());
    }

    size_t tc = techniques.GetSize();
    if (tc == 0) tc = 1;
    size_t tech = (variant % (tc+1))-1;
    const Technique& technique = techniques[tech];
    csShaderConditionResolver* resolver = technique.resolver;

    csRef<iDocumentNode> programNode;
    if (compiler->doDumpConds)
    {
      csString tree;
      programNode.AttachNew (compiler->wrapperFact->CreateWrapperStatic (
        programRoot, resolver, &tree));
      resolver->DumpConditionTree (tree);
      compiler->vfs->WriteFile (csString().Format ("/tmp/shader/%s_%zu.txt",
        dumpFN.GetData(), variant), tree.GetData(), tree.Length ());
    }
    else
      programNode.AttachNew (compiler->wrapperFact->CreateWrapperStatic (
      programRoot, resolver, 0));

    if (compiler->doDumpXML)
    {
      csRef<iDocumentSystem> docsys;
      docsys.AttachNew (new csTinyDocumentSystem);
      csRef<iDocument> newdoc = docsys->CreateDocument();
      CS::DocSystem::CloneNode (programNode, newdoc->CreateRoot());
      newdoc->Write (compiler->vfs, csString().Format ("/tmp/shader/%s_%zu.xml",
        dumpFN.GetData(), variant));
    }
    return programNode;
  }

}
CS_PLUGIN_NAMESPACE_END(XMLShader)
