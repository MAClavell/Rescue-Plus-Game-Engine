#pragma once
#include <PxPhysicsAPI.h>
#include "GameObject.h"
#include "CollisionResolver.h"

// --------------------------------------------------------
// A rigid body definition.
//
// Allows a gameobject to be acted upon by the physics world
// --------------------------------------------------------
class RigidBody : public Component
{
private:
	physx::PxRigidDynamic* body;
	CollisionResolver* collisionResolver;

	// --------------------------------------------------------
	// Attach all children colliders to this GO
	// --------------------------------------------------------
	void FindChildrenColliders(GameObject*, bool first = true);

	// --------------------------------------------------------
	// Update collisions
	// --------------------------------------------------------
	void FixedUpdate(float deltaTime) override;

public:
	RigidBody(GameObject* gameObject, float mass);

	~RigidBody();

	// --------------------------------------------------------
	// Update the gameobject's world position from an inputted transform
	// --------------------------------------------------------
	void UpdateWorldPosition();

	// --------------------------------------------------------
	// Update the gameobject's rigidbody from it's world position
	// --------------------------------------------------------
	void UpdateRigidbodyPosition();

	// --------------------------------------------------------
	// Set the mass of this rigid body
	// --------------------------------------------------------
	void SetMass(float mass);

	// --------------------------------------------------------
	// If an object is kinematic, no forces will act on it
	// --------------------------------------------------------
	void SetKinematic(bool kinematic);

	// --------------------------------------------------------
	// Get the mass of this rigid body (a mass of 0 is not dynamic)
	// --------------------------------------------------------
	float GetMass();

	// --------------------------------------------------------
	// Get the actual physx rigid body
	// --------------------------------------------------------
	physx::PxRigidDynamic* GetRigidBody();

	// --------------------------------------------------------
	// Add a force to this rigidbody
	//
	// force - the force vector to add
	// mode - the way to apply this force
	// --------------------------------------------------------
	void AddForce(DirectX::XMFLOAT3 force, physx::PxForceMode::Enum mode = physx::PxForceMode::eFORCE);

	// --------------------------------------------------------
	// Add a force to this rigidbody
	//
	// x - x component of the force
	// y - y component of the force
	// z - z component of the force
	// mode - the way to apply this force
	// --------------------------------------------------------
	void AddForce(float x, float y, float z, physx::PxForceMode::Enum mode = physx::PxForceMode::eFORCE);

	// --------------------------------------------------------
	// Get the current linear velocity for this rigidbody
	// --------------------------------------------------------
	DirectX::XMFLOAT3 GetLinearVelocity();

	// --------------------------------------------------------
	// Set the current linear velocity for this rigidbody
	// --------------------------------------------------------
	void SetLinearVelocity(DirectX::XMFLOAT3 velocity);

	// --------------------------------------------------------
	// Set the current linear velocity for this rigidbody
	// --------------------------------------------------------
	void SetLinearVelocity(float x, float y, float z);

	// --------------------------------------------------------
	// Get the maximum linear velocity for this rigidbody
	// --------------------------------------------------------
	float GetMaxLinearVelocity();

	// --------------------------------------------------------
	// Set the maximum linear velocity for this rigidbody
	// --------------------------------------------------------
	void SetMaxLinearVelocity(float max);

	// --------------------------------------------------------
	// WARNING: THIS IS FOR INTERNAL ENGINE USE ONLY. DO NOT USE
	// Get the collision resolver for this rigidbody.
	// --------------------------------------------------------
	CollisionResolver* GetCollisionResolver();
};

