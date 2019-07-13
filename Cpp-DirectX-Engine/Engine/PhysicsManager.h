#pragma once
#include <DirectXMath.h>
#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"
#include "RigidBody.h"

class PhysicsManager
{
private:
	btDiscreteDynamicsWorld* world;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* overlappingPairCache;
	btSequentialImpulseConstraintSolver* solver;

	//Values
	DirectX::XMFLOAT3 gravity;

	// --------------------------------------------------------
	// Singleton Constructor - Set up the singleton instance of the Physics
	// --------------------------------------------------------
	PhysicsManager();
	~PhysicsManager();

public:
	// --------------------------------------------------------
	// Initialize values and start the physics world
	// --------------------------------------------------------
	void Init();

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
	// Set the gravity of the physics engine
	// --------------------------------------------------------
	void SetGravity(float y);
	
	// --------------------------------------------------------
	// Add a rigidbody to the sim
	// --------------------------------------------------------
	void AddRigidBody(RigidBody* rigidBody);

	// --------------------------------------------------------
	// Remove a rigidbody from the sim
	// --------------------------------------------------------
	void RemoveRigidBody(RigidBody* rigidBody);

};

