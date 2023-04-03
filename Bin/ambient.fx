//Basic ambient light shader with no texture support 
//these lines get the matrices to transform the points 
float4x4 World; 
float4x4 View; 
float4x4 Projection; 
//values for colour (RGBA) and intensity 
float4 AmbientColor : AMBIENT = float4(1.0,1.0,1.0,1.0); 
float AmbientIntensity = 1.0; 

//a struct containing position and colour of each vertex 
struct MyVertexStruct { 
			float4 position : POSITION0; 
			float4 color : COLOR0; 
};

//The vertex shader - takes in each vertex using the struct above 
MyVertexStruct VertexShaderFunction(MyVertexStruct input_param) 
{ 
	//zero a struct to send out once calculated 
	MyVertexStruct output = (MyVertexStruct)0;
 
	//combine world + view + projection matrices 
	float4x4 WorldViewProj = mul(World,mul(View,Projection)); 

	//transform the current vertex 
	output.position = mul(input_param.position, WorldViewProj); 

	//calculate colour 
	output.color.rgb = AmbientColor * AmbientIntensity; 
	output.color.a = AmbientColor.a; 

	//return the transformed vertex 
	return output; 
} 

//pixel shader - gets each vertex colour 
float4 PixelshaderFunction(float4 c:COLOR0) : COLOR 
{ 
	//simply returns the vertex colour 
	return c; 
}

//Ambient shader technique - where the magic happens 
technique Ambient 
{ 
	//single pass 
	Pass P0 
	{ 
	//bind functions to shaders and compile 
	//HLSL shader version 2 
	VertexShader = compile vs_2_0 VertexShaderFunction(); 
	Pixelshader = compile ps_2_0 PixelshaderFunction(); 
	}
}