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
	// Update the gameobject after the sim ticks
	// --------------------------------------------------------
	bool UpdatePhysicsPosition();

	// --------------------------------------------------------
	// Set the mass of this rigid body
	// --------------------------------------------------------
	void SetMass(float mass);

	// --------------------------------------------------------
	// Get the mass of this rigid body (a mass of 0 is not dynamic)
	// --------------------------------------------------------
	float GetMass();

	// --------------------------------------------------------
	// Get the actual physx rigid body
	// --------------------------------------------------------
	physx::PxRigidDynamic* GetRigidBody();
};

