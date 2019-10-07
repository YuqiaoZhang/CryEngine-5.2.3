// 3ds max effect file
// Simple vertex color - work with the Vertex Paint tool.  The max effect parser
// allows you to define any arbitary map channel to be passed in via a texcoord.
// In this case we are interested in Vertex Color, Illumination and Alpha which 
// are stored in 0,-1,-2 respectively.



// light direction (view space)

// transformations
float4x4 World      : 		WORLD;
float4x4 View       : 		VIEW;
float4x4 Projection : 		PROJECTION;
float4x4 WorldViewProj : 	WORLDVIEWPROJ;
float4x4 WorldView : 		WORLDVIEW;

float3 lightDir : Direction 
<  
	string UIName = "Target Light";
	string Object = "TargetLight";
> = {-0.577, -0.577, 0.577};

float baseOffset <
	string UIName = "base offset";
	string UIType = "slider";
	float UIMin = 0.0f;
	float UIMax = 10.0f;	
>  = 0.0f;

float waveMotion <
	string UIName = "wave strength";
	string UIType = "slider";
	float UIMin = 0.0f;
	float UIMax = 10.0f;	
>  = 0.5f;

float waveFreq <
	string UIName = "wave frequency";
	string UIType = "slider";
	float UIMin = 0.0f;
	float UIMax = 10.0f;	
>  = 2.0f;

float spatialPhase <
	string UIName = "spatial phase";
	string UIType = "slider";
	float UIMin = 0.0f;
	float UIMax = 10.0f;	
>  = 1.0f;

float sliderTime : TIME ;

int texcoord0 : Texcoord
<
	int Texcoord = 0;
	int MapChannel = 0;
>;
int texcoord1 : Texcoord
<
	int Texcoord = 1;
	int MapChannel = -2;
>;
int texcoord2 : Texcoord
<
	int Texcoord = 2;
	int MapChannel = -1;
>;


float4 I_a = { 0.1f, 0.1f, 0.1f, 1.0f };    // ambient
float4 I_d = { 1.0f, 1.0f, 1.0f, 1.0f };    // diffuse
float4 I_s = { 1.0f, 1.0f, 1.0f, 1.0f };    // specular

// material reflectivity
float4 k_a  ={ 0.0f, 0.0f, 0.0f, 1.0f };    // ambient
float4 k_d  ={ 1.0f, 1.0f, 1.0f, 1.0f };    // diffuse


struct AppData
{
    float3 Pos  : POSITION; 
    float3 Norm : NORMAL;
    float3 col	: TEXCOORD0;
    float3 alpha :TEXCOORD1;
    float3 illum :TEXCOORD2;
};

struct VS_OUTPUT
{
    float4 Pos  : POSITION;
    float4 color : COLOR;

};

VS_OUTPUT VS(
    AppData IN,	
    uniform bool shaded,
    uniform bool illum
  
)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;
	float phase = dot(sliderTime*float3(1,1,1)*waveFreq + spatialPhase*IN.Pos, float3(.333,.333,.333));
	float3 OffsetP = (baseOffset + sin(phase)*waveMotion)*IN.Norm*IN.alpha.x;
	
    float3 L = lightDir;
    float3 N = normalize(mul(IN.Norm,(float3x3)World)); // normal (view space)
    float3 R = normalize(2 * dot(N, L) * N - L);          // reflection vector (view space)
	float3 P = mul(float4(IN.Pos + OffsetP, 1),(float4x4)World);  // position (view space)
    float3 V = normalize(P);                             // view direction (view space)
    Out.Pos  = mul(float4(IN.Pos + OffsetP,1),WorldViewProj);    // position (projected)
    
    float4 diff;
    if(shaded == true)
    	diff = I_a * k_a + I_d * float4(IN.col,1) * max(0, dot(N, L)); // diffuse + ambient
    else
    	diff = float4(IN.col,1);
    
    if(illum == true)
    	Out.color = diff + float4(IN.illum,1);
    else
    	Out.color = diff;
    	
    //Out.color.a = IN.alpha.x;

    return Out;
    
}


technique Shaded_Unilluminated
{
    pass P0
    {
    	ZEnable = true;
	ZWriteEnable = true;
    	AlphaBlendEnable = TRUE;
        SrcBlend         = SRCALPHA;
        DestBlend        = InvSrcAlpha;  
        CullMode = None;
        ShadeMode = Gouraud;  
        // shaders
        
        VertexShader = compile vs_2_0 VS(true,false);
	PixelShader = NULL;
    }  
}

technique Shaded_Illuminated
{
    pass P0
    {
    	ZEnable = true;
	ZWriteEnable = true;
    	AlphaBlendEnable = TRUE;
        SrcBlend         = SRCALPHA;
        DestBlend        = InvSrcAlpha;  
        CullMode = None;
        ShadeMode = Gouraud;          
        // shaders
        
        VertexShader = compile vs_2_0 VS(true,true);
	PixelShader = NULL;
    }  
}

technique Unshaded_Illuminated
{
    pass P0
    {
    	ZEnable = true;
	ZWriteEnable = true;
    	AlphaBlendEnable = TRUE;
        SrcBlend         = SRCALPHA;
        DestBlend        = InvSrcAlpha;  
        CullMode = None;
        ShadeMode = Gouraud;          
        // shaders
        
        VertexShader = compile vs_2_0 VS(false,true);
	PixelShader = NULL;
    }  
}

technique Unshaded_Unilluminated
{
    pass P0
    {
    	ZEnable = true;
	ZWriteEnable = true;
    	AlphaBlendEnable = TRUE;
        SrcBlend         = SRCALPHA;
        DestBlend        = InvSrcAlpha;  
        CullMode = None;
        ShadeMode = Gouraud;          
        // shaders
        
        VertexShader = compile vs_2_0 VS(false,false);
	PixelShader = NULL;
    }  
}