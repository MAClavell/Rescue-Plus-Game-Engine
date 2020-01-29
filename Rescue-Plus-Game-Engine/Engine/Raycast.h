#pragma once
#include <limits>
#include "Collider.h"
#include "DebugShapes.h"

struct RaycastHit
{
	ColliderBase* collider;
	float distance;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT3 point;
	RigidBody* rigidBody;
	GameObject* gameObject;
};

bool Raycast(DirectX::XMFLOAT3 origin, DirectX::XMFLOAT3 direction,
	float maxDistance = std::numeric_limits<float>::infinity(),
	ShapeDrawType drawType = ShapeDrawType::None, float drawDuration = 1
);

bool Raycast(DirectX::XMFLOAT3 origin, DirectX::XMFLOAT3 direction,
	RaycastHit* hitInfo,
	float maxDistance = std::numeric_limits<float>::infinity(), 
	ShapeDrawType drawType = ShapeDrawType::None, float drawDuration = 1
);