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

struct PixelInputType
{
    float4 position  : SV_Position;
    float2 texcoord  : TEXCOORD;
    
    float3 normal    : NORMAL;
    float3 tangent   : TANGENT;
    float3 bitangent : BITANGENT;
    float3 tbnsigns  : TBNSIGNS;
    
    float3 barypos   : BARYPOS;
ni  float3 faceplane : FACEPLANE;
    
    float3 vertexpos : VERTEXPOS;
ni  float3 camerapos : CAMERAPOS;

ni  float2 areas     : AREAS;
};

////////////////////////////////////////////////////////////////////////////////
// Geometry Shader
////////////////////////////////////////////////////////////////////////////////
[maxvertexcount(3)]
void RenderSceneGS(triangle GeometryInputType input[3], inout TriangleStream<PixelInputType> output)
{
  PixelInputType tri;
  
	float3 crosspos = cross(
    float3(input[0].vertexpos.xyz) - float3(input[1].vertexpos.xyz), 
    float3(input[0].vertexpos.xyz) - float3(input[2].vertexpos.xyz));
	float3 crosstex = cross(
    float3(input[0].texcoord.xy, 0) - float3(input[1].texcoord.xy, 0), 
    float3(input[0].texcoord.xy, 0) - float3(input[2].texcoord.xy, 0));

  /* pass through the three connected vertices to all fragments of the triangle */
  [unroll]
  for (uint i = 0; i < 3; ++i) {
    tri.position  = input[i].position;
    tri.texcoord  = input[i].texcoord;
    
    tri.normal    = input[i].normal;
    tri.tangent   = input[i].tangent;
    tri.bitangent = input[i].bitangent;
    
    tri.areas.x   = length(crosspos);
    tri.areas.y   = length(crosstex);

    /* as the interpolator in the rasterizer is working
     * barycentric, this will automatically return the
     * exact barycentric coordinates of the position
     * of the pixel in the pixel shader
     */
    tri.barypos   = float3(i == 0 ? 1 : 0, i == 1 ? 1 : 0, i == 2 ? 1 : 0);
    tri.faceplane = normalize(crosspos);

    tri.vertexpos = input[i].vertexpos;
    tri.camerapos = input[i].camerapos;
    tri.tbnsigns  = input[i].tbnsigns;
  
    output.Append(tri);
  }
  
  output.RestartStrip();
}
