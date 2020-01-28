#pragma once
#include "PhysX\include\foundation\PxSimpleTypes.h"

template <class T>
struct PhysXFlags {
public:
	physx::PxU32 flags = 0;
	bool IsSet(T flag) const { return 0 != (flags & (1 << (physx::PxU32)flag)); }
	void Set(T flag) { flags |= (1 << (physx::PxU32)flag); }
	void Unset(T flag) { flags &= ~(1 << (physx::PxU32)flag); }
};

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


struct CollisionLayers : PhysXFlags<CollisionLayer>
{ };

enum class CharacterControllerCollisionFlag { None=1, Sides, Above, Below };

struct CharacterControllerCollisionFlags : PhysXFlags<CharacterControllerCollisionFlag>
{ };