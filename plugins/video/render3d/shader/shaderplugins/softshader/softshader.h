/*
    Copyright (C) 2002 by Anders Stenberg
			  Marten Svanfeldt
    
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

#ifndef __SOFTSHADER_H__
#define __SOFTSHADER_H__

#include "iutil/comp.h"
#include "ivideo/shader/shader.h"

#include "csplugincommon/softshader/defaultshader.h"
#include "csplugincommon/softshader/renderinterface.h"
#include "csplugincommon/shader/shaderplugin.h"

#include "scanline_base.h"

CS_PLUGIN_NAMESPACE_BEGIN(SoftShader)
{

using namespace CS::PluginCommon::SoftShader;

class csSoftShader : 
  public scfImplementation3<csSoftShader, 
			    iShaderProgramPlugin,
			    iDefaultShader,
			    iComponent>
{
public:
  iObjectRegistry* object_reg;
  csRef<ScanlineRendererBase> scanlineRenderer;
  csRef<iSoftShaderRenderInterface> softSRI;

private:
  bool enable;
public:
  csSoftShader (iBase *parent);
  virtual ~csSoftShader ();

  /**\name iShaderProgramPlugin implementation
   * @{ */
  virtual csPtr<iShaderProgram> CreateProgram(const char* type) ;

  virtual bool SupportType(const char* type);

  virtual void Open();

  bool Precache (const char* type, iShaderDestinationResolver* resolve, 
    iDocumentNode* node, iHierarchicalCache* cacheTo,
    csRef<iBase>* outObj = 0) { return false; }
  /** @} */

  /**\name iComponent implementation
   * @{ */
  bool Initialize (iObjectRegistry* reg);
  /** @} */

  /**\name iDefaultShader implementation
   * @{ */
  iDefaultScanlineRenderer* GetDefaultRenderer()
  {
    Open();
    return scanlineRenderer;
  }
  /** @} */
};

}
CS_PLUGIN_NAMESPACE_END(SoftShader)

#endif //__SOFTSHADER_H__

