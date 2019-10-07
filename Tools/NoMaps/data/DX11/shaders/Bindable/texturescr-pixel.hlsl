/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer : register(b0)
{
    matrix wvpMatrix;
    matrix wldMatrix;
    matrix lgtMatrix;
    float3 eyeVector;
};

cbuffer ResolutionBuffer : register(b1)
{
    float2 resolution;
    float2 offset;
    float  zoom;
    float  tile;
    float  channels;
    float  gamma;
    float4 multiplier;
    float4 transform[4];
};

static float3 GammaToLinear(float3 xyz)
{
	return float3(
		(xyz.x <= 0.04045f) ? xyz.x / 12.92f : pow(max(0.0f, (xyz.x + 0.055f) / 1.055f), 2.4f),
		(xyz.y <= 0.04045f) ? xyz.y / 12.92f : pow(max(0.0f, (xyz.y + 0.055f) / 1.055f), 2.4f),
		(xyz.z <= 0.04045f) ? xyz.z / 12.92f : pow(max(0.0f, (xyz.z + 0.055f) / 1.055f), 2.4f));
}

static float3 LinearToGamma(float3 xyz)
{
	return float3(
		(xyz.x <= 0.0031308f) ? xyz.x * 12.92f : 1.055f * pow(max(0.0f, xyz.x), 1.0f / 2.4f) - 0.055f,
		(xyz.y <= 0.0031308f) ? xyz.y * 12.92f : 1.055f * pow(max(0.0f, xyz.y), 1.0f / 2.4f) - 0.055f,
		(xyz.z <= 0.0031308f) ? xyz.z * 12.92f : 1.055f * pow(max(0.0f, xyz.z), 1.0f / 2.4f) - 0.055f);
}

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_Position;
    float2 texcoord : TEXCOORD;
};

Texture2D text;
SamplerState smpl;

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 RenderScenePS(PixelInputType input) : SV_Target
{
    float2 screencenter = 0.5f;
    float2 screenratio;
    float2 screencoord;
    float2 screenpixel;

    uint Width;
    uint Height;
    uint NumberOfLevels;

    text.GetDimensions(0, Width, Height, NumberOfLevels);

    screenratio = resolution.x < resolution.y
      ? float2(1.0f, resolution.x / resolution.y)
      : float2(resolution.y / resolution.x, 1.0f);
    
    screenpixel = (float2(1.0f, 1.0f) / resolution) / screenratio;
    screenpixel = screenpixel * zoom;

    screencoord = (input.position.xy / resolution) / screenratio;
    screencoord = (0.5f           / screenratio ) - screencoord;
    screencoord = (screencoord * zoom);
    screencoord = (0.5f - (offset / screenratio)) - screencoord;
		
    // fetch point-sampled texels
    float3   texcrd = float3(
      screencoord.x, screencoord.y, 1.0f);
    float2   trncrd = float2(
      dot(transform[0].xyz, texcrd) * transform[0].w,
      dot(transform[1].xyz, texcrd) * transform[1].w
    );
    
    int2 icoord = int2(frac(trncrd) * int2(Width, Height));

    float4 texel = text.Load(int3(icoord, 0)).rgba;
//  float4 texel = text.Sample(smpl, screencoord).rgba;
//  float4 texel = text.Sample(smpl, input.texcoord).rgba;

    texel.rgb = float3(
      dot(transform[3].xy, texel.rg * 2.0f - 1.0f),
      dot(transform[3].zw, texel.rg * 2.0f - 1.0f), texel.b * 2.0f - 1.0f) * 0.5f + 0.5f;
    
    if (channels < 1.0f)
      texel = float4(   0.0f,    0.0f,    0.0f, 1.0f);
    else if (channels < 2.0f)
      texel = float4(texel.r, texel.r, texel.r, 1.0f);
    else if (channels < 3.0f)
      texel = float4(texel.r, texel.g, texel.g, 1.0f);
    else if (channels < 4.0f)
      texel = float4(texel.r, texel.g, texel.b, 1.0f);

    float4
      raster = float4(0.50f, 0.50f, 0.50f, 1.0f);
    if (abs(0.5f - frac(screencoord.x * 2)) > (0.5f - screenpixel.x) ||
        abs(0.5f - frac(screencoord.y * 2)) > (0.5f - screenpixel.y))
      raster = float4(0.45f, 0.45f, 0.45f, 1.0f);
    if (abs(0.5f - frac(screencoord.x * 1)) > (0.5f - screenpixel.x) ||
        abs(0.5f - frac(screencoord.y * 1)) > (0.5f - screenpixel.y))
      raster = float4(0.33f, 0.33f, 0.33f, 1.0f);

    // draw raster
    if (tile == 0.0f) {
      if ((trncrd.x < 0.0f) || (trncrd.x >= 1.0f) ||
          (trncrd.y < 0.0f) || (trncrd.y >= 1.0f)) {
        return raster;
      }
    }
    else if (tile < 0.0f) {
      if ((screencoord.x < 0.0f) || (screencoord.x >= 1.0f) ||
          (screencoord.y < 0.0f) || (screencoord.y >= 1.0f)) {
        return raster;
      }
    }

    texel.rgb *= multiplier.rgb * multiplier.a;
    if (gamma >= 0.5f)
      texel.rgb = LinearToGamma(texel.rgb);

    if (0/*blend*/) {
      float step = texel.a == 1.0f ? 1.0f : 0.0f;

      texel.rgb = texel.rgb * step + raster.rgb * (1.0f - step);
    }
		
    return float4(texel.rgb, 1.0f);
}
