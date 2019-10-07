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
struct GeometryInputType
{
    float4 position : POSITION;
};

struct PixelInputType
{
    float4 position : SV_Position;
    uint   rtindex  : SV_RenderTargetArrayIndex;
    uint   vpindex  : SV_ViewportArrayIndex;
};

////////////////////////////////////////////////////////////////////////////////
// Geometry Shader
////////////////////////////////////////////////////////////////////////////////
[maxvertexcount(18)]
void RenderSceneGS(triangle GeometryInputType input[3], inout TriangleStream<PixelInputType> output)
{
  PixelInputType tri;

  for (int f = 0; f < 6; ++f) {
    tri.rtindex = f;
    tri.vpindex = f;

    for (int v = 0; v < 3; v++) {
      tri.position = mul(input[v].position, wvpMatrix[f]);

      output.Append(tri);
    }

    output.RestartStrip();
  }
}
