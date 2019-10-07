/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer : register(b0)
{
    matrix wvpMatrix;
    matrix wldMatrix;
    float3 eyeVector;
};

//////////////
// TYPEDEFS //
//////////////
struct DomainInputType
{
    float4 position : WORLDPOS;
};

struct TessInputType
{
    float edges[3] : SV_TessFactor;
    float inside   : SV_InsideTessFactor;
};

struct PixelInputType
{
    float4 position : SV_Position;
};

////////////////////////////////////////////////////////////////////////////////
// Domain Shader
////////////////////////////////////////////////////////////////////////////////
[domain("tri")]
PixelInputType RenderSceneDS(TessInputType factors, float3 BarycentricCoordinates : SV_DomainLocation, const OutputPatch<DomainInputType, 3> inputs)
{
    DomainInputType input;
    PixelInputType output;
    
    input.position.w = 1.0f;
    input.position.xyz = BarycentricCoordinates.x * inputs[0].position.xyz + 
                         BarycentricCoordinates.y * inputs[1].position.xyz + 
                         BarycentricCoordinates.z * inputs[2].position.xyz;
    
    output.position = mul(input.position, wvpMatrix);

    return output;
}
