#include "PhysicsMaterial.h"
#include "PhysicsManager.h"

using namespace physx;

// Create a physics material
PhysicsMaterial::PhysicsMaterial(float dynamicFriction, float staticFriction, float restitution,
	PxCombineMode::Enum frictionCombineMode, PxCombineMode::Enum restitutionCombineMode)
{
	this->dynamicFriction = dynamicFriction;
	this->staticFriction = staticFriction;
	this->restitution = restitution;
	this->frictionCombineMode = frictionCombineMode;
	this->restitutionCombineMode = restitutionCombineMode;

	materialDirty = true;
	GeneratePxMaterial();
}

PhysicsMaterial::~PhysicsMaterial()
{ }

// Get  the PxMaterial representation of this mat
physx::PxMaterial* PhysicsMaterial::GetPxMaterial()
{
	if (materialDirty)
		GeneratePxMaterial();

	return mat;
}

// Generate the PxMaterial from class members
void PhysicsMaterial::GeneratePxMaterial()
{
	PxPhysics* physics = PhysicsManager::GetInstance()->GetPhysics();

	mat = physics->createMaterial(staticFriction, dynamicFriction, restitution);
	mat->setFrictionCombineMode(frictionCombineMode);
	mat->setRestitutionCombineMode(restitutionCombineMode);
	
	materialDirty = false;
}

// Get the dynamic friction of this material
float PhysicsMaterial::GetDynamicFriction()
{
	return dynamicFriction;
}

// Set the dynamic friction of this material
void PhysicsMaterial::SetDynamicFriction(float dynamicFriction)
{
	this->dynamicFriction = dynamicFriction;
	materialDirty = true;
}

// Get the static friction of this material
float PhysicsMaterial::GetStaticFriction()
{
	return staticFriction;
}

// Set the static friction of this material
void PhysicsMaterial::SetStaticFriction(float staticFriction)
{
	this->staticFriction = staticFriction;
	materialDirty = true;
}

// Get the restitution this material
float PhysicsMaterial::GetRestitution()
{
	return restitution;
}

// Set the restitution this material
void PhysicsMaterial::SetRestitution(float restitution)
{
	this->restitution = restitution;
	materialDirty = true;
}

// Get the friction combne mode of this material
PxCombineMode::Enum PhysicsMaterial::GetFrictionCombineMode()
{
	return frictionCombineMode;
}

// Set the friction combine mode of this material
void PhysicsMaterial::SetFrictionCombineMode(PxCombineMode::Enum combineMode)
{
	this->frictionCombineMode = combineMode;
	materialDirty = true;
}

// Get the restitution combine mode of this material
PxCombineMode::Enum PhysicsMaterial::GetResitutionCombineMode()
{
	return restitutionCombineMode;
}

// Set the restitution combine mode of this material
void PhysicsMaterial::SetRestitutionCombineMode(PxCombineMode::Enum combineMode)
{
	this->restitutionCombineMode = combineMode;
	materialDirty = true;
}
