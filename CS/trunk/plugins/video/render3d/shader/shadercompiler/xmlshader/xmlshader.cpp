/*
  Copyright (C) 2003 by Marten Svanfeldt

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
#include "iutil/comp.h"
#include "iutil/plugin.h"
#include "iutil/vfs.h"
#include "iutil/cmdline.h"
#include "imap/services.h"
#include "ivaria/reporter.h"
#include "ivideo/rendermesh.h"
#include "csutil/util.h"
#include "csutil/scfstr.h"
#include "csutil/scfstrset.h"
#include "csutil/xmltiny.h"
#include "xmlshader.h"

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_FACTORY (csXMLShaderCompiler)

SCF_IMPLEMENT_IBASE(csXMLShaderCompiler)
  SCF_IMPLEMENTS_INTERFACE(iComponent)
  SCF_IMPLEMENTS_INTERFACE(iShaderCompiler)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_IBASE(csXMLShader)
  SCF_IMPLEMENTS_INTERFACE(iShader)
SCF_IMPLEMENT_IBASE_END

csXMLShaderCompiler::csXMLShaderCompiler(iBase* parent)
{
  SCF_CONSTRUCT_IBASE(parent);
  init_token_table (xmltokens);
  fail_reason = 0;
}

csXMLShaderCompiler::~csXMLShaderCompiler()
{
  delete[] fail_reason;
  SCF_DESTRUCT_IBASE();
}

void csXMLShaderCompiler::SetFailReason (const char* reason)
{
  delete[] fail_reason;
  fail_reason = csStrNew (reason);
}

void csXMLShaderCompiler::Report (int severity, const char* msg, ...)
{
  va_list args;
  va_start (args, msg);
  csReportV (objectreg, severity, 
    "crystalspace.graphics3d.shadercompiler.xmlshader", msg, args);
  va_end (args);
}

bool csXMLShaderCompiler::Initialize (iObjectRegistry* object_reg)
{
  objectreg = object_reg;

  strings = CS_QUERY_REGISTRY_TAG_INTERFACE (
    object_reg, "crystalspace.shared.stringset", iStringSet);

  g3d = CS_QUERY_REGISTRY (object_reg, iGraphics3D);
  synldr = CS_QUERY_REGISTRY (object_reg, iSyntaxService);
  csRef<iCommandLineParser> cmdline =
    CS_QUERY_REGISTRY (object_reg, iCommandLineParser);
  if (cmdline)
    do_verbose = (cmdline->GetOption ("verbose") != 0);
  else
    do_verbose = false;

  return true;
}

int csXMLShaderCompiler::CompareTechniqueKeeper (void const* item1,
	void const* item2)
{
  techniqueKeeper *t1, *t2;
  t1 = (techniqueKeeper*) item1;
  t2 = (techniqueKeeper*) item2;
  if (t1->priority < t2->priority)
    return 1;
  else if (t1->priority > t2->priority)
    return -1;
  return 0;
}

csPtr<iShader> csXMLShaderCompiler::CompileShader (iDocumentNode *templ)
{
  if (!ValidateTemplate (templ))
    return 0;

  csRef<iDocumentNodeIterator> it = templ->GetNodes();

  csArray<techniqueKeeper> techniquesTmp;

  // Read in the techniques.
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () == CS_NODE_ELEMENT &&
      xmltokens.Request (child->GetValue ()) == XMLTOKEN_TECHNIQUE)
    {
      //save it
      unsigned int p = child->GetAttributeValueAsInt ("priority");
      techniquesTmp.Push (techniqueKeeper (child, p));
    }
  }

  techniquesTmp.Sort (&CompareTechniqueKeeper);

  //now try to load them one in a time, until we are successful
  csRef<csXMLShader> shader;
  const char* shaderName = templ->GetAttributeValue ("name");
  csArray<techniqueKeeper>::Iterator techIt = techniquesTmp.GetIterator ();
  while (techIt.HasNext ())
  {
    techniqueKeeper tk = techIt.Next();
    shader = CompileTechnique (tk.node, shaderName, templ);
    if (shader.IsValid())
    {
      if (do_verbose)
	Report (CS_REPORTER_SEVERITY_NOTIFY,
	  "Shader '%s': Technique with priority %d succeeds!",
	  shaderName, tk.priority);
      break;
    }
    else
    {
      if (do_verbose)
	Report (CS_REPORTER_SEVERITY_NOTIFY,
	  "Shader '%s': Technique with priority %d fails. Reason: %s.",
	  shaderName, tk.priority,
	  fail_reason);
    }
  }

  if (!shader.IsValid())
  {
    // @@@ Or a warning instead?
    Report (CS_REPORTER_SEVERITY_WARNING,
      "No technique validated for shader '%s'", shaderName);
  }

  csRef<iShader> ishader(shader);
  return csPtr<iShader>(ishader);
}

csPtr<csXMLShader> csXMLShaderCompiler::CompileTechnique (
	iDocumentNode *node,
	const char* shaderName,
	iDocumentNode *parentSV)
{
  //check nodetype
  if (node->GetType()!=CS_NODE_ELEMENT || 
      xmltokens.Request (node->GetValue()) != XMLTOKEN_TECHNIQUE)
  {
    if (do_verbose) SetFailReason ("Node is not a well formed technique");
    return 0;
  }

  csRef<csXMLShader> newShader = csPtr<csXMLShader> (new csXMLShader (g3d));
  newShader->name = csStrNew (shaderName);

  //count passes
  newShader->passesCount = 0;
  csRef<iDocumentNodeIterator> it = node->GetNodes (
    xmltokens.Request (XMLTOKEN_PASS));
  while(it->HasNext ())
  {
    it->Next ();
    newShader->passesCount++;
  }

  //load shadervariable definitions
  if (parentSV)
  {
    csRef<iDocumentNode> varNode = parentSV->GetNode(
      xmltokens.Request (XMLTOKEN_SHADERVARS));
    if (varNode)
      LoadSVBlock (varNode, newShader);
  }

  csRef<iDocumentNode> varNode = node->GetNode(
    xmltokens.Request (XMLTOKEN_SHADERVARS));

  if (varNode)
    LoadSVBlock (varNode, newShader);

  //alloc passes
  newShader->passes = 
    new csXMLShader::shaderPass[newShader->passesCount];
  uint i;
  for (i = 0; i < newShader->passesCount; i++)
  {
    csXMLShader::shaderPass& pass = newShader->passes[i];
    pass.alphaMode.autoAlphaMode = true;
    pass.alphaMode.autoModeTexture = 
      strings->Request (CS_MATERIAL_TEXTURE_DIFFUSE);
  }


  //first thing we load is the programs for each pass.. if one of them fail,
  //fail the whole technique
  int currentPassNr = 0;
  it = node->GetNodes(xmltokens.Request (XMLTOKEN_PASS));
  while (it->HasNext ())
  {
    csRef<iDocumentNode> passNode = it->Next ();
    newShader->passes[currentPassNr].owner = newShader;
    if (!LoadPass(passNode, &newShader->passes[currentPassNr++]))
    {
      return 0;
    }
  }

  return csPtr<csXMLShader> (newShader);
}

bool csXMLShaderCompiler::LoadPass (iDocumentNode *node, 
				    csXMLShader::shaderPass *pass)
{
  //Load shadervar block
  csRef<iDocumentNode> varNode = node->GetNode(
    xmltokens.Request (XMLTOKEN_SHADERVARS));
 
  if (varNode)
    LoadSVBlock (varNode, &pass->svcontext);

  //load vp
  csRef<iDocumentNode> programNode;
  csRef<iShaderProgram> program;
  programNode = node->GetNode(xmltokens.Request (XMLTOKEN_VP));

  if (programNode)
  {
    program = LoadProgram (programNode, pass);
    if (program)
      pass->vp = program;
    else
    {
      if (do_verbose)
        SetFailReason ("vertex program failed to load");
      else
        Report (CS_REPORTER_SEVERITY_WARNING, 
	  "Vertex Program for shader '%s' failed to load",
	  pass->owner->GetName ());
      return false;
    }
  }

  programNode = node->GetNode(xmltokens.Request (XMLTOKEN_FP));

  if (programNode)
  {
    program = LoadProgram (programNode, pass);
    if (program)
      pass->fp = program;
    else
    {
      if (do_verbose)
        SetFailReason ("fragment program failed to load");
      else
        Report (CS_REPORTER_SEVERITY_WARNING, 
	  "Fragment Program for shader '%s' failed to load",
	  pass->owner->GetName ());
      return false;
    }
  }

  {
    csRef<iDocumentNode> nodeMixMode = node->GetNode ("mixmode");
    if (nodeMixMode != 0)
    {
      uint mm;
      if (synldr->ParseMixmode (nodeMixMode, mm, true))
	pass->mixMode = mm;
    }

    csRef<iDocumentNode> nodeAlphaMode = node->GetNode ("alphamode");
    if (nodeAlphaMode != 0)
    {
      csAlphaMode am;
      if (synldr->ParseAlphaMode (nodeAlphaMode, strings, am))
      {
	pass->alphaMode = am;
      }
    }

    csRef<iDocumentNode> nodeZmode = node->GetNode ("zmode");
    if (nodeZmode != 0)
    {
      csRef<iDocumentNode> node;
      csRef<iDocumentNodeIterator> it;
      it = nodeZmode->GetNodes ();
      while (it->HasNext ())
      {
	csRef<iDocumentNode> child = it->Next ();
	if (child->GetType () != CS_NODE_ELEMENT) continue;
	node = child;
	break;
      }

      if (synldr->ParseZMode (node, pass->zMode, true))
      {
	pass->overrideZmode = true;
      }
      else
      {
	synldr->ReportBadToken (node);
      }
    }

    pass->wmRed = true;
    pass->wmGreen = true;
    pass->wmBlue = true;
    pass->wmAlpha = true;
    csRef<iDocumentNode> nodeWM = node->GetNode ("writemask");
    if (nodeWM != 0)
    {
      if (nodeWM->GetAttributeValue ("r") != 0)
	pass->wmRed = !strcasecmp (nodeWM->GetAttributeValue ("r"), "true");
      if (nodeWM->GetAttributeValue ("g") != 0)
	pass->wmGreen = !strcasecmp (nodeWM->GetAttributeValue ("g"), "true");
      if (nodeWM->GetAttributeValue ("b") != 0)
	pass->wmBlue = !strcasecmp (nodeWM->GetAttributeValue ("b"), "true");
      if (nodeWM->GetAttributeValue ("a") != 0)
	pass->wmAlpha = !strcasecmp (nodeWM->GetAttributeValue ("a"), "true");
    }
  }
  if (pass->alphaMode.autoAlphaMode)
  {
    if (pass->alphaMode.autoModeTexture != csInvalidStringID)
    {
      csShaderVariable *var;
      var = pass->svcontext.GetVariable (pass->alphaMode.autoModeTexture);

      if(!var)
	var = pass->owner->GetVariable (pass->alphaMode.autoModeTexture);
      if (var)
	pass->autoAlphaTexRef = var;
    }
  }

  //if we got this far, load buffermappings
  csRef<iDocumentNodeIterator> it;
  it = node->GetNodes(xmltokens.Request (XMLTOKEN_BUFFER));
  pass->bufferCount = 0;
  while(it->HasNext ())
  {
    csRef<iDocumentNode> mapping = it->Next ();
    if (mapping->GetType () != CS_NODE_ELEMENT) continue;
    
    const char* dest = mapping->GetAttributeValue ("destination");
    csVertexAttrib attrib = CS_VATTRIB_0;
    bool found = false;
    int i;
    for(i=0;i<16;i++)
    {
      char str[13];
      sprintf (str, "attribute %d", i);
      if (strcasecmp(str, dest)==0)
      {
        found = true;
        attrib = (csVertexAttrib)(CS_VATTRIB_0 + i);
        break;
      }
    }
    if (!found)
    {
      if (strcasecmp (dest, "position") == 0)
      {
        attrib = CS_VATTRIB_POSITION;
        found = true;
      }
      else if (strcasecmp (dest, "normal") == 0)
      {
        attrib = CS_VATTRIB_NORMAL;
        found = true;
      }
      else if (strcasecmp (dest, "color") == 0)
      {
        attrib = CS_VATTRIB_COLOR;
        found = true;
      }
      else if (strcasecmp (dest, "primary color") == 0)
      {
        attrib = CS_VATTRIB_PRIMARY_COLOR;
        found = true;
      }
      else if (strcasecmp (dest, "secondary color") == 0)
      {
        attrib = CS_VATTRIB_SECONDARY_COLOR;
        found = true;
      }
      else if (strcasecmp (dest, "texture coordinate") == 0)
      {
        attrib = CS_VATTRIB_TEXCOORD;
        found = true;
      }
      else
      {
        static const char mapName[] = "texture coordinate %d";
        char buf[sizeof (mapName)];

        for (int u = 0; u < 8; u++)
        {
          sprintf (buf, mapName, u);
          if (strcasecmp (dest, buf) == 0)
          {
            attrib = (csVertexAttrib)((int)CS_VATTRIB_TEXCOORD0 + u);
            found = true;
            break;
          }
        }
      }
    }
    if (found)
    {
      //int a = CS_VATTRIB_IS_SPECIFIC (attrib) ? 
      //attrib - CS_VATTRIB_SPECIFIC_FIRST : attrib;
      csStringID varID = strings->Request (mapping->GetAttributeValue("name"));
      pass->bufferID[pass->bufferCount] = varID; //MUST HAVE STRINGS
      pass->bufferGeneric[pass->bufferCount] = CS_VATTRIB_IS_GENERIC (attrib);

      csShaderVariable *varRef=0;
      //csRef<csShaderVariable>& varRef = pass->bufferRef[a];
      varRef = pass->svcontext.GetVariable(pass->bufferID[pass->bufferCount]);

      if(!varRef)
        varRef = pass->owner->GetVariable (
		pass->bufferID[pass->bufferCount]);

      if (varRef)
	pass->bufferRef[pass->bufferCount] = varRef;
      pass->vertexattributes[pass->bufferCount] = attrib;
      pass->bufferCount++;
      //pass->bufferCount = MAX (pass->bufferCount, a + 1);
    }
    else
    {
      Report (CS_REPORTER_SEVERITY_WARNING,
	"Shader '%s', pass %d: invalid buffer destination '%s'",
	pass->owner->GetName (), pass->owner->GetPassNumber (pass),
	dest);
      
    }
  }

  if (pass->bufferCount == 0)
  {
    Report (CS_REPORTER_SEVERITY_WARNING,
      "Shader '%s', pass %d has no buffer mappings",
      pass->owner->GetName (), pass->owner->GetPassNumber (pass));
    
  }

  //get texturemappings
  pass->textureCount = 0;
  it = node->GetNodes (xmltokens.Request (XMLTOKEN_TEXTURE));
  while(it->HasNext ())
  {
    csRef<iDocumentNode> mapping = it->Next ();
    if (mapping->GetType() != CS_NODE_ELEMENT) continue;
    if (mapping->GetAttribute("name") && mapping->GetAttribute("destination"))
    {
      const char* dest = mapping->GetAttributeValue ("destination");
      char unitName[8];
      int i;
      int texUnit = -1;
      for (i = 0; i < csXMLShader::shaderPass::TEXTUREMAX; i++)
      {
	sprintf (unitName, "unit %d", i);
	if (strcasecmp(unitName, dest) == 0)
	{
	  texUnit = i;
	  break;
	}
      }
      if (texUnit < 0) continue;
      csStringID varID = strings->Request (mapping->GetAttributeValue("name"));
      pass->textureID[texUnit] = varID;
      //pass->textureUnits[pass->textureCount] = texUnit;

      csShaderVariable *varRef=0;
      //csRef<csShaderVariable>& varRef = pass->bufferRef[a];
      varRef = pass->svcontext.GetVariable (varID);

      if(!varRef)
        varRef = pass->owner->GetVariable (varID);

      if (varRef)
	pass->textureRef[texUnit] = varRef;

      pass->textureCount = MAX(pass->textureCount, texUnit + 1);
    }
  }

  if (pass->textureCount == 0)
  {
    Report (CS_REPORTER_SEVERITY_WARNING,
      "Shader '%s', pass %d has no texture mappings",
      pass->owner->GetName (), pass->owner->GetPassNumber (pass));
    
  }

  return true;
}

bool csXMLShaderCompiler::LoadSVBlock (iDocumentNode *node,
  iShaderVariableContext *context)
{
  csRef<csShaderVariable> svVar;
  
  csRef<iDocumentNodeIterator> it = node->GetNodes ("shadervar");
  while (it->HasNext ())
  {
    csRef<iDocumentNode> var = it->Next ();
    svVar.AttachNew (new csShaderVariable (
      strings->Request(var->GetAttributeValue ("name"))));

    if (synldr->ParseShaderVar (var, *svVar))
      context->AddVariable(svVar);
  }

  return true;
}

csPtr<iShaderProgram> csXMLShaderCompiler::LoadProgram (
  iDocumentNode *node, csXMLShader::shaderPass *pass)
{
  if (node->GetAttributeValue("plugin") == 0)
  {
    Report (CS_REPORTER_SEVERITY_WARNING,
      "No shader program plugin specified for <%s> in shader '%s'",
      node->GetValue (), pass->owner->GetName ());
    return 0;
  }

  csRef<iShaderProgram> program;

  const char *pluginprefix = "crystalspace.graphics3d.shader.";
  char *plugin = new char[strlen(pluginprefix) + 255 + 1];
  strcpy (plugin, pluginprefix);

  strncat (plugin, node->GetAttributeValue("plugin"), 255);
  // @@@ Also check if 'plugin' is a full class ID

  //load the plugin
  csRef<iPluginManager> plugin_mgr = CS_QUERY_REGISTRY  (objectreg,
    iPluginManager);

  csRef<iShaderProgramPlugin> plg;
  plg = CS_QUERY_PLUGIN_CLASS(plugin_mgr, plugin, iShaderProgramPlugin);
  if(!plg)
  {
    plg = CS_LOAD_PLUGIN(plugin_mgr, plugin, iShaderProgramPlugin);
    if (!plg)
    {
      Report (CS_REPORTER_SEVERITY_WARNING,
	"Couldn't retrieve shader plugin '%s' for <%s> in shader '%s'",
	plugin, node->GetValue (), pass->owner->GetName ());
      delete[] plugin;
      return 0;
    }
  }

  delete[] plugin;
  const char* programType = node->GetAttributeValue("type");
  if (programType == 0)
    programType = node->GetValue ();
  program = plg->CreateProgram (programType);
  if (program == 0)
    return 0;
  csRef<iDocumentNode> programNode;
  if (node->GetAttributeValue ("file") != 0)
  {
    csRef<iVFS> VFS = CS_QUERY_REGISTRY(objectreg, iVFS);
    csRef<iFile> programFile = VFS->Open (node->GetAttributeValue ("file"),
      VFS_FILE_READ);
    if(!programFile)
    {
      Report (CS_REPORTER_SEVERITY_WARNING,
        "Unable to load shader program '%s'",
        node->GetAttributeValue ("file"));
      return 0;
    }
    csRef<iDocumentSystem> docsys (
      CS_QUERY_REGISTRY(objectreg, iDocumentSystem));
    if (docsys == 0)
      docsys.AttachNew (new csTinyDocumentSystem ());

    csRef<iDocument> programDoc = docsys->CreateDocument ();
    programDoc->Parse (programFile);
    /*csRef<iDocumentNodeIterator> it = programDoc->GetRoot ()->GetNodes ();
    while (it->HasNext ())
    {
      csRef<iDocumentNode> child = it->Next ();
      if (child->GetType () != CS_NODE_ELEMENT) continue;
      programNode = child;
      break;
    }*/
    programNode = programDoc->GetRoot ();
  }
  else
    programNode = node;
  program->Load (programNode);

  csArray<iShaderVariableContext*> staticContexts;
  staticContexts.Push (&pass->svcontext);
  staticContexts.Push (pass->owner);
  if (!program->Compile (staticContexts))
    return 0;

  return csPtr<iShaderProgram> (program);
}

bool csXMLShaderCompiler::ValidateTemplate(iDocumentNode *templ)
{
  if (!IsTemplateToCompiler(templ))
    return false;

  /*@@TODO: Validation without accual compile. should check correct xml
  syntax, and that we have at least one techqniue which can load. Also check
  that we have valid texturemapping and buffermapping*/

  return true;
}

bool csXMLShaderCompiler::IsTemplateToCompiler(iDocumentNode *templ)
{
  //Check that we got an element
  if (templ->GetType() != CS_NODE_ELEMENT) return false;

  //With name shader  (<shader>....</shader>
  if (xmltokens.Request (templ->GetValue())!=XMLTOKEN_SHADER) return false;

  //Check the type-string in <shader>
  const char* shaderName = templ->GetAttributeValue ("name");
  const char* shaderType = templ->GetAttributeValue ("type");
  if ((shaderType == 0) || (xmltokens.Request (shaderType) != 
    XMLTOKEN_XMLSHADER))
  {
    Report (CS_REPORTER_SEVERITY_WARNING, 
      "Type of shader '%s' is not 'xmlshader', but '%s'",
      shaderName, shaderType);
    return false;
  }

  //Check that we have children, no children == not a template to this one at
  //least.
  if (!templ->GetNodes()->HasNext()) return false;

  //Ok, passed check. We will try to validate it
  return true;
}

iRenderBuffer* csXMLShader::last_buffers[shaderPass::STREAMMAX*2];
iRenderBuffer* csXMLShader::clear_buffers[shaderPass::STREAMMAX*2];
//csVertexAttrib csXMLShader::vertexattributes[shaderPass::STREAMMAX*2];
int csXMLShader::lastBufferCount;

iTextureHandle* csXMLShader::last_textures[shaderPass::TEXTUREMAX];
iTextureHandle* csXMLShader::clear_textures[shaderPass::TEXTUREMAX];
int csXMLShader::textureUnits[shaderPass::TEXTUREMAX];
int csXMLShader::lastTexturesCount;

csXMLShader::csXMLShader (iGraphics3D* g3d) : passes(NULL), passesCount(0), 
  currentPass(~0)
{
  SCF_CONSTRUCT_IBASE(0);

  name = 0; 
  csXMLShader::g3d = g3d;
  int i;
  for (i = 0; i < shaderPass::TEXTUREMAX; i++)
    textureUnits[i] = i;
}

csXMLShader::~csXMLShader ()
{
  delete [] passes;
  delete[] name;
  SCF_DESTRUCT_IBASE();
}

bool csXMLShader::ActivatePass (unsigned int number)
{
  if(number>=passesCount)
    return false;

  currentPass = number;

  shaderPass *thispass = &passes[currentPass];
  if(thispass->vp) thispass->vp->Activate ();
  if(thispass->fp) thispass->fp->Activate ();
  
  if (thispass->overrideZmode)
  {
    oldZmode = g3d->GetZMode ();
    g3d->SetZMode (thispass->zMode);
  }

  g3d->GetWriteMask (orig_wmRed, orig_wmGreen, orig_wmBlue, orig_wmAlpha);
  g3d->SetWriteMask (thispass->wmRed, thispass->wmGreen, thispass->wmBlue,
    thispass->wmAlpha);

  return true;
}

bool csXMLShader::DeactivatePass ()
{
  if(currentPass>=passesCount)
    return false;
  shaderPass *thispass = &passes[currentPass];
  currentPass = ~0;

  if(thispass->vp) thispass->vp->Deactivate ();
  if(thispass->fp) thispass->fp->Deactivate ();

  g3d->SetBufferState(thispass->vertexattributes, clear_buffers, 
    lastBufferCount);
  lastBufferCount=0;

  g3d->SetTextureState(textureUnits, clear_textures, 
    lastTexturesCount);
  lastTexturesCount=0;
  
  if (thispass->overrideZmode)
    g3d->SetZMode (oldZmode);

  g3d->SetWriteMask (orig_wmRed, orig_wmGreen, orig_wmBlue, orig_wmAlpha);

  return true;
}

bool csXMLShader::SetupPass (csRenderMesh *mesh, 
	const CS_SHADERVAR_STACK &stacks)
{
  if(currentPass>=passesCount)
    return false;

  shaderPass *thispass = &passes[currentPass];

  //now map our buffers. all refs should be set
  int i;
  for (i = 0; i < thispass->bufferCount; i++)
  {
    if (thispass->bufferRef[i] != 0)
      thispass->bufferRef[i]->GetValue(last_buffers[i]);
    else if (thispass->bufferID[i] < (csStringID)stacks.Length ())
    {
      csShaderVariable* var = 0;
      if (stacks[thispass->bufferID[i]].Length () > 0)
        var = stacks[thispass->bufferID[i]].Top ();
      if (var)
        var->GetValue(last_buffers[i]);
      else
        last_buffers[i] = 0;
    }
    else
      last_buffers[i] = 0;
  }
  g3d->SetBufferState (thispass->vertexattributes, last_buffers, 
    thispass->bufferCount);
  lastBufferCount = thispass->bufferCount;

  //and the textures
  for (i = 0; i < thispass->textureCount; i++)
  {
    if (thispass->textureRef[i] != 0)
    {
      iTextureWrapper* wrap;
      thispass->textureRef[i]->GetValue(wrap);
      if (wrap)
      {
        wrap->Visit ();
        last_textures[i] = wrap->GetTextureHandle ();
      }
      else
        last_textures[i] = 0;
    }
    else if (thispass->textureID[i] < (csStringID)stacks.Length ())
    {
      csShaderVariable* var = 0;
      if (stacks[thispass->textureID[i]].Length () > 0)
        var = stacks[thispass->textureID[i]].Top ();
      if (var)
      {
        iTextureWrapper* wrap;
        var->GetValue(wrap);
        if (wrap) 
        {
          wrap->Visit ();
          last_textures[i] = wrap->GetTextureHandle ();
        } else 
          var->GetValue(last_textures[i]);
      } else
        last_textures[i] = 0;
    }
    else
      last_textures[i] = 0;
  }
  g3d->SetTextureState (textureUnits, last_textures, 
    thispass->textureCount);
  lastTexturesCount = thispass->textureCount;

  // @@@ Is it okay to modify the render mesh here?
  if (thispass->alphaMode.autoAlphaMode)
  {
    iTextureHandle* tex = 0;
    if (thispass->autoAlphaTexRef != 0)
      thispass->autoAlphaTexRef->GetValue (tex);
    else if (thispass->alphaMode.autoModeTexture != csInvalidStringID)
    {
      if (thispass->alphaMode.autoModeTexture < (csStringID)stacks.Length ())
      {
        csShaderVariable* var = 0;
        if (stacks[thispass->alphaMode.autoModeTexture].Length ()>0)
          var = stacks[thispass->alphaMode.autoModeTexture].Top ();
        if (var)
          var->GetValue (tex);
      }
    }
    if (tex != 0)
      mesh->alphaType = tex->GetAlphaType ();
    else
      mesh->alphaType = csAlphaMode::alphaNone;
  }
  else
    mesh->alphaType = thispass->alphaMode.alphaType;
  // Override mixmode, if requested
  if ((thispass->mixMode & CS_FX_MASK_MIXMODE) == CS_FX_MESH)
  {
    if ((mesh->mixmode & CS_FX_MASK_MIXMODE) == CS_FX_ALPHA)
      mesh->alphaType = csAlphaMode::alphaSmooth;
  }
  else
    mesh->mixmode = thispass->mixMode;

  if(thispass->vp) thispass->vp->SetupState (mesh, stacks);
  if(thispass->fp) thispass->fp->SetupState (mesh, stacks);

  return true;
}

bool csXMLShader::TeardownPass ()
{
  shaderPass *thispass = &passes[currentPass];

  if(thispass->vp) thispass->vp->ResetState ();
  if(thispass->fp) thispass->fp->ResetState ();

  return true;
}

int csXMLShader::GetPassNumber (shaderPass* pass)
{
  if ((pass >= passes) && (pass < passes + passesCount))
  {
    return pass - passes;
  }
  else
    return -1;
}
