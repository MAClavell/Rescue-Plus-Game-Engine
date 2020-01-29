#pragma once
#include <PxPhysicsAPI.h>
#include "CollisionLayers.h"
#include "GameObject.h"
#include "CollisionResolver.h"
#include <optional>

enum class ColliderType { Box = 0, Sphere = 1, Capsule = 2, Controller = 3 };

// --------------------------------------------------------
// A base class for collider and controller control
//
// Allows layering
// --------------------------------------------------------
class ColliderBase : public Component
{
private:
	ColliderType type;

	// --------------------------------------------------------
	// The attached GameObject's position changed
	// --------------------------------------------------------
	virtual void OnPositionChanged(DirectX::XMFLOAT3 position, bool fromParent,
		bool fromRigidBody) = 0;

	// --------------------------------------------------------
	// The attached GameObject's rotation changed
	// --------------------------------------------------------
	virtual void OnRotationChanged(DirectX::XMFLOAT4 rotation, bool fromParent,
		bool fromRigidBody) = 0;

	// --------------------------------------------------------
	// The attached GameObject's scale changed
	// --------------------------------------------------------
	virtual void OnScaleChanged(DirectX::XMFLOAT3 scale) = 0;

protected:
	CollisionResolver* collisionResolver;
	bool debug;
	physx::PxShape* shape;

	//Filtering
	CollisionLayers layers;
	std::optional<CollisionLayer> layerType;

	ColliderBase(GameObject* gameObject, ColliderType type);

	// --------------------------------------------------------
	// Update debug view
	// --------------------------------------------------------
	virtual void Update(float deltaTime) override = 0;

	// --------------------------------------------------------
	// Abstract function to set filter data
	// --------------------------------------------------------
	virtual void SetFilterData(physx::PxShape* shape) = 0;

public:
	~ColliderBase();

	// --------------------------------------------------------
	// Update collisions
	// --------------------------------------------------------
	virtual void FixedUpdate(float deltaTime) override = 0;

	// --------------------------------------------------------
	// Get the type of collider this is
	// --------------------------------------------------------
	ColliderType GetType();

	// --------------------------------------------------------
	// Get this collider's collision layer type 
	// (what type of layer this collider belongs to)
	// --------------------------------------------------------
	CollisionLayer GetCollisionLayerType();
	// --------------------------------------------------------
	// Set this collider's collision layer type
	// (what type of layer this collider belongs to)
	// --------------------------------------------------------
	void SetCollisionLayerType(CollisionLayer layerType);

	// --------------------------------------------------------
	// Get this collider's collision layers 
	// (what layers this collider will collide with)
	// --------------------------------------------------------
	CollisionLayers GetCollisionLayers();
	// --------------------------------------------------------
	// Get if this collider has the collision layer set 
	// (what layers this collider will collide with)
	// --------------------------------------------------------
	bool GetIfCollisionLayerSet(CollisionLayer layer);
	// --------------------------------------------------------
	// Set a SINGLE collision layer for this collider 
	// (what layers this collider will collide with)
	// --------------------------------------------------------
	void SetCollisionLayers(CollisionLayer layer);
	// --------------------------------------------------------
	// Set this collider's collision layers
	// (what layers this collider will collide with)
	// --------------------------------------------------------
	void SetCollisionLayers(CollisionLayers layers);
	// --------------------------------------------------------
	// Set ALL COLLISION LAYERS for this collider
	// Based on the parameter, this will set all layers to collide
	// or ignore collisions
	// --------------------------------------------------------
	void SetCollisionLayers(bool ignoreCollisions);


	// --------------------------------------------------------
	// WARNING: THIS IS FOR INTERNAL ENGINE USE ONLY. DO NOT USE
	// Get the collision resolver for this collider.
	// --------------------------------------------------------
	CollisionResolver* GetCollisionResolver();

	// --------------------------------------------------------
	// Get the debug status of this collider
	// --------------------------------------------------------
	bool GetDebug();
	// --------------------------------------------------------
	// Set the debug status of this collider
	// --------------------------------------------------------
	void SetDebug(bool debug);
};