<include>

<variablemap variable="resolution" destination="g_Resolution" />
<variablemap variable="far clip distance" destination="g_FarClipDistance" />
<variablemap variable="sample radius" destination="g_SampleRadius" />
<variablemap variable="detail sample radius" destination="g_DetailSampleRadius" />
<variablemap variable="num passes" destination="numPasses" />
<variablemap variable="self occlusion" destination="g_SelfOcclusion" />
<variablemap variable="occlusion strength" destination="g_OcclusionStrength" />
<variablemap variable="max occluder distance" destination="g_MaxOccluderDistance" />
<variablemap variable="bounce strength" destination="g_IndirectLightStrength" />
<variablemap variable="enable ambient occlusion" destination="enableAO" />
<variablemap variable="enable indirect light" destination="enableIndirectLight" />

<program>
<![CDATA[


uniform sampler2D g_NormalTexture;
uniform sampler2D g_DepthTexture;
uniform sampler2D g_DirectRadianceTexture;
uniform sampler2D g_RndNormalsTexture;
uniform float2 g_Resolution;
uniform float g_FarClipDistance;
uniform float g_SampleRadius;
uniform float g_DetailSampleRadius;
//uniform int numPasses;
uniform float g_SelfOcclusion;
uniform float g_OcclusionStrength;
uniform float g_MaxOccluderDistance;
uniform float g_IndirectLightStrength;
//uniform float enableAO;
//uniform float enableIndirectLight;
//uniform float enableGlobalAO;

]]>
// Cg profiles for older hardware don't support data-dependent loops
<? if vars."num passes".int <= 1 ?>
#define NUM_PASSES  1
<? elsif vars."num passes".int == 2 ?>
#define NUM_PASSES  2
<? elsif vars."num passes".int == 3 ?>
#define NUM_PASSES  3
<? elsif vars."num passes".int == 4 ?>
#define NUM_PASSES  4
<? elsif vars."num passes".int == 5 ?>
#define NUM_PASSES  5
<? else ?>
#define NUM_PASSES  6
<? endif ?>
<![CDATA[

struct vertex2fragment 
{
  float2 TexCoord : TEXCOORD0;
  float4 ScreenPos : TEXCOORD1;
};

void GetSamplePositionAndNormal(float2 texCoord, float3 ray, float3 scale, out float2 sampleTC,
                                out float3 sampleScreenPos, out float3 sampleNormal)
{
  ray *= scale;
  sampleTC = texCoord + ray.xy;
  float2 sampleXY = (sampleTC - 0.5) * 2.0;
  float sampleDepth = tex2D (g_DepthTexture, sampleTC).x * g_FarClipDistance;
  
  sampleScreenPos = float3 (sampleXY.x, -sampleXY.y, sampleDepth);
  sampleNormal = tex2D (g_NormalTexture, sampleTC).xyz * 2.0 - 1.0;
}

float ComputeOcclusion(float3 vecToOccluder, float3 normal, float3 sampleNormal)
{  
  float distance = length (vecToOccluder);
  float deltaN = (1.0 - max (0.0, dot (normal, sampleNormal))) *
      max (0.0 - g_SelfOcclusion, dot (normal, normalize (vecToOccluder)));
  return deltaN * (1.0 - smoothstep (0.01, g_MaxOccluderDistance, distance));      
}

float3 ComputeIndirectRadiance(float2 sampleTC, float3 vecToOccluder, float3 normal, float3 sampleNormal, float ao)
{

  float3 occluderRadiance = tex2D (g_DirectRadianceTexture, sampleTC).rgb;
  return g_IndirectLightStrength * occluderRadiance * ao;
}

float4 main(float2 texCoord : TEXCOORD0) : COLOR
{
  const float3 samples[8] =
	{
		normalize (float3 ( 1, 1, 1)), 
		normalize (float3 (-1,-1,-1)), 
		normalize (float3 (-1,-1, 1)), 
		normalize (float3 (-1, 1,-1)), 
		normalize (float3 (-1, 1 ,1)), 
		normalize (float3 ( 1,-1,-1)), 
		normalize (float3 ( 1,-1, 1)), 
		normalize (float3 ( 1, 1,-1)) 
	};	
  
  float2 screenXY = texCoord*2 - 1;

  
  float depth = tex2D (g_DepthTexture, texCoord).x * g_FarClipDistance;
  float3 normal = tex2D (g_NormalTexture, texCoord).xyz * 2.0 - 1.0;
  float3 screenPos = float3 (screenXY.x, -screenXY.y, depth);

    
  float invDepth = 1.0 / depth;
  if (depth < 0.00001) invDepth = 1.0f;
  float3 scale[2];
  scale[0] = float3 (g_SampleRadius * invDepth, g_SampleRadius * invDepth, g_SampleRadius / g_FarClipDistance);
]]>
<?if (vars."enable ambient occlusion".float == 1) && (vars."detail sample radius".float != 0) ?>
  scale[1] = float3 (g_DetailSampleRadius * invDepth, g_DetailSampleRadius * invDepth, g_DetailSampleRadius / g_FarClipDistance);
<?else?>
  scale[1] = scale[0];
<?endif?>
<![CDATA[
  float AO = 0.0;
  float AOsum = 0.0;
  float3 indirectRadiance = float3(0.0);
  float totalSamples = 8.0 * NUM_PASSES;
  float sampleStep = 0.5 / totalSamples;
  float sampleLength = 0.5;
  
  float3 randomNormal = tex2D (g_RndNormalsTexture, texCoord * ((g_Resolution / 64))).rgb * 2.0 - 1.0;
    randomNormal = normalize (randomNormal);
  
  for (int n=0; n < NUM_PASSES; n++)
  {    
    for (int i=0; i < 8; i++)
    {      
      float3 ray = reflect (samples[i] * sampleLength, randomNormal);
      
      sampleLength += sampleStep;
      float3 sampleScreenPos, sampleNormal;
      float2 sampleTC;
      GetSamplePositionAndNormal (texCoord, ray, scale[i % 2], sampleTC, sampleScreenPos, sampleNormal);
      float3 vecToOccluder = sampleScreenPos - screenPos;
]]>
<?if vars."enable ambient occlusion".float == 1 || vars."enable indirect light".float == 1 ?>
      AO = ComputeOcclusion (vecToOccluder, normal, sampleNormal);      
<?endif?>
<?if vars."enable indirect light".float == 1 ?>
      indirectRadiance += ComputeIndirectRadiance (sampleTC, vecToOccluder, normal, sampleNormal, AO);
<?endif?>
<?if vars."enable ambient occlusion".float == 1 ?>
      AOsum += g_OcclusionStrength * AO;
<?endif?>
    }
  }
<?if vars."enable ambient occlusion".float == 1 ?>
  AOsum /= totalSamples;
  AOsum += g_SelfOcclusion;
<?endif?>
<?if vars."enable indirect light".float == 1 ?>
  indirectRadiance /= 2.0 * totalSamples;
<?endif?>
<![CDATA[
  
  return float4 (indirectRadiance, 1.0 - saturate (AOsum));
}

]]>
</program>
</include>