//////////////
// TYPEDEFS //
//////////////
struct BufferStruct
{
    float4 color;
};

Texture2D<uint> input;
RWStructuredBuffer<BufferStruct> ouput;

////////////////////////////////////////////////////////////////////////////////
// Compute Shader
////////////////////////////////////////////////////////////////////////////////
[numthreads(1, 1, 1)]
void RenderBufferCS(uint3 location : SV_DispatchThreadID)
{
  uint w, h, l;
  uint location_xy;

  input.GetDimensions(0, w, h, l);
  location_xy = (location.y * w) + location.x;

  ouput[location_xy].color = input[location.xy];
}
