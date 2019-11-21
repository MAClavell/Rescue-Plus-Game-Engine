#pragma once
#include <PxPhysicsAPI.h>

class PhysicsMaterial
{
private:
	float dynamicFriction;
	float staticFriction;
	float restitution;
	physx::PxCombineMode::Enum frictionCombineMode;
	physx::PxCombineMode::Enum restitutionCombineMode;

	bool materialDirty;
	physx::PxMaterial* mat;

	// --------------------------------------------------------
	// Generate the PxMaterial from class members
	// --------------------------------------------------------
	void GeneratePxMaterial();

public:
	// --------------------------------------------------------
	// Create a physics material
	// 
	// dynamicFriction - The friction used when already moving
	// staticFriction - The friction used when an object is laying still on a surface
	// restitution - how bouncy the surface is
	// frictionCombineMode - How the friction of two colliding objects is combined
	// restitutionCombineMode - How the bounciness of two colliding objects is combined
	// --------------------------------------------------------
	PhysicsMaterial(float dynamicFriction = 0.6f, float staticFriction = 0.6f,
		float restitution = 0.6f,
		physx::PxCombineMode::Enum frictionCombineMode = physx::PxCombineMode::eAVERAGE,
		physx::PxCombineMode::Enum restitutionCombineMode = physx::PxCombineMode::eAVERAGE);
	~PhysicsMaterial();

	// --------------------------------------------------------
	// NOT FOR USER USE
	// Get  the PxMaterial representation of this mat
	// --------------------------------------------------------
	physx::PxMaterial* GetPxMaterial();

	// --------------------------------------------------------
	// Get the dynamic friction of this material
	// --------------------------------------------------------
	float GetDynamicFriction();
	// --------------------------------------------------------
	// Set the dynamic friction of this material
	// --------------------------------------------------------
	void SetDynamicFriction(float dynamicFriction);

	// --------------------------------------------------------
	// Get the static friction of this material
	// --------------------------------------------------------
	float GetStaticFriction();
	// --------------------------------------------------------
	// Set the static friction of this material
	// --------------------------------------------------------
	void SetStaticFriction(float staticFriction);

	// --------------------------------------------------------
	// Get the restitution this material
	// --------------------------------------------------------
	float GetRestitution();
	// --------------------------------------------------------
	// Set the restitution this material
	// --------------------------------------------------------
	void SetRestitution(float restitution);

	// --------------------------------------------------------
	// Get the friction combne mode of this material
	// --------------------------------------------------------
	physx::PxCombineMode::Enum GetFrictionCombineMode();
	// --------------------------------------------------------
	// Set the friction combine mode of this material
	// --------------------------------------------------------
	void SetFrictionCombineMode(physx::PxCombineMode::Enum combineMode);

	// --------------------------------------------------------
	// Get the restitution combine mode of this material
	// --------------------------------------------------------
	physx::PxCombineMode::Enum GetResitutionCombineMode();
	// --------------------------------------------------------
	// Set the restitution combine mode of this material
	// --------------------------------------------------------
	void SetRestitutionCombineMode(physx::PxCombineMode::Enum combineMode);

};

