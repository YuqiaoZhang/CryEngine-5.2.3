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

    output.position = input.position;

    return output;
}
