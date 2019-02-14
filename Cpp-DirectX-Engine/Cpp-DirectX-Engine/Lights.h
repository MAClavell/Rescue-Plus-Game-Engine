#pragma once
#include <DirectXMath.h>
#include "GameObject.h"

// --------------------------------------------------------
// A directional light struct definition
//
// Directional Light data to be passed to shaders
// --------------------------------------------------------
struct LightStruct
{
	DirectX::XMFLOAT4 AmbientColor;
	DirectX::XMFLOAT4 DiffuseColor;
	DirectX::XMFLOAT3 Position;
	float dummy; //dummy float because of 16-byte rule
	DirectX::XMFLOAT3 Direction;
	float intensity;
};

// --------------------------------------------------------
// A light definition
//
// Base light to be inherited by other classes
// --------------------------------------------------------
class Light : public GameObject
{
protected:
	LightStruct* lightStruct;

public:
	// --------------------------------------------------------
	// Constructor - Set up a light with default values.
	// --------------------------------------------------------
	Light();

	// --------------------------------------------------------
	// Constructor - Set up a light
	//
	// diffuseColor - Diffuse color of the light
	// intensity - how intense the light is
	// --------------------------------------------------------
	Light(DirectX::XMFLOAT4 diffuseColor, float intensity);

	// --------------------------------------------------------
	// Destructor for when an instance is deleted
	// --------------------------------------------------------
	~Light();

	// --------------------------------------------------------
	// Get the light struct to pass to the shader
	// --------------------------------------------------------
	LightStruct* GetLightStruct();

	// --------------------------------------------------------
	// Set the diffuse color of this light
	// --------------------------------------------------------
	void SetDiffuseColor(DirectX::XMFLOAT4 diffuseColor);

	// --------------------------------------------------------
	// Set the diffuse color of this light
	//
	// r - red
	// g - green
	// b - blue
	// a - alpha
	// --------------------------------------------------------
	void SetDiffuseColor(float r, float g, float b, float a);

	// --------------------------------------------------------
	// Get the diffuse color of this light
	// --------------------------------------------------------
	DirectX::XMFLOAT4 GetDiffuseColor();

	// --------------------------------------------------------
	// Get the intensity for this light
	// --------------------------------------------------------
	float GetIntensity();

	// --------------------------------------------------------
	// Set the intensity for this light
	// --------------------------------------------------------
	void SetIntensity(float intensity);
};

// --------------------------------------------------------
// A directional light definition
//
// Directional Lights are applied to all objects
// --------------------------------------------------------
class DirectionalLight : public Light
{
public:
	// --------------------------------------------------------
	// Constructor - Set up a directional light with default values.
	// --------------------------------------------------------
	DirectionalLight();

	// --------------------------------------------------------
	// Constructor - Set up a directional light
	//
	// ambientColor - Ambient color of the light
	// diffuseColor - Diffuse color of the light
	// --------------------------------------------------------
	DirectionalLight(DirectX::XMFLOAT4 ambientColor, 
		DirectX::XMFLOAT4 diffuseColor, float intensity);

	// --------------------------------------------------------
	// Destructor for when an instance is deleted
	// --------------------------------------------------------
	~DirectionalLight();
	
	// --------------------------------------------------------
	// Set the ambient color of this light
	// --------------------------------------------------------
	void SetAmbientColor(DirectX::XMFLOAT4 ambientColor);
	
	// --------------------------------------------------------
	// Set the diffuse color of this light
	//
	// r - red
	// g - green
	// b - blue
	// a - alpha
	// --------------------------------------------------------
	void SetAmbientColor(float r, float g, float b, float a);

	// --------------------------------------------------------
	// Get the ambient color of this light
	// --------------------------------------------------------
	DirectX::XMFLOAT4 GetAmbientColor();

	// --------------------------------------------------------
	// Get the direction of this light
	// --------------------------------------------------------
	DirectX::XMFLOAT3 GetDirection();
};