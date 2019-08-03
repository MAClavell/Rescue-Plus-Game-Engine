#include "RigidBody.h"
#include "PhysicsManager.h"
#include "PhysicsHelper.h"

using namespace physx;

RigidBody::RigidBody(GameObject* gameObject, PxBoxGeometry geometry, float mass) : Component::Component(gameObject)
{
	PxPhysics* physics = PhysicsManager::GetInstance()->GetPhysics();
	PxMaterial* material = physics->createMaterial(0.5f, 0.5f, 0.6f);

	//Create body
	body = physics->createRigidDynamic(PxTransform(PxVec3(0.f, 2.5f, 0.f)));

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

// Update the gameobject after the sim ticks
bool RigidBody::UpdatePhysicsPosition()
{
	try
	{
		PxTransform tr = body->getGlobalPose();
		gameObject()->SetPosition(PhysicsHelper::Vec3ToFloat3(tr.p));
		gameObject()->SetRotation(PhysicsHelper::QuatToFloat4(tr.q));
		return true;
	}
	catch(std::exception e)
	{
		return false;
	}
}

// Set the mass of this rigid body
void RigidBody::SetMass(float mass)
{
	body->setMass(mass);
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

