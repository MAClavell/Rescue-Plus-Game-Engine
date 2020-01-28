#pragma once
#include "ColliderBase.h"
#include "CollisionLayers.h"

class CharacterController : public ColliderBase
{
private:
	physx::PxFilterData filterData;
	physx::PxController* pxController;
	float radius;
	float height;

	// --------------------------------------------------------
	// Set the filter data of the controller
	// --------------------------------------------------------
	virtual void SetFilterData(physx::PxShape* shape) = 0;

public:
	CharacterController(GameObject* gameObject, float radius, float height);
	~CharacterController();

	// --------------------------------------------------------
	// 
	// --------------------------------------------------------
	CharacterControllerCollisionFlags Move(
		DirectX::XMFLOAT3 displacement, float deltaTime, 
		bool applyGravity = true);

	// --------------------------------------------------------
	// Update debug view
	// --------------------------------------------------------
	void Update(float deltaTime) override;

	// --------------------------------------------------------
	// Update collisions
	// --------------------------------------------------------
	void FixedUpdate(float deltaTime) override;

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

