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
    
    float3 barypos : BARYPOS;
};

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 RenderScenePS(PixelInputType input) : SV_Target
{
    float3 ddxHeights = ddx(input.barypos);
    float3 ddyHeights = ddy(input.barypos);
    float3 ddHeights2 =  ddxHeights * ddxHeights + ddyHeights * ddyHeights;
    float3 pixHeights2 = (input.barypos * input.barypos) / ddHeights2;
    
    float dist = sqrt(min(min(pixHeights2.x, pixHeights2.y), pixHeights2.z));
    
    return float4(color * saturate(dist), 1.0f);
}
