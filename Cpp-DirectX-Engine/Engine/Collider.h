#pragma once
#include <DirectXMath.h>
#include "GameObject.h"
#include "RigidBody.h"
#include "PhysicsMaterial.h"

enum class ColliderType { Box = 0, Sphere = 1, Capsule = 2 };

class Collider : Component
{
private:
	ColliderType type;
	RigidBody* attachedRigidBody;
	PhysicsMaterial* physicsMaterial;

protected:
	// --------------------------------------------------------
	// Create a collider and try to find a rigidbody
	// --------------------------------------------------------
	Collider(GameObject* gameObject, ColliderType type, PhysicsMaterial* physicsMaterial = nullptr);

public:
	// --------------------------------------------------------
	// Get the rigidbody this collider is attached to (null if none)
	// --------------------------------------------------------
	RigidBody* GetAttachedRigidBody();

	virtual void Attach(RigidBody* rigidBody) = 0;
};

