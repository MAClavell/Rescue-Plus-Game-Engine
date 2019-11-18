#pragma once
#include <vector>
#include <GameObject.h>
#include <string>

struct EntityRemoval {
	GameObject* e;
	bool release;
};

class EntityManager
{
private:
	// --------------------------------------------------------
	// Singleton Constructor - Set up the singleton instance of the EntityManager
	// --------------------------------------------------------
	EntityManager() { }
	~EntityManager() { };

	std::vector<GameObject*> entities;       //A vector of entities
	std::vector<EntityRemoval> remove_entities;       //A vector of entities

	// --------------------------------------------------------
	// Remove an entity by its object
	// --------------------------------------------------------
	void RemoveEntityFromList(GameObject* entity, bool release);

public:

	// Returns an Entity Manager Instance ---
	static EntityManager* GetInstance()
	{
		static EntityManager instance;
		return &instance;
	}

	// --------------------------------------------------------
	// Releases the entities in the Entity Manager
	// --------------------------------------------------------
	void Release();

	//Delete this
	EntityManager(EntityManager const&) = delete;
	void operator=(EntityManager const&) = delete;

	// Entity Methods -----------------------

	// --------------------------------------------------------
	// Add an entity to the entity manager
	// (checks if it is already in it)
	// --------------------------------------------------------
	void AddEntity(GameObject* entity);

	// --------------------------------------------------------
	// Get an entity by its name
	// --------------------------------------------------------
	GameObject* GetEntity(std::string name);

	// --------------------------------------------------------
	// Remove an entity by its name
	// --------------------------------------------------------
	void RemoveEntity(std::string name, bool deleteEntity = true);

	// --------------------------------------------------------
	// Remove an entity by its object
	// --------------------------------------------------------
	void RemoveEntity(GameObject* entity, bool deleteEntity = true);

	// --------------------------------------------------------
	// Run Update() for all entities in the manager
	// --------------------------------------------------------
	void Update(float deltaTime);

	// --------------------------------------------------------
	// Run FixedUpdate() for all entities in the manager
	// --------------------------------------------------------
	void FixedUpdate(float deltaTime);
};