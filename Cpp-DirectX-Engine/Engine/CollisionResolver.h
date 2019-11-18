#pragma once
#include <vector>

class GameObject;
class Collider;
struct Collision
{
	GameObject* gameObject;
	Collider* collider;

	Collision(GameObject* gameObject, Collider* collider)
		: gameObject(gameObject), collider(collider) {}

	bool operator==(const Collision& other)
	{
		return gameObject == other.gameObject && collider == other.collider;
	}
};

class UserComponent;
class CollisionResolver
{
private: 
	struct CollisionResolveInfo
	{
		Collision col;

		CollisionResolveInfo(Collision col) : col(col) { }
	};

	std::vector<CollisionResolveInfo> enterCollisions;
	std::vector<CollisionResolveInfo> stayCollisions;
	std::vector<CollisionResolveInfo> exitCollisions;

public:
	CollisionResolver() { };
	~CollisionResolver() { }

	void AddEnterCollision(Collision collision);
	void AddExitCollision(Collision collision);

	void ResolveCollisions(const std::vector<UserComponent*>& ucs);
};

