// Sprite shader file

// cosntant buffer for matrices
cbuffer cbPerObject
{
	float4x4 gFinalMatrix; 
	float4x4 gWorldMatrix;
	float4x4 gTextureMatrix;
};

// Nonnumeric values cannot be added to a cbuffer.
Texture2D gDiffuseMap;

BlendState AlphaBlendingOn
{	
	BlendEnable[0] = true;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = ADD;
    SrcBlendAlpha = ZERO;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

SamplerState gTriLinearSam
{
	Filter = ANISOTROPIC;
	AddressU = Clamp;
	AddressV = Wrap;
};

struct VertexInputType
{
	float2 fPosLocal    : POSITION;
	float2 fTexCoord    : TEXCOORD;
};

struct PixelInputType
{
	float4 fPosHom      : SV_POSITION;
    float3 fPosWorld    : POSITION;
    float2 fTexCoord    : TEXCOORD;
};
 
PixelInputType VertexShaderFunction(VertexInputType input)
{
	PixelInputType output;
	
	float3 fPos = float3(input.fPosLocal, 0.0f);

	// Transform to world space space.
	output.fPosWorld = mul(fPos, gWorldMatrix);
		
	// Transform to homogeneous clip space
	output.fPosHom = mul(float4(fPos, 1.0f), gFinalMatrix);

	// Texture coords
	output.fTexCoord  = mul(float4(input.fTexCoord, 0.0f, 1.0f), gTextureMatrix);

	return output;
}

float4 PixelShaderFunction(PixelInputType input) : SV_Target
{
	// Get materials from texture maps.
	float4 fDiffuse = gDiffuseMap.Sample(gTriLinearSam, input.fTexCoord);

	clip(fDiffuse.a - 0.95f);

	// return final colour
	return fDiffuse;
}

technique10 TextureTechnique
{
    pass P0
    {
		SetBlendState(AlphaBlendingOn, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader(CompileShader( vs_4_0, VertexShaderFunction()));
        SetGeometryShader( NULL );
        SetPixelShader(CompileShader( ps_4_0, PixelShaderFunction()));
    }
}
