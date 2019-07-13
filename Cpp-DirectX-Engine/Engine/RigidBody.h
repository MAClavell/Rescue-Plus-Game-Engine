#pragma once
#include "GameObject.h"
#include "btBulletDynamicsCommon.h"

enum class CollisionShapes { Box, Capsule, Cone, Cylinder, Sphere};

// --------------------------------------------------------
// A rigid body definition.
//
// Allows a gameobject to be acted upon by the physics world
// --------------------------------------------------------
class RigidBody : public Component
{
private:
	btRigidBody* rigidBody;
	btCollisionShape* shape;
	btDefaultMotionState* motionState;
	float mass;

public:
	RigidBody(GameObject* gameObject, btCollisionShape* shape, float mass);
	~RigidBody();

	// --------------------------------------------------------
	// Set the mass of this rigid body (a mass of 0 is not dynamic)
	// --------------------------------------------------------
	void SetMass(float newMass);

	// --------------------------------------------------------
	// Get the mass of this rigid body (a mass of 0 is not dynamic)
	// --------------------------------------------------------
	float GetMass();

	// --------------------------------------------------------
	// Get the actual bullet3 rigid body
	// --------------------------------------------------------
	btRigidBody* GetRigidBody();
};

