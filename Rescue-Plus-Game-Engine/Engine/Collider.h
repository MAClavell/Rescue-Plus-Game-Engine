#pragma once
#include "PhysicsMaterial.h"
#include "ColliderBase.h"
#include "RigidBody.h"


// --------------------------------------------------------
// The physics collider class
//
// Allows collision between gameobjects with rigidbodies
// --------------------------------------------------------
class Collider : public ColliderBase
{
private:
	
	// --------------------------------------------------------
	// Set the collider shape's filters
	// --------------------------------------------------------
	void SetFilterData(physx::PxShape* shape);

protected:
	PhysicsMaterial* physicsMaterial;
	RigidBody* attachedRigidBody;
	physx::PxRigidStatic* staticActor;
	DirectX::XMFLOAT3 center;
	bool isTrigger;
	bool isInChildObj;

	// --------------------------------------------------------
	// Create a collider and try to find a rigidbody
	// --------------------------------------------------------
	Collider(GameObject* gameObject, ColliderType type, bool isTrigger,
		PhysicsMaterial* physicsMaterial, DirectX::XMFLOAT3 center);

	// --------------------------------------------------------
	// The attached GameObject's position changed
	// --------------------------------------------------------
	void OnPositionChanged(DirectX::XMFLOAT3 position, bool fromParent,
		bool fromRigidBody) override;

	// --------------------------------------------------------
	// The attached GameObject's rotation changed
	// --------------------------------------------------------
	void OnRotationChanged(DirectX::XMFLOAT4 rotation, bool fromParent,
		bool fromRigidBody) override;

	// --------------------------------------------------------
	// The attached GameObject's scale changed
	// --------------------------------------------------------
	void OnScaleChanged(DirectX::XMFLOAT3 scale) override;

	// --------------------------------------------------------
	// Abstract function for generating the collider's physx shape
	// --------------------------------------------------------
	virtual physx::PxShape* GenerateShape(physx::PxPhysics* physics) = 0;

	// --------------------------------------------------------
	// Search to see if there is already a rigidbody we can attach to
	// It'll attach to the first rigidbody it finds in the parent chain
	// --------------------------------------------------------
	void FindParentRigidBody();

	// --------------------------------------------------------
	// DeAttach this collider from it's rigidbody
	// --------------------------------------------------------
	void DeAttachFromStatic();

	// --------------------------------------------------------
	// Re-calculate the shape and re-attach this collider to a rigidbody
	// --------------------------------------------------------
	void ReAttach();

	// --------------------------------------------------------
	// Attach this collider to a static shape
	// --------------------------------------------------------
	void AttachToStatic();

	// --------------------------------------------------------
	// Get this shape's transform based on its position from the parent
	// --------------------------------------------------------
	physx::PxTransform GetChildTransform();

public:
	// --------------------------------------------------------
	// Clean up the collider instance
	// --------------------------------------------------------
	~Collider();

	// --------------------------------------------------------
	// Update collisions
	// --------------------------------------------------------
	void FixedUpdate(float deltaTime) override;

	// --------------------------------------------------------
	// Get the rigidbody this collider is attached to (null if none)
	// --------------------------------------------------------
	RigidBody* GetAttachedRigidBody();

	// --------------------------------------------------------
	// Attach this collider to a rigidbody
	// --------------------------------------------------------
	void AttachToRB(RigidBody* rigidBody, bool isChildObj);

	// --------------------------------------------------------
	// DeAttach this collider from it's rigidbody and make it a static collider
	// --------------------------------------------------------
	void DeAttachFromRB(bool makeStatic = true);

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

	// --------------------------------------------------------
	// Get if this collider is a trigger shape
	// --------------------------------------------------------
	bool GetTrigger();
	// --------------------------------------------------------
	// Set if this collider is a trigger shape
	// --------------------------------------------------------
	void SetTrigger(bool isTrigger);
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
	BoxCollider(GameObject* gameObject, DirectX::XMFLOAT3 size = DirectX::XMFLOAT3(1, 1, 1), bool isTrigger = false,
		PhysicsMaterial* physicsMaterial = nullptr, DirectX::XMFLOAT3 center = DirectX::XMFLOAT3(0, 0, 0));

	// --------------------------------------------------------
	// Update debug view
	// --------------------------------------------------------
	void Update(float deltaTime) override;

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
	SphereCollider(GameObject* gameObject, float radius = 1.0f, bool isTrigger = false,
		PhysicsMaterial* physicsMaterial = nullptr, DirectX::XMFLOAT3 center = DirectX::XMFLOAT3(0, 0, 0));

	// --------------------------------------------------------
	// Update debug view
	// --------------------------------------------------------
	void Update(float deltaTime) override;

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
	DirectX::XMFLOAT4 rot;

	physx::PxShape* GenerateShape(physx::PxPhysics* physics);

public:
	CapsuleCollider(GameObject* gameObject, float radius = 1.0f, float height = 2.0f,
		CapsuleDirection dir = CapsuleDirection::X, bool isTrigger = false,
		PhysicsMaterial* physicsMaterial = nullptr, DirectX::XMFLOAT3 center = DirectX::XMFLOAT3(0, 0, 0));

	// --------------------------------------------------------
	// Update debug view
	// --------------------------------------------------------
	void Update(float deltaTime) override;

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