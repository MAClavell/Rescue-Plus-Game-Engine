#include "RigidBody.h"
#include "PhysicsManager.h"
#include "PhysicsHelper.h"

using namespace physx;

RigidBody::RigidBody(GameObject* gameObject, PxBoxGeometry geometry, float mass) : Component::Component(gameObject)
{
	PxPhysics* physics = PhysicsManager::GetInstance()->GetPhysics();
	PxMaterial* material = physics->createMaterial(0.5f, 0.5f, 0.6f);

	//Create body
	body = physics->createRigidDynamic(PxTransform(PhysicsHelper::Float3ToVec3(gameObject->GetPosition())));

	//Attatch shape
	PxShape* shape = physics->createShape(geometry, *material, true);
	body->attachShape(*shape);
	shape->release();
	body->setMass(mass);
	
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

