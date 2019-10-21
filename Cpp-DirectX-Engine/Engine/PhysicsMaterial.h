#pragma once
#include <PxPhysicsAPI.h>

class PhysicsMaterial
{
public:
	PhysicsMaterial();
	~PhysicsMaterial();

	physx::PxMaterial* GetMaterial();
};

