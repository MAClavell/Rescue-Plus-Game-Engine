#pragma once
#include "PhysX/include/PxPhysicsAPI.h"
#include "GameObject.h"
#include "CollisionLayers.h"

class CharacterController : public Component
{
private:
	physx::PxController* pxController;

public:
	CharacterController(GameObject* gameObject);
	~CharacterController();

	// --------------------------------------------------------
	// 
	// --------------------------------------------------------
	CharacterControllerCollisionFlags Move(
		DirectX::XMFLOAT3 displacement, float deltaTime, 
		bool applyGravity = true);

	// --------------------------------------------------------
	// Get the foot position of the character controller
	// (position bottom-most position of the controller)
	// --------------------------------------------------------
	DirectX::XMFLOAT3 GetFootPosition();

	// --------------------------------------------------------
	// Set the max height the controller can step up objects
	// --------------------------------------------------------
	void SetStepOffset(float offset);

	// --------------------------------------------------------
	// Get the max height the controller can step up objects
	// --------------------------------------------------------
	float GetStepOffset();

	// --------------------------------------------------------
	// Resize the character controller.
	// This function maintains ground the controller's position
	//  on the ground. Modifies height and center position.
	// --------------------------------------------------------
	void Resize(float height);

	
};

