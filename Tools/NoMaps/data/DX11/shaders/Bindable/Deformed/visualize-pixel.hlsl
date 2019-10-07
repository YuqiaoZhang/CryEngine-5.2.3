/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer : register(b0)
{
    matrix wvpMatrix;
    matrix wldMatrix;
    matrix lgtMatrix;
    float3 eyeVector;
};

cbuffer ConfigBuffer : register(b1)
{
#define	RS_FLAT			0
#define	RS_UV			1
#define	RS_UVtoPOS		2
#define	RS_FNORM		3
#define	RS_TNORM		4
#define	RS_TTANG		5
#define	RS_TBITG		6
#define	RS_FTDEV		7
#define	RS_TORTH		8
#define	RS_TSIGN		21
#define	RS_TEX			9
#define	RS_TEXOS		22
#define	RS_TEXHT		10
#define	RS_TEXtoTS		11
#define	RS_TEXtoOS		12

#define	RS_FLIT			13
#define	RS_TLIT			14
#define	RS_TEXLITfromOS		15
#define	RS_TEXLITfromTS		16

#define	RS_FENV			17
#define	RS_TENV			18
#define	RS_TEXENVfromOS		19
#define	RS_TEXENVfromTS		20

  int mode;
  int channels;
  
  float  gamma;
  float  pad1;
  float4 multiplier;
  float4 transform[4];
};

static float3 light_key  = float3( 1.0f,  1.0f, 1.0f);
static float3 light_fill = float3(-1.0f,  0.5f, 0.5f);
static float3 light_back = float3( 0.0f, -1.0f, 0.0f);

static float3 color_key  = float3(0.97f, 1.00f, 0.95f) * 0.85f;
static float3 color_fill = float3(1.00f, 0.90f, 0.90f) * 0.35f;
static float3 color_back = float3(0.90f, 0.90f, 1.00f) * 0.25f;

//////////////
// TYPEDEFS //
//////////////
#define	ni	 nointerpolation
#define	np	 noperspective

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

Texture2D    text  : register(t0);
TextureCube envref : register(t13);
TextureCube envdif : register(t14);
Texture2D   envlut : register(t15);

SamplerState  smpl : register(s0);
SamplerState esmpl : register(s13);
SamplerState dsmpl : register(s14);
SamplerState lsmpl : register(s15);

////////////////////////////////////////////////////////////////////////////////
// Tools
////////////////////////////////////////////////////////////////////////////////
float3 ObjectSpaceToTangentSpace(in float3 tangent, in float3 bitangent, in float3 normal, in float3 vec, in bool invert)
{
	float m00, m01, m02;
	float m10, m11, m12;
	float m20, m21, m22;

	// SetFromVectors
	m00 = tangent.x; m01 = bitangent.x; m02 = normal.x;
	m10 = tangent.y; m11 = bitangent.y; m12 = normal.y;
	m20 = tangent.z; m21 = bitangent.z; m22 = normal.z;

	// Invert
	if (invert)
	{
#if 0
		// transpose instead
		m00 = tangent.x; m10 = bitangent.x; m20 = normal.x;
		m01 = tangent.y; m11 = bitangent.y; m21 = normal.y;
		m02 = tangent.z; m12 = bitangent.z; m22 = normal.z;
#else
		// rescue members
		float _m00 = m00, _m01 = m01, _m02 = m02;
		float _m10 = m10, _m11 = m11, _m12 = m12;
		float _m20 = m20, _m21 = m21, _m22 = m22;

		// calculate the cofactor-matrix (=transposed adjoint-matrix)
		m00 = _m22 * _m11 - _m12 * _m21;	m01 = _m02 * _m21 - _m22 * _m01;	m02 = _m12 * _m01 - _m02 * _m11;
		m10 = _m12 * _m20 - _m22 * _m10;	m11 = _m22 * _m00 - _m02 * _m20;	m12 = _m02 * _m10 - _m12 * _m00;
		m20 = _m10 * _m21 - _m20 * _m11;	m21 = _m20 * _m01 - _m00 * _m21;	m22 = _m00 * _m11 - _m10 * _m01;

		// calculate determinant
		float det = (_m00 * m00 + _m10 * m01 + _m20 * m02);
		if (abs(det) >= 1e-20f)
		{
			// divide the cofactor-matrix by the determinant
			float idet = 1.0f / det;

			m00 *= idet; m01 *= idet; m02 *= idet;
			m10 *= idet; m11 *= idet; m12 *= idet;
			m20 *= idet; m21 *= idet; m22 *= idet;
		}
#endif
	}
	
	return float3(
		m00 * vec.x + m01 * vec.y + m02 * vec.z,
		m10 * vec.x + m11 * vec.y + m12 * vec.z,
		m20 * vec.x + m21 * vec.y + m22 * vec.z
	);
}

static float3 GammaToLinear(float3 xyz)
{
	return float3(
		(xyz.x <= 0.04045f) ? xyz.x / 12.92f : pow(max(0.0f, (xyz.x + 0.055f) / 1.055f), 2.4f),
		(xyz.y <= 0.04045f) ? xyz.y / 12.92f : pow(max(0.0f, (xyz.y + 0.055f) / 1.055f), 2.4f),
		(xyz.z <= 0.04045f) ? xyz.z / 12.92f : pow(max(0.0f, (xyz.z + 0.055f) / 1.055f), 2.4f));
}

static float3 LinearToGamma(float3 xyz)
{
	return float3(
		(xyz.x <= 0.0031308f) ? xyz.x * 12.92f : 1.055f * pow(max(0.0f, xyz.x), 1.0f / 2.4f) - 0.055f,
		(xyz.y <= 0.0031308f) ? xyz.y * 12.92f : 1.055f * pow(max(0.0f, xyz.y), 1.0f / 2.4f) - 0.055f,
		(xyz.z <= 0.0031308f) ? xyz.z * 12.92f : 1.055f * pow(max(0.0f, xyz.z), 1.0f / 2.4f) - 0.055f);
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 RenderScenePS(PixelInputType input) : SV_Target
{
    // wireframe ---------------------------------------------------------------
    float3 ddxHeights = ddx(input.barypos);
    float3 ddyHeights = ddy(input.barypos);
    float3 ddHeights2 =  ddxHeights * ddxHeights + ddyHeights * ddyHeights;
    float3 pixHeights2 = (input.barypos * input.barypos) / ddHeights2;
    
    float dist = pow(min(min(pixHeights2.x, pixHeights2.y), pixHeights2.z), 1.0f / 8.0f);
    
    // flat color --------------------------------------------------------------
    float3 viz = float3(1.0f, 1.0f, 1.0f);
      
    // shared vars -------------------------------------------------------------
    input.faceplane = -normalize(input.faceplane);

#undef	NORMALNORMALIZATION_PIXEL
#ifdef	NORMALNORMALIZATION_PIXEL
    input.tangent   = normalize(input.tangent);
    input.bitangent = normalize(input.bitangent);
    input.normal    = normalize(input.normal);
#endif
    
    float3 interpolatedTangent   = input.tangent;
    float3 interpolatedBitangent = input.bitangent;
    float3 interpolatedNormal    = input.normal;
		
#define	NORMALINTERPOLATION_VERTEX
#ifdef	NORMALINTERPOLATION_VERTEX
    float3 tangentFrameNormal    = interpolatedNormal;
#else
    float3 tangentFrameNormal    = normalize(cross(input.tangent.xyz, input.bitangent.xyz));
    if (dot(tangentFrameNormal, interpolatedNormal) < 0)
        tangentFrameNormal = -tangentFrameNormal;
#endif
    
    // allow dynamic branching
    float2 tex,
           dx2,
           dy2;
    
    tex = float2(
    	dot(transform[0].xyz, float3(input.texcoord, 1.0f)) * transform[0].w,
    	dot(transform[1].xyz, float3(input.texcoord, 1.0f)) * transform[1].w
    );
    	
    dx2 = ddx_fine(tex);
    dy2 = ddx_fine(tex);

    // display mode ------------------------------------------------------------
    switch (abs(mode)) {
      case RS_FLAT:
				break;
      case RS_UV:
				viz = float3(input.texcoord, 1.0f);
				break;
      case RS_UVtoPOS:
				viz = (input.areas.x / input.areas.y) * 0.005f;
				break;
      case RS_FNORM:
				viz = normalize(input.faceplane) * 0.5f + 0.5f;
				break;
      case RS_TNORM:
				viz = normalize(input.normal) * 0.5f + 0.5f;
				break;
      case RS_TTANG:
				viz = normalize(input.tangent) * 0.5f + 0.5f;
				break;
      case RS_TBITG:
				viz = normalize(input.bitangent) * 0.5f + 0.5f;
				break;
      case RS_FTDEV:
				viz = pow(dot(input.faceplane, input.normal), 8.0f);
				break;
      case RS_TORTH:
				viz = dot(interpolatedTangent, interpolatedBitangent) * 0.5f + 0.5f;
				break;
      case RS_TSIGN:
				viz = input.tbnsigns.zzz * 0.5f + 0.25f;
				break;
      case RS_TEX:
				if (channels == 1)
					viz = text.SampleLevel(smpl, tex, 0).rrr;
				else
					viz = text.SampleLevel(smpl, tex, 0).rgb;

				viz.rgb = float3(
					dot(transform[3].xy, viz.rg * 2.0f - 1.0f),
					dot(transform[3].zw, viz.rg * 2.0f - 1.0f), viz.b * 2.0f - 1.0f) * 0.5f + 0.5f;
				
				viz.rgb *= multiplier.rgb;
//			viz.rgb *= multiplier.a;
				if (gamma >= 0.5f)
					viz.rgb = LinearToGamma(viz.rgb);
				break;
      case RS_TEXOS:
				if (channels == 1)
					viz = text.SampleLevel(smpl, tex, 0).rrr;
				else
					viz = text.SampleLevel(smpl, tex, 0).rgb;

				viz.rgb = float3(
					dot(transform[3].xy, viz.rg * 2.0f - 1.0f),
					dot(transform[3].zw, viz.rg * 2.0f - 1.0f), viz.b * 2.0f - 1.0f) * 0.5f + 0.5f;
				
				viz = mul(float4(viz, 1.0f), wldMatrix).xyz;
				break;
      case RS_TEXHT:
				if (channels == 1)
					viz = text.SampleLevel(smpl, tex, 0).rrr;
				else
					viz = text.SampleLevel(smpl, tex, 0).rgb;

				viz = dot(viz, float3(1.0f, 1.0f, 1.0f)) == 3.0f ? float3(1.0f, 1.0f, 1.0f) : float3(1.0f, 0.0f, 0.0f);
				break;
      case RS_TEXtoOS:
				viz = normalize(text.Sample(smpl, input.texcoord).rgb * 2.0f - 1.0f);
				viz = normalize(ObjectSpaceToTangentSpace(interpolatedTangent, interpolatedBitangent, tangentFrameNormal, viz, false));
				viz = viz * 0.5f + 0.5f;
				break;
      case RS_TEXtoTS:
				viz = normalize(text.Sample(smpl, input.texcoord).rgb * 2.0f - 1.0f);
				viz = mul(float4(viz, 1.0f), wldMatrix).xyz;
				viz = normalize(ObjectSpaceToTangentSpace(interpolatedTangent, interpolatedBitangent, tangentFrameNormal, viz, true));
				viz = viz * 0.5f + 0.5f;
				break;
      case RS_FLIT:
				viz = 
					color_key  * saturate(dot(normalize(input.faceplane), normalize(mul(float4(light_key , 1.0f), lgtMatrix).xyz))) +
					color_fill * saturate(dot(normalize(input.faceplane), normalize(mul(float4(light_fill, 1.0f), lgtMatrix).xyz))) +
					color_back * saturate(dot(normalize(input.faceplane), normalize(mul(float4(light_back, 1.0f), lgtMatrix).xyz)));
				break;
      case RS_TLIT:
				viz = 
					color_key  * saturate(dot(normalize(input.normal), normalize(mul(float4(light_key , 1.0f), lgtMatrix).xyz))) +
					color_fill * saturate(dot(normalize(input.normal), normalize(mul(float4(light_fill, 1.0f), lgtMatrix).xyz))) +
					color_back * saturate(dot(normalize(input.normal), normalize(mul(float4(light_back, 1.0f), lgtMatrix).xyz)));
				break;
      case RS_TEXLITfromOS:
				viz = normalize(text.Sample(smpl, input.texcoord).rgb * 2.0f - 1.0f);
				viz = mul(float4(viz, 1.0f), wldMatrix).xyz;
				viz = 
					color_key  * saturate(dot(viz, normalize(mul(float4(light_key , 1.0f), lgtMatrix).xyz))) +
					color_fill * saturate(dot(viz, normalize(mul(float4(light_fill, 1.0f), lgtMatrix).xyz))) +
					color_back * saturate(dot(viz, normalize(mul(float4(light_back, 1.0f), lgtMatrix).xyz)));
				break;
      case RS_TEXLITfromTS:
				viz = normalize(text.Sample(smpl, input.texcoord).rgb * 2.0f - 1.0f);
				viz = 
					color_key  * saturate(dot(normalize(ObjectSpaceToTangentSpace(interpolatedTangent, interpolatedBitangent, tangentFrameNormal, viz, false)), normalize(light_key ))) +
					color_fill * saturate(dot(normalize(ObjectSpaceToTangentSpace(interpolatedTangent, interpolatedBitangent, tangentFrameNormal, viz, false)), normalize(light_fill))) +
					color_back * saturate(dot(normalize(ObjectSpaceToTangentSpace(interpolatedTangent, interpolatedBitangent, tangentFrameNormal, viz, false)), normalize(light_back)));
				break;

      case RS_FENV:
				viz = 
					envdif.Sample(dsmpl, normalize(input.faceplane)).rgb;
				break;
      case RS_TENV:
				viz = 
					envdif.Sample(dsmpl, normalize(input.normal)).rgb;
				break;
      case RS_TEXENVfromOS:
				viz = normalize(text.Sample(smpl, input.texcoord).rgb * 2.0f - 1.0f);
				viz = mul(float4(viz, 1.0f), wldMatrix).xyz;
				viz = 
					envdif.Sample(dsmpl, viz).rgb;
				break;
      case RS_TEXENVfromTS:
				viz = normalize(text.Sample(smpl, input.texcoord).rgb * 2.0f - 1.0f);
				viz = 
					envdif.Sample(dsmpl, normalize(ObjectSpaceToTangentSpace(interpolatedTangent, interpolatedBitangent, tangentFrameNormal, viz, false))).rgb;
				break;
    }
    
//  viz = LinearToGamma(viz);
    if (mode <= 0)
      viz -= 0.25f - 0.25f * saturate(dist);
    
    return float4(viz, 1.0f);
}
