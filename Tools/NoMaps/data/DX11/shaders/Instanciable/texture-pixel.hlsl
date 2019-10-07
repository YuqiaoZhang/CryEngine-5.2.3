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

cbuffer ColorBuffer : register(b1)
{
    float3 color;
};

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
    return text.Sample(smpl, input.texcoord);
}
