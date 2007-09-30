/*
  Copyright (C) 2006 by Marten Svanfeldt

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

#ifndef __DIRECTLIGHT_H__
#define __DIRECTLIGHT_H__

#include "csutil/noncopyable.h"

#include "light.h"
#include "primitive.h"
#include "raytracer.h"
#include "sampler.h"

namespace lighter
{
  class Sector;
  class Raytracer;
  class Primitive;

  class PartialElementIgnoreCallback;  
  
  // Class to calculate direct lighting
  class DirectLighting : private CS::NonCopyable
  {
  public:
    // Setup
    static void Initialize ();

    // Shade by using all primitives within range
    static void ShadeDirectLighting (Sector* sector, 
      Statistics::Progress& progress);

    //-- Shade a point
    typedef csColor (*PVLPointShader)(Sector* sector, Object* obj, 
      const csVector3& point, const csVector3& normal, 
      SamplerSequence<2>& lightSampler);

    // Shade a single point in space with direct lighting
    static csColor UniformShadeAllLightsNonPD (Sector* sector, Object* obj, 
      const csVector3& point, const csVector3& normal, 
      SamplerSequence<2>& lightSampler);

    // Shade a single point in space with direct lighting using a single light
    static csColor UniformShadeRndLightNonPD (Sector* sector, Object* obj,
      const csVector3& point, const csVector3& normal, 
      SamplerSequence<2>& lightSampler);

    //-- Shade a lightmap element
    typedef csColor (*LMElementShader)(Sector* sector, ElementProxy element,
      SamplerSequence<2>& lightSampler);

    // Shade a primitive element with direct lighting
    static csColor UniformShadeAllLightsNonPD (Sector* sector, ElementProxy element,
      SamplerSequence<2>& lightSampler);

    // Shade a primitive element with direct lighting using a single light
    static csColor UniformShadeRndLightNonPD (Sector* sector, ElementProxy element,
      SamplerSequence<2>& lightSampler);

    //-- Shade using one light
    // Shade a primitive element with direct lighting
    static csColor UniformShadeOneLight (Sector* sector, Object* obj, 
      const csVector3& point, const csVector3& normal, Light* light, 
      SamplerSequence<2>& lightSampler);

    // Shade a primitive element with direct lighting
    static csColor UniformShadeOneLight (Sector* sector, ElementProxy element,
      Light* light, SamplerSequence<2>& lightSampler);

    template<typename T>
    inline static csColor UniformShadeElement (T& shade, ElementProxy element, 
      SamplerSequence<2>& lightSampler);

  private:
    struct ShadeAllLightsNonPD
    {
      const LightRefArray& allLights;
      ShadeAllLightsNonPD (const LightRefArray& allLights) : 
        allLights (allLights) {}
      inline csColor ShadeLight (Object* obj, const csVector3& point, 
        const csVector3& normal, SamplerSequence<2>& lightSampler, 
        const Primitive* shadowIgnorePrimitive = 0, 
        bool fullIgnore = false);
    };

    struct ShadeRndLightNonPD
    {
      const LightRefArray& allLights;
      SamplerSequence<3>& lightSampler;
      ShadeRndLightNonPD (const LightRefArray& allLights,
        SamplerSequence<3>& lightSampler) : allLights (allLights), 
        lightSampler (lightSampler) {}
      inline csColor ShadeLight (Object* obj, const csVector3& point, 
        const csVector3& normal, SamplerSequence<2>& sampler, 
        const Primitive* shadowIgnorePrimitive = 0, 
        bool fullIgnore = false);
    };
    struct ShadeOneLight
    {
      Light* light;
      ShadeOneLight (Light* light) : light (light) {}
      inline csColor ShadeLight (Object* obj, const csVector3& point, 
        const csVector3& normal, SamplerSequence<2>& sampler, 
        const Primitive* shadowIgnorePrimitive = 0, 
        bool fullIgnore = false);
    };

    // Static methods...
    inline static csColor ShadeLight (Light* light, Object* obj, 
      const csVector3& point, const csVector3& normal, 
      SamplerSequence<2>& lightSampler, const Primitive* shadowIgnorePrimitive = 0, 
      bool fullIgnore = false);

    class ProgressState
    {
      Statistics::Progress& progress;
      size_t updateFreq;
      size_t u;
      float progressStep;

    public:
      ProgressState (Statistics::Progress& progress, size_t total) : 
        progress (progress), 
        updateFreq (progress.GetUpdateFrequency (total)), u (updateFreq),
        progressStep (float (updateFreq) / total) {}

      CS_FORCEINLINE void Advance ()
      {
        if (--u == 0)
        {
          progress.IncProgress (progressStep);
          u = updateFreq;
          globalTUI.Redraw (TUI::TUI_DRAW_RAYCORE);
        }
      }
    };

    static void ShadeLightmap (Sector* sector, Object* obj, 
      SamplerSequence<2>& masterSampler, ProgressState& progress);

    static void ShadePerVertex (Sector* sector, Object* obj,
      SamplerSequence<2>& masterSampler, ProgressState& progress);

    // Static data
    static PVLPointShader pvlPointShader;
    static LMElementShader lmElementShader;    
  };
}

#endif
