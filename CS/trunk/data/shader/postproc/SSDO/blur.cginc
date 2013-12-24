<include>
<![CDATA[

uniform sampler2D g_NormalTexture;
uniform sampler2D g_DepthTexture;
uniform float2 g_InvResolution;
uniform sampler2D g_SSDOTexture;
uniform int KernelSize;
uniform float2 Direction; // X:(1,0) - Y:(0,1)
uniform float PositionThreshold;
uniform float NormalThreshold;

const float TWO_PI = 6.28318f;
const float EPSILON = 0.01f;
]]>
// Cg profiles for older hardware don't support data-dependent loops
<? if vars."ssao blur kernelsize".int <= 1 ?>
#define KERNEL_SIZE  1
<? elsif vars."ssao blur kernelsize".int == 2 ?>
#define KERNEL_SIZE  2
<? elsif vars."ssao blur kernelsize".int == 3 ?>
#define KERNEL_SIZE  3
<? elsif vars."ssao blur kernelsize".int == 4 ?>
#define KERNEL_SIZE  4
<? elsif vars."ssao blur kernelsize".int == 5 ?>
#define KERNEL_SIZE  5
<? elsif vars."ssao blur kernelsize".int == 6 ?>
#define KERNEL_SIZE  6
<? else ?>
#define KERNEL_SIZE  7
<? endif ?>

<![CDATA[
float4 weightedRadiance = float4(0.0f);
float4 radiance = float4(0.0f);

float GaussianCoeff(float sampleDist, float sqrSigma)
{  
  float g = 1.0f / sqrt (TWO_PI * sqrSigma);
  return g * exp (-sampleDist * sampleDist / (2.0f * sqrSigma));
}

float WeightSample(float sampleDist, float2 sampleTexCoord,
                   float pixelDepth, float3 pixelNormalVS, float sqrGaussianSigma)
{  
  float3 sampleNormalVS = tex2D (g_NormalTexture, sampleTexCoord).xyz * 2.0 - 1.0;
  float sampleDepth = tex2D (g_DepthTexture, sampleTexCoord).x;
      
  float deltaZ = abs (sampleDepth - pixelDepth);
  float dotN = dot (pixelNormalVS, sampleNormalVS);       
  float4 sampleGI = tex2D (g_SSDOTexture, sampleTexCoord);
  
  float totalWeight = 0.0f;
  if (deltaZ < PositionThreshold && dotN > 1.0f - NormalThreshold)
  {
    totalWeight = GaussianCoeff (sampleDist, sqrGaussianSigma) * pow (dotN, 32.0f) / 
        (EPSILON + deltaZ);
    
    weightedRadiance += sampleGI * totalWeight;    
  }
  
  radiance += sampleGI;
  return totalWeight;
}

float4 main(in float2 texCoord : TEXCOORD0) : COLOR
{
  //texCoord.y = 1.0-texCoord.y;
  float3 pixelNormalVS = tex2D (g_NormalTexture, texCoord).xyz * 2.0 - 1.0;
  float pixelDepth = tex2D (g_DepthTexture, texCoord).x;
  
  float weightSum = 0.0f;
  float sqrGaussianSigma = KERNEL_SIZE * 0.3333f;
  sqrGaussianSigma *= sqrGaussianSigma;
  //g_InvResolution *= 3.0f * Direction; // strided blur (should make this a uniform)
  g_InvResolution *= Direction;
  
  weightSum += WeightSample (0, texCoord, pixelDepth, pixelNormalVS, sqrGaussianSigma); 
  
	for (int i=1; i <= KERNEL_SIZE; i++)
	{
    float2 tcOffset = i*g_InvResolution;
	      
    weightSum += WeightSample (i, texCoord + tcOffset, pixelDepth, pixelNormalVS, sqrGaussianSigma);    
    weightSum += WeightSample (i, texCoord - tcOffset, pixelDepth, pixelNormalVS, sqrGaussianSigma);
	}
  
  if (weightSum > 0.0f)
  {
    radiance = weightedRadiance / weightSum;
  }
  else
  {
    radiance /= 2.0f * KERNEL_SIZE + 1.0f;
  }
  
  return radiance;
  //return float4(pixelNormalVS,1);
  //return float4(pixelDepth,pixelDepth,pixelDepth,1);
  //return float4(radiance.xyz,1);
  //return float4(radiance.w,radiance.w,radiance.w,1);
}

]]>

</include>
