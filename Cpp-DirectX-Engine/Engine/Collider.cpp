#include "Collider.h"

using namespace DirectX;

// Create a collider and try to find a rigidbody
Collider::Collider(GameObject* gameObject, ColliderType type, PhysicsMaterial* physicsMaterial) : Component(gameObject)
{
	this->type = type;
	this->physicsMaterial = physicsMaterial;

	//Try to find a rigidbody to attach to
	RigidBody* rigidBody = gameObject->GetComponent<RigidBody>();
	if (rigidBody != nullptr)
	{
		Attach(rigidBody);
	}
}

// Get the rigidbody this collider is attached to (null if none)
RigidBody* Collider::GetAttachedRigidBody()
{
	return attachedRigidBody;
}
