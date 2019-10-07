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
    uint   vertexid : SV_VertexID;
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
    
#define	radius	1.0
    /* rotate the axis-access */
    uint mod = input.vertexid % 3;
    
    /* the cube's vertices are axis-aligned */
    float3 corner = input.position.xyz;
    float3 axis = input.position.xyz;
    
    /* projected axis */
    /**/ if (mod == 0)
      axis[0] = 0;
    else if (mod == 1)
      axis[1] = 0;
    else if (mod == 2)
      axis[2] = 0;
    
    /* angle, sine */
    float angle = length(cross(corner, axis)) / (length(corner) * length(axis));
    float ref = length(corner) - radius;
    
    /* move align the axis towards the center */
    /**/ if (mod == 0)
      input.position[0] -= ref / angle;
    else if (mod == 1)
      input.position[1] -= ref / angle;
    else if (mod == 2)
      input.position[2] -= ref / angle;
		
    input.position.w = 1.0f;

    output.position = mul(input.position, wvpMatrix);

    return output;
}
