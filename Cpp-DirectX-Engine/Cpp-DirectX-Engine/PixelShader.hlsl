
// Struct representing the data for any light
struct Light
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 position;
	float3 direction;
	float intensity;
};

//External data for the pixel shader
cbuffer externalData : register(b0)
{
	Light light1;
	Light light2;
	float4 surfaceColor;
	float3 cameraPosition;
	float specularity;
};

// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL;        // XYZ normal
	float3 worldPos		: POSITION;		 // world position of the vertex
	float2 uv			: TEXCOORD;		 // XY uv
};

// --------------------------------------------------------
// Calculate directional light and apply the color
// --------------------------------------------------------
float4 dLightCalc(VertexToPixel input, Light light, float3 toCamera)
{
	//Calc direction to the light
	float3 dirToLight = normalize(-light.direction);

	//Get NdotL "brightness" factor
	float NdotL = saturate(dot(dirToLight, input.normal));

	//Calculate specularness ("shinyness")
	float dirSpec = pow(max(dot(reflect(-dirToLight, input.normal), toCamera), 0), specularity);

	//Add all colors
	return (light.ambientColor + 
		(light.diffuseColor * NdotL * surfaceColor) +
		float4(dirSpec, dirSpec, dirSpec, 0)) * light.intensity;
}

// --------------------------------------------------------
// Calculate point light and apply the color
// --------------------------------------------------------
float4 pLightCalc(VertexToPixel input, Light light, float3 toCamera)
{
	//Calc direction to the light
	float3 dirToLight = normalize(light.position - input.worldPos);

	//Get NdotL "brightness" factor
	float NdotL = saturate(dot(dirToLight, input.normal));

	//Calculate specularness ("shinyness")
	float dirSpec = pow(max(dot(reflect(-dirToLight, input.normal), toCamera), 0), specularity);

	//Add all colors
	return ((light.diffuseColor * NdotL * surfaceColor) +
		float4(dirSpec, dirSpec, dirSpec, 0)) * light.intensity;
}

// --------------------------------------------------------
// Calculate spot light and apply the color
// --------------------------------------------------------
float4 sLightCalc(VertexToPixel input, Light light, float3 toCamera)
{
	//Calc direction to the light
	float3 dirToLight = normalize(light.position - input.worldPos);

	//Get NdotL "brightness" factor
	float NdotL = saturate(dot(dirToLight, input.normal));

	//Calculate specularness ("shinyness")
	float dirSpec = pow(max(dot(reflect(-dirToLight, input.normal), toCamera), 0), specularity);

	//Calculate spotlight amount
	float spotAmnt = pow(max(dot(-dirToLight, light.direction), 0.0f), light.intensity);

	//Add all colors
	return ((light.diffuseColor * NdotL * surfaceColor) +
		float4(dirSpec, dirSpec, dirSpec, 0)) * spotAmnt;
}

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	//Renormalize normals
	input.normal = normalize(input.normal);

	//Calculate direction to camera
	float3 toCamera = normalize(cameraPosition - input.worldPos);

	//Calculate the directional light effect
	float4 finalColor = dLightCalc(input, light1, toCamera) +
		dLightCalc(input, light2, toCamera);

	//Return final pixel color after lighting
	return finalColor;
}