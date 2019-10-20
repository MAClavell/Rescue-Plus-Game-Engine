#include "RigidBody.h"
#include "PhysicsManager.h"
#include "PhysicsHelper.h"
#include "Collider.h"

using namespace physx;
using namespace DirectX;

RigidBody::RigidBody(GameObject* gameObject, PxPhysics* physics, float mass) : Component::Component(gameObject)
{
	PxPhysics* physics = PhysicsManager::GetInstance()->GetPhysics();

	//Create body
	body = physics->createRigidDynamic(PxTransform(PhysicsHelper::Float3ToVec3(gameObject->GetPosition())));
	body->setMass(mass);

	//See if there is already a collider attached to this gameobject
	Collider* collider = gameObject->GetComponent<Collider>();
	if (collider != nullptr)
		collider->Attach(this);
	
	//Add to the scene
	body->userData = this;
	PhysicsManager::GetInstance()->AddRigidBody(this);
}

RigidBody::~RigidBody()
{
	PhysicsManager::GetInstance()->RemoveRigidBody(this);
}

// Update the gameobject's world position from it's rigidbody
void RigidBody::UpdateWorldPosition()
{
	PxTransform tr = body->getGlobalPose();
	gameObject()->SetPositionFromRigidBody(PhysicsHelper::Vec3ToFloat3(tr.p));
	gameObject()->SetRotationFromRigidBody(PhysicsHelper::QuatToFloat4(tr.q));
}

// Update the gameobject's rigidbody from it's world position
void RigidBody::UpdateRigidbodyPosition()
{
	PxTransform tr;
	tr.p = PhysicsHelper::Float3ToVec3(gameObject()->GetPosition());
	tr.q = PhysicsHelper::Float4ToQuat(gameObject()->GetRotation());
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
	body->addForce(PhysicsHelper::Float3ToVec3(force), mode);
}

// Add a force to this rigidbody
void RigidBody::AddForce(float x, float y, float z, PxForceMode::Enum mode)
{
	body->addForce(PxVec3(x, y, z), mode);
}

// Get the current linear velocity for this rigidbody
XMFLOAT3 RigidBody::GetLinearVelocity()
{
	return PhysicsHelper::Vec3ToFloat3(body->getLinearVelocity());
}

// Set the current linear velocity for this rigidbody
void RigidBody::SetLinearVelocity(XMFLOAT3 velocity)
{
	body->setLinearVelocity(PhysicsHelper::Float3ToVec3(velocity));
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

void RigidBody::AddShape(PxBoxGeometry geometry, PxMaterial* material)
{
	body
}

/*
//Create rigidbody with box shape
RigidBody::RigidBody(GameObject* gameObject, physx::PxBoxGeometry geometry, float mass) : Component::Component(gameObject)
{
	PxPhysics* physics = PhysicsManager::GetInstance()->GetPhysics();
	PxMaterial* material = physics->createMaterial(0.5f, 0.5f, 0.6f);

	CreateRigidBody(gameObject, physics, physics->createShape(geometry, *material, true), mass);
}

//Create rigidbody with sphere shape
RigidBody::RigidBody(GameObject* gameObject, physx::PxSphereGeometry geometry, float mass) : Component::Component(gameObject)
{
	PxPhysics* physics = PhysicsManager::GetInstance()->GetPhysics();
	PxMaterial* material = physics->createMaterial(0.5f, 0.5f, 0.6f);

	CreateRigidBody(gameObject, physics, physics->createShape(geometry, *material, true), mass);
}


//Create rigidbody with capsule shape
RigidBody::RigidBody(GameObject* gameObject, physx::PxCapsuleGeometry geometry, float mass) : Component::Component(gameObject)
{
	PxPhysics* physics = PhysicsManager::GetInstance()->GetPhysics();
	PxMaterial* material = physics->createMaterial(0.5f, 0.5f, 0.6f);

	CreateRigidBody(gameObject, physics, physics->createShape(geometry, *material, true), mass);
}

RigidBody::RigidBody(GameObject* gameObject, physx::PxPlaneGeometry geometry, float mass) : Component::Component(gameObject)
{
	PxMaterial* material = physics->createMaterial(0.5f, 0.5f, 0.6f);

	CreateRigidBody(gameObject, physics, physics->createShape(geometry, *material, true), mass);
}*/