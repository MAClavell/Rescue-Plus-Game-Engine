#include "Collider.h"
#include "PhysicsManager.h"
#include "PhysicsHelper.h"

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

// Abstract function for generating the collider's physx shape
physx::PxShape* BoxCollider::GenerateShape(PxPhysics* physics)
{
	XMFLOAT3 half;
	XMStoreFloat3(&half, XMVectorScale(XMLoadFloat3(&size), 1/2.0f));
	PxBoxGeometry box = PxBoxGeometry(PhysicsHelper::Float3ToVec3(half));

	PxMaterial* mat;
	if (physicsMaterial == nullptr)
		mat = physics->createMaterial(0.6f, 0.6f, 0);
	else mat = physicsMaterial->GetMaterial();

	return physics->createShape(box, *mat, true);
}
#pragma endregion

#pragma region Sphere Collider
// ----------------------------------------------------------------------------
//									SPHERE COLLIDER
// ----------------------------------------------------------------------------
#pragma endregion

#pragma region Capsule Collider
// ----------------------------------------------------------------------------
//									CAPSULE COLLIDER
// ----------------------------------------------------------------------------
#pragma endregion