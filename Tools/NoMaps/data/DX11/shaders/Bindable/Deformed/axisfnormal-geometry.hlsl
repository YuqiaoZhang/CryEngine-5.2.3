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
struct GeometryInputType
{
    float4 position : POSITION;
};

struct PixelInputType
{
    float4 position : SV_Position;
};

////////////////////////////////////////////////////////////////////////////////
// Geometry Shader
////////////////////////////////////////////////////////////////////////////////
[maxvertexcount(3)]
void RenderSceneGS(triangle GeometryInputType input[3], inout TriangleStream<PixelInputType> output)
{
  PixelInputType center;
  PixelInputType direct;
  PixelInputType offset;
  PixelInputType tri[3];

  /* form an arrow in world-space */
  /* form an arrow in homogenous view-space */
  /* form an arrow in perspective screen-space */
  center.position = (
    input[0].position * 0.334f +
    input[1].position * 0.333f +
    input[2].position * 0.333f
  );

  /* align arrow-triangle with vertex[0] */
  direct.position = (
    input[0].position -
    center.position
  );

  /* handedness matters */
  offset.position = cross(
    input[1].position - input[0].position,
    input[2].position - input[0].position
  );

  /* the resulting arrow will be perspective correct */
  tri[0].position = center.position + normalize(direct.position) * 0.01f;
  tri[1].position = center.position - normalize(direct.position) * 0.01f;
  tri[2].position = center.position + normalize(offset.position) * 0.10f;

  tri[0].position = mul(tri[0].position, wvpMatrix);
  tri[1].position = mul(tri[1].position, wvpMatrix);
  tri[2].position = mul(tri[2].position, wvpMatrix);

  output.Append(tri[0]);
  output.Append(tri[1]);
  output.Append(tri[2]);

  output.RestartStrip();
}
