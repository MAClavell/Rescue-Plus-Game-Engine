#include "Lights.h"

using namespace DirectX;

#pragma region Base Light
// ----------------------------------------------------------------------------
//									BASE LIGHT
// ----------------------------------------------------------------------------

// Constructor - Set up a light with default values.
Light::Light()
{
	lightStruct = new LightStruct();
	lightStruct->DiffuseColor = XMFLOAT4(0.5f, 0.5f, 0.5f, 1);
	lightStruct->AmbientColor = XMFLOAT4(0.1f, 0.1f, 0.1f, 1);
	SetRotation(0, -90, 0);
	lightStruct->intensity = 1;
}

// Constructor - Set up a light
Light::Light(DirectX::XMFLOAT4 diffuseColor, float intensity)
{
	lightStruct->DiffuseColor = diffuseColor;
	lightStruct->intensity = intensity;
}

// Destructor for when an instance is deleted
Light::~Light()
{
	if (lightStruct)
		delete lightStruct;
}

// Get the light struct to pass to the shader
LightStruct* Light::GetLightStruct()
{
	//Set direction
	lightStruct->Direction = GetForwardAxis();

	//Set position
	lightStruct->Position = GetPosition();

	return lightStruct;
}

// Set the diffuse color of this light
void Light::SetDiffuseColor(XMFLOAT4 diffuseColor)
{
	lightStruct->DiffuseColor = diffuseColor;
}

// Set the diffuse color of this light
void Light::SetDiffuseColor(float r, float g, float b, float a)
{
	lightStruct->DiffuseColor = XMFLOAT4(r, g, b, a);
}

// Get the diffuse color of this light
XMFLOAT4 Light::GetDiffuseColor()
{
	return lightStruct->DiffuseColor;
}

// Get the intensity for this light
float Light::GetIntensity()
{
	return lightStruct->intensity;
}

// Set the intensity for this light
void Light::SetIntensity(float intensity)
{
	lightStruct->intensity = intensity;
}
#pragma endregion


#pragma region Directional Light
// ----------------------------------------------------------------------------
//							DIRECTIONAL LIGHT
// ----------------------------------------------------------------------------

// Constructor - Set up a directional light with default values.
// White ambient and diffuse color.
DirectionalLight::DirectionalLight()
{ }

// Constructor - Set up a directional light
DirectionalLight::DirectionalLight(XMFLOAT4 ambientColor,
	XMFLOAT4 diffuseColor, float intensity)
{
	Light::Light(diffuseColor, intensity);

	lightStruct->AmbientColor = ambientColor;
	SetRotation(0, -90, 0);
}

// Destructor for when an instance is deleted
DirectionalLight::~DirectionalLight()
{ }

// Set the ambient color of this light
void DirectionalLight::SetAmbientColor(XMFLOAT4 ambientColor)
{
	lightStruct->AmbientColor = ambientColor;
}

// Set the ambient color of this light
void DirectionalLight::SetAmbientColor(float r, float g, float b, float a)
{
	lightStruct->AmbientColor = XMFLOAT4(r, g, b, a);
}

// Get the ambient color of this light
XMFLOAT4 DirectionalLight::GetAmbientColor()
{
	return lightStruct->AmbientColor;
}

// Get the direction of this light
XMFLOAT3 DirectionalLight::GetDirection()
{
	XMFLOAT3 dir;
	XMStoreFloat3(&dir, XMVector3Normalize(
		XMVector3Rotate(XMVectorSet(0, 0, 1, 0), XMLoadFloat4(&GetQuatRotation()))));
	return dir;
}
#pragma endregion