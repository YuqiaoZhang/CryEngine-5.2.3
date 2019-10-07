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

    /* 5:9 compression */
    float2 normal   : NORMAL;
    float2 tangent  : TANGENT;
    int    tbnsign  : TBNSIGNS;
};

struct PixelInputType
{
    float4 position  : SV_Position;
    float2 texcoord  : TEXCOORD;

    float3 normal    : NORMAL;
    float3 tangent   : TANGENT;
    float3 bitangent : BITANGENT;
};

////////////////////////////////////////////////////////////////////////////////
float getsign(int bitfield, int pos) {
    return asfloat(((bitfield << pos) & 0x80000000) | 0x3F800000);
}

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType RenderSceneVS(VertexInputType input)
{
    PixelInputType output;
		
    input.position.w = 1.0f;

    output.normal.xy     = input.normal.xy;
    output.tangent.xy    = input.tangent.xy;
    output.normal.z      = getsign(input.tbnsign, 0) * sqrt(1.0f - (output.normal.x  * output.normal.x ) - (output.normal.y  * output.normal.y ));
    output.tangent.z     = getsign(input.tbnsign, 2) * sqrt(1.0f - (output.tangent.x * output.tangent.x) - (output.tangent.y * output.tangent.y));
    output.bitangent.xyz = getsign(input.tbnsign, 1) * cross(output.normal.xyz, output.tangent.xyz);

    output.position  = mul(input.position  , wvpMatrix);
    output.texcoord  = input.texcoord;

    /*
    output.normal    = mul(output.normal   , wldMatrix).xyz;
    output.tangent   = mul(output.tangent  , wldMatrix).xyz;
    output.bitangent = mul(output.bitangent, wldMatrix).xyz;
    */

    return output;
}
