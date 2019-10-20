#include "PhysicsManager.h"

using namespace physx;

#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL;	}

PhysicsManager::~PhysicsManager()
{ 
	PX_RELEASE(scene);
	PX_RELEASE(dispatcher);
	PX_RELEASE(physics);
	
#ifdef DEBUG_PHYSICS
	if (pvd)
	{
		PxPvdTransport* transport = pvd->getTransport();
		pvd->release();	pvd = NULL;
		PX_RELEASE(transport);
	}
#endif

	PX_RELEASE(foundation);
}

// Initialize values and start the physics world
void PhysicsManager::Init()
{
	static PxDefaultErrorCallback gDefaultErrorCallback;
	static PxDefaultAllocator gDefaultAllocatorCallback;

	//Create foundation
	foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback,
		gDefaultErrorCallback);
	if (!foundation)
		printf("Error starting PhysX Foundation!");
	
#ifdef DEBUG_PHYSICS
	//The Physics Visual Debugger should only start in a certain debug configuration
	pvd = PxCreatePvd(*foundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
#endif

	//Create physics
	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, PxTolerancesScale(), true
#ifdef DEBUG_PHYSICS
		, pvd
#endif
	);
	if (!physics)
		printf("Error starting PhysX Physics!");

	//Create scene
	PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	dispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = dispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	sceneDesc.flags.set(PxSceneFlag::eENABLE_ACTIVE_ACTORS);
	scene = physics->createScene(sceneDesc);
	
#ifdef DEBUG_PHYSICS
	//Start debugger
	PxPvdSceneClient* pvdClient = scene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
#endif
}


bool PhysicsManager::Simulate(float deltaTime)
{
	static float accumulator = 0.0f;
	static float stepSize = 1.0f / 60.0f;

	accumulator += deltaTime;
	if (accumulator < stepSize)
		return false;

	accumulator -= stepSize;

	scene->simulate(stepSize);
	scene->fetchResults(true);

	// retrieve array of actors that moved
	PxU32 nbActiveActors;
	PxActor** activeActors = scene->getActiveActors(nbActiveActors);

	// update each render object with the new transform
	for (PxU32 i = 0; i < nbActiveActors; ++i)
	{
		RigidBody* rigidBody = static_cast<RigidBody*>(activeActors[i]->userData);
		if (rigidBody)
		{
			rigidBody->UpdateWorldPosition();
		}
	}

	return true;
}

// Get the physx Physics of this world
PxPhysics* PhysicsManager::GetPhysics()
{
	return physics;
}

// Set the gravity of the physics engine
void PhysicsManager::SetGravity(float gravity)
{
	scene->setGravity(PxVec3(0, gravity, 0));
}

// Get the gravity of the physics engine
float PhysicsManager::GetGravity()
{
	return scene->getGravity().y;
}

// Add a rigidbody to the sim
void PhysicsManager::AddRigidBody(RigidBody* rigidBody)
{
	if (foundation && scene)
		scene->addActor(*(rigidBody->GetRigidBody()));
}

// Remove a rigidbody from the sim
void PhysicsManager::RemoveRigidBody(RigidBody* rigidBody)
{
	if (foundation && scene)
		scene->removeActor(*(rigidBody->GetRigidBody()));
}

