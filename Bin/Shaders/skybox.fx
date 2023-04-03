float4x4 World;
float4x4 View;
float4x4 Projection;
float4 AmbientColor = float4(1.0,1.0,1.0,1);
float AmbientIntensity = 1;
texture Texture;

struct MyVertexStruct
{
	float4 position : POSITION0;
	float4 texcoord : TEXCOORD0;
};

MyVertexStruct VertexShaderFunction( MyVertexStruct input )
{
	//create struct variable to return
	MyVertexStruct output = (MyVertexStruct)0;
	
	//combine world + view + projection matrices
	float4x4 viewProj = mul(View,Projection);
	float4x4 WorldViewProj = mul(World,viewProj);
	
	//translate the current vertex
	output.position = mul(input.position, WorldViewProj);
	
	//set texture coordinates
	output.texcoord = input.texcoord;
	return output;
}

sampler TextureSampler = sampler_state
{
	texture = <Texture>;
	minfilter = linear;
	magfilter = linear;
	mipfilter = point;
	addressu = clamp;
	addressv = clamp;
};

float4 PixelShaderFunction(float2 texcoord : TEXCOORD0) : COLOR
{
	return ( tex2D( TextureSampler,texcoord ) * AmbientColor * AmbientIntensity);
}


technique Ambient
{
	pass P0
	{
		vertexShader = compile vs_2_0 VertexShaderFunction();
		pixelShader = compile ps_2_0 PixelShaderFunction();
	
	}
}