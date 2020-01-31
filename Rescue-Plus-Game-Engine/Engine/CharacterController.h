#pragma once
#include "ColliderBase.h"
#include "CollisionLayers.h"

class CharacterController : public ColliderBase
{
private:
	physx::PxControllerFilters filters;
	physx::PxFilterData filterData;
	physx::PxController* pxController;
	float radius;
	float height;
	bool isGrounded;

	// --------------------------------------------------------
	// Set the filter data of the controller
	// --------------------------------------------------------
	void SetFilterData(physx::PxShape* shape);

	// --------------------------------------------------------
	// The attached GameObject's position changed
	// --------------------------------------------------------
	void OnPositionChanged(DirectX::XMFLOAT3 position, bool fromParent,
		bool fromRigidBody) override;

	// --------------------------------------------------------
	// The attached GameObject's rotation changed
	// --------------------------------------------------------
	void OnRotationChanged(DirectX::XMFLOAT4 rotation, bool fromParent,
		bool fromRigidBody) override {};

	// --------------------------------------------------------
	// The attached GameObject's scale changed
	// --------------------------------------------------------
	void OnScaleChanged(DirectX::XMFLOAT3 scale) override {};

public:
	CharacterController(GameObject* gameObject, float radius, float height);
	~CharacterController();

	// --------------------------------------------------------
	// Move the character by a displacement (in world coordinates)
	// --------------------------------------------------------
	CharacterControllerCollisionFlags Move(
		DirectX::XMFLOAT3 displacement, float deltaTime, 
		bool applyGravity = true, float gravityScale = 1);

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
	// Get if this controller is touching the ground
	// --------------------------------------------------------
	bool IsGrounded();

	// --------------------------------------------------------
	// Get the radius of this character controller
	// --------------------------------------------------------
	float GetRadius();

	// --------------------------------------------------------
	// Get the height of this character controller
	// --------------------------------------------------------
	float GetHeight();

	// --------------------------------------------------------
	// Resize the character controller.
	// This function maintains ground the controller's position
	//  on the ground. Modifies height and center position.
	// --------------------------------------------------------
	void Resize(float height);
};

