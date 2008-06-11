/*
    Copyright (C) 2008 by Frank Richter

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

#ifndef __CS_CSPLUGINCOMMON_RENDERMANAGER_DEBUGCOMMON_H__
#define __CS_CSPLUGINCOMMON_RENDERMANAGER_DEBUGCOMMON_H__

#include "iutil/dbghelp.h"
#include "csplugincommon/rendermanager/renderview.h"

namespace CS
{
  namespace RenderManager
  {
    /* Helper class containing stuff which doesn't require any of the template
     * parameters to RMDebugCommon
     */
    class CS_CRYSTALSPACE_EXPORT RMDebugCommonBase : public virtual iDebugHelper
    {
    protected:
      bool wantDebugLockLines;
      
      virtual bool HasDebugLockLines() = 0;
      virtual void DeleteDebugLockLines() = 0;
    public:
      RMDebugCommonBase();
    
      /**\name iDebugHelper implementation
      * @{ */
      csTicks Benchmark (int num_iterations) { return 0; }
      bool DebugCommand (const char *cmd);
      void Dump (iGraphics3D *g3d) {}
      csPtr<iString> Dump () { return 0; }
      int GetSupportedTests () const { return 0; }
      csPtr<iString> StateTest () { return  0; }
      csPtr<iString> UnitTest () { return  0; }
      /** @} */
    };
    
    /**
     * Common debug helpers in render manager plugins.
     * Provides an implementation of iDebugHelper. Thus deriving classes should
     * add <tt>scfFakeInterface<iDebugHelper></tt> to their SCF implementation
     * base class.
     *
     * At the end of view rendering, \c DebugFrameRender() should be called.
     */
    template<typename RenderTreeType>
    class RMDebugCommon : public RMDebugCommonBase
    {
      typedef typename RenderTreeType::DebugLines DebugLinesType;
      DebugLinesType* lockedDebugLines;
      
      bool HasDebugLockLines() { return lockedDebugLines != 0; }
      void DeleteDebugLockLines()
      { delete lockedDebugLines; lockedDebugLines = 0; }
    public:
      RMDebugCommon() : lockedDebugLines (0) {}
      ~RMDebugCommon() { delete lockedDebugLines; }
    
      void DebugFrameRender (CS::RenderManager::RenderView* rview,
                             RenderTreeType& renderTree)
      {
	if (wantDebugLockLines)
	{
	  lockedDebugLines =
	    new DebugLinesType (renderTree.GetDebugLines());
	  wantDebugLockLines = false;
	}
	else if (lockedDebugLines)
	  renderTree.SetDebugLines (*lockedDebugLines);
	renderTree.DrawDebugLines (rview->GetGraphics3D (), rview);
	renderTree.RenderDebugTextures (rview->GetGraphics3D ());
      }
    };
  
  } // namespace RenderManager
} // namespace CS

#endif // __CS_CSPLUGINCOMMON_RENDERMANAGER_DEBUGCOMMON_H__
