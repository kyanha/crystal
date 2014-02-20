/*
    Copyright (C) 2010 by Joe Forte

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

#ifndef __GBUFFER_H__
#define __GBUFFER_H__

#include "cssysdef.h"

#include "csutil/scfstr.h"
#include "csutil/refcount.h"
#include "itexture.h"

CS_PLUGIN_NAMESPACE_BEGIN(RMDeferred)
{

  /**
   * Utility class for holding a collection of render targets used 
   * for deferred shading.
   */
  class GBuffer : public CS::Utility::FastRefCount<GBuffer>
  {
  public:

    /**
     * Data used to define the structure of the gbuffer.
     */
    struct Description
    {
      int width;
      int height;

      size_t colorBufferCount;
      size_t accumBufferCount;

      // Set to NULL to use the default format.
      const char *colorBufferFormat;
      // Set to NULL to use the default format.
      const char *accumBufferFormat;
    };

    GBuffer() : width(0), height(0), graphics3D(nullptr) {}
    ~GBuffer() {}

    /// Initializes the gbuffer.
    bool Initialize(const Description &desc, 
                    iGraphics3D *g3D, 
                    iShaderVarStringSet *stringSet,
                    iObjectRegistry *registry)
    {
      const char *messageID = "crystalspace.rendermanager.deferred.gbuffer";

      const int flags = CS_TEXTURE_2D | CS_TEXTURE_NOMIPMAPS | CS_TEXTURE_CLAMP | CS_TEXTURE_NPOTS;
      width = desc.width;
      height = desc.height;

      const char *colorFmt = desc.colorBufferFormat;
      if (!colorFmt)
        colorFmt = "rgba16_f";

      const char *accumFmt = desc.accumBufferFormat;
      if (!accumFmt)
	accumFmt = "rgb16_f";

      iTextureManager *texMgr = g3D->GetTextureManager ();

      // Checks for device support. NOTE: Even if this test passes it is not guaranteed that
      // the gbuffer format will be supported on the device.
      const csGraphics3DCaps *caps = g3D->GetCaps ();
      if (desc.colorBufferCount > (size_t)caps->MaxRTColorAttachments)
      {
        csReport(registry, 
                 CS_REPORTER_SEVERITY_ERROR, 
                 messageID, 
                 "Too many color buffers requested (%d requested, device supports %d)!", 
                 (int)desc.colorBufferCount, 
                 (int)caps->MaxRTColorAttachments);
        return false;
      }

      size_t count = desc.colorBufferCount;
      for (size_t i = 0; i < count; i++)
      {
        scfString errString;

        csRef<iTextureHandle> colorBuffer = texMgr->CreateTexture (width, height,
          csimg2D, colorFmt, flags, &errString);

        if (!colorBuffer)
        {
          csReport(registry, CS_REPORTER_SEVERITY_ERROR, messageID, 
            "Could not create color buffer %d! %s", (int)i, errString.GetCsString ().GetDataSafe ());
          return false;
        }

        csString svName;
        svName.Format ("tex gbuffer %d", (int)i);

        colorBuffers.Push (colorBuffer);
        colorBufferSVNames.Push (stringSet->Request (svName.GetDataSafe ()));
      }

      count = desc.accumBufferCount;
      for(size_t i = 0; i < count; ++i)
      {
	scfString errString;

	csRef<iTextureHandle> accumBuffer = texMgr->CreateTexture (width, height,
	  csimg2D, accumFmt, flags, &errString);

	if(!accumBuffer)
	{
	  csReport(registry, CS_REPORTER_SEVERITY_ERROR, messageID,
	    "Could not create accumulation buffer %d! %s", (int)i, errString.GetCsString().GetDataSafe());
	  return false;
	}

	csString svName;
	svName.Format("tex accumulation %d", (int)i);

	accumBuffers.Push(accumBuffer);
	accumBufferSVNames.Push(stringSet->Request(svName.GetDataSafe()));
      }

      const char *depthFmt[] = { /*"d24s8", */ "d32", "d16" };
      const size_t fmtCount = sizeof(depthFmt) / sizeof(const char *);

      // Iterate through the depth formats until we find a valid format.
      for (size_t i = 0; i < fmtCount; i++)
      {
        depthBuffer = texMgr->CreateTexture (width, height, csimg2D, depthFmt[i], flags, NULL);

        if (depthBuffer)
          break;
      }

      if (!depthBuffer)
      {
        csReport(registry, CS_REPORTER_SEVERITY_ERROR, messageID, 
          "Could not create depth buffer!");
        return false;
      }

      depthBufferSVName = stringSet->Request ("tex gbuffer depth");

      graphics3D = g3D;

      // Test if the gbuffer format is supported.
      if (!Attach ())
      {
        csReport(registry, CS_REPORTER_SEVERITY_ERROR, messageID, 
            "Failed to attach GBuffer to the device!");
        return false;
      }

      if (!Detach ())
      {
        csReport(registry, CS_REPORTER_SEVERITY_WARNING, messageID, 
            "Failed to detach GBuffer!");
      }

      if (!AttachAccumulation())
      {
	csReport(registry, CS_REPORTER_SEVERITY_ERROR, messageID,
	    "Failed to attach Accumulation Buffers to the device!");
	return false;
      }

      if (!Detach ())
      {
        csReport(registry, CS_REPORTER_SEVERITY_WARNING, messageID, 
            "Failed to detach Accumulation Buffers!");
      }

      return true;
    }

    /**
     * Attaches the gbuffer as the output render targets.
     */
    bool Attach()
    {
      size_t count = GetColorBufferCount ();
      for (size_t i = 0; i < count; i++)
      {
        csRenderTargetAttachment attach = (csRenderTargetAttachment)((size_t)rtaColor0 + i);

        if (!graphics3D->SetRenderTarget (GetColorBuffer (i), false, 0, attach))
	{
          return false;
	}
      }

      {
        if (!graphics3D->SetRenderTarget (GetDepthBuffer (), false, 0, rtaDepth))
	{
          return false;
	}
      }

      return graphics3D->ValidateRenderTargets();
    }

    /**
     * Attaches the gbuffer as the output render targets.
     */
    bool AttachAccumulation()
    {
      size_t count = GetAccumulationBufferCount ();
      for (size_t i = 0; i < count; i++)
      {
        csRenderTargetAttachment attach = (csRenderTargetAttachment)((size_t)rtaColor0 + i);

        if (!graphics3D->SetRenderTarget (GetAccumulationBuffer (i), false, 0, attach))
	{
          return false;
	}
      }

      return graphics3D->ValidateRenderTargets();
    }

    /// Detaches the gbuffer as the output render targets. Only required if you didn't call FinishDraw.
    bool Detach()
    {
      graphics3D->UnsetRenderTargets ();

      return true;
    }

    /// Updates shader variables.
    bool UpdateShaderVars(iShaderVariableContext *ctx)
    {
      size_t count = GetColorBufferCount ();
      for (size_t i = 0; i < count; i++)
      {
        csShaderVariable *colorSV = ctx->GetVariableAdd (colorBufferSVNames[i]);
        colorSV->SetValue (GetColorBuffer (i));
      }

      count = GetAccumulationBufferCount ();
      for (size_t i = 0; i < count; ++i)
      {
	csShaderVariable *accumSV = ctx->GetVariableAdd (accumBufferSVNames[i]);
	accumSV->SetValue (GetAccumulationBuffer (i));
      }

      {
        csShaderVariable *depthSV = ctx->GetVariableAdd (depthBufferSVName);
        depthSV->SetValue (GetDepthBuffer ());
      }

      return true;
    }

    /// Returns the number of color buffers in the gbuffer.
    size_t GetColorBufferCount() const
    {
      return colorBuffers.GetSize ();
    }

    /// Returns the ith color buffer.
    iTextureHandle *GetColorBuffer(size_t i)
    {
      return colorBuffers[i];
    }

    /// Returns the depth buffer.
    iTextureHandle *GetDepthBuffer()
    {
      return depthBuffer;
    }

    /// Returns the number of color buffers in the gbuffer.
    size_t GetAccumulationBufferCount() const
    {
      return accumBuffers.GetSize ();
    }

    /// Returns the ith color buffer.
    iTextureHandle *GetAccumulationBuffer(size_t i)
    {
      return accumBuffers[i];
    }


    /// Gets the width and height of the buffers.
    void GetDimensions (int &w, int &h)
    {
      w = width;
      h = height;
    }

    void Invalidate ()
    {
      colorBuffers.DeleteAll ();
      accumBuffers.DeleteAll ();
      depthBuffer.Invalidate ();
      colorBufferSVNames.DeleteAll ();
      accumBufferSVNames.DeleteAll ();
    }

  private:
    int width, height;

    csRefArray<iTextureHandle> colorBuffers;
    csRefArray<iTextureHandle> accumBuffers;
    csRef<iTextureHandle> depthBuffer;

    // Stores the shader variable names.
    csArray<ShaderVarStringID> colorBufferSVNames;
    csArray<ShaderVarStringID> accumBufferSVNames;
    ShaderVarStringID depthBufferSVName;

    iGraphics3D *graphics3D;
  };

}
CS_PLUGIN_NAMESPACE_END(RMDeferred)

#endif // __GBUFFER_H__
