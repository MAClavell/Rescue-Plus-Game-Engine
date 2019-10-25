#include "Collider.h"
#include "PhysicsManager.h"
#include "PhysicsHelper.h"

#define DEFAULT_PHYSICS_MAT (physics->createMaterial(0.6f, 0.6f, 0))

using namespace DirectX;
using namespace physx;

#pragma region Base Collider
// ----------------------------------------------------------------------------
//									BASE COLLIDER
// ----------------------------------------------------------------------------

// Create a collider and try to find a rigidbody
Collider::Collider(GameObject* gameObject, ColliderType type,
	PhysicsMaterial* physicsMaterial, XMFLOAT3 center)
	: Component(gameObject)
{
	this->type = type;
	this->physicsMaterial = physicsMaterial;
	this->center = center;
}

Collider::~Collider()
{
	if (shape != nullptr)
		shape->release();
}

// Get the rigidbody this collider is attached to (null if none)
RigidBody* Collider::GetAttachedRigidBody()
{
	return attachedRigidBody;
}

// Search to see if there is already a rigidbody we can attach to
void Collider::FindInitialRigidBody(GameObject* gameObject)
{
	//Try to find a rigidbody to attach to
	RigidBody* rigidBody = gameObject->GetComponent<RigidBody>();
	if (rigidBody != nullptr)
		Attach(rigidBody);
	else attachedRigidBody = nullptr;
}

// DeAttach this collider to a rigidbody
void Collider::DeAttach()
{
	if (attachedRigidBody != nullptr)
	{
		attachedRigidBody->GetRigidBody()->detachShape(*shape);
		attachedRigidBody = nullptr;
	}
	if (shape != nullptr)
		shape->release();
}

// Attach this collider to a rigidbody
void Collider::Attach(RigidBody* rigidBody)
{
	PxPhysics* physics = PhysicsManager::GetInstance()->GetPhysics();

	//Create shape
	shape = GenerateShape(physics);

	//Attach
	rigidBody->GetRigidBody()->attachShape(*shape);
	attachedRigidBody = rigidBody;

	if(!(rigidBody->GetRigidBody()->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC))
		rigidBody->GetRigidBody()->wakeUp();
}

// Re-calculate the shape and re-attach this collider to a rigidbody
void Collider::ReAttach()
{
	RigidBody* rb = attachedRigidBody;
	DeAttach();
	Attach(rb);
}

// Get the center of the Collider
DirectX::XMFLOAT3 Collider::GetCenter()
{
	return center;
}
// Set the center of the Collider
void Collider::SetCenter(DirectX::XMFLOAT3 center)
{
	this->center = center;
	ReAttach();
}
// Set the center of the Collider
void Collider::SetCenter(float x, float y, float z)
{
	this->center = XMFLOAT3(x, y, z);
	ReAttach();
}

// Get the physics material of the Collider
PhysicsMaterial * Collider::GetPhysicsMaterial()
{
	return physicsMaterial;
}
// Set the physics material of the Collider
void Collider::SetPhysicsMaterial(PhysicsMaterial* physicsMaterial)
{
	this->physicsMaterial = physicsMaterial;
	ReAttach();
}


#pragma endregion

#pragma region Box Collider
// ----------------------------------------------------------------------------
//									BOX COLLIDER
// ----------------------------------------------------------------------------

BoxCollider::BoxCollider(GameObject* gameObject, DirectX::XMFLOAT3 size, 
	PhysicsMaterial* physicsMaterial, XMFLOAT3 center)
	: Collider(gameObject, ColliderType::Box, physicsMaterial, center)
{
	this->size = size;
	FindInitialRigidBody(gameObject); //has to be in derived constructor because of GenerateShape pure virtual
}

// Get the size of the BoxCollider
DirectX::XMFLOAT3 BoxCollider::GetSize()
{
	return size;
}
// Set the size of the BoxCollider
void BoxCollider::SetSize(DirectX::XMFLOAT3 size)
{
	this->size = size;
	ReAttach();
}
// Set the size of the BoxCollider
void BoxCollider::SetSize(float x, float y, float z)
{
	this->size = XMFLOAT3(x, y, z);
	ReAttach();
}

// Implementation of the abstract function for generating the collider's physx shape
physx::PxShape* BoxCollider::GenerateShape(PxPhysics* physics)
{
	XMFLOAT3 half;
	XMStoreFloat3(&half, XMVectorScale(XMLoadFloat3(&size), 1/2.0f));
	PxBoxGeometry box = PxBoxGeometry(Float3ToVec3(half));
	
	PxMaterial* mat;
	if (physicsMaterial == nullptr)
		mat = DEFAULT_PHYSICS_MAT;
	else mat = physicsMaterial->GetPxMaterial();

	PxShape* shape = physics->createShape(box, *mat, true);
	shape->setLocalPose(PxTransform(Float3ToVec3(center)));
	return shape;
}
#pragma endregion

#pragma region Sphere Collider
// ----------------------------------------------------------------------------
//									SPHERE COLLIDER
// ----------------------------------------------------------------------------

SphereCollider::SphereCollider(GameObject* gameObject, float radius, 
	PhysicsMaterial* physicsMaterial, XMFLOAT3 center)
	: Collider(gameObject, ColliderType::Sphere, physicsMaterial, center)
{
	this->radius = radius;
	FindInitialRigidBody(gameObject);
}

// Implementation of the abstract function for generating the collider's physx shape
physx::PxShape* SphereCollider::GenerateShape(physx::PxPhysics * physics)
{
	PxSphereGeometry sphere = PxSphereGeometry(radius);

	PxMaterial* mat;
	if (physicsMaterial == nullptr)
		mat = DEFAULT_PHYSICS_MAT;
	else mat = physicsMaterial->GetPxMaterial();

	PxShape* shape = physics->createShape(sphere, *mat, true);
	shape->setLocalPose(PxTransform(Float3ToVec3(center)));
	return shape;
}

// Get the radius of the SphereCollider
float SphereCollider::GetRadius()
{
	return radius;
}
// Set the radius of the SphereCollider
void SphereCollider::SetRadius(float radius)
{
	this->radius = radius;
	ReAttach();
}
#pragma endregion

#pragma region Capsule Collider
// ----------------------------------------------------------------------------
//									CAPSULE COLLIDER
// ----------------------------------------------------------------------------

CapsuleCollider::CapsuleCollider(GameObject* gameObject, float radius, float height, 
	CapsuleDirection dir, 
	PhysicsMaterial* physicsMaterial, XMFLOAT3 center)
	: Collider(gameObject, ColliderType::Capsule, physicsMaterial, center)
{
	this->radius = radius;
	this->height = height;
	this->dir = dir;
	FindInitialRigidBody(gameObject);
}

// Implementation of the abstract function for generating the collider's physx shape
physx::PxShape* CapsuleCollider::GenerateShape(physx::PxPhysics * physics)
{
	PxCapsuleGeometry capsule = PxCapsuleGeometry(radius, height / 2);

	PxMaterial* mat;
	if (physicsMaterial == nullptr)
		mat = DEFAULT_PHYSICS_MAT;
	else mat = physicsMaterial->GetPxMaterial();

	//Create shape
	PxShape* shape = physics->createShape(capsule, *mat, true);

	//Rotate capsule
	PxTransform transform = shape->getLocalPose();
	switch (dir)
	{
		case CapsuleDirection::Y:
			shape->setLocalPose(PxTransform(
				Float3ToVec3(center), PxQuat(PxHalfPi, PxVec3(0, 0, 1))));
			break;

		case CapsuleDirection::Z:
			shape->setLocalPose(PxTransform(
				Float3ToVec3(center), PxQuat(PxHalfPi, PxVec3(0, 1, 0))));
			break;

		default:
			break;
	}
	
	return shape;
}

// Get the radius of the CapsuleCollider
float CapsuleCollider::GetRadius()
{
	return radius;
}
// Set the radius of the CapsuleCollider
void CapsuleCollider::SetRadius(float radius)
{
	this->radius = radius;
	ReAttach();
}

// Get the height of the CapsuleCollider
float CapsuleCollider::GetHeight()
{
	return height;
}
// Set the height of the CapsuleCollider
void CapsuleCollider::SetHeight(float height)
{
	this->height = height;
	ReAttach();
}

// Get the direction of the CapsuleCollider
CapsuleDirection CapsuleCollider::GetCapsuleDirection()
{
	return dir;
}
// Set the direction of the CapsuleCollider
void CapsuleCollider::SetCapsuleDirection(CapsuleDirection dir)
{
	this->dir = dir;
	ReAttach();
}
#pragma endregion