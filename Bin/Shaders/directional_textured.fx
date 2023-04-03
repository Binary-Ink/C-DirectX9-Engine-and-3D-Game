float4x4 World;
float4x4 View;
float4x4 Projection;

float4x4 WorldInverseTranspose;
texture Texture;

float3 LightVector = float3(0,0,1);
float4 LightColor = float4(1,1,1,1);
float LightPower = 0.5;

struct MyVertexInput{
	float4 position : POSITION;
	float2 texcoord : TEXCOORD0;
	float4 normal : NORMAL;
};

struct MyVertexOutput{
	float4 position : POSITION;
	float2 texcoord : TEXCOORD0;
	float4 color : COLOR0;
	float fogFactor : TEXCOORD1;
};

MyVertexOutput VertexShaderFunction(MyVertexInput input){
	MyVertexOutput output = (MyVertexOutput)0;
	float4x4 viewProj = mul(View, Projection);
	float4x4 WorldViewProj = mul(World, viewProj);
	output.position = mul(input.position, WorldViewProj);

	float4 normal = mul(input.normal, WorldInverseTranspose);
	float4 intensity = dot(normal, LightVector);
	output.color = saturate(LightColor * LightPower * intensity);

	output.texcoord = input.texcoord;
	
	float fogStart = 1;
	float fogEnd = 400;
	float4 cameraPosition;

	cameraPosition = mul(input.position, World);
	cameraPosition = mul(cameraPosition, View);
	output.fogFactor = saturate((fogEnd - cameraPosition.z) / (fogEnd - fogStart));

	return output;
}

sampler MySampler = sampler_state{
	texture = <Texture>;
};

float4 PixelShaderFunction(MyVertexOutput input):COLOR0{
	float4 light = saturate (input.color + LightColor * LightPower * input.fogFactor);
	return (tex2D(MySampler, input.texcoord)* light);
}

technique DirectionalTextured{
	pass P0{
		vertexShader = compile vs_2_0 VertexShaderFunction();
		pixelShader = compile ps_2_0 PixelShaderFunction();
	}
}