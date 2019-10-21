#pragma once
#include "PhysicsMaterial.h"
#include "RigidBody.h"

enum class ColliderType { Box = 0, Sphere = 1, Capsule = 2 };

// --------------------------------------------------------
// The base collider class
//
// Allows collision between gameobjects with rigidbodies
// --------------------------------------------------------
class Collider : public Component
{
protected:
	PhysicsMaterial* physicsMaterial;

	// --------------------------------------------------------
	// Create a collider and try to find a rigidbody
	// --------------------------------------------------------
	Collider(GameObject* gameObject, ColliderType type, PhysicsMaterial* physicsMaterial = nullptr);

	// --------------------------------------------------------
	// Abstract function for generating the collider's physx shape
	// --------------------------------------------------------
	virtual physx::PxShape* GenerateShape(physx::PxPhysics* physics) = 0;
	
	// --------------------------------------------------------
	// Search to see if there is already a rigidbody we can attach to
	// --------------------------------------------------------
	void FindInitialRigidBody(GameObject* gameObject);

private:
	ColliderType type;
	RigidBody* attachedRigidBody;

public:
	// --------------------------------------------------------
	// Get the rigidbody this collider is attached to (null if none)
	// --------------------------------------------------------
	RigidBody* GetAttachedRigidBody();

	// --------------------------------------------------------
	// Attach this collider to a rigidbody
	// --------------------------------------------------------
	void Attach(RigidBody* rigidBody);
};

// --------------------------------------------------------
// The box collider class
//
// Adds a box shaped collider around the rigidbody
// --------------------------------------------------------
class BoxCollider : public Collider
{
private:
	DirectX::XMFLOAT3 size;

	physx::PxShape* GenerateShape(physx::PxPhysics* physics);

public:
	BoxCollider(GameObject* gameObject, DirectX::XMFLOAT3 size, PhysicsMaterial* physicsMaterial = nullptr);

};