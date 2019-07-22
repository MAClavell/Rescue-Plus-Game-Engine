#include "PhysicsManager.h"

using namespace physx;

PhysicsManager::~PhysicsManager()
{ 
	if(foundation)
		foundation->release();
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

	foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback,
		gDefaultErrorCallback);
	if (!foundation)
		printf("Error starting PhysX Foundation!");
	
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

