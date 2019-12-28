#pragma once
#include <vector>

class GameObject;
class Collider;
// --------------------------------------------------------
// A struct for holding collision information
// --------------------------------------------------------
struct Collision
{
	GameObject* gameObject;
	Collider* collider;

	Collision(GameObject* gameObject, Collider* collider)
		: gameObject(gameObject), collider(collider) {}

	bool operator==(const Collision& other)
	{
		return collider == other.collider;
	}
};

class UserComponent;
// --------------------------------------------------------
// Class for resolving collisions in colliders and rigidbodys
// --------------------------------------------------------
class CollisionResolver
{
private: 

	struct CollisionResolveInfo
	{
		Collision col;
		bool isTrigger;
		CollisionResolveInfo(Collision col, bool isTrigger) : col(col), isTrigger(isTrigger) { }
	};

	//Lists of collisions
	std::vector<CollisionResolveInfo> enterCollisions;
	std::vector<CollisionResolveInfo> stayCollisions;
	std::vector<CollisionResolveInfo> exitCollisions;

public:
	CollisionResolver() { };
	~CollisionResolver() { }


	// --------------------------------------------------------
	// Send a trigger to the resolver and have it decide what events to run.
	// Since PhysX doesn't have PxTriggerPairFlags for enter and exit,
	// we have to manually check if a collision is entering or exiting
	// --------------------------------------------------------
	void SendTriggerCollision(Collision collision);

	// --------------------------------------------------------
	// Add a collision to the resolver
	// --------------------------------------------------------
	void AddEnterCollision(Collision collision);
	// --------------------------------------------------------
	// Exit a collision to the resolver. 
	// Will remove an existing collision and add it to the exit list
	// --------------------------------------------------------
	void AddExitCollision(Collision collision);

	// --------------------------------------------------------
	// Resolve all collision events for this resolver
	// (OnCollisionEnter, OnCollisionStay, OnCollisionExit)
	// (OnTriggerEnter, OnTriggerStay, OnTriggerExit)
	// --------------------------------------------------------
	void ResolveCollisions(GameObject* obj);
};

