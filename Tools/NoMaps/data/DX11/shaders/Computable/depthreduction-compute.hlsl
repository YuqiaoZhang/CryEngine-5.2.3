//////////////
// TYPEDEFS //
//////////////
Texture2D<uint> input;
RWTexture2DArray<uint> ouput;

/////////////
// GLOBALS //
/////////////
#define		 read001x001	input
#define		 read002x002	s008x008
#define		 read004x004	s004x004
#define		 read008x008	s002x002
#define		 read016x016	s001x001

groupshared uint s008x008[8][8];
groupshared uint s004x004[4][4];
groupshared uint s002x002[2][2];

////////////////////////////////////////////////////////////////////////////////
// Compute Shader
////////////////////////////////////////////////////////////////////////////////
[numthreads(8, 8, 1)]
void RenderBufferCS(uint3 location : SV_DispatchThreadID)
{
/*oupshar*/ uint s001x001[1][1];

  /* average first level */
  if ((location.y | location.x) < 8) { location.z = 0;

    ouput[(location.xyz + 0) >> 0] =
    read002x002[location.y][location.x] = (uint)dot(0.25f, float4(
      input[(location.xy << 1) + uint2(0, 0)],
      input[(location.xy << 1) + uint2(0, 1)],
      input[(location.xy << 1) + uint2(1, 0)],
      input[(location.xy << 1) + uint2(1, 1)]));
  }

  /* average second level */
  if ((location.y | location.x) < 4) { location.z = 1;

    ouput[(location.xyz + 1) >> 1] =
    read004x004[location.y][location.x] = (uint)dot(0.25f, float4(
      read002x002[(location.y << 1) + 0][(location.x << 1) + 0],
      read002x002[(location.y << 1) + 0][(location.x << 1) + 1],
      read002x002[(location.y << 1) + 1][(location.x << 1) + 0],
      read002x002[(location.y << 1) + 1][(location.x << 1) + 1]));
  }

  /* average third level */
  if ((location.y | location.x) < 2) { location.z = 2;

    ouput[(location.xyz + 3) >> 2] =
    read008x008[location.y][location.x] = (uint)dot(0.25f, float4(
      read004x004[(location.y << 1) + 0][(location.x << 1) + 0],
      read004x004[(location.y << 1) + 0][(location.x << 1) + 1],
      read004x004[(location.y << 1) + 1][(location.x << 1) + 0],
      read004x004[(location.y << 1) + 1][(location.x << 1) + 1]));
  }

  /* average fourth level */
  if ((location.y | location.x) < 1) { location.z = 3;

    ouput[(location.xyz + 7) >> 3] =
    read016x016[location.y][location.x] = (uint)dot(0.25f, float4(
      read008x008[(location.y << 1) + 0][(location.x << 1) + 0],
      read008x008[(location.y << 1) + 0][(location.x << 1) + 1],
      read008x008[(location.y << 1) + 1][(location.x << 1) + 0],
      read008x008[(location.y << 1) + 1][(location.x << 1) + 1]));
  }
}
