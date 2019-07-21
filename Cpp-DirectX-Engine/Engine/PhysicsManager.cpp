#include "PhysicsManager.h"
#include "PxPhysics.h"
#include "PxScene.h"
#include "PxRigidDynamic.h"
#include "PxShape.h"
#include "PxPhysicsAPI.h"

PhysicsManager::~PhysicsManager()
{ 
	/*
	delete world;
	delete solver;
	delete overlappingPairCache;
	delete dispatcher;
	delete collisionConfiguration;
	*/
}

// Initialize values and start the physics world
void PhysicsManager::Init()
{
	static PxDefaultErrorCallback gDefaultErrorCallback;
	static PxDefaultAllocator gDefaultAllocatorCallback;

	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback,
		gDefaultErrorCallback);
	if (!mFoundation)
		fatalError("PxCreateFoundation failed!");
}

// Set the gravity of the physics engine
void PhysicsManager::SetGravity(float y)
{
	//world->setGravity(btVector3(0, y, 0));
}

// Add a rigidbody to the sim
void PhysicsManager::AddRigidBody(RigidBody* rigidBody)
{
	//if (world)
	//	world->addRigidBody(rigidBody->GetRigidBody());
}

// Remove a rigidbody from the sim
void PhysicsManager::RemoveRigidBody(RigidBody* rigidBody)
{
	//if(world)
	//	world->removeRigidBody(rigidBody->GetRigidBody());
}

