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
    float2 texcoord : TEXCOORD;

    uint   vertexid : SV_VertexID;
};

struct PixelInputType
{
    float4 position : SV_Position;
    float2 texcoord : TEXCOORD;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType RenderSceneVS(VertexInputType input)
{
    PixelInputType output;

    input.position.w = 1.0f;
    output.position  = mul(input.position, wvpMatrix);
    output.texcoord  = input.texcoord;
    
    switch (input.vertexid) {
      case 0: output.position.xyzw = float4(-1, -1, 0, 1); break;
      case 1: output.position.xyzw = float4(-1,  1, 0, 1); break;
      case 2: output.position.xyzw = float4( 1, -1, 0, 1); break;
      case 3: output.position.xyzw = float4( 1,  1, 0, 1); break;
    }
    
    return output;
}
