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
};

struct GeometryInputType
{
    float4 position : SV_Position;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
GeometryInputType RenderSceneVS(VertexInputType input)
{
    GeometryInputType output;
		
    input.position.w = 1.0f;

    output.position = mul(input.position, wvpMatrix);

    return output;
}
