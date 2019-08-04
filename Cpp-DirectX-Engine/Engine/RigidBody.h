#pragma once
#include "GameObject.h"
#include <PxPhysicsAPI.h>

enum class CollisionShapes { Box, Capsule, Cone, Cylinder, Sphere};

// --------------------------------------------------------
// A rigid body definition.
//
// Allows a gameobject to be acted upon by the physics world
// --------------------------------------------------------
class RigidBody : public Component
{
private:
	physx::PxRigidDynamic* body;

public:
	RigidBody(GameObject* gameObject, physx::PxBoxGeometry geometry, float mass);
	~RigidBody();

	// --------------------------------------------------------
	// Update the gameobject's world position from it's rigidbody
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
};

