// haxd 3ds max effect file - preview shader for cryEngine 3 blend layer shader
// Simple example of hooking up additional data from lights - in this case
// the diffuse value is obtained from the cuurent light color
// 



// light direction (view space)
float3 lightDir : Direction <  
	string UIName = "Light Direction"; 
	string Object = "TargetLight";
	int RefID = 0;
	> = {-0.577, -0.577, 0.577};

// light intensity
float4 I_a = { 0.1f, 0.1f, 0.1f, 1.0f };    // ambient
float4 I_d = { 1.0f, 1.0f, 1.0f, 1.0f };    // diffuse
float4 I_s = { 1.0f, 1.0f, 1.0f, 1.0f };    // specular

// material reflectivity
float4 k_a  <
	string UIName = "Ambient";
> = float4( 0.0f, 0.0f, 0.0f, 1.0f );    // ambient

//diffuse setting controlled by the light if available	
float4 k_d : LIGHTCOLOR <
	int LightRef = 0;
> = float4( 1.0f, 1.0f, 1.0f, 1.0f );    // diffuse
	
float BlendFactor <
	string UIName = "Blend Factor";
	string UIType = "slider";
	float UIMin = 0.0f;
	float UIMax = 16.0f;	
	>  = 16;
	
float BlendFalloff <
	string UIName = "Blend Falloff";
	string UIType = "slider";
	float UIMin = 0.0f;
	float UIMax = 128.0f;	
	>  = 15;
/*
float4 k_s  <
	string UIName = "Specular";
	> = float4( 1.0f, 1.0f, 1.0f, 1.0f );    // diffuse    // specular

int n<
	string UIName = "Specular Power";
	string UIType = "IntSpinner";
	float UIMin = 0.0f;
	float UIMax = 50.0f;	
	>  = 15;
*/

// texture
texture Tex0 : DiffuseMap < 
	string name = "tiger.bmp"; 
	string UIName = "Top Diffuse Layer";
	>;

texture Tex1 : DiffuseMap < 
	string name = "bark.dds"; 
	string UIName = "Bottom Diffuse Layer";
	>;	
	
texture Mask : DiffuseMap < 
	string name = "test_noise.dds"; 
	string UIName = "Blend Mask";
	>;

// tell 3dsmax to pass vertexcolors to shader

int texcoord0 : Texcoord
<
	int Texcoord = 0;
	int MapChannel = 0;
>;
int texcoord1 : Texcoord
<
	int Texcoord = 1;
	int MapChannel = 1;
>;
int texcoord2 : Texcoord
<
	int Texcoord = 2;
	int MapChannel = -2;
>;	

// transformations
float4x4 World      : 		WORLD;
float4x4 View       : 		VIEW;
float4x4 Projection : 		PROJECTION;
float4x4 WorldViewProj : 	WORLDVIEWPROJ;
float4x4 WorldView : 		WORLDVIEW;

struct appdata {
    float3 Pos	: POSITION;
	float4 Color	: TEXCOORD0;
    float4 Norm	: NORMAL;
	float4 Tex	: TEXCOORD1;
	float4 Alpha	: TEXCOORD2;
};

struct VS_OUTPUT
{
    float4 Pos  : POSITION;
    float4 Diff : COLOR0;
    float4 VertexCol	: TEXCOORD1;
    float2 Tex  : TEXCOORD0;
};

VS_OUTPUT VS(
    appdata IN
    )
{
    VS_OUTPUT Out = (VS_OUTPUT)0;

    float3 L = lightDir;
       

    float3 P = mul(float4(IN.Pos, 1),(float4x4)World);  // position (view space)
    float3 N = normalize(mul(IN.Norm,(float3x3)World)); // normal (view space)

    float3 R = normalize(2 * dot(N, L) * N - L);          // reflection vector (view space)
    float3 V = normalize(P);                             // view direction (view space)

    Out.Pos  = mul(float4(IN.Pos,1),WorldViewProj);    // position (projected)
    
	Out.VertexCol = float4(IN.Color.xyz,IN.Alpha.x);
    Out.Diff = k_a + I_d * k_d * max(0, dot(N, L)); // diffuse + ambient
    //Out.Spec = I_s * k_s * pow(max(0, dot(R, V)), n/4);   // specular
    Out.Tex  = IN.Tex;   

    return Out;
}

sampler Sampler0 = sampler_state
{
    Texture   = (Tex0);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU = WRAP;
    ADDRESSV = WRAP;
};

sampler Sampler1 = sampler_state
{
    Texture   = (Tex1);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU = WRAP;
    ADDRESSV = WRAP;
};

sampler BlendMapSampler = sampler_state
{
    Texture   = (Mask);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    ADDRESSU = WRAP;
    ADDRESSV = WRAP;
};

float4 PS(
    float4 Diff : COLOR0,
    float3 Tex  : TEXCOORD0,
	float4 VertexCol	: TEXCOORD1
    ) : COLOR
{
	float4 blendMap = tex2D( BlendMapSampler, Tex );
	float blendFac = VertexCol.a * (pow(blendMap.r,2.2)) * (1 + BlendFactor);
	blendFac = saturate( pow( blendFac, BlendFalloff ) );  // Falloff

	float4 cDiffuseMap = lerp( tex2D(Sampler0, Tex), tex2D(Sampler1, Tex), blendFac );

    float4 color = cDiffuseMap * Diff * VertexCol;
	color.a = 1.0;
    return  color ;
}


technique DefaultTechnique
{
    pass P0
    {
        // shaders
        CullMode = None;
       	VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_1_1 PS();
    }  
}

