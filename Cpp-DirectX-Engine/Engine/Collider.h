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
	DirectX::XMFLOAT3 center;

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
// Adds a box shaped collider to the world
// --------------------------------------------------------
class BoxCollider : public Collider
{
private:
	DirectX::XMFLOAT3 size;

	physx::PxShape* GenerateShape(physx::PxPhysics* physics);

public:
	BoxCollider(GameObject* gameObject, DirectX::XMFLOAT3 size = DirectX::XMFLOAT3(1, 1, 1), PhysicsMaterial* physicsMaterial = nullptr);

};

// --------------------------------------------------------
// The sphere collider class
//
// Adds a sphere shaped collider to the world
// --------------------------------------------------------
class SphereCollider : public Collider
{
private:
	float radius;

	physx::PxShape* GenerateShape(physx::PxPhysics* physics);

public:
	SphereCollider(GameObject* gameObject, float radius = 1.0f, PhysicsMaterial* physicsMaterial = nullptr);

};


enum class CapsuleDirection { X = 0, Y = 1, Z = 2 };
// --------------------------------------------------------
// The capsule collider class
//
// Adds a capsule shaped collider to the world
// --------------------------------------------------------
class CapsuleCollider : public Collider
{
private:
	float radius;
	float height;
	CapsuleDirection dir;

	physx::PxShape* GenerateShape(physx::PxPhysics* physics);

public:
	CapsuleCollider(GameObject* gameObject, float radius = 1.0f, float height = 2.0f,
		CapsuleDirection dir = CapsuleDirection::X, PhysicsMaterial* physicsMaterial = nullptr);

};