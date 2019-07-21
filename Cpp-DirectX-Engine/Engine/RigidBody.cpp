#include "RigidBody.h"
#include "PhysicsHelper.h"
#include "PhysicsManager.h"

RigidBody::RigidBody(GameObject* gameObject, float mass) : Component::Component(gameObject)
{
	/*
	btTransform groundTransform;
	groundTransform.setRotation(PhysicsHelper::XMQuatToBtQuat(gameObject->GetRotation()));
	groundTransform.setOrigin(PhysicsHelper::XMVec3ToBtVec3(gameObject->GetPosition()));

	btVector3 localInertia(0, 0, 0);
	if (mass != 0)
		shape->calculateLocalInertia(mass, localInertia);

	motionState = new btDefaultMotionState(groundTransform);
	this->mass = mass;
	this->shape = shape;

	btScalar massSc(mass);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
	rigidBody = new btRigidBody(rbInfo);
	PhysicsManager::GetInstance()->AddRigidBody(this);
	*/
}

RigidBody::~RigidBody()
{
	PhysicsManager::GetInstance()->RemoveRigidBody(this);

	/*
	if (motionState)
		delete motionState;
	if (shape)
		delete shape;
	if (rigidBody)
		delete rigidBody;
		*/
}

// Get the mass of this rigid body (a mass of 0 is not dynamic)
float RigidBody::GetMass()
{
	return mass;
}
/*
// Get the actual bullet3 rigid body
btRigidBody* RigidBody::GetRigidBody()
{
	return rigidBody;
}
*/
