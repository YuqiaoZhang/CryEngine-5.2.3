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
struct VertexInputType
{
    float4 position : POSITION;
    uint   instid   : SV_InstanceID;

    float4 transformu : TRANSFORMU;
    float4 transformv : TRANSFORMV;
    float4 transformw : TRANSFORMW;
    float4 transformt : TRANSFORMT;
};

struct PixelInputType
{
    float4 position : SV_Position;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType RenderSceneVS(VertexInputType input)
{
    PixelInputType output;
		
    input.position.w = 1.0f;

//  output.position = mul(input.position, float4x4(transformu, transformv, transformw, transformt));
    output.position = mul(input.position, wvpMatrix);

    return output;
}
