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
#define	ni	 nointerpolation
#define	np	 noperspective

struct VertexInputType
{
    float4 position  : POSITION;
    float2 texcoord  : TEXCOORD;

    /* 5:9 compression */
    float2 tangent   : TANGENT;
    float2 bitangent : BITANGENT;
    int    tbnsign   : TBNSIGNS;
};

struct GeometryInputType
{
    float4 position  : SV_Position;
    float2 texcoord  : TEXCOORD;

    float3 normal    : NORMAL;
    float3 tangent   : TANGENT;
    float3 bitangent : BITANGENT;
    float3 tbnsigns  : TBNSIGNS;

    float3 vertexpos : VERTEXPOS;
ni  float3 camerapos : CAMERAPOS;
};

////////////////////////////////////////////////////////////////////////////////
float getsign(int bitfield, int pos) {
    return asfloat(((bitfield << pos) & 0x80000000) | 0x3F800000);
}

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
GeometryInputType RenderSceneVS(VertexInputType input)
{
    GeometryInputType output;

    input.position.w     = 1.0f;

    output.tangent.xy    = input.tangent.xy;
    output.bitangent.xy  = input.bitangent.xy;
    output.tangent.z     = getsign(input.tbnsign, 2) * sqrt(1.0f - saturate((output.tangent.x   * output.tangent.x  ) + (output.tangent.y   * output.tangent.y  )));
    output.bitangent.z   = getsign(input.tbnsign, 1) * sqrt(1.0f - saturate((output.bitangent.x * output.bitangent.x) + (output.bitangent.y * output.bitangent.y)));
    output.normal.xyz    = getsign(input.tbnsign, 0) * normalize(cross(output.tangent.xyz, output.bitangent.xyz));
    
    output.position  = mul(       input.position.xyzw    , wvpMatrix);
    output.tangent   = mul(float4(output.tangent  , 1.0f), wldMatrix).xyz;
    output.bitangent = mul(float4(output.bitangent, 1.0f), wldMatrix).xyz;
    output.normal    = mul(float4(output.normal   , 1.0f), wldMatrix).xyz;

    output.texcoord  = input.texcoord;
    output.vertexpos = mul(float4(input.position.xyz, 1.0f), wldMatrix).xyz;
//  output.camerapos = mul(float4(eyeVector         , 1.0f), wldMatrix).xyz;
//  output.vertexpos = input.position.xyz;
//  output.vertexpos = input.position.xzy;
    output.camerapos = eyeVector;
    output.tbnsigns  = float3(getsign(input.tbnsign, 2) == -1.0f ? 1.0f : 0.0f, getsign(input.tbnsign, 1) == -1.0f ? 1.0f : 0.0f, getsign(input.tbnsign, 0) == -1.0f ? 1.0f : 0.0f);
		
    /*
    output.normal    = mul(output.normal   , wldMatrix).xyz;
    output.tangent   = mul(output.tangent  , wldMatrix).xyz;
    output.bitangent = mul(output.bitangent, wldMatrix).xyz;
    */

    return output;
}
