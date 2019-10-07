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
struct HullInputType
{
    float4 position : WORLDPOS;
};

struct TessInputType
{
    float edges[3] : SV_TessFactor;
    float inside   : SV_InsideTessFactor;
};

struct DomainInputType
{
    float4 position : WORLDPOS;
};

////////////////////////////////////////////////////////////////////////////////
// Hull Shader
////////////////////////////////////////////////////////////////////////////////
TessInputType TesselateSceneHS(InputPatch<HullInputType, 3> inputs, uint PatchID : SV_PrimitiveID)
{
    TessInputType output;

    output.edges[0] = 1.0f;
    output.edges[1] = 1.0f;
    output.edges[2] = 1.0f;
    output.inside   = 1.0f;

    return output;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("TesselateSceneHS")]
[maxtessfactor(15.0)]
DomainInputType RenderSceneHS(InputPatch<HullInputType, 3> inputs, uint PointID : SV_OutputControlPointID)
{
    HullInputType input = inputs[PointID];
    DomainInputType output;

    input.position.w = 1.0f;
//  output.position = mul(input.position, wvpMatrix);
    output.position = input.position;

    return output;
}
