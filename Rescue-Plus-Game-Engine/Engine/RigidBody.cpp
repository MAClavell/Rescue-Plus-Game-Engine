#include "RigidBody.h"
#include "PhysicsManager.h"
#include "PhysicsHelper.h"

using namespace physx;
using namespace DirectX;

RigidBody::RigidBody(GameObject* gameObject, float mass) : Component::Component(gameObject)
{
	PxPhysics* physics = PhysicsManager::GetInstance()->GetPhysics();
	collisionResolver = new CollisionResolver();

	//Create body
	PxTransform tr;
	tr.p = Float3ToVec3(gameObject->GetPosition());
	tr.q = Float4ToQuat(gameObject->GetRotation());
	body = physics->createRigidDynamic(tr);
	body->setMass(mass);

	//See if there is already a collider attached to this or any child gameobjects
	FindChildrenColliders(gameObject);
	
	//Add to the scene
	body->userData = this;
	PhysicsManager::GetInstance()->AddRigidBody(this);
}

RigidBody::~RigidBody()
{
	PhysicsManager::GetInstance()->RemoveRigidBody(this);
	if (collisionResolver != nullptr)
		delete collisionResolver;
}

// Attach all children colliders to this GO
void RigidBody::FindChildrenColliders(GameObject* go, bool first)
{
	//If there's a rigidbody in a children, ignore all those colliders
	if (!first)
	{
		RigidBody* rb = go->GetComponent<RigidBody>();
		if (rb != nullptr)
			return;
	}
	
	Collider* col = go->GetComponent<Collider>();
	if (col != nullptr)
	{
		col->Attach(this);
	}

	for each (GameObject* c in go->GetChildren())
	{
		FindChildrenColliders(c, false);
	}
}

void RigidBody::FixedUpdate(float deltaTime)
{
	collisionResolver->ResolveCollisions(gameObject()->GetAllUserComponents());
}

// Update the gameobject's world position from an inputted transform
void RigidBody::UpdateWorldPosition()
{
	PxTransform tr = body->getGlobalPose();
	gameObject()->SetRotationFromRigidBody(QuatToFloat4(tr.q));
	gameObject()->SetPositionFromRigidBody(Vec3ToFloat3(tr.p));
}

// Update the gameobject's rigidbody from it's world position
void RigidBody::UpdateRigidbodyPosition()
{
	PxTransform tr;
	tr.p = Float3ToVec3(gameObject()->GetPosition());
	tr.q = Float4ToQuat(gameObject()->GetRotation());
	body->setGlobalPose(tr);
}

// Set the mass of this rigid body
void RigidBody::SetMass(float mass)
{
	body->setMass(mass);
}

void RigidBody::SetKinematic(bool kinematic)
{
	body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, kinematic);
}

// Get the mass of this rigid body
float RigidBody::GetMass()
{
	return body->getMass();
}

// Get the actual bullet3 rigid body
PxRigidDynamic* RigidBody::GetRigidBody()
{
	return body;
}

// Add a force to this rigidbody
void RigidBody::AddForce(XMFLOAT3 force, PxForceMode::Enum mode)
{
	body->addForce(Float3ToVec3(force), mode);
}

// Add a force to this rigidbody
void RigidBody::AddForce(float x, float y, float z, PxForceMode::Enum mode)
{
	body->addForce(PxVec3(x, y, z), mode);
}

// Get the current linear velocity for this rigidbody
XMFLOAT3 RigidBody::GetLinearVelocity()
{
	return Vec3ToFloat3(body->getLinearVelocity());
}

// Set the current linear velocity for this rigidbody
void RigidBody::SetLinearVelocity(XMFLOAT3 velocity)
{
	body->setLinearVelocity(Float3ToVec3(velocity));
}

// Set the current linear velocity for this rigidbody
void RigidBody::SetLinearVelocity(float x, float y, float z)
{
	body->setLinearVelocity(PxVec3(x, y, z));
}

// Get the maximum linear velocity for this rigidbody
float RigidBody::GetMaxLinearVelocity()
{
	return body->getMaxLinearVelocity();
}

// Set the maximum linear velocity for this rigidbody
void RigidBody::SetMaxLinearVelocity(float max)
{
	body->setMaxLinearVelocity(max);
}

// --------------------------------------------------------
// WARNING: THIS IS FOR INTERNAL ENGINE USE ONLY. DO NOT USE
// Get the collision resolver for this rigidbody.
CollisionResolver* RigidBody::GetCollisionResolver()
{
	return collisionResolver;
}
