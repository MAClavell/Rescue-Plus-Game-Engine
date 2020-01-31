#pragma once
#include <limits>
#include "Collider.h"
#include "DebugShapes.h"

// --------------------------------------------------------
// Struct for holding raycast hit info
// --------------------------------------------------------
struct RaycastHit
{
	ColliderBase* collider;
	float distance;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT3 point;
	RigidBody* rigidBody;
	GameObject* gameObject;
};

// --------------------------------------------------------
// Struct for holding sweep hit info
// --------------------------------------------------------
struct SweepHit
{
	ColliderBase* collider;
	float distance;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT3 point;
	RigidBody* rigidBody;
	GameObject* gameObject;
};

// --------------------------------------------------------
// Cast a ray into the physics scene
//
// returns bool whether an object was hit or not
// --------------------------------------------------------
bool Raycast(DirectX::XMFLOAT3 origin, DirectX::XMFLOAT3 direction,
	float maxDistance = std::numeric_limits<float>::infinity(),
	ShapeDrawType drawType = ShapeDrawType::None, float drawDuration = 1
);

// --------------------------------------------------------
// Cast a ray into the physics scene
// hitInfo - out variable to get hit information out of the raycast
//
// returns bool whether an object was hit or not
// --------------------------------------------------------
bool Raycast(DirectX::XMFLOAT3 origin, DirectX::XMFLOAT3 direction,
	RaycastHit* hitInfo,
	float maxDistance = std::numeric_limits<float>::infinity(), 
	ShapeDrawType drawType = ShapeDrawType::None, float drawDuration = 1
);

// --------------------------------------------------------
// Cast a geometry shape into the physics scene
// hitInfo - out variable to get hit information out of the sweep
//
// startInfront - when true, moves the starting point to not intersect
//		with the given collider
//
// returns bool whether an object was hit or not
// --------------------------------------------------------
bool Sweep(ColliderBase* collider, DirectX::XMFLOAT3 direction,
	SweepHit* hitInfo,
	float maxDistance = PX_MAX_SWEEP_DISTANCE,
	CollisionLayers layers = CollisionLayers(),
	ShapeDrawType drawType = ShapeDrawType::None, float drawDuration = 1);