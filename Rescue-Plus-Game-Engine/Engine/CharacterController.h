#pragma once
#include "PhysX/include/PxPhysicsAPI.h"
#include "GameObject.h"

class CharacterController : public Component
{
private:
	physx::PxController* pxController;

public:
	CharacterController(GameObject* gameObject);

	~CharacterController();

	void Move(DirectX::XMFLOAT3 displacement);

	void SetPosition(DirectX::XMFLOAT3 position);

	DirectX::XMFLOAT3 GetPosition();

	void SetFootPosition(DirectX::XMFLOAT3 footPosition);

	DirectX::XMFLOAT3 GetFootPosition();

	void SetStepOffset(float offset);

	float GetStepOffset();

	void Resize(float height);
};

