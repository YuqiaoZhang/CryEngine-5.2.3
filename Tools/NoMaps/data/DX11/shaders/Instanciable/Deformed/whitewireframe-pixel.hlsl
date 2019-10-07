//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_Position;
    
    float4 positionx : MINEPOS;
    float4 positiona : PREVPOS;
    float4 positionb : CURRPOS;
    float4 positionc : NEXTPOS;
    
    float3 barypos : BARYPOS;
};

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 RenderScenePS(PixelInputType input) : SV_Target
{
    float3 ddxHeights = ddx(input.barypos);
    float3 ddyHeights = ddy(input.barypos);
    float3 ddHeights2 = ddxHeights * ddxHeights + ddyHeights * ddyHeights;
    float3 pixHeights2 = (input.barypos * input.barypos) / ddHeights2;
    
    float dist = sqrt(min(min(pixHeights2.x, pixHeights2.y), pixHeights2.z));
    
    return dist;
    
    float2 dsta = distance(input.positionx.xy, input.positiona.xy);
    float2 dstb = distance(input.positionx.xy, input.positionb.xy);
    float2 dstc = distance(input.positionx.xy, input.positionc.xy);
    float2 dst  = max(dsta, max(dstb, dstc));

    return float4(length(dst) * 0.1f, 0.5f, 0.0f, 1.0f);
}
