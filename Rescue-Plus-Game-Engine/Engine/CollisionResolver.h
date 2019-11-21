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

		CollisionResolveInfo(Collision col) : col(col) { }
	};

	//Lists of collisions
	std::vector<CollisionResolveInfo> enterCollisions;
	std::vector<CollisionResolveInfo> stayCollisions;
	std::vector<CollisionResolveInfo> exitCollisions;

public:
	CollisionResolver() { };
	~CollisionResolver() { }

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
	// --------------------------------------------------------
	void ResolveCollisions(const std::vector<UserComponent*>& ucs);
};

