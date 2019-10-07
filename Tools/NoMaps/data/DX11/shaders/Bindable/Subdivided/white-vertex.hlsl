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

struct HullInputType
{
    float4 position : WORLDPOS;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
HullInputType RenderSceneVS(VertexInputType input)
{
    HullInputType output;
		
    input.position.w = 1.0f;

//  output.position = mul(input.position, wvpMatrix);
    output.position = input.position;

    return output;
}
