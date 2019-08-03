#pragma once
#include <DirectXMath.h>
#include "RigidBody.h"

class PhysicsManager
{
private:
	physx::PxFoundation* foundation;
	physx::PxPhysics* physics;

	physx::PxDefaultCpuDispatcher*	dispatcher;
	physx::PxScene* scene;

	/*
	btDiscreteDynamicsWorld* world;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* overlappingPairCache;
	btSequentialImpulseConstraintSolver* solver;
	*/
	//Values
	DirectX::XMFLOAT3 gravity;

	// --------------------------------------------------------
	// Singleton Constructor - Set up the singleton instance of the Physics
	// --------------------------------------------------------
	PhysicsManager() { Init(); }
	~PhysicsManager();

	// --------------------------------------------------------
	// Initialize values and start the physics world
	// --------------------------------------------------------
	void Init();

public:

	// --------------------------------------------------------
	// Get the singleton instance of the PhysicsManager
	// --------------------------------------------------------
	static PhysicsManager* GetInstance()
	{
		static PhysicsManager instance;

		return &instance;
	}

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
	void AddRigidBody(RigidBody* rigidBody);

	// --------------------------------------------------------
	// Remove a rigidbody from the sim
	// --------------------------------------------------------
	void RemoveRigidBody(RigidBody* rigidBody);

};

