/*
Copyright (C) 2002 by M�rten Svanfeldt
                      Anders Stenberg

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

#include "csutil/csvector.h"
#include "csutil/hashmap.h"
#include "csutil/objreg.h"
#include "csutil/ref.h"
#include "csutil/scf.h"
#include "csutil/csmd5.h"
#include "csutil/scfstr.h"
#include "csgeom/vector3.h"
#include "csutil/xmltiny.h"

#include "iengine/engine.h"
#include "iutil/document.h"
#include "iutil/comp.h"
#include "iutil/plugin.h"
#include "ivideo/material.h"
#include "ivaria/reporter.h"
#include "ivideo/graph2d.h"
#include "ivideo/render3d.h"
#include "ivideo/rndbuf.h"
#include "ivideo/shader/shader.h"
#include "video/canvas/openglcommon/glstates.h"

#include "../../opengl/glextmanager.h"
#include "../../opengl/gl_txtmgr.h"
#include "../../common/txtmgr.h"
#include "../../opengl/gl_txtcache.h"

#include "glshader_mtex.h"


CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_FACTORY (csGLShader_MTEX)

SCF_EXPORT_CLASS_TABLE (glshader_mtex)
SCF_EXPORT_CLASS_DEP (csGLShader_MTEX, "crystalspace.render3d.shader.glmtex",
                      "OpenGL specific shaderprogram provider for Render3D",
                      "")
SCF_EXPORT_CLASS_TABLE_END


SCF_IMPLEMENT_IBASE(csGLShader_MTEX)
SCF_IMPLEMENTS_INTERFACE(iShaderProgramPlugin)
SCF_IMPLEMENTS_EMBEDDED_INTERFACE(iComponent)
SCF_IMPLEMENT_IBASE_END

SCF_IMPLEMENT_EMBEDDED_IBASE (csGLShader_MTEX::eiComponent)
SCF_IMPLEMENTS_INTERFACE (iComponent)
SCF_IMPLEMENT_EMBEDDED_IBASE_END


csGLShader_MTEX::csGLShader_MTEX(iBase* parent)
{
  SCF_CONSTRUCT_IBASE (parent);
  SCF_CONSTRUCT_EMBEDDED_IBASE(scfiComponent);
}

csGLShader_MTEX::~csGLShader_MTEX()
{

}

////////////////////////////////////////////////////////////////////
//                      iShaderProgramPlugin
////////////////////////////////////////////////////////////////////
bool csGLShader_MTEX::SupportType(const char* type)
{
  if( strcasecmp(type, "gl_mtex_fp") == 0)
    return true;
  return false;
}

csPtr<iShaderProgram> csGLShader_MTEX::CreateProgram(const char* type)
{
  return csPtr<iShaderProgram>(new csShaderGLMTEX(object_reg));;
}

void csGLShader_MTEX::Open()
{
  if(!object_reg)
    return;

  csRef<iRender3D> r = CS_QUERY_REGISTRY(object_reg,iRender3D);
  csRef<iShaderRenderInterface> sri = SCF_QUERY_INTERFACE(r, iShaderRenderInterface);

  ext = (csGLExtensionManager*) sri->GetPrivateObject("ext");
}

csPtr<iString> csGLShader_MTEX::GetProgramID(const char* programstring)
{
  csMD5::Digest d = csMD5::Encode(programstring);
  scfString* str = new scfString();
  str->Append((const char*)d.data[0], 16);
  return csPtr<iString>(str);
}

////////////////////////////////////////////////////////////////////
//                          iComponent
////////////////////////////////////////////////////////////////////
bool csGLShader_MTEX::Initialize(iObjectRegistry* reg)
{
  object_reg = reg;
  return true;
}



////////////////////////////////////////////////////////////////////
//                          csShaderGLMTEX
////////////////////////////////////////////////////////////////////

SCF_IMPLEMENT_IBASE(csShaderGLMTEX)
SCF_IMPLEMENTS_INTERFACE(iShaderProgram)
SCF_IMPLEMENT_IBASE_END

csShaderGLMTEX::csShaderGLMTEX(iObjectRegistry* objreg)
{
  SCF_CONSTRUCT_IBASE (NULL);
  this->object_reg = objreg;
  this->ext = ext;
  programstring = NULL;
  validProgram = true;

  
}

void csShaderGLMTEX::BuildTokenHash()
{
  xmltokens.Register ("alphaoperation", XMLTOKEN_ALPHAOP);
  xmltokens.Register ("alphasource", XMLTOKEN_ALPHASOURCE);
  xmltokens.Register ("coloroperation", XMLTOKEN_COLOROP);
  xmltokens.Register ("colorsource", XMLTOKEN_COLORSOURCE);
  xmltokens.Register ("texturesource", XMLTOKEN_TEXTURESOURCE);
  xmltokens.Register ("layer", XMLTOKEN_LAYER);
  xmltokens.Register ("environment", XMLTOKEN_ENVIRONMENT);

  xmltokens.Register("integer", 100+iShaderVariable::INT);
  xmltokens.Register("float", 100+iShaderVariable::VECTOR1);
  xmltokens.Register("string", 100+iShaderVariable::STRING);
  xmltokens.Register("vector3", 100+iShaderVariable::VECTOR3);

  xmltokens.Register("primary color", GL_PRIMARY_COLOR);
  xmltokens.Register("texture", GL_TEXTURE);
  xmltokens.Register("constant color", GL_CONSTANT_ARB);
  xmltokens.Register("previous layer", GL_PREVIOUS_ARB);
  
  xmltokens.Register("color", GL_SRC_COLOR);
  xmltokens.Register("invertcolor", GL_ONE_MINUS_SRC_COLOR);
  xmltokens.Register("alpha", GL_SRC_ALPHA);
  xmltokens.Register("invertalpha", GL_ONE_MINUS_SRC_ALPHA);

  xmltokens.Register("replace", GL_REPLACE);
  xmltokens.Register("modulate", GL_MODULATE);
  xmltokens.Register("add", GL_ADD);
  xmltokens.Register("add signed", GL_ADD_SIGNED_ARB);
  xmltokens.Register("interpolate", GL_INTERPOLATE_ARB);
  xmltokens.Register("subtract", GL_SUBTRACT_ARB);
  xmltokens.Register("dot3", GL_DOT3_RGB_ARB);
  xmltokens.Register("dot3 alpha", GL_DOT3_RGBA_ARB);
}


////////////////////////////////////////////////////////////////////
//                          iShaderProgram
////////////////////////////////////////////////////////////////////

bool csShaderGLMTEX::Load(iDocumentNode* node)
{
  if(!node)
    return false;

  BuildTokenHash();

  csRef<iDocumentNode> mtexnode = node->GetNode("mtex");
  if(mtexnode)
  {
    csRef<iDocumentNodeIterator> it = mtexnode->GetNodes();
    while(it->HasNext())
    {
      csRef<iDocumentNode> child = it->Next();
      if(child->GetType() != CS_NODE_ELEMENT) continue;
      const char* value = child->GetValue ();
      csStringID id = xmltokens.Request (value);
      switch(id)
      {
      case XMLTOKEN_LAYER:
        {
          mtexlayer* ml = new mtexlayer();
          if(!LoadLayer(ml, child))
            return false;
          texlayers.Push(ml);
        }
        break;
      }
    } 
  }
  return true;
}

bool csShaderGLMTEX::LoadLayer(mtexlayer* layer, iDocumentNode* node)
{
  if(layer == NULL || node == NULL)
    return false;

  csRef<iDocumentNodeIterator> it = node->GetNodes();
  csRef<iEngine> engine = CS_QUERY_REGISTRY (object_reg, iEngine);

  while(it->HasNext())
  {
    csRef<iDocumentNode> child = it->Next();
    if(child->GetType() != CS_NODE_ELEMENT) continue;
    csStringID id = xmltokens.Request(child->GetValue());
    switch (id)
    {
    case XMLTOKEN_TEXTURESOURCE:
      {
        const char* texname = child->GetAttributeValue("name");
        if (texname)
        {
          if (engine)
            layer->texturehandle = 
              engine->FindTexture (texname)->GetTextureHandle ();
        } 
        
        if (child->GetAttributeValue ("number"))
        {
          int tnum = child->GetAttributeValueAsInt("number");
          if(tnum < 0) continue;
          layer->texnum = tnum;
        }
      }
      break;
    case XMLTOKEN_ENVIRONMENT:
      {
        LoadEnvironment(layer, child);
      }
      break;
    }
  }
  return true;
}

bool csShaderGLMTEX::LoadEnvironment(mtexlayer* layer, iDocumentNode* node)
{
  if(layer == NULL || node == NULL)
    return false;

  csRef<iDocumentNodeIterator> it = node->GetNodes();

  while(it->HasNext())
  {
    csRef<iDocumentNode> child = it->Next();
    if(child->GetType() != CS_NODE_ELEMENT) continue;
    csStringID id = xmltokens.Request(child->GetValue());
    switch (id)
    {
    case XMLTOKEN_COLORSOURCE:
      {
        int num = child->GetAttributeValueAsInt("num");
        
        if(num < 0 || num > 3 )
          continue;
        
        int i = xmltokens.Request(child->GetAttributeValue("source"));
        if(i == GL_PRIMARY_COLOR_ARB||i == GL_TEXTURE||i == GL_CONSTANT_ARB||i==GL_PREVIOUS_ARB)
          layer->colorsource[num] = i;

        int m = xmltokens.Request(child->GetAttributeValue("modifier"));
        if(m == GL_SRC_COLOR ||m == GL_ONE_MINUS_SRC_COLOR||m == GL_SRC_ALPHA||m == GL_ONE_MINUS_SRC_ALPHA)
          layer->colormod[num] = m;
      }
      break;
    case XMLTOKEN_ALPHASOURCE:
      {
        int num = child->GetAttributeValueAsInt("num");
        
        if(num < 0 || num > 3 )
          continue;
        
        int i = xmltokens.Request(child->GetAttributeValue("source"));
        if(i == GL_PRIMARY_COLOR_ARB||i == GL_TEXTURE||i == GL_CONSTANT_ARB||i==GL_PREVIOUS_ARB)
          layer->alphasource[num] = i;

        int m = xmltokens.Request(child->GetAttributeValue("modifier"));
        if(m == GL_SRC_ALPHA||m == GL_ONE_MINUS_SRC_ALPHA)
          layer->alphamod[num] = m;
      }
      break;
    case XMLTOKEN_COLOROP:
      {
        int o = xmltokens.Request(child->GetAttributeValue("operation"));
        if(o == GL_REPLACE|| o == GL_MODULATE||o == GL_ADD||o == GL_ADD_SIGNED_ARB||
          o == GL_INTERPOLATE_ARB||o == GL_SUBTRACT_ARB||o == GL_DOT3_RGB_ARB||o == GL_DOT3_RGBA_ARB)
          layer->colorp = o;
        if(child->GetAttribute("scale") != NULL)
          layer->scale_rgb = child->GetAttributeValueAsFloat ("scale");
      }
      break;
    case XMLTOKEN_ALPHAOP:
      {
        int o = xmltokens.Request(child->GetAttributeValue("operation"));
        if(o == GL_REPLACE|| o == GL_MODULATE||o == GL_ADD||o == GL_ADD_SIGNED_ARB||
          o == GL_INTERPOLATE_ARB||o == GL_SUBTRACT_ARB||o == GL_DOT3_RGB_ARB||o == GL_DOT3_RGBA_ARB)
          layer->alphap = o;
        if(child->GetAttribute("scale") != NULL)
          layer->scale_alpha = child->GetAttributeValueAsFloat ("scale");
      }
      break;
    }
  }
  return true;
}

bool csShaderGLMTEX::Load(iDataBuffer* program)
{
  csRef<iDocumentSystem> xml (CS_QUERY_REGISTRY (object_reg, iDocumentSystem));
  if (!xml) xml = csPtr<iDocumentSystem> (new csTinyDocumentSystem ());
  csRef<iDocument> doc = xml->CreateDocument ();
  const char* error = doc->Parse (program);
  if (error != NULL)
  { 
    csReport( object_reg, CS_REPORTER_SEVERITY_ERROR, "crystalspace.render3d.shader.glmtex",
      "XML error '%s'!", error);
    return false;
  }
  return Load(doc->GetRoot());
}

bool csShaderGLMTEX::Prepare()
{
  glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &maxlayers);

  //get a statecache
  csRef<iGraphics2D> g2d = CS_QUERY_REGISTRY(object_reg, iGraphics2D);
  g2d->PerformExtension("getstatecache", &statecache);

  //get extension-object
  r3d = CS_QUERY_REGISTRY(object_reg,iRender3D);
  csRef<iShaderRenderInterface> sri = SCF_QUERY_INTERFACE(r3d, iShaderRenderInterface);

  ext = (csGLExtensionManager*) sri->GetPrivateObject("ext");
  txtcache = (iGLTextureCache*) sri->GetPrivateObject("txtcache");

  if (texlayers.Length () > maxlayers)
    return false;

  return true;
}

void csShaderGLMTEX::Activate(iShaderPass* current, csRenderMesh* mesh)
{
  for(int i = 0; i < MIN(maxlayers, texlayers.Length()); ++i)
  {
    mtexlayer* layer = (mtexlayer*) texlayers.Get(i);
    ext->glActiveTextureARB(GL_TEXTURE0_ARB + i);
    ext->glClientActiveTextureARB(GL_TEXTURE0_ARB + i);


    if(ext->CS_GL_ARB_texture_env_combine || ext->CS_GL_EXT_texture_env_combine)
    {
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
        glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);

        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, layer->colorsource[0]);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB_ARB, layer->colormod[0]);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, layer->colorsource[1]);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB_ARB, layer->colormod[1]);
        if (layer->colorsource[2] != -1)
        {
          glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB_ARB, layer->colorsource[2]);
          glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB_ARB, layer->colormod[2]);
        }

        if( (layer->colorp != GL_DOT3_RGB_ARB) && (layer->colorp  != GL_DOT3_RGBA_ARB))
          glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, layer->colorp );
        else if (ext->CS_GL_ARB_texture_env_dot3 || ext->CS_GL_EXT_texture_env_dot3)
          glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, layer->colorp );

	glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, layer->scale_rgb);

        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, layer->alphasource[0]);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA_ARB, layer->alphamod[0]);
        glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_ARB, layer->alphasource[1]);
        glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA_ARB, layer->alphamod[1]);
        if (layer->alphasource[2] != -1)
        {
          glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA_ARB, layer->alphasource[2]);
          glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_ALPHA_ARB, layer->alphamod[2]);
        }

        if( (layer->colorp != GL_DOT3_RGB_ARB) && (layer->colorp  != GL_DOT3_RGBA_ARB))
          glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, layer->alphap);
        else if (ext->CS_GL_ARB_texture_env_dot3 || ext->CS_GL_EXT_texture_env_dot3)
          glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, layer->alphap);
        
	glTexEnvf(GL_TEXTURE_ENV, GL_ALPHA_SCALE, layer->scale_alpha);
    }
  }
}

void csShaderGLMTEX::Deactivate(iShaderPass* current)
{
  ext->glActiveTextureARB(GL_TEXTURE0_ARB);
  ext->glClientActiveTextureARB(GL_TEXTURE0_ARB);
  glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void csShaderGLMTEX::SetupState (iShaderPass *curret, csRenderMesh *mesh)
{
}

void csShaderGLMTEX::ResetState ()
{
}


csBasicVector csShaderGLMTEX::GetAllVariableNames()
{
  csBasicVector res;

  csHashIterator c( &variables);
  while(c.HasNext())
  {
    res.PushSmart( (void*)((iShaderVariable*)c.Next())->GetName());
  }
  return res;
}

iShaderVariable* csShaderGLMTEX::GetVariable(int namehash)
{
  csHashIterator c(&variables, namehash);

  if(c.HasNext())
  {
    return (iShaderVariable*)c.Next();
  }

  return NULL;
}

csPtr<iString> csShaderGLMTEX::GetProgramID()
{
  csMD5::Digest d = csMD5::Encode(programstring);
  scfString* str = new scfString();
  str->Append((const char*)d.data[0], 16);
  return csPtr<iString>(str);
}
