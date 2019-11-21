#include "PhysicsManager.h"
#include "JobSystem.h"

using namespace physx;

#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL;	}

void PhysicsManager::Release()
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

PxFilterFlags CollisionFilterShader(
	PxFilterObjectAttributes attributes0, PxFilterData filterData0,
	PxFilterObjectAttributes attributes1, PxFilterData filterData1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	// let triggers through
	if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	// generate contacts for all that were not filtered above
	pairFlags = PxPairFlag::eCONTACT_DEFAULT 
		| PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_LOST;

	// trigger the contact callback for pairs (A,B) where 
	// the filtermask of A contains the ID of B and vice versa.
	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;

	return PxFilterFlag::eDEFAULT;
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
	sceneDesc.filterShader = CollisionFilterShader;
	sceneDesc.flags.set(PxSceneFlag::eENABLE_ACTIVE_ACTORS);
	sceneDesc.simulationEventCallback = this;
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

void PhysicsManager::onContact(const physx::PxContactPairHeader & pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
{
	//Get shapes
	Collider* shape1 = (Collider*)(pairs->shapes[0]->userData);
	Collider* shape2 = (Collider*)(pairs->shapes[1]->userData);
	if (shape1 == shape2)
		return;

	//Use the rigidbody if the shape is attached to a rigidbody
	RigidBody* shape1Rb = shape1->GetAttachedRigidBody();
	RigidBody* shape2Rb = shape2->GetAttachedRigidBody();
	if (shape1Rb == shape2Rb)
		return;

	//Calculate collisions
	Collision col1 = shape2Rb == nullptr ? Collision(shape2->gameObject(), shape2)
		: Collision(shape2Rb->gameObject(), shape2);
	Collision col2 = shape1Rb == nullptr ? Collision(shape1->gameObject(), shape1)
		: Collision(shape1Rb->gameObject(), shape1);

	//OnEnter
	if (pairHeader.pairs->flags.isSet(PxContactPairFlag::eACTOR_PAIR_HAS_FIRST_TOUCH))
	{
		if (shape1Rb != nullptr)
			shape1Rb->GetCollisionResolver()->AddEnterCollision(col1);
		else shape1->GetCollisionResolver()->AddEnterCollision(col1);

		if (shape2Rb != nullptr)
			shape2Rb->GetCollisionResolver()->AddEnterCollision(col2);
		else shape2->GetCollisionResolver()->AddEnterCollision(col2);
	}
	//OnExit
	else if (pairHeader.pairs->flags.isSet(PxContactPairFlag::eACTOR_PAIR_LOST_TOUCH))
	{
		if (shape1Rb != nullptr)
			shape1Rb->GetCollisionResolver()->AddExitCollision(col1);
		else shape1->GetCollisionResolver()->AddExitCollision(col1);

		if (shape2Rb != nullptr)
			shape2Rb->GetCollisionResolver()->AddExitCollision(col2);
		else shape2->GetCollisionResolver()->AddExitCollision(col2);
	}
}

void PhysicsManager::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	
}

static void UpdateRigidBody(Job* job, const void* userData)
{
	RigidBody* rigidBody = *((RigidBody**)userData);
	if (rigidBody)
	{
		//Update gameobject
		rigidBody->UpdateWorldPosition();
	}
}

bool PhysicsManager::Simulate(float deltaTime)
{
	scene->simulate(deltaTime);
	scene->fetchResults(true);

	// retrieve array of actors that moved
	PxU32 nbActiveActors;
	PxActor** activeActors = scene->getActiveActors(nbActiveActors);

	// update each render object with the new transform
	Job* root = JobSystem::CreateJob(&EmptyJob);
	for (PxU32 i = 0; i < nbActiveActors; ++i)
	{
		Job* job = JobSystem::CreateJobAsChild(root, &UpdateRigidBody, 
			&(activeActors[i]->userData));
		JobSystem::Run(job);
	}
	JobSystem::Run(root);
	JobSystem::Wait(root);

	// resolve collisions
	for (PxU32 i = 0; i < nbActiveActors; ++i)
	{
		PxRigidBody* rb = static_cast<PxRigidBody*>(activeActors[i]);
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
