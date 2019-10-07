/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer : register(b0)
{
    matrix wvpMatrix[6];
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

struct GeometryInputType
{
    float4 position : POSITION;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
GeometryInputType RenderSceneVS(VertexInputType input)
{
    GeometryInputType output;
		
    input.position.w = 1.0f;

//  output.position = mul(input.position, float4x4(transformu, transformv, transformw, transformt));
    output.position = input.position;

    return output;
}
