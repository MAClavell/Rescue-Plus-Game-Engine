#pragma once
#include <DirectXMath.h>
#include "Collider.h"
#include "PxSimulationEventCallback.h"

class PhysicsManager : public physx::PxSimulationEventCallback
{
private:
	physx::PxFoundation* foundation;
	physx::PxPhysics* physics;

	physx::PxDefaultCpuDispatcher*	dispatcher;
	physx::PxScene* scene;
	
#ifdef DEBUG_PHYSICS
	physx::PxPvd* pvd;
#endif

	// --------------------------------------------------------
	// Singleton Constructor - Set up the singleton instance of the Physics
	// --------------------------------------------------------
	PhysicsManager() { Init(); }
	~PhysicsManager() { };

	// --------------------------------------------------------
	// Initialize values and start the physics world
	// --------------------------------------------------------
	void Init();

	// Implements PxSimulationEventCallback
	virtual void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs);
	virtual void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count);
	virtual void onConstraintBreak(physx::PxConstraintInfo*, physx::PxU32) {}
	virtual void onWake(physx::PxActor**, physx::PxU32) {}
	virtual void onSleep(physx::PxActor**, physx::PxU32) {}
	virtual void onAdvance(const physx::PxRigidBody*const*, const physx::PxTransform*, const physx::PxU32) {}

public:

	// --------------------------------------------------------
	// Get the singleton instance of the PhysicsManager
	// --------------------------------------------------------
	static PhysicsManager* GetInstance()
	{
		static PhysicsManager instance;

		return &instance;
	}

	// --------------------------------------------------------
	// Deinitialize values
	// --------------------------------------------------------
	void Release();

	//Delete this
	PhysicsManager(PhysicsManager const&) = delete;
	void operator=(PhysicsManager const&) = delete;

	// --------------------------------------------------------
	// Simulate a step in the physics sim
	// --------------------------------------------------------
	bool Simulate(float deltaTime);

	// --------------------------------------------------------
	// Get the physx Physics of this world
	// --------------------------------------------------------
	physx::PxPhysics* GetPhysics();

	// --------------------------------------------------------
	// Set the gravity of the physics engine
	// --------------------------------------------------------
	void SetGravity(float y);

	// --------------------------------------------------------
	// Get the gravity of the physics engine
	// --------------------------------------------------------
	float GetGravity();
	
	// --------------------------------------------------------
	// Add a rigidbody to the sim
	// --------------------------------------------------------
	void AddActor(physx::PxActor* actor);

	// --------------------------------------------------------
	// Remove a rigidbody from the sim
	// --------------------------------------------------------
	void RemoveActor(physx::PxActor* actor);

};

