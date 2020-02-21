#include "CameraShaker.h"
#include <algorithm>
#include "PerlinNoise.h"

using namespace DirectX;

CameraShaker::CameraShaker()
{
	camera = nullptr;
	intensity = 1;
	trauma = 0;
}

CameraShaker::CameraShaker(GameObject* camera, float intensity)
{
	this->camera = camera;
	this->intensity = intensity;
	trauma = 0;
}

void CameraShaker::Update(float deltaTime, XMFLOAT3 baseRotation)
{
	if (trauma <= 0)
		return;

	//Shake the camera with perlin noise
	static double time = 0;
	float power = intensity * pow(trauma, 2);
	baseRotation.x += power * (float)PerlinNoise::Noise(std::fmod(time, 1), std::fmod(time, 1), 0);
	baseRotation.y += power * (float)PerlinNoise::Noise(std::fmod(time + 0.25f, 1) + 1, std::fmod(time + 0.25f, 1), 0);
	baseRotation.z += power * (float)PerlinNoise::Noise(std::fmod(time + 0.75f, 1), std::fmod(time + 0.75f, 1), 0);
	camera->SetRotation(baseRotation);

	//Reduce trauma by deltaTime
	trauma -= deltaTime * 2;

	//Clamp is always between 0 and 1
	trauma = std::clamp(trauma, 0.0f, 1.0f);

	time += (double)deltaTime * 2;
}

void CameraShaker::AddTrauma(float traumaToAdd)
{
	this->trauma += traumaToAdd;
	this->trauma = std::clamp(trauma, 0.0f, 1.0f);
}

void CameraShaker::SetTrauma(float trauma)
{
	this->trauma = trauma;
	this->trauma = std::clamp(trauma, 0.0f, 1.0f);
}

void CameraShaker::SetIntensity(float intensity)
{
	this->intensity = intensity;
}