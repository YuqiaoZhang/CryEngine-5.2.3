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

struct HullInputType
{
    float4 position : WORLDPOS;

    float4 transformu : TRANSFORMU;
    float4 transformv : TRANSFORMV;
    float4 transformw : TRANSFORMW;
    float4 transformt : TRANSFORMT;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
HullInputType RenderSceneVS(VertexInputType input)
{
    HullInputType output;
		
    input.position.w = 1.0f;

//  output.position = mul(input.position, float4x4(transformu, transformv, transformw, transformt));
//  output.position = mul(input.position, wvpMatrix);
    output.position = input.position;

    output.transformu = input.transformu;
    output.transformv = input.transformv;
    output.transformw = input.transformw;
    output.transformt = input.transformt;

    return output;
}
