#pragma once
#include "PhysX\include\foundation\PxSimpleTypes.h"

enum class CollisionLayer : physx::PxU32
{
	WorldStatic = 1,
	// --------------------------------------------------------
	// --------------------------------------------------------
	// DO NO EDIT LAYERS ABOVE THESE LINES
	// THEY ARE FOR INTERNAL ENGINE USE
	// --------------------------------------------------------
	// PUT CUSTOM LAYERS BELOW HERE
	//
	// EX: 
	// Vehicles,
	// Bullets,
	// Player
	// v v v v v v v v v v v v v v v v v v v v v v v v v v v v

	Player,

	// ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^
	// PUT CUSTOM LAYERS ABOVE HERE
	// --------------------------------------------------------
	// DO NO EDIT LAYERS BELOW THESE LINES
	// THEY ARE FOR INTERNAL ENGINE USE
	// --------------------------------------------------------
	// --------------------------------------------------------
	WorldDynamic
};

struct CollisionLayers
{
public:
	physx::PxU32 layers = 0;
	bool IsSet(CollisionLayer layer) const { return 0 != (layers & (1 << (physx::PxU32)layer)); }
	void Set(CollisionLayer layer) { layers |= (1 << (physx::PxU32)layer); }
	void Unset(CollisionLayer layer) { layers &= ~(1 << (physx::PxU32)layer); }
};