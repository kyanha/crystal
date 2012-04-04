<!--
  Copyright (C) 2011 Alexandru - Teodor Voicu
      Imperial College London
      http://www3.imperial.ac.uk/

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
-->
<include>
<?Include lightfuncs.cginc ?>
<![CDATA[

#ifndef __SHADOW_OSM_CG_INC__
#define __SHADOW_OSM_CG_INC__

struct ShadowShadowMapDepth : ShadowShadowMap
{
  float4 shadowMapCoords;
  float4 shadowMapCoordsProj;
  int lightNum;
  float bias;
  float EPS;
  
  void InitVP (int lightNum, float4 surfPositionWorld,
               float3 normWorld,
               out float4 vp_shadowMapCoords,
               out float vp_distance)
  {
    float4x4 lightTransformInv = lightProps_transformInv[lightNum];
    // Transform world position into light space
    float4 view_pos = mul(lightTransformInv, surfPositionWorld);

    float4x4 shadowMapTF = lightPropsOM_opacityMapTF[lightNum];
    /* CS' render-to-texture Y-flips render targets (so the upper left
       gets rendered to 0,0), we need to unflip here again. */
    float4x4 flipY;
    flipY[0] = float4 (1, 0, 0, 0);
    flipY[1] = float4 (0, -1, 0, 0);
    flipY[2] = float4 (0, 0, 1, 0);
    flipY[3] = float4 (0, 0, 0, 1);
    shadowMapTF = mul (flipY, shadowMapTF);
    
    vp_shadowMapCoords = mul (shadowMapTF, view_pos);
  }
  
  void Init (int lightN, float4 vp_shadowMapCoords, float vp_distance)
  {
    shadowMapCoords = vp_shadowMapCoords;
    lightNum = lightN;
    
    // Project SM coordinates
    shadowMapCoordsProj = shadowMapCoords;
    shadowMapCoordsProj.xyz /= shadowMapCoordsProj.w;    
  }
  
  // Blur using PCF
  float4 blurTex2D(sampler2D tex, float2 position)
  {
    float4 sum = 0;
    float x, y;

    sum += tex2D(tex, position + float2(-bias, -bias));
    sum += tex2D(tex, position + float2(-bias, 0));
    sum += tex2D(tex, position + float2(-bias, bias));
    sum += tex2D(tex, position + float2(0, -bias));
    sum += tex2D(tex, position + float2(0, 0));
    sum += tex2D(tex, position + float2(0, bias));
    sum += tex2D(tex, position + float2(bias, -bias));
    sum += tex2D(tex, position + float2(bias, 0));
    sum += tex2D(tex, position + float2(bias, bias));
      
    sum /= 9.0; 

    return sum;
  }
  
  // PCF - Percentage Close Filtering 4x4
  float getVisibility(sampler2D tex, float2 position, float compareDepth)
  {   
    float sum = 0;
    float x, y;
    
    if (position.x < EPS || position.y < EPS || position.x > 1 - EPS ||
      position.y > 1 - EPS)
      return 0;
     
    sum += float(compareDepth > tex2D(tex, position + float2(-bias, -bias)));
    sum += float(compareDepth > tex2D(tex, position + float2(-bias, 0)));
    sum += float(compareDepth > tex2D(tex, position + float2(-bias, bias)));
    sum += float(compareDepth > tex2D(tex, position + float2(0, -bias)));
    sum += float(compareDepth > tex2D(tex, position + float2(0, 0)));
    sum += float(compareDepth > tex2D(tex, position + float2(0, bias)));
    sum += float(compareDepth > tex2D(tex, position + float2(bias, -bias)));
    sum += float(compareDepth > tex2D(tex, position + float2(bias, 0)));
    sum += float(compareDepth > tex2D(tex, position + float2(bias, bias)));
    
    sum /= 9.0; 

    return sum;    
  }
  
  float getMapValue(int i, float2 position)
  {
    // Automatically generate textures read
]]>      
  
    <?Generate I 0 7?>
      if (i == 4 * $I$)
        return blurTex2D(lightPropsOM_opacityMap[8 * lightNum + $I$], position).r;
      if (i == 4 * $I$ + 1)
        return blurTex2D(lightPropsOM_opacityMap[8 * lightNum + $I$], position).g;
      if (i == 4 * $I$ + 2)
        return blurTex2D(lightPropsOM_opacityMap[8 * lightNum + $I$], position).b;
      if (i == 4 * $I$ + 3)
        return blurTex2D(lightPropsOM_opacityMap[8 * lightNum + $I$], position).a;	    
    <?Endgenerate?>
        
<![CDATA[  
      
    return 0;
  }
   
  half GetVisibility()
  { 
    EPS = 0.0001;
    bias = 1.0 / lightPropsOM_size[lightNum];
    half inLight;
    int numSplits = lightPropsOM_opacityMapNumSplits[lightNum];
  
    float3 shadowMapCoordsBiased = (float3(0.5)*shadowMapCoordsProj.xyz) + float3(0.5);
    float2 position = shadowMapCoordsBiased.xy;
   
    int i;

    float compareDepth = (1 - shadowMapCoordsBiased.z) - EPS;
    float depthStart = tex2D(lightPropsOM_shadowMapStart[lightNum], position).x;
    float depthEnd = tex2D(lightPropsOM_shadowMapEnd[lightNum], position).x;    
      
    i = min( tex2D( lightPropsOM_splitFunc[lightNum], 
      float2( min( (compareDepth - depthStart) / 
      abs(1 - depthEnd - depthStart) , 0.9999 ) , 0 ) ).x * (numSplits - 1), 
      numSplits - 1);      

    float previousMap = getMapValue(i, position) * 
      getVisibility( lightPropsOM_shadowMapStart[lightNum] , position, compareDepth ); 

    i = i * (i != -1);
    inLight = previousMap; 
    inLight = exp(-1.0 * inLight);

    /*
    if (compareDepth > depthStart && depthStart > 0.01)
      inLight = 0;
    else
      inLight = 1;
    */
    return inLight;
  }
};

#endif // __SHADOW_OSM_CG_INC__
 
]]>
</include>
