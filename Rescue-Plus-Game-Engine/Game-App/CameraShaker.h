#pragma once
#include "GameObject.h"

// --------------------------------------------------------
// Based on https://www.youtube.com/watch?v=tu-Qe66AvtY
// Trauma is 0-1
// Shake amount is intensity * trauma^2 * perlinNoise
// --------------------------------------------------------
class CameraShaker
{
private:
	GameObject* camera;
	float intensity;
	float trauma;

public:
	CameraShaker();
	CameraShaker(GameObject* camera, float intensity = 1);
	~CameraShaker() { };

	// --------------------------------------------------------
	// Update the shaker to apply any shaking
	// --------------------------------------------------------
	void Update(float deltaTime, DirectX::XMFLOAT3 baseRotation);

	// --------------------------------------------------------
	// Add a trauma amount to the shaker (0-1)
	// --------------------------------------------------------
	void AddTrauma(float traumaToAdd);
	// --------------------------------------------------------
	// Set trauma to an amount (0-1)
	// --------------------------------------------------------
	void SetTrauma(float trauma);
	// --------------------------------------------------------
	// Set the overall intensity of the shaker
	// --------------------------------------------------------
	void SetIntensity(float intensity);
};

