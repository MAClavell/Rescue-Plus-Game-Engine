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
	Collider(GameObject* gameObject, ColliderType type, 
		PhysicsMaterial* physicsMaterial, DirectX::XMFLOAT3 center);

	// --------------------------------------------------------
	// Abstract function for generating the collider's physx shape
	// --------------------------------------------------------
	virtual physx::PxShape* GenerateShape(physx::PxPhysics* physics) = 0;
	
	// --------------------------------------------------------
	// Search to see if there is already a rigidbody we can attach to
	// --------------------------------------------------------
	void FindInitialRigidBody(GameObject* gameObject);

	// --------------------------------------------------------
	// Re-calculate the shape and re-attach this collider to a rigidbody
	// --------------------------------------------------------
	void ReAttach();

private:
	ColliderType type;
	RigidBody* attachedRigidBody;
	physx::PxShape* shape;

	// --------------------------------------------------------
	// DeAttach this collider from it's rigidbody
	// --------------------------------------------------------
	void DeAttach();

public:
	// --------------------------------------------------------
	// Clean up the collider instance
	// --------------------------------------------------------
	~Collider();

	// --------------------------------------------------------
	// Get the rigidbody this collider is attached to (null if none)
	// --------------------------------------------------------
	RigidBody* GetAttachedRigidBody();

	// --------------------------------------------------------
	// Attach this collider to a rigidbody
	// --------------------------------------------------------
	void Attach(RigidBody* rigidBody);

	// --------------------------------------------------------
	// Get the center of the Collider
	// --------------------------------------------------------
	DirectX::XMFLOAT3 GetCenter();
	// --------------------------------------------------------
	// Set the center of the Collider
	// --------------------------------------------------------
	void SetCenter(DirectX::XMFLOAT3 center);
	// --------------------------------------------------------
	// Set the center of the Collider
	// --------------------------------------------------------
	void SetCenter(float x, float y, float z);

	// --------------------------------------------------------
	// Get the physics material of the Collider
	// --------------------------------------------------------
	PhysicsMaterial* GetPhysicsMaterial();
	// --------------------------------------------------------
	// Set the physics material of the Collider
	// --------------------------------------------------------
	void SetPhysicsMaterial(PhysicsMaterial* physicsMaterial);

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
	BoxCollider(GameObject* gameObject, DirectX::XMFLOAT3 size = DirectX::XMFLOAT3(1, 1, 1), 
		PhysicsMaterial* physicsMaterial = nullptr, DirectX::XMFLOAT3 center = DirectX::XMFLOAT3(0, 0, 0));

	// --------------------------------------------------------
	// Get the size of the BoxCollider
	// --------------------------------------------------------
	DirectX::XMFLOAT3 GetSize();
	// --------------------------------------------------------
	// Set the size of the BoxCollider
	// --------------------------------------------------------
	void SetSize(DirectX::XMFLOAT3 size);
	// --------------------------------------------------------
	// Set the size of the BoxCollider
	// --------------------------------------------------------
	void SetSize(float x, float y, float z);
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
	SphereCollider(GameObject* gameObject, float radius = 1.0f, 
		PhysicsMaterial* physicsMaterial = nullptr, DirectX::XMFLOAT3 center = DirectX::XMFLOAT3(0, 0, 0));

	// --------------------------------------------------------
	// Get the radius of the SphereCollider
	// --------------------------------------------------------
	float GetRadius();
	// --------------------------------------------------------
	// Set the radius of the SphereCollider
	// --------------------------------------------------------
	void SetRadius(float radius);
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
		CapsuleDirection dir = CapsuleDirection::X, 
		PhysicsMaterial* physicsMaterial = nullptr, DirectX::XMFLOAT3 center = DirectX::XMFLOAT3(0, 0, 0));

	// --------------------------------------------------------
	// Get the radius of the CapsuleCollider
	// --------------------------------------------------------
	float GetRadius();
	// --------------------------------------------------------
	// Set the radius of the CapsuleCollider
	// --------------------------------------------------------
	void SetRadius(float radius);

	// --------------------------------------------------------
	// Get the height of the CapsuleCollider
	// --------------------------------------------------------
	float GetHeight();
	// --------------------------------------------------------
	// Set the height of the CapsuleCollider
	// --------------------------------------------------------
	void SetHeight(float height);

	// --------------------------------------------------------
	// Get the direction of the CapsuleCollider
	// --------------------------------------------------------
	CapsuleDirection GetCapsuleDirection();
	// --------------------------------------------------------
	// Set the direction of the CapsuleCollider
	// --------------------------------------------------------
	void SetCapsuleDirection(CapsuleDirection dir);
};