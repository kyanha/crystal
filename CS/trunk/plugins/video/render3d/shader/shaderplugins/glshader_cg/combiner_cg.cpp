/*
  Copyright (C) 2003-2007 by Marten Svanfeldt
            (C) 2004-2007 by Frank Richter

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

#include "imap/services.h"
#include "iutil/cfgmgr.h"
#include "iutil/document.h"
#include "iutil/objreg.h"
#include "iutil/vfs.h"
#include "ivaria/reporter.h"

#include "csplugincommon/shader/weavertypes.h"
#include "csutil/cfgacc.h"
#include "csutil/documenthelper.h"
#include "csutil/fifo.h"
#include "csutil/scfstr.h"
#include "csutil/set.h"
#include "csutil/xmltiny.h"

#include "combiner_cg.h"

CS_PLUGIN_NAMESPACE_BEGIN(GLShaderCg)
{
  using namespace CS::PluginCommon;

  CS_LEAKGUARD_IMPLEMENT (ShaderCombinerLoaderCg);
  
  SCF_IMPLEMENT_FACTORY (ShaderCombinerLoaderCg)
  
  ShaderCombinerLoaderCg::ShaderCombinerLoaderCg (iBase* parent) : 
    scfImplementationType (this, parent)
  {
    InitTokenTable (xmltokens);
  }
  
  csPtr<ShaderWeaver::iCombiner> 
    ShaderCombinerLoaderCg::GetCombiner (iDocumentNode* params)
  {
    bool writeVP = true/*false*/, writeFP = true/*false*/;
    
#if 0
    if (params != 0)
    {
      csRef<iDocumentNode> vertexNode = params->GetNode ("vertex");
      if (vertexNode.IsValid())
      {
        const char* content = vertexNode->GetContentsValue ();
        if (content)
          writeVP = (strcasecmp (content, "yes") == 0)
            || (strcasecmp (content, "true") == 0)
            || (atoi (content) != 0);
      }
      
      csRef<iDocumentNode> fragmentNode = params->GetNode ("fragment");
      if (fragmentNode.IsValid())
      {
        const char* content = fragmentNode->GetContentsValue ();
        if (content)
          writeFP = (strcasecmp (content, "yes") == 0)
            || (strcasecmp (content, "true") == 0)
            || (atoi (content) != 0);
      }
    }
#endif
    csRef<ShaderCombinerCg> newCombiner;
    newCombiner.AttachNew (new ShaderCombinerCg (this, writeVP, writeFP));
    
    return csPtr<ShaderWeaver::iCombiner> (newCombiner);
  }

  void ShaderCombinerLoaderCg::GenerateConstantInputBlocks (
    iDocumentNode* node, const char* locationPrefix, const csVector4& value,
    int usedComponents, const char* outputName)
  {
    csString code;
    code << outputName;
    code << " = ";
    if (usedComponents > 1) code << "float" << usedComponents << '(';
    code << value[0];
    for (int i = 1; i < usedComponents; i++)
      code << ", " << value[i];
    if (usedComponents > 1) code << ')';
    code << ';';

    csRef<iDocumentNode> blockNode;
    csRef<iDocumentNode> contents;

    blockNode = node->CreateNodeBefore (CS_NODE_ELEMENT);
    blockNode->SetValue ("block");
    blockNode->SetAttribute ("location", 
      csString().Format ("%s:fragmentMain", locationPrefix));
    contents = blockNode->CreateNodeBefore (CS_NODE_TEXT);
    contents->SetValue (code);

    blockNode = node->CreateNodeBefore (CS_NODE_ELEMENT);
    blockNode->SetValue ("block");
    blockNode->SetAttribute ("location", 
      csString().Format ("%s:vertexMain", locationPrefix));
    contents = blockNode->CreateNodeBefore (CS_NODE_TEXT);
    contents->SetValue (code);
  }

  static inline bool IsAlpha (char c)
  { return ((c >= 'A') && (c <= 'Z')) || ((c >= 'a') || (c <= 'z')); }

  static inline bool IsAlNum (char c)
  { return ((c >= '0') && (c <= '9')) || IsAlpha (c); }

  static csString MakeIdentifier (const char* s)
  {
    csString res;
    if (!IsAlpha (*s)) res << '_';
    while (*s != 0)
    {
      char ch = *s++;
      if (IsAlNum (ch))
        res << ch;
      else
        res.AppendFmt ("_%02x", ch);
    }
    return res;
  }

  void ShaderCombinerLoaderCg::GenerateSVInputBlocks (iDocumentNode* node, 
    const char* locationPrefix, const char* svName, const char* outputType, 
    const char* outputName, const char* uniqueTag)
  {
    csString cgIdent = MakeIdentifier (uniqueTag);

    csRef<iDocumentNode> blockNode;

    blockNode = node->CreateNodeBefore (CS_NODE_ELEMENT);
    blockNode->SetValue ("block");
    blockNode->SetAttribute ("location", 
      csString().Format ("%s:variablemap", locationPrefix));
    {
      csRef<iDocumentNode> varMapNode;

      varMapNode = blockNode->CreateNodeBefore (CS_NODE_ELEMENT);
      varMapNode->SetValue ("variablemap");
      varMapNode->SetAttribute ("variable", svName);
      varMapNode->SetAttribute ("destination", 
        csString().Format ("vertexIn.%s", cgIdent.GetData()));

      varMapNode = blockNode->CreateNodeBefore (CS_NODE_ELEMENT);
      varMapNode->SetValue ("variablemap");
      varMapNode->SetAttribute ("variable", svName);
      varMapNode->SetAttribute ("destination", 
        csString().Format ("fragmentIn.%s", cgIdent.GetData()));
    }

    {
      csRef<iDocumentNode> uniformNode;

      blockNode = node->CreateNodeBefore (CS_NODE_ELEMENT);
      blockNode->SetValue ("block");
      blockNode->SetAttribute ("location", 
        csString().Format ("%s:fragmentIn", locationPrefix));

      uniformNode = blockNode->CreateNodeBefore (CS_NODE_ELEMENT);
      uniformNode->SetValue ("uniform");
      uniformNode->SetAttribute ("type", outputType);
      uniformNode->SetAttribute ("name", cgIdent);

      blockNode = node->CreateNodeBefore (CS_NODE_ELEMENT);
      blockNode->SetValue ("block");
      blockNode->SetAttribute ("location", 
        csString().Format ("%s:vertexIn", locationPrefix));

      uniformNode = blockNode->CreateNodeBefore (CS_NODE_ELEMENT);
      uniformNode->SetValue ("uniform");
      uniformNode->SetAttribute ("type", outputType);
      uniformNode->SetAttribute ("name", cgIdent);
    }

    {
      csRef<iDocumentNode> contents;

      blockNode = node->CreateNodeBefore (CS_NODE_ELEMENT);
      blockNode->SetValue ("block");
      blockNode->SetAttribute ("location", 
        csString().Format ("%s:fragmentMain", locationPrefix));
      contents = blockNode->CreateNodeBefore (CS_NODE_TEXT);
      contents->SetValue (csString().Format ("%s = fragmentIn.%s;",
        outputName, cgIdent.GetData()));

      blockNode = node->CreateNodeBefore (CS_NODE_ELEMENT);
      blockNode->SetValue ("block");
      blockNode->SetAttribute ("location", 
        csString().Format ("%s:vertexMain", locationPrefix));
      contents = blockNode->CreateNodeBefore (CS_NODE_TEXT);
      contents->SetValue (csString().Format ("%s = vertexIn.%s;",
        outputName, cgIdent.GetData()));
    }
  }

  bool ShaderCombinerLoaderCg::Initialize (iObjectRegistry* reg)
  {
    object_reg = reg;
    
    csConfigAccess config (object_reg);
    
    const char* libraryPath =
      config->GetStr ("Video.OpenGL.Shader.Cg.Combiner.CoerceLibrary");
    if (!libraryPath || !*libraryPath)
    {
      Report (CS_REPORTER_SEVERITY_ERROR, "No coercion library set up");
      return false;
    }
    
    if (!LoadCoercionLibrary (libraryPath))
      return false;
    
    annotateCombined = config->GetBool ("Video.ShaderWeaver.AnnotateOutput");

    return true;
  }
  
  namespace
  {
    static const char messageID[] =
      "crystalspace.graphics3d.shader.combiner.glcg";
  }
  
  void ShaderCombinerLoaderCg::Report (int severity, const char* msg, ...)
  {
    va_list args;
    va_start (args, msg);
    csReportV (object_reg, severity, messageID, msg, args);
    va_end (args);
  }
  
  void ShaderCombinerLoaderCg::Report (int severity, iDocumentNode* node, 
                                       const char* msg, ...)
  {
    va_list args;
    va_start (args, msg);
    
    csRef<iSyntaxService> synsrv = csQueryRegistry<iSyntaxService> (
      object_reg);
    if (synsrv.IsValid())
    {
      csString msgStr;
      msgStr.FormatV (msg, args);
      synsrv->Report (messageID, severity, node, "%s", msgStr.GetData());
    }
    else
    {
      csReportV (object_reg, severity, messageID, msg, args);
    }
    va_end (args);
  }
    
  csPtr<WeaverCommon::iCoerceChainIterator> 
  ShaderCombinerLoaderCg::QueryCoerceChain (const char* fromType, 
                                            const char* toType)
  {
    csRef<CoerceChainIterator> iterator;
    iterator.AttachNew (new CoerceChainIterator);
    
    FindCoerceChain (fromType, toType, iterator->nodes);
    
    return csPtr<WeaverCommon::iCoerceChainIterator> (iterator);
  }
  
  uint ShaderCombinerLoaderCg::CoerceCost (const char* fromType, 
                                           const char* toType)
  {
    csArray<const CoerceItem*> chain;
    
    FindCoerceChain (fromType, toType, chain);
    if (chain.GetSize() == 0)
      return ShaderWeaver::NoCoercion;
      
    uint cost = 0;
    for (size_t i = 0; i < chain.GetSize(); i++)
      cost += chain[i]->cost;
    return cost;
  }
  
  bool ShaderCombinerLoaderCg::LoadCoercionLibrary (const char* path)
  {
    csRef<iVFS> vfs (csQueryRegistry<iVFS> (object_reg));
    if (!vfs.IsValid()) return false;
    
    csRef<iFile> libfile = vfs->Open (path, VFS_FILE_READ);
    if (!libfile.IsValid())
    {
      Report (CS_REPORTER_SEVERITY_ERROR, "Can't open %s", path);
      return false;
    }
    
    csRef<iDocumentSystem> docsys (
      csQueryRegistry<iDocumentSystem> (object_reg));
    if (!docsys.IsValid())
      docsys.AttachNew (new csTinyDocumentSystem);
    csRef<iDocument> doc = docsys->CreateDocument();
    {
      const char* err = doc->Parse (libfile);
      if (err)
      {
        Report (CS_REPORTER_SEVERITY_ERROR, "Error parsing %s: %s",
          path, err);
	return false;
      }
    }
    csRef<iDocumentNode> startNode = 
      doc->GetRoot ()->GetNode ("combinerlibrary");
    if (!startNode.IsValid())
    {
      Report (CS_REPORTER_SEVERITY_ERROR,
	"Expected 'combinerlibrary' node in file '%s'", path);
      return false;
    }
    
    CoercionTemplates templates;
    csRef<iDocumentNodeIterator> nodes = startNode->GetNodes();
    while (nodes->HasNext())
    {
      csRef<iDocumentNode> child = nodes->Next();
      if (child->GetType() != CS_NODE_ELEMENT) continue;
      
      csStringID id = xmltokens.Request (child->GetValue());
      switch (id)
      {
        case XMLTOKEN_COERCION:
          if (!ParseCoercion (child))
            return false;
          break;
        case XMLTOKEN_COERCIONTEMPLATE:
          if (!ParseCoercionTemplates (child, templates))
            return false;
          break;
	default:
	  {
	    csRef<iSyntaxService> synsrv = csQueryRegistry<iSyntaxService> (
	      object_reg);
	    if (synsrv.IsValid())
	      synsrv->ReportBadToken (child);
	    return false;
	  }
      }
    }
    return SynthesizeDefaultCoercions (templates);
  }
  
  bool ShaderCombinerLoaderCg::ParseCoercion (iDocumentNode* node)
  {
    const char* from = node->GetAttributeValue ("from");
    if (!from || !*from)
    {
      Report (CS_REPORTER_SEVERITY_ERROR, node, 
        "Non-empty 'from' attribute expeected");
      return false;
    }
    const char* to = node->GetAttributeValue ("to");
    if (!to || !*to)
    {
      Report (CS_REPORTER_SEVERITY_ERROR, node, 
        "Non-empty 'to' attribute expeected");
      return false;
    }
    int cost;
    csRef<iDocumentAttribute> costAttr = node->GetAttribute ("cost");
    if (!costAttr.IsValid())
    {
      Report (CS_REPORTER_SEVERITY_WARNING, node,
        "No 'cost' attribute, assuming cost 0");
      cost = 0;
    }
    else
    {
      cost = costAttr->GetValueAsInt ();
    }

    csRef<iDocumentNode> inputNode = 
      node->CreateNodeBefore (CS_NODE_ELEMENT);
    inputNode->SetValue ("input");
    inputNode->SetAttribute ("name", "input");
    inputNode->SetAttribute ("type", from);
    
    csRef<iDocumentNode> outputNode = 
      node->CreateNodeBefore (CS_NODE_ELEMENT);
    outputNode->SetValue ("output");
    outputNode->SetAttribute ("name", "output");
    outputNode->SetAttribute ("type", to);
    
    CoerceItem item;
    item.fromType = StoredTypeName (from);
    item.toType = StoredTypeName (to);
    item.cost = cost;
    item.node = node;
    CoerceItems* items = coercions.GetElementPointer (from);
    if (items == 0)
    {
      coercions.Put (StoredTypeName (from), CoerceItems());
      items = coercions.GetElementPointer (from);
    }
    items->InsertSorted (item, &CoerceItemCompare);
    
    return true;
  }

  bool ShaderCombinerLoaderCg::ParseCoercionTemplates (iDocumentNode* node, 
    CoercionTemplates& templates)
  {
    const char* name = node->GetAttributeValue ("name");
    if (!name || !*name)
    {
      Report (CS_REPORTER_SEVERITY_ERROR, node, 
        "Non-empty 'name' attribute expeected");
      return false;
    }
    templates.PutUnique (name, node);
    return true;
  }

  bool ShaderCombinerLoaderCg::SynthesizeDefaultCoercions (
    const CoercionTemplates& templates)
  {
    iDocumentNode* templNormalize = templates.Get ("normalize", 
      (iDocumentNode*)0);
    if (!templNormalize)
    {
      Report (CS_REPORTER_SEVERITY_ERROR, 
        "No 'normalize' coercion template");
      return false;
    }
    iDocumentNode* templPassthrough = templates.Get ("passthrough", 
      (iDocumentNode*)0);
    if (!templPassthrough)
    {
      Report (CS_REPORTER_SEVERITY_ERROR, 
        "No 'passthrough' coercion template");
      return false;
    }

    csRef<iDocumentSystem> docsys (
      csQueryRegistry<iDocumentSystem> (object_reg));
    if (!docsys.IsValid())
      docsys.AttachNew (new csTinyDocumentSystem);

    ShaderWeaver::TypeInfoIterator typeIt;
    while (typeIt.HasNext())
    {
      csString type;
      const ShaderWeaver::TypeInfo& typeInfo = *typeIt.Next (type);
      iDocumentNode* templ = 0;
      ShaderWeaver::TypeInfo newTypeInfo (typeInfo);
      if (typeInfo.semantics == ShaderWeaver::TypeInfo::Direction)
      {
        if (typeInfo.unit)
        {
          templ = templPassthrough;
          newTypeInfo.unit = false;
        }
        else
        {
          templ = templNormalize;
          newTypeInfo.unit = true;
        }
      }
      else if (typeInfo.space != ShaderWeaver::TypeInfo::NoSpace)
      {
        templ = templPassthrough;
        newTypeInfo.space = ShaderWeaver::TypeInfo::NoSpace;
      }
      const char* newTypeName = ShaderWeaver::QueryType (newTypeInfo);
      if( (templ != 0) && (newTypeName != 0))
      {
        csRef<iDocument> doc = docsys->CreateDocument();
        csRef<iDocumentNode> root = doc->CreateRoot ();
        csRef<iDocumentNode> main = root->CreateNodeBefore (CS_NODE_ELEMENT);
        CS::DocSystem::CloneNode (templ, main);
        main->SetAttribute ("from", type);
        main->SetAttribute ("to", newTypeName);
        ParseCoercion (main);
      }
    }
    return true;
  }

  int ShaderCombinerLoaderCg::CoerceItemCompare (CoerceItem const& i1, 
						 CoerceItem const& i2)
  {
    if (i1.cost < i2.cost)
      return -1;
    else if (i1.cost < i2.cost)
      return 1;
      
    // If two types have same cost, put the more specialized one first.
    const ShaderWeaver::TypeInfo* t1 = ShaderWeaver::QueryTypeInfo (i1.toType);
    const ShaderWeaver::TypeInfo* t2 = ShaderWeaver::QueryTypeInfo (i2.toType);
    if ((t1 == 0) || (t2 == 0)) return 0;
    
    if ((t1->semantics != ShaderWeaver::TypeInfo::NoSemantics)
      && (t2->semantics == ShaderWeaver::TypeInfo::NoSemantics))
      return -1;
    if ((t1->semantics == ShaderWeaver::TypeInfo::NoSemantics)
      && (t2->semantics != ShaderWeaver::TypeInfo::NoSemantics))
      return 1;
    
    if ((t1->space != ShaderWeaver::TypeInfo::NoSpace)
      && (t2->space == ShaderWeaver::TypeInfo::NoSpace))
      return -1;
    if ((t1->space == ShaderWeaver::TypeInfo::NoSpace)
      && (t2->space != ShaderWeaver::TypeInfo::NoSpace))
      return 1;
    
    /*
    if (t1->unit && !t2->unit)
      return -1;
    if (!t1->unit && t2->unit)
      return 1;
    */
    
    return 0;
  }
  
  /* Bit of a kludge... helpers uses by FindCoerceChain, however, can't be made
   * local b/c used as template parameters...
   */
  namespace
  {
    template<typename T>
    struct Hierarchy
    {
      const T* item;
      size_t parent;
      
      Hierarchy (const T* item, size_t parent) : item (item),
        parent (parent) {}
    };
    template<typename T>
    struct TestSource
    {
      csString type;
      const T* item;
      size_t depth;
      size_t hierarchy;
      
      TestSource (const T& item, size_t depth, size_t hierarchy) :
        type (item.toType), item (&item), depth (depth), hierarchy (hierarchy) {}
    };
  }
  
  void ShaderCombinerLoaderCg::FindCoerceChain (const char* from, 
                                                const char* to,
                                                csArray<const CoerceItem*>& chain)
  {
    csArray<Hierarchy<CoerceItem> > hierarchy;
    csFIFO<TestSource<CoerceItem> > sourcesToTest;
    
    const CoerceItems* items = coercions.GetElementPointer (from);
    if (items != 0)
    {
      // Search for direct match
      for (size_t i = 0; i < items->GetSize(); i++)
      {
	if (strcmp (items->Get (i).toType, to) == 0)
	{
	  chain.Push (&items->Get (i));
	  return;
	}
	else
	{
	  // Otherwise, search if no match is found.
          const CoerceItem& item = items->Get (i);
	  sourcesToTest.Push (TestSource<CoerceItem> (item, 
	    0, 
	    hierarchy.Push (Hierarchy<CoerceItem> (&item, 
	      csArrayItemNotFound))));
	}
      }
    }

    // To avoid unnecessary complicated each type can only appear once.
    csSet<csString> seenTypes;
    seenTypes.Add (from);
    // Keep track of used coercions to prevent loops.
    csSet<csConstPtrKey<CoerceItem> > checkedItems;
    while (sourcesToTest.GetSize() > 0)
    {
      TestSource<CoerceItem> testFrom = sourcesToTest.PopTop ();
      if (checkedItems.Contains (testFrom.item)) continue;
      const CoerceItems* items = coercions.GetElementPointer (testFrom.type);
      if (items != 0)
      {
        // Search for direct match
	for (size_t i = 0; i < items->GetSize(); i++)
	{
          const CoerceItem& item = items->Get (i);
	  if (strcmp (item.toType, to) == 0)
	  {
	    // Generate chain
	    size_t d = testFrom.depth+1;
	    chain.SetSize (d+1);
            chain[d] = &item;
            chain[--d] = testFrom.item;
	    size_t h = testFrom.hierarchy;
	    while (d-- > 0)
	    {
	      h = hierarchy[h].parent;
              const CoerceItem* hItem = hierarchy[h].item;
	      chain[d] = hItem;
	    }
            return;
	  }
	  else
	  {
	    // Otherwise, search if no match is found.
            if (!seenTypes.Contains (item.toType))
            {
	      sourcesToTest.Push (TestSource<CoerceItem> (item,
	        testFrom.depth+1, 
	        hierarchy.Push (Hierarchy<CoerceItem> (&item, 
	          testFrom.hierarchy))));
            }
	  }
	}
      }
      checkedItems.AddNoTest (testFrom.item);
      seenTypes.Add (testFrom.type);
    }
  }
  
  //---------------------------------------------------------------------
  
  ShaderCombinerCg::ShaderCombinerCg (ShaderCombinerLoaderCg* loader, 
                                      bool vp, bool fp) : 
    scfImplementationType (this), loader (loader), writeVP (vp), writeFP (fp),
    uniqueCounter (0)
  {
  }
  
  void ShaderCombinerCg::BeginSnippet (const char* annotation)
  {
    currentSnippet.annotation = annotation;
  }
  
  void ShaderCombinerCg::AddInput (const char* name, const char* type)
  {
    if (!currentSnippet.localIDs.Contains (name))
    {
      currentSnippet.localIDs.AddNoTest (name);
      if (loader->annotateCombined)
        currentSnippet.locals.AppendFmt ("// Input: %s %s\n", type, name);
      currentSnippet.locals.AppendFmt ("%s %s;\n", 
	CgType (type).GetData(), name);
    }
  }
  
  void ShaderCombinerCg::AddOutput (const char* name, const char* type)
  {
    if (!currentSnippet.localIDs.Contains (name))
    {
      currentSnippet.localIDs.AddNoTest (name);
      if (loader->annotateCombined)
        currentSnippet.locals.AppendFmt ("// Output: %s %s\n", type, name);
      currentSnippet.locals.AppendFmt ("%s %s;\n", 
	CgType (type).GetData(), name);
    }
  }
  
  void ShaderCombinerCg::InputRename (const char* fromName, 
                                      const char* toName)
  {
    currentSnippet.inputMaps.Put (fromName, toName);
  }
  
  void ShaderCombinerCg::OutputRename (const char* fromName, 
				      const char* toName)
  {
    currentSnippet.outputMaps.Put (fromName, toName);
  }
  
  csPtr<WeaverCommon::iCoerceChainIterator> 
  ShaderCombinerCg::QueryCoerceChain (const char* fromType, const char* toType)
  {
    return loader->QueryCoerceChain (fromType, toType);
  }
  
  void ShaderCombinerCg::Link (const char* fromName, const char* toName)
  {
    currentSnippet.links.AppendFmt ("%s = %s;\n",
      toName, fromName);
  }
      
  void ShaderCombinerCg::WriteBlock (const char* location, 
                                     iDocumentNode* blockNode)
  {
    csRefArray<iDocumentNode>* destNodes = 0;
    if (strcmp (location, "variablemap") == 0)
    {
      destNodes = &variableMaps;
    }
    else if (strcmp (location, "vertexToFragment") == 0)
    {
      destNodes = &currentSnippet.vert2frag;
    }
    else if (strcmp (location, "vertexMain") == 0)
    {
      destNodes = &currentSnippet.vertexBody;
    }
    else if (strcmp (location, "fragmentMain") == 0)
    {
      destNodes = &currentSnippet.fragmentBody;
    }
    else if (strcmp (location, "vertexIn") == 0)
    {
      destNodes = &currentSnippet.vertexIn;
    }
    else if (strcmp (location, "fragmentIn") == 0)
    {
      destNodes = &currentSnippet.fragmentIn;
    }
    else if (strcmp (location, "definitions") == 0)
    {
      destNodes = &definitions;
    }
    
    if (destNodes != 0)
    {
      csRef<iDocumentNodeIterator> nodes = blockNode->GetNodes();
      while (nodes->HasNext())
      {
        csRef<iDocumentNode> next = nodes->Next();
        destNodes->Push (next);
      }
    }
  }
  
  bool ShaderCombinerCg::EndSnippet ()
  {
    for (size_t n = 0; n < currentSnippet.vert2frag.GetSize(); n++)
    {
      iDocumentNode* node = currentSnippet.vert2frag[n];
      if (node->GetType() == CS_NODE_ELEMENT)
      {
        const char* name = node->GetAttributeValue ("name");
        if (name != 0)
        {
          csString uniqueName;
          uniqueName.Format ("%s_%zu", name, uniqueCounter++);
          currentSnippet.v2fMaps.PutUnique (name, uniqueName);
        }
      }
    }

    snippets.Push (currentSnippet);
    currentSnippet = Snippet ();
    return true;
  }
    
  void ShaderCombinerCg::AddGlobal (const char* name, const char* type,
                                    const char* annotation)
  {
    if (!globalIDs.Contains (name))
    {
      globalIDs.AddNoTest (name);
      if (annotation) globals.AppendFmt (MakeComment (annotation));
      globals.AppendFmt ("%s %s;\n", CgType (type).GetData(), name);
    }
  }
    
  void ShaderCombinerCg::SetOutput (const char* name,
                                    const char* annotation)
  {
    outputAssign.Empty();
    if (annotation) outputAssign.AppendFmt (MakeComment (annotation));
    outputAssign.AppendFmt ("outputColor = %s;\n", name);
  }
  
  uint ShaderCombinerCg::CoerceCost (const char* fromType, const char* toType)
  {
    return loader->CoerceCost (fromType, toType);
  }
        
  void ShaderCombinerCg::WriteToPass (iDocumentNode* pass)
  {
    if (writeVP)
    {
      csRef<iDocumentNode> vpNode = pass->CreateNodeBefore (CS_NODE_ELEMENT);
      vpNode->SetValue ("vp");
      vpNode->SetAttribute ("plugin", "glcg");
      
      csRef<iDocumentNode> cgvpNode = vpNode->CreateNodeBefore (CS_NODE_ELEMENT);
      cgvpNode->SetValue ("cgvp");
      
      csRef<iDocumentNode> entryNode = cgvpNode->CreateNodeBefore (CS_NODE_ELEMENT);
      entryNode->SetValue ("entry");
      csRef<iDocumentNode> entryValue = entryNode->CreateNodeBefore (CS_NODE_TEXT);
      entryValue->SetValue ("vertexMain");
      
      for (size_t n = 0; n < variableMaps.GetSize(); n++)
      {
        csRef<iDocumentNode> newNode = 
          cgvpNode->CreateNodeBefore (variableMaps[n]->GetType());
        CS::DocSystem::CloneNode (variableMaps[n], newNode);
      }
      
      csRef<iDocumentNode> programNode = cgvpNode->CreateNodeBefore (CS_NODE_ELEMENT);
      programNode->SetValue ("program");
      
      DocNodeCgAppender appender (programNode);

      if (definitions.GetSize() > 0)
      {
        appender.Append ("\n");
        appender.Append (definitions);
        appender.Append ("\n");
      }
      
      appender.Append ("struct vertex2fragment\n");
      appender.Append ("{\n");
      appender.Append ("  void dummy() {}\n");
      for (size_t s = 0; s < snippets.GetSize(); s++)
      {
        AppendProgramInput_V2FDecl (snippets[s], appender);
      }
      
      appender.Append ("};\n\n");
      
      appender.Append ("struct VertexInput\n");
      appender.Append ("{\n");
      appender.Append ("  void dummy() {}\n");
      for (size_t s = 0; s < snippets.GetSize(); s++)
      {
        AppendProgramInput (snippets[s].vertexIn, appender);
      }
      appender.Append ("};\n\n");
      
      appender.Append ("vertex2fragment vertexMain (VertexInput vertexIn)\n");
      appender.Append ("{\n");
      appender.Append ("  vertex2fragment vertexToFragment;\n");
      appender.Append (globals);
      
      for (size_t s = 0; s < snippets.GetSize(); s++)
      {
        if (!snippets[s].locals.IsEmpty()
          || !snippets[s].inputMaps.IsEmpty()
          || (snippets[s].vertexBody.GetSize() > 0)
          || !snippets[s].outputMaps.IsEmpty())
	{
          if (!snippets[s].annotation.IsEmpty())
            appender.AppendFmt (MakeComment (snippets[s].annotation.GetData()));
	  appender.Append ("{\n");
          if (loader->annotateCombined)
            appender.Append ("// Locally used names for inputs + outputs\n");
	  appender.Append (snippets[s].locals);
          AppendSnippetMap (snippets[s].inputMaps, appender);
	  appender.Append (snippets[s].vertexBody);
          AppendProgramInput_V2FVP (snippets[s], appender);
	  AppendSnippetMap (snippets[s].outputMaps, appender);
	  appender.Append ("}\n");
	}
	appender.Append (snippets[s].links);
        appender.Append ("\n");
      }
      
      appender.Append ("  return vertexToFragment;\n");
      appender.Append ("}\n");
    }
    
    if (writeFP)
    {
      csRef<iDocumentNode> fpNode = pass->CreateNodeBefore (CS_NODE_ELEMENT);
      fpNode->SetValue ("fp");
      fpNode->SetAttribute ("plugin", "glcg");
      
      csRef<iDocumentNode> cgfpNode = fpNode->CreateNodeBefore (CS_NODE_ELEMENT);
      cgfpNode->SetValue ("cgfp");
      
      csRef<iDocumentNode> entryNode = cgfpNode->CreateNodeBefore (CS_NODE_ELEMENT);
      entryNode->SetValue ("entry");
      csRef<iDocumentNode> entryValue = entryNode->CreateNodeBefore (CS_NODE_TEXT);
      entryValue->SetValue ("fragmentMain");
      
      for (size_t n = 0; n < variableMaps.GetSize(); n++)
      {
        csRef<iDocumentNode> newNode = 
          cgfpNode->CreateNodeBefore (variableMaps[n]->GetType());
        CS::DocSystem::CloneNode (variableMaps[n], newNode);
      }
      
      csRef<iDocumentNode> programNode = cgfpNode->CreateNodeBefore (CS_NODE_ELEMENT);
      programNode->SetValue ("program");
      
      DocNodeCgAppender appender (programNode);
      
      if (definitions.GetSize() > 0)
      {
        appender.Append ("\n");
        appender.Append (definitions);
        appender.Append ("\n");
      }
      
      appender.Append ("struct vertex2fragment\n");
      appender.Append ("{\n");
      appender.Append ("  void dummy() {}\n");
      for (size_t s = 0; s < snippets.GetSize(); s++)
      {
        AppendProgramInput_V2FDecl (snippets[s], appender);
      }
      
      appender.Append ("};\n\n");
      
      appender.Append ("struct FragmentInput\n");
      appender.Append ("{\n");
      appender.Append ("  void dummy() {}\n");
      for (size_t s = 0; s < snippets.GetSize(); s++)
      {
        AppendProgramInput (snippets[s].fragmentIn, appender);
      }
      appender.Append ("};\n\n");
      
      appender.Append ("float4 fragmentMain (vertex2fragment vertexToFragment, FragmentInput fragmentIn) : COLOR\n");
      appender.Append ("{\n");
      if (loader->annotateCombined)
        appender.Append ("// Fragment program output\n");
      appender.Append ("  float4 outputColor;\n");
      appender.Append (globals);
      
      for (size_t s = 0; s < snippets.GetSize(); s++)
      {
        if (!snippets[s].locals.IsEmpty()
          || !snippets[s].inputMaps.IsEmpty()
          || (snippets[s].fragmentBody.GetSize() > 0)
          || !snippets[s].outputMaps.IsEmpty())
	{
          if (!snippets[s].annotation.IsEmpty())
            appender.AppendFmt (MakeComment (snippets[s].annotation.GetData()));
	  appender.Append ("{\n");
          if (loader->annotateCombined)
            appender.Append ("// Locally used names for inputs + outputs\n");
	  appender.Append (snippets[s].locals);
          AppendSnippetMap (snippets[s].inputMaps, appender);
          AppendProgramInput_V2FFP (snippets[s], appender);
	  appender.Append (snippets[s].fragmentBody);
          AppendSnippetMap (snippets[s].outputMaps, appender);
	  appender.Append ("}\n");
	}
	appender.Append (snippets[s].links);
        appender.Append ("\n");
      }
      
      appender.Append (outputAssign);
      appender.Append ("  return outputColor;\n");
      appender.Append ("}\n");
    }
  }
    
  bool ShaderCombinerCg::CompatibleParams (iDocumentNode* params)
  {
    return true;
  }
    
  csRef<iString> ShaderCombinerCg::QueryInputTag (const char* location, 
                                                  iDocumentNode* blockNode)
  {
    csRef<iString> result;
    /* @@@ FIXME: Also check vertexToFragment? */
    if ((strcmp (location, "vertexIn") == 0)
      || (strcmp (location, "fragmentIn") == 0))
    {
      csRef<iDocumentNodeIterator> nodes = blockNode->GetNodes();
      while (nodes->HasNext())
      {
        csRef<iDocumentNode> node = nodes->Next();
        if (node->GetType() != CS_NODE_ELEMENT) continue;

        csStringID id = loader->xmltokens.Request (node->GetValue());
        if ((id == ShaderCombinerLoaderCg::XMLTOKEN_UNIFORM)
          || (id == ShaderCombinerLoaderCg::XMLTOKEN_VARYING))
        {
          const char* binding = node->GetAttributeValue ("binding");
          if (!binding || !*binding) continue;
          // For now only support 1 tag...
          if (result.IsValid() 
            && (strcmp (result->GetData(), binding) != 0)) return 0;
          result.AttachNew (new scfString (binding));
        }
      }
    }
    else if (strcmp (location, "variablemap") == 0)
    {
      csRef<iDocumentNodeIterator> nodes = blockNode->GetNodes();
      while (nodes->HasNext())
      {
        csRef<iDocumentNode> node = nodes->Next();
        if (node->GetType() != CS_NODE_ELEMENT) continue;

        csStringID id = loader->xmltokens.Request (node->GetValue());
        if (id == ShaderCombinerLoaderCg::XMLTOKEN_VARIABLEMAP)
        {
          const char* variable = node->GetAttributeValue ("variable");
          if (!variable || !*variable) continue;
          // For now only support 1 tag...
          if (result.IsValid() 
            && (strcmp (result->GetData(), variable) != 0)) return 0;
          result.AttachNew (new scfString (variable));
        }
      }
    }
    return result;
  }

  void ShaderCombinerCg::AppendProgramInput (
    const csRefArray<iDocumentNode>& nodes,
    DocNodeCgAppender& appender)
  {
    // FIXME: error handling here
    for (size_t n = 0; n < nodes.GetSize(); n++)
    {
      iDocumentNode* node = nodes[n];
      AppendProgramInput (node, appender);
    }
  }
  
  void ShaderCombinerCg::AppendProgramInput_V2FDecl (
    const Snippet& snippet, DocNodeCgAppender& appender)
  {
    // FIXME: error handling here
    for (size_t n = 0; n < snippet.vert2frag.GetSize(); n++)
    {
      iDocumentNode* node = snippet.vert2frag[n];
      if (node->GetType() == CS_NODE_ELEMENT)
      {
        csStringID id = loader->xmltokens.Request (node->GetValue());
        if (id == ShaderCombinerLoaderCg::XMLTOKEN_VARYING)
        {
          csString name = node->GetAttributeValue ("name");
          if (name.IsEmpty()) continue;
          const csString& uniqueName = snippet.v2fMaps.Get (name, name);
          csString defineName;
          defineName.Format ("PARAM_vertexToFragment_%s_UNUSED", 
            uniqueName.GetData());
          appender.AppendFmt ("#ifndef %s\n", defineName.GetData());
	  const char* type = node->GetAttributeValue ("type");
	  const char* binding = node->GetAttributeValue ("binding");
	  if (type && *type)
	  {
	    csString bindingStr;
	    if (binding) bindingStr.Format (" : %s", binding);
	    csString str;
            str.Format ("varying %s %s%s;\n", 
              CgType (type).GetData(), uniqueName.GetData(), 
              bindingStr.GetDataSafe());
  	    appender.Append (str);
          }
          appender.Append ("#endif\n");
        }
      }
      else
      {
        AppendProgramInput (node, appender);
      }
    }
  }
  
  void ShaderCombinerCg::AppendProgramInput_V2FVP (
    const Snippet& snippet, DocNodeCgAppender& appender)
  {
    // FIXME: error handling here
    for (size_t n = 0; n < snippet.vert2frag.GetSize(); n++)
    {
      iDocumentNode* node = snippet.vert2frag[n];
      if (node->GetType() == CS_NODE_ELEMENT)
      {
        csStringID id = loader->xmltokens.Request (node->GetValue());
        if (id == ShaderCombinerLoaderCg::XMLTOKEN_VARYING)
        {
          csString name = node->GetAttributeValue ("name");
          const csString& uniqueName = snippet.v2fMaps.Get (name, name);
          csString defineName;
          defineName.Format ("PARAM_vertexToFragment_%s_UNUSED", 
            uniqueName.GetData());
          appender.AppendFmt ("#ifndef %s\n", defineName.GetData());
          appender.AppendFmt ("vertexToFragment.%s = %s;\n", 
            uniqueName.GetData(), name.GetData());
          appender.Append ("#endif\n");
        }
      }
      else
      {
        AppendProgramInput (node, appender);
      }
    }
  }
  
  void ShaderCombinerCg::AppendProgramInput_V2FFP (
    const Snippet& snippet, DocNodeCgAppender& appender)
  {
    // FIXME: error handling here
    for (size_t n = 0; n < snippet.vert2frag.GetSize(); n++)
    {
      iDocumentNode* node = snippet.vert2frag[n];
      if (node->GetType() == CS_NODE_ELEMENT)
      {
        csStringID id = loader->xmltokens.Request (node->GetValue());
        if (id == ShaderCombinerLoaderCg::XMLTOKEN_VARYING)
        {
          csString name = node->GetAttributeValue ("name");
          const csString& uniqueName = snippet.v2fMaps.Get (name, name);
          csString defineName;
          defineName.Format ("PARAM_vertexToFragment_%s_UNUSED", 
            uniqueName.GetData());
          appender.AppendFmt ("#ifndef %s\n", defineName.GetData());
          appender.AppendFmt ("%s = vertexToFragment.%s;\n", 
            name.GetData(), uniqueName.GetData());
          appender.Append ("#else\n");
          appender.AppendFmt ("%s = %s(0);\n", 
            name.GetData(), 
            CgType (node->GetAttributeValue ("type")).GetData());
          appender.Append ("#endif\n");
        }
      }
      else
      {
        AppendProgramInput (node, appender);
      }
    }
  }
  
  void ShaderCombinerCg::AppendProgramInput (iDocumentNode* node,
    DocNodeCgAppender& appender)
  {
    if (node->GetType() == CS_NODE_ELEMENT)
    {
      csStringID id = loader->xmltokens.Request (node->GetValue());
      switch (id)
      {
	case ShaderCombinerLoaderCg::XMLTOKEN_UNIFORM:
	case ShaderCombinerLoaderCg::XMLTOKEN_VARYING:
          {
	    const char* name = node->GetAttributeValue ("name");
	    const char* type = node->GetAttributeValue ("type");
	    const char* binding = node->GetAttributeValue ("binding");
	    if (name && *name && type && *type)
	    {
	      csString bindingStr;
	      if (binding) bindingStr.Format (" : %s", binding);
	      csString str;
              str.Format ("%s %s %s%s;\n", 
	        (id == ShaderCombinerLoaderCg::XMLTOKEN_UNIFORM) ? "uniform" :
	        "varying",
	      CgType (type).GetData(), name, bindingStr.GetDataSafe());
    	      appender.Append (str);
            }
          }
          break;
      }
    }
    else
    {
      appender.Append (node);
    }
  }
  
  csString ShaderCombinerCg::CgType (const char* weaverType)
  {
    const ShaderWeaver::TypeInfo* typeInfo = 
      ShaderWeaver::QueryTypeInfo (weaverType);
    
    if (typeInfo)
    {
      switch (typeInfo->baseType)
      {
	case ShaderWeaver::TypeInfo::Vector:
	case ShaderWeaver::TypeInfo::VectorB:
	case ShaderWeaver::TypeInfo::VectorI:
          {
            static const char* const baseTypeStrs[] = 
            { "float", "bool", "int" };
            const char* baseTypeStr = baseTypeStrs[typeInfo->baseType -
              ShaderWeaver::TypeInfo::Vector];
	    if (typeInfo->dimensions == 1)
	      return baseTypeStr;
	    else
	      return csString().Format ("%s%d", baseTypeStr, typeInfo->dimensions);
          }
	case ShaderWeaver::TypeInfo::Sampler:
	  if (typeInfo->samplerIsCube)
	    return "samplerCUBE";
	  else
	    return csString().Format ("sampler%dD", typeInfo->dimensions);
      }
    }
    return weaverType; // @@@ Hmmm... what fallback, if any?
  }

  const char* ShaderCombinerCg::MakeComment (const char* s)
  {
    const char* linebreak;
    linebreak = strpbrk (s, "\r\n");
    if (strpbrk (s, "\r\n") == 0)
    {
      annotateStr.Format ("// %s\n", s);
    }
    else
    {
      annotateStr.Replace ("/* ");
      const char* lineStart = s;
      while (linebreak != 0)
      {
        annotateStr.Append (lineStart, linebreak - lineStart);
        annotateStr.Append ("\n   ");
        lineStart = linebreak+1;
        linebreak = strpbrk (lineStart, "\r\n");
      }
      annotateStr.Append (lineStart);
        annotateStr.Append ("\n */\n");
    }
    return annotateStr;
  }

  void ShaderCombinerCg::AppendSnippetMap (const csHash<csString, csString>& map, 
                                           DocNodeCgAppender& appender)
  {
    csHash<csString, csString>::ConstGlobalIterator it = map.GetIterator ();
    while (it.HasNext())
    {
      csString fromName;
      const csString& toName = it.Next (fromName);
      appender.AppendFmt ("%s = %s;\n", toName.GetData(), fromName.GetData());
    }
  }
  
  //-------------------------------------------------------------------------
  
  void ShaderCombinerCg::DocNodeCgAppender::FlushAppendString ()
  {
    if (!stringAppend.IsEmpty ())
    {
      csRef<iDocumentNode> newNode = node->CreateNodeBefore (CS_NODE_TEXT);
      newNode->SetValue (stringAppend);
      stringAppend.Empty();
    }
  }
  
  ShaderCombinerCg::DocNodeCgAppender::DocNodeCgAppender (iDocumentNode* node) :
    node (node), beautifier (stringAppend)
  {
    stringAppend.SetGrowsBy (0);
  }
  
  ShaderCombinerCg::DocNodeCgAppender::~DocNodeCgAppender ()
  {
    FlushAppendString();
  }

  void ShaderCombinerCg::DocNodeCgAppender::Append (const char* str)
  {
    if (str == 0) return;
    beautifier.Append (str);
  }
  
  void ShaderCombinerCg::DocNodeCgAppender::Append (iDocumentNode* appendNode)
  {
    csDocumentNodeType nodeType = appendNode->GetType();
    if (nodeType == CS_NODE_TEXT)
    {
      Append (appendNode->GetValue());
    }
    else if (nodeType == CS_NODE_COMMENT)
    {
      // Skip
    }
    else
    {
      FlushAppendString();
      csRef<iDocumentNode> newNode = 
        node->CreateNodeBefore (nodeType);
      CS::DocSystem::CloneNode (appendNode, newNode);
    }
  }

  void ShaderCombinerCg::DocNodeCgAppender::Append (
    const csRefArray<iDocumentNode>& nodes)
  {
    for (size_t n = 0; n < nodes.GetSize(); n++)
    {
      Append (nodes[n]);
    }
  }
}
CS_PLUGIN_NAMESPACE_END(GLShaderCg)
