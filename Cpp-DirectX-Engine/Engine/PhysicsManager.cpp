#include "PhysicsManager.h"

PhysicsManager::PhysicsManager()
{ }

PhysicsManager::~PhysicsManager()
{ 
	delete world;
	delete solver;
	delete overlappingPairCache;
	delete dispatcher;
	delete collisionConfiguration;
}

// Initialize values and start the physics world
void PhysicsManager::Init()
{
	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	dispatcher = new btCollisionDispatcher(collisionConfiguration);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	overlappingPairCache = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	solver = new btSequentialImpulseConstraintSolver;

	world = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

	world->setGravity(btVector3(0, -10, 0));
}

// Set the gravity of the physics engine
void PhysicsManager::SetGravity(float y)
{
	world->setGravity(btVector3(0, y, 0));
}

// Add a rigidbody to the sim
void PhysicsManager::AddRigidBody(RigidBody* rigidBody)
{
	if (world)
		world->addRigidBody(rigidBody->GetRigidBody());
}

// Remove a rigidbody from the sim
void PhysicsManager::RemoveRigidBody(RigidBody* rigidBody)
{
	if(world)
		world->removeRigidBody(rigidBody->GetRigidBody());
}

