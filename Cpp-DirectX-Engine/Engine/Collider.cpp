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
Collider::Collider(GameObject* gameObject, ColliderType type, PhysicsMaterial* physicsMaterial) : Component(gameObject)
{
	this->type = type;
	this->physicsMaterial = physicsMaterial;
	center = XMFLOAT3(0, 0, 0);
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

// Attach this collider to a rigidbody
void Collider::Attach(RigidBody* rigidBody)
{
	PxPhysics* physics = PhysicsManager::GetInstance()->GetPhysics();

	//Create shape
	PxShape* shape = GenerateShape(physics);

	//Attach
	rigidBody->GetRigidBody()->attachShape(*shape);
	attachedRigidBody = rigidBody;
	shape->release();
}
#pragma endregion

#pragma region Box Collider
// ----------------------------------------------------------------------------
//									BOX COLLIDER
// ----------------------------------------------------------------------------

BoxCollider::BoxCollider(GameObject* gameObject, DirectX::XMFLOAT3 size, PhysicsMaterial* physicsMaterial)
	: Collider(gameObject, ColliderType::Box, physicsMaterial)
{
	this->size = size;
	FindInitialRigidBody(gameObject); //has to be in derived constructor because of GenerateShape pure virtual
}

// Implementation of the abstract function for generating the collider's physx shape
physx::PxShape* BoxCollider::GenerateShape(PxPhysics* physics)
{
	XMFLOAT3 half;
	XMStoreFloat3(&half, XMVectorScale(XMLoadFloat3(&size), 1/2.0f));
	PxBoxGeometry box = PxBoxGeometry(PhysicsHelper::Float3ToVec3(half));
	
	PxMaterial* mat;
	if (physicsMaterial == nullptr)
		mat = DEFAULT_PHYSICS_MAT;
	else mat = physicsMaterial->GetMaterial();

	return physics->createShape(box, *mat, true);
}
#pragma endregion

#pragma region Sphere Collider
// ----------------------------------------------------------------------------
//									SPHERE COLLIDER
// ----------------------------------------------------------------------------

SphereCollider::SphereCollider(GameObject* gameObject, float radius, PhysicsMaterial* physicsMaterial)
	: Collider(gameObject, ColliderType::Sphere, physicsMaterial)
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
	else mat = physicsMaterial->GetMaterial();

	return physics->createShape(sphere, *mat, true);
}

#pragma endregion

#pragma region Capsule Collider
// ----------------------------------------------------------------------------
//									CAPSULE COLLIDER
// ----------------------------------------------------------------------------

CapsuleCollider::CapsuleCollider(GameObject* gameObject, float radius, float height, 
	CapsuleDirection dir, PhysicsMaterial* physicsMaterial)
	: Collider(gameObject, ColliderType::Capsule, physicsMaterial)
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
	else mat = physicsMaterial->GetMaterial();

	//Create shape
	PxShape* shape = physics->createShape(capsule, *mat, true);

	//Rotate capsule
	PxTransform transform = shape->getLocalPose();
	switch (dir)
	{
		case CapsuleDirection::Y:
			shape->setLocalPose(PxTransform(PxQuat(PxHalfPi, PxVec3(0, 0, 1))));
			break;

		case CapsuleDirection::Z:
			shape->setLocalPose(PxTransform(PxQuat(PxHalfPi, PxVec3(0, 1, 0))));
			break;

		default:
			break;
	}
	
	return shape;
}
#pragma endregion