#include "PhysicsManager.h"
#include "PhysicsHelper.h"
#include "JobSystem.h"
#include "Renderer.h"
#include "Raycast.h"
#include "CharacterController.h"

using namespace physx;
using namespace DirectX;

#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL;	}

void PhysicsManager::Release()
{ 
	PX_RELEASE(controllerManager);
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
		if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		{
			pairFlags = PxPairFlag::eTRIGGER_DEFAULT
				| PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_LOST;;
			return PxFilterFlag::eDEFAULT;
		}
	}

	// generate contacts for all that were not filtered above
	// trigger the contact callback for pairs (A,B) where 
	// the filtermask of A contains the ID of B and vice versa.
	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		pairFlags = PxPairFlag::eCONTACT_DEFAULT | PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_TOUCH_LOST;

	return PxFilterFlag::eDEFAULT;
}

PxQueryHitType::Enum PhysicsManager::preFilter(const PxFilterData& filterData,
	const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags)
{
	auto shapeFilterData = shape->getQueryFilterData();
	if (shape->getFlags() & PxShapeFlag::eTRIGGER_SHAPE)
		return PxQueryHitType::eNONE;

	//Only let like filters through
	if ((filterData.word0 & shapeFilterData.word1) && (shapeFilterData.word0 & filterData.word1))
		return PxQueryHitType::Enum::eBLOCK;

	return PxQueryHitType::eNONE;
}

//POST FILTER IS TURNED OFF
PxQueryHitType::Enum PhysicsManager::postFilter(const physx::PxFilterData& filterData, const physx::PxQueryHit& hit)
{
	//POST FILTER IS TURNED OFF
	return PxQueryHitType::eNONE;
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
	
	//Create character controller manager
	controllerManager = PxCreateControllerManager(*scene);

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
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		//Get shapes
		ColliderBase* shape1 = (Collider*)(pairs[i].shapes[0]->userData);
		ColliderBase* shape2 = (Collider*)(pairs[i].shapes[1]->userData);
		if (shape1 == shape2)
			return;

		//Use the rigidbody if the shape is attached to a rigidbody
		RigidBody* shape1Rb = nullptr;
		RigidBody* shape2Rb = nullptr;
		if(shape1->GetType() != ColliderType::Controller)
			shape1Rb = ((Collider*)shape1)->GetAttachedRigidBody();
		if (shape2->GetType() != ColliderType::Controller)
			shape2Rb= ((Collider*)shape2)->GetAttachedRigidBody();
		if (shape1Rb != nullptr && shape1Rb == shape2Rb)
			return;

		//Calculate collisions
		Collision col1 = shape2Rb == nullptr ? Collision(shape2->gameObject(), shape2)
			: Collision(shape2Rb->gameObject(), shape2);
		Collision col2 = shape1Rb == nullptr ? Collision(shape1->gameObject(), shape1)
			: Collision(shape1Rb->gameObject(), shape1);

		//OnEnter
		if (pairs[i].flags.isSet(PxContactPairFlag::eACTOR_PAIR_HAS_FIRST_TOUCH))
		{
			if (shape1->GetType() != ColliderType::Controller)
			{
				if (shape1Rb != nullptr)
					shape1Rb->GetCollisionResolver()->AddEnterCollision(col1);
				else shape1->GetCollisionResolver()->AddEnterCollision(col1);
			}

			if (shape2->GetType() != ColliderType::Controller)
			{
				if (shape2Rb != nullptr)
					shape2Rb->GetCollisionResolver()->AddEnterCollision(col2);
				else shape2->GetCollisionResolver()->AddEnterCollision(col2);
			}
		}
		//OnExit
		else if (pairs[i].flags.isSet(PxContactPairFlag::eACTOR_PAIR_LOST_TOUCH)
			|| pairs[i].flags.isSet(PxContactPairFlag::eREMOVED_SHAPE_0)
			|| pairs[i].flags.isSet(PxContactPairFlag::eREMOVED_SHAPE_1))
		{
			if (shape1->GetType() != ColliderType::Controller)
			{
				if (shape1Rb != nullptr)
					shape1Rb->GetCollisionResolver()->AddExitCollision(col1);
				else shape1->GetCollisionResolver()->AddExitCollision(col1);
			}

			if (shape2->GetType() != ColliderType::Controller)
			{
				if (shape2Rb != nullptr)
					shape2Rb->GetCollisionResolver()->AddExitCollision(col2);
				else shape2->GetCollisionResolver()->AddExitCollision(col2);
			}
		}
	}
}

void PhysicsManager::onTrigger(PxTriggerPair* pairs, PxU32 nbPairs)
{
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		Collider* trigger = (Collider*)(pairs[i].triggerShape->userData);
		ColliderBase* shape = (Collider*)(pairs[i].otherShape->userData);
		RigidBody* shapeRb = nullptr;
		if (shape->GetType() != ColliderType::Controller)
		{
			shapeRb = ((Collider*)shape)->GetAttachedRigidBody();
		}

		Collision col = shapeRb == nullptr ? Collision(shape->gameObject(), shape)
			: Collision(shapeRb->gameObject(), shape);

		// Since PhysX doesn't have PxTriggerPairFlags for enter and exit,
		// we have to manually check if a collision is entering or exiting in the resolver
		trigger->GetCollisionResolver()->SendTriggerCollision(col);
	}
}

void PhysicsManager::onControllerHit(const physx::PxControllersHit& hit)
{
	//Get controllers
	CharacterController* ctlr1 = (CharacterController*)(hit.controller->getUserData());
	CharacterController* ctlr2 = (CharacterController*)(hit.other->getUserData());

	//Add first collision
	Collision col1 = Collision(ctlr2->gameObject(), ctlr2);
	ctlr1->GetCollisionResolver()->SendControllerCollision(col1);

	//Add second collision
	Collision col2 = Collision(ctlr1->gameObject(), ctlr1);
	ctlr2->GetCollisionResolver()->SendControllerCollision(col2);
}

void PhysicsManager::onShapeHit(const physx::PxControllerShapeHit& hit)
{
	//Get data
	CharacterController* ctlr = (CharacterController*)(hit.controller->getUserData());
	Collider* shape = (Collider*)(hit.shape->userData);
	RigidBody* shapeRb = shape->GetAttachedRigidBody();

	//Create collisions
	Collision col = shapeRb == nullptr ? Collision(shape->gameObject(), shape)
		: Collision(shapeRb->gameObject(), shape);

	//Send collision to controller
	ctlr->GetCollisionResolver()->SendControllerCollision(col);
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

	if (nbActiveActors > 0)
	{
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
	}

	return true;
}

// Get the physx Physics of this world
PxPhysics* PhysicsManager::GetPhysics()
{
	return physics;
}

// Get the physx Scene of this world
PxScene* PhysicsManager::GetScene()
{
	return scene;
}

// Get the physx Controller Manager of this world
PxControllerManager* PhysicsManager::GetControllerManager()
{
	return controllerManager;
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
void PhysicsManager::AddActor(PxActor* actor)
{
	if (foundation && scene)
		scene->addActor(*actor);
}

// Remove a rigidbody from the sim
void PhysicsManager::RemoveActor(PxActor* actor)
{
	if (foundation && scene)
		scene->removeActor(*actor);
}

// Set the filter data of the controller
PxQueryFilterData GetQueryFilterData(CollisionLayers layers)
{
	PxQueryFilterData data;
	data.flags = PxQueryFlags(PxQueryFlag::eSTATIC | PxQueryFlag::eDYNAMIC | PxQueryFlag::ePREFILTER | PxQueryFlag::eANY_HIT);
	data.data = PxFilterData();
	data.data.word0 = (PxU32)(CollisionLayers(true).flags);
	data.data.word1 = (PxU32)layers.flags;
	return data;
}

// Cast a ray into the physics scene
bool Raycast(XMFLOAT3 origin, XMFLOAT3 direction,
	float maxDistance, ShapeDrawType drawType, float drawDuration)
{
	PxRaycastBuffer hit;
	bool didHit = PhysicsManager::GetInstance()->GetScene()->raycast(
		Float3ToVec3(origin), Float3ToVec3(direction),
		maxDistance, hit);
	
	//Debug drawing
	if (drawType != ShapeDrawType::None)
	{
		if (didHit)
			Renderer::GetInstance()->AddDebugRay(hit.block.distance, origin, direction, drawType, drawDuration);
		else Renderer::GetInstance()->AddDebugRay(maxDistance, origin, direction, drawType, drawDuration);
	}
	
	return didHit;
}

// Cast a ray into the physics scene
// hitInfo - out variable to get hit information out of the raycast
bool Raycast(DirectX::XMFLOAT3 origin, DirectX::XMFLOAT3 direction, RaycastHit* hitInfo,
	float maxDistance, ShapeDrawType drawType, float drawDuration)
{
	PxRaycastBuffer pxHit;
	if (PhysicsManager::GetInstance()->GetScene()->raycast(
		Float3ToVec3(origin), Float3ToVec3(direction),
		maxDistance, pxHit))
	{
		auto flags = pxHit.block.flags;

		//Point
		if (flags.isSet(PxHitFlag::ePOSITION))
			hitInfo->point = Vec3ToFloat3(pxHit.block.position);
		//Normal
		if (flags.isSet(PxHitFlag::eNORMAL))
			hitInfo->normal = Vec3ToFloat3(pxHit.block.normal);
		//Distance
		hitInfo->distance = pxHit.block.distance;
		
		//GameObject info
		hitInfo->collider = (ColliderBase*)pxHit.block.shape->userData;
		if (hitInfo->collider->GetType() != ColliderType::Controller)
		{
			hitInfo->rigidBody = ((Collider*)hitInfo->collider)->GetAttachedRigidBody();
			if (hitInfo->rigidBody != nullptr)
				hitInfo->gameObject = hitInfo->rigidBody->gameObject();
			else hitInfo->gameObject = hitInfo->collider->gameObject();
		}
		else hitInfo->gameObject = hitInfo->collider->gameObject();

		//Debug drawing
		if (drawType != ShapeDrawType::None)
			Renderer::GetInstance()->AddDebugRay(pxHit.block.distance, origin, direction, drawType, drawDuration);

		return true;
	}

	//Debug drawing
	if (drawType != ShapeDrawType::None)
		Renderer::GetInstance()->AddDebugRay(maxDistance, origin, direction, drawType, drawDuration);

	return false;
}

// Cast a geometry shape into the physics scene
bool Sweep(ColliderBase* collider, XMFLOAT3 direction, SweepHit* hitInfo, float maxDistance,
	CollisionLayers layerMask, ShapeDrawType drawType, float drawDuration)
{
	PxShape* shape = collider->GetPxShape();
	PxVec3 uhhhhhh = shape->getLocalPose().p;
	PxVec3 uhh = shape->getActor()->getGlobalPose().p;

	//Get correct position to start at
	PxVec3 pos = shape->getActor()->getGlobalPose().p + shape->getLocalPose().p;
	PxQuat rot = shape->getActor()->getGlobalPose().q * shape->getLocalPose().q;

	//Create transform
	PxTransform swpTrns = PxTransform(pos, rot);

	//Perform the sweep
	PxSweepBuffer pxHit;
	if (PhysicsManager::GetInstance()->GetScene()->sweep(
		shape->getGeometry().any(), swpTrns, Float3ToVec3(direction),
		maxDistance, pxHit, PxHitFlag::eDEFAULT, GetQueryFilterData(layerMask), PhysicsManager::GetInstance()))
	{
		auto flags = pxHit.block.flags;

		//Point
		if (flags.isSet(PxHitFlag::ePOSITION))
			hitInfo->point = Vec3ToFloat3(pxHit.block.position);
		//Normal
		if (flags.isSet(PxHitFlag::eNORMAL))
			hitInfo->normal = Vec3ToFloat3(pxHit.block.normal);
		//Distance
		hitInfo->distance = pxHit.block.distance;

		//GameObject info
		hitInfo->collider = (ColliderBase*)pxHit.block.shape->userData;
		if (hitInfo->collider->GetType() != ColliderType::Controller)
		{
			hitInfo->rigidBody = ((Collider*)hitInfo->collider)->GetAttachedRigidBody();
			if (hitInfo->rigidBody != nullptr)
				hitInfo->gameObject = hitInfo->rigidBody->gameObject();
			else hitInfo->gameObject = hitInfo->collider->gameObject();
		}
		else hitInfo->gameObject = hitInfo->collider->gameObject();

		//Debug drawing
		if (drawType != ShapeDrawType::None)
		{
			Renderer::GetInstance()->AddDebugRay(pxHit.block.distance, Vec3ToFloat3(pos),
				direction, drawType, drawDuration);

			switch (collider->GetType())
			{
				case ColliderType::Box:
				{
					BoxCollider* box = (BoxCollider*)collider;
					Renderer::GetInstance()->AddDebugCube(Vec3ToFloat3(pos), QuatToFloat4(rot),
						box->GetSize(), drawType, drawDuration);
				}
					break;

				case ColliderType::Sphere:
				{
					SphereCollider* sphere = (SphereCollider*)collider;
					Renderer::GetInstance()->AddDebugSphere(Vec3ToFloat3(pos), QuatToFloat4(rot),
						sphere->GetRadius(), drawType, drawDuration);
				}
					break;

				case ColliderType::Capsule:
				{
					CapsuleCollider* capsule = (CapsuleCollider*)collider;
					Renderer::GetInstance()->AddDebugCapsule(capsule->GetRadius(), capsule->GetHeight(),
						Vec3ToFloat3(pos), QuatToFloat4(rot),
						drawType, drawDuration);
				}
					break;

				case ColliderType::Controller:
				{
					CharacterController* controller = (CharacterController*)collider;
					Renderer::GetInstance()->AddDebugCapsule(controller->GetRadius(), controller->GetHeight(),
						Vec3ToFloat3(pos), QuatToFloat4(rot),
						drawType, drawDuration);
				}
					break;

				default:
					break;
			}
		}


		return true;
	}

	//Debug drawing
	if (drawType != ShapeDrawType::None)
		Renderer::GetInstance()->AddDebugRay(maxDistance, Vec3ToFloat3(pos),
			direction, drawType, drawDuration);

	return false;
}