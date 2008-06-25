/*
  Copyright (C) 2002-2005 by Marten Svanfeldt
			     Anders Stenberg
			     Frank Richter

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

#include "ivaria/reporter.h"

#include "glshader_cg.h"
#include "glshader_cgcommon.h"

CS_PLUGIN_NAMESPACE_BEGIN(GLShaderCg)
{

bool csShaderGLCGCommon::ParseClip (iDocumentNode* node)
{
  Clip newClip;
  
  const char* space = node->GetAttributeValue ("space");
  if (space && *space)
  {
    if (strcmp (space, "eye") == 0)
      newClip.space = ShaderProgramPluginGL::ClipPlanes::Eye;
    else if (strcmp (space, "object") == 0)
      newClip.space = ShaderProgramPluginGL::ClipPlanes::Object;
    else if (strcmp (space, "world") == 0)
      newClip.space = ShaderProgramPluginGL::ClipPlanes::World;
    else
    {
      if (shaderPlug->doVerbose)
        synsrv->Report ("crystalspace.graphics3d.shader.glcg",
          CS_REPORTER_SEVERITY_WARNING,
          node,
          "Invalid 'space' attribute %s", space);
      return false;
    }
  }
  else
    newClip.space = ShaderProgramPluginGL::ClipPlanes::Object;
  
  csRef<iDocumentNode> planeNode = node->GetNode ("plane");
  if (planeNode.IsValid())
  {
    if (!ParseProgramParam (planeNode, newClip.plane,
        ParamVector4 | ParamShaderExp))
      return false;
  }
  else
  {
    newClip.plane.SetValue (csVector4 (0, 0, 1, 0));
  }
  
  newClip.distComp = 0;
  csRef<iDocumentNode> distNode = node->GetNode ("dist");
  if (distNode.IsValid())
  {
    if (!ParseProgramParam (node, newClip.distance,
        ParamVector | ParamShaderExp))
      return false;
      
    const char* compStr = distNode->GetAttributeValue ("comp");
    if (compStr)
    {
      if (strcmp (compStr, "x") == 0)
	newClip.distComp = 0;
      else if (strcmp (compStr, "y") == 0)
	newClip.distComp = 1;
      else if (strcmp (compStr, "z") == 0)
	newClip.distComp = 2;
      else if (strcmp (compStr, "w") == 0)
	newClip.distComp = 3;
      else
      {
	if (shaderPlug->doVerbose)
	  synsrv->Report ("crystalspace.graphics3d.shader.glcg",
	    CS_REPORTER_SEVERITY_WARNING,
	    node,
	    "Invalid 'comp' attribute %s", compStr);
	return false;
      }
    }
  }
  else
  {
    newClip.distance.SetValue (0);
  }
  clips.Push (newClip);
  
  return true;
}

bool csShaderGLCGCommon::Load (iShaderDestinationResolver* resolve, 
			       iDocumentNode* program)
{
  if(!program)
    return false;

  csRef<iShaderManager> shadermgr = 
  	csQueryRegistry<iShaderManager> (shaderPlug->object_reg);

  const char* progTypeNode = 0;
  switch (programType)
  {
    case progVP: progTypeNode = "cgvp"; break;
    case progFP: progTypeNode = "cgfp"; break;
  }
  csRef<iDocumentNode> variablesnode = program->GetNode (progTypeNode);
  if(variablesnode)
  {
    csRef<iDocumentNodeIterator> it = variablesnode->GetNodes ();
    while(it->HasNext())
    {
      csRef<iDocumentNode> child = it->Next();
      if(child->GetType() != CS_NODE_ELEMENT) continue;
      const char* value = child->GetValue ();
      csStringID id = xmltokens.Request (value);
      switch(id)
      {
        case XMLTOKEN_PROFILE:
          cg_profile = child->GetContentsValue ();
          break;
        case XMLTOKEN_ENTRY:
          entrypoint = child->GetContentsValue ();
          break;
        case XMLTOKEN_COMPILERARGS:
          shaderPlug->SplitArgsString (child->GetContentsValue (), 
            compilerArgs);
          break;
	case XMLTOKEN_VARIABLEMAP:
	  {
	    //@@ REWRITE
	    const char* destname = child->GetAttributeValue ("destination");
	    if (!destname)
	    {
	      synsrv->Report ("crystalspace.graphics3d.shader.common",
		CS_REPORTER_SEVERITY_WARNING, child,
		"<variablemap> has no 'destination' attribute");
	      return false;
	    }
	    
	    bool assumeConst = child->GetAttributeValueAsBool ("assumeconst",
	      false);
    
	    const char* varname = child->GetAttributeValue ("variable");
	    if (!varname)
	    {
	      // "New style" variable mapping
	      VariableMapEntry vme (CS::InvalidShaderVarStringID, destname);
	      if (!ParseProgramParam (child, vme.mappingParam,
		ParamFloat | ParamVector2 | ParamVector3 | ParamVector4))
		return false;
	      ShaderParameter* sparam = shaderPlug->paramAlloc.Alloc();
	      sparam->assumeConstant = assumeConst;
	      vme.userVal = reinterpret_cast<intptr_t> (sparam);
	      variablemap.Push (vme);
	    }
	    else
	    {
	      // "Classic" variable mapping
	      CS::Graphics::ShaderVarNameParser nameParse (varname);
	      VariableMapEntry vme (
		stringsSvName->Request (nameParse.GetShaderVarName()),
		destname);
	      for (size_t n = 0; n < nameParse.GetIndexNum(); n++)
	      {
		vme.mappingParam.indices.Push (nameParse.GetIndexValue (n));
	      }
	      ShaderParameter* sparam = shaderPlug->paramAlloc.Alloc();
	      sparam->assumeConstant = assumeConst;
	      vme.userVal = reinterpret_cast<intptr_t> (sparam);
	      variablemap.Push (vme);
	    }
	  }
	  break;
	case XMLTOKEN_CLIP:
	  if (!ParseClip (child))
	    return false;
	  break;
        default:
	  if (!ParseCommon (child))
	    return false;
      }
    }
  }

  cgResolve = scfQueryInterfaceSafe<iShaderDestinationResolverCG> (resolve);
  clips.ShrinkBestFit ();
  ClipsToVmap ();

  return true;
}

}
CS_PLUGIN_NAMESPACE_END(GLShaderCg)
