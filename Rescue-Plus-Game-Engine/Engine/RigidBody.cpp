#include "RigidBody.h"
#include "PhysicsManager.h"
#include "PhysicsHelper.h"

using namespace physx;
using namespace DirectX;

RigidBody::RigidBody(GameObject* gameObject, float mass) : Component::Component(gameObject)
{
	PxPhysics* physics = PhysicsManager::GetInstance()->GetPhysics();
	collisionResolver = new CollisionResolver();

	//Create body
	PxTransform tr;
	tr.p = Float3ToVec3(gameObject->GetPosition());
	tr.q = Float4ToQuat(gameObject->GetRotation());
	body = physics->createRigidDynamic(tr);
	body->setMass(mass);

	//Add to the scene
	body->userData = this;
	PhysicsManager::GetInstance()->AddActor(body);

	gameObject->AddListenerOnPositionChanged(std::bind(&RigidBody::UpdateRigidbodyPosition, this,
		std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	//See if there is already a collider attached to this or any child gameobjects
	FindChildrenColliders(gameObject, true);
}

RigidBody::~RigidBody()
{
	if (body != nullptr)
	{
		//Detatch all shapes
		auto numShapes = body->getNbShapes();
		PxShape** shapes = new PxShape*[numShapes];
		numShapes = body->getShapes(shapes, numShapes);
		for (int i = numShapes - 1; i >= 0; i--)
		{
			((Collider*)(shapes[i]->userData))->DeAttachFromRB();
		}
		delete[] shapes;
	}
	PhysicsManager::GetInstance()->RemoveActor(body);
	body->release();
	body = nullptr;
	if (collisionResolver != nullptr)
		delete collisionResolver;
}

// Attach all children colliders to this GO
void RigidBody::FindChildrenColliders(GameObject* go, bool firstObj)
{
	//If there's a rigidbody in a child, ignore all those colliders
	if (!firstObj)
	{
		RigidBody* rb = go->GetComponent<RigidBody>();
		if (rb != nullptr)
			return;
	}
	
	//Find collider
	Collider* col = go->GetComponent<Collider>();
	if (col != nullptr)
	{
		col->AttachToRB(this, !firstObj);
	}

	//Check children for more colliders
	for each (GameObject* c in go->GetChildren())
	{
		FindChildrenColliders(c, false);
	}
}

void RigidBody::FixedUpdate(float deltaTime)
{
	collisionResolver->ResolveCollisions(gameObject()->GetAllUserComponents());
}

// Update the gameobject's world position from an inputted transform
void RigidBody::UpdateWorldPosition()
{
	PxTransform tr = body->getGlobalPose();
	gameObject()->SetRotationFromRigidBody(QuatToFloat4(tr.q));
	gameObject()->SetPositionFromRigidBody(Vec3ToFloat3(tr.p));
}

// Update the gameobject's rigidbody from it's world position
void RigidBody::UpdateRigidbodyPosition(XMFLOAT3 pos, bool fromParent, bool fromRigidBody)
{
	if (!fromRigidBody)
	{
		PxTransform tr;
		tr.p = Float3ToVec3(gameObject()->GetPosition());
		tr.q = Float4ToQuat(gameObject()->GetRotation());
		body->setGlobalPose(tr);
	}
}

// Set the mass of this rigid body
void RigidBody::SetMass(float mass)
{
	body->setMass(mass);
}

void RigidBody::SetKinematic(bool kinematic)
{
	body->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, kinematic);
}

// Get the mass of this rigid body
float RigidBody::GetMass()
{
	return body->getMass();
}

// Get the actual bullet3 rigid body
PxRigidDynamic* RigidBody::GetRigidBody()
{
	return body;
}

// Add a force to this rigidbody
void RigidBody::AddForce(XMFLOAT3 force, PxForceMode::Enum mode)
{
	body->addForce(Float3ToVec3(force), mode);
}

// Add a force to this rigidbody
void RigidBody::AddForce(float x, float y, float z, PxForceMode::Enum mode)
{
	body->addForce(PxVec3(x, y, z), mode);
}

// Get the current linear velocity for this rigidbody
XMFLOAT3 RigidBody::GetLinearVelocity()
{
	return Vec3ToFloat3(body->getLinearVelocity());
}

// Set the current linear velocity for this rigidbody
void RigidBody::SetLinearVelocity(XMFLOAT3 velocity)
{
	body->setLinearVelocity(Float3ToVec3(velocity));
}

// Set the current linear velocity for this rigidbody
void RigidBody::SetLinearVelocity(float x, float y, float z)
{
	body->setLinearVelocity(PxVec3(x, y, z));
}

// Get the maximum linear velocity for this rigidbody
float RigidBody::GetMaxLinearVelocity()
{
	return body->getMaxLinearVelocity();
}

// Set the maximum linear velocity for this rigidbody
void RigidBody::SetMaxLinearVelocity(float max)
{
	body->setMaxLinearVelocity(max);
}

// Set the contraints of the rigidbody
// Restricts movement or rotation on the axis
void RigidBody::SetContraints(bool lockPosX, bool lockPosY, bool lockPosZ,
	bool lockRotX, bool lockRotY, bool lockRotZ)
{
	PxU16 flags = 0;
	if (lockPosX)
		flags |= PxRigidDynamicLockFlag::eLOCK_LINEAR_X;
	if (lockPosY)
		flags |= PxRigidDynamicLockFlag::eLOCK_LINEAR_Y;
	if (lockPosZ)
		flags |= PxRigidDynamicLockFlag::eLOCK_LINEAR_Z;
	if (lockRotX)
		flags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_X;
	if (lockRotY)
		flags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y;
	if (lockRotZ)
		flags |= PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;

	body->setRigidDynamicLockFlags(PxRigidDynamicLockFlags(flags));
}

// --------------------------------------------------------
// WARNING: THIS IS FOR INTERNAL ENGINE USE ONLY. DO NOT USE
// Get the collision resolver for this rigidbody.
CollisionResolver* RigidBody::GetCollisionResolver()
{
	return collisionResolver;
}
