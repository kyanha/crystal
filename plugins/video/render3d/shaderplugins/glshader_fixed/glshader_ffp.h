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

#ifndef __GLSHADER_FIXEDFP_H__
#define __GLSHADER_FIXEDFP_H__

#include "ivideo/shader/shader.h"
#include "imap/services.h"

class csGLRender3D;

class csGLShaderFFP : public iShaderProgram
{
private:
  csRef<iGraphics3D> g3d;
  iGLTextureCache* txtcache;
  csGLExtensionManager* ext;
  csRef<iObjectRegistry> object_reg;

  /// Parser for common stuff like MixModes, vectors, matrices, ...
  csRef<iSyntaxService> SyntaxService;

  csGLStateCache* statecache;

  char* programstring;

  csPDelArray<csSymbolTable> symtabs;
  csSymbolTable *symtab;

  csHashMap variables;
  bool validProgram;

  // programloading stuff
  enum
  {
    XMLTOKEN_LAYER = 1,
    XMLTOKEN_COLORSOURCE,
    XMLTOKEN_ENVIRONMENT,
    XMLTOKEN_ALPHASOURCE,
    XMLTOKEN_COLOROP,
    XMLTOKEN_ALPHAOP,
    XMLTOKEN_COLORSCALE,
    XMLTOKEN_ALPHASCALE,
  };

  csStringHash xmltokens;

  void BuildTokenHash();

  // Layers of multitexturing
  enum COLORSOURCE
  {
    CS_COLORSOURCE_MESH,
    CS_COLORSOURCE_STREAM,
    CS_COLORSOURCE_NONE
  };

  enum TEXCOORDSOURCE
  {
    CS_TEXCOORDSOURCE_MESH,
    CS_TEXCOORDSOURCE_STREAM,
    CS_TEXCOORDSOURCE_NONE
  };

  struct mtexlayer
  {
    //remember which vars to reset on deactivate
    bool doTexture;

    //texturenumber to use
    int texnum;
    csRef<iTextureHandle> texturehandle;

    //colorsource
    int colorsource[3];
    //color modifier
    int colormod[3];
    //color operation
    int colorp;

    //alphasource
    int alphasource[4];
    //alpha modifier
    int alphamod[4];
    //alpha operation
    int alphap;

    // rgb scale
    float scale_rgb;
    // alpha scale
    float scale_alpha;
    mtexlayer()
    {
      texnum = -1;
      doTexture = false;

      colorsource[0] = GL_PREVIOUS_ARB; colorsource[1] = GL_TEXTURE; colorsource[2] = -1;
      colormod[0] = GL_SRC_COLOR; colormod[1] = GL_SRC_COLOR; colormod[2] = GL_SRC_COLOR;
      colorp = GL_MODULATE;

      alphasource[0] = GL_PREVIOUS_ARB; alphasource[1] = GL_TEXTURE; alphasource[2] = -1;
      alphamod[0] = GL_SRC_ALPHA; alphamod[1] = GL_SRC_ALPHA; alphamod[2] = GL_SRC_ALPHA;
      alphap = GL_MODULATE;

      scale_rgb = 1.0f;
      scale_alpha = 1.0f;
    }
  };

  //array of mtexlayers
  csArray<mtexlayer> texlayers;

  //maximum number of layers
  int maxlayers;

  void UpdateValid();
  
  bool LoadLayer(mtexlayer* layer, iDocumentNode* node);
  bool LoadEnvironment(mtexlayer* layer, iDocumentNode* node);
public:
  SCF_DECLARE_IBASE;

  csGLShaderFFP(iObjectRegistry* objreg, csGLExtensionManager* ext);

  virtual ~csGLShaderFFP ()
  {
    Deactivate(0);
    if(programstring) delete programstring;
  }

  ////////////////////////////////////////////////////////////////////
  //                      iShaderProgram
  ////////////////////////////////////////////////////////////////////

  virtual csPtr<iString> GetProgramID();

  /// Sets this program to be the one used when rendering
  virtual void Activate(iShaderPass* current, csRenderMesh* mesh);

  /// Deactivate program so that it's not used in next rendering
  virtual void Deactivate(iShaderPass* current);

    /// Setup states needed for proper operation of the shader
  virtual void SetupState (iShaderPass* current, csRenderMesh* mesh);

  /// Reset states to original
  virtual void ResetState ();

  /* Propertybag - get property, return false if no such property found
   * Which properties there is is implementation specific
   */
  virtual bool GetProperty(const char* name, iString* string) {return false;};
  virtual bool GetProperty(const char* name, int* string) {return false;};
  virtual bool GetProperty(const char* name, csVector3* string) {return false;};
//  virtual bool GetProperty(const char* name, csVector4* string) {};

  /* Propertybag - set property.
   * Which properties there is is implementation specific
   */
  virtual bool SetProperty(const char* name, iString* string) {return false;};
  virtual bool SetProperty(const char* name, int* string) {return false;};
  virtual bool SetProperty(const char* name, csVector3* string) {return false;};
//  virtual bool SetProperty(const char* name, csVector4* string) {return false;};

  virtual void AddChild(iShaderBranch *b) {}
  virtual void AddVariable(csShaderVariable* variable) {}
  virtual csShaderVariable* GetVariable(csStringID s)
    { return symtab->GetSymbol(s); }
  virtual csSymbolTable* GetSymbolTable() { return symtab; }
  virtual csSymbolTable* GetSymbolTable(int i) {
    if (symtabs.Length () <= i) symtabs.SetLength (i + 1, csSymbolTable ());
    return symtabs[i];
  }
  virtual void SelectSymbolTable(int i) {
    if (symtabs.Length () <= i) symtabs.SetLength (i + 1, csSymbolTable ());
    symtab = symtabs[i];
  }

  /// Check if valid
  virtual bool IsValid() { return validProgram;} 

  /// Loads shaderprogram from buffer
  virtual bool Load(iDataBuffer* program);

  /// Loads from a document-node
  virtual bool Load(iDocumentNode* node);

  /// Prepares the shaderprogram for usage. Must be called before the shader is assigned to a material
  virtual bool Prepare();
};


#endif //__GLSHADER_FIXEDFP_H__

