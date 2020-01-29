#include "CharacterController.h"
#include "PhysicsManager.h"
#include "PhysicsHelper.h"
#include "Renderer.h"

using namespace DirectX;
using namespace physx;

static PhysicsManager* physicsManager;
//TODO: hook up gameobject position with char controller position

CharacterController::CharacterController(GameObject* gameObject, float radius, float height) 
	: ColliderBase(gameObject)
{
	physicsManager = PhysicsManager::GetInstance();

	//Set members
	this->radius = radius;
	this->height = height;

	//Create description for controller
	PxCapsuleControllerDesc desc;
	desc.radius = radius;
	desc.height = height;
	desc.material = physicsManager->GetPhysics()->createMaterial(0.6f, 0.6f, 0);
	desc.position = PxExtendedVec3(0, 3, 0);

	//Create controller
	pxController = physicsManager->GetControllerManager()->createController(desc);
}

CharacterController::~CharacterController()
{
	//Release internal physx controller
	if (pxController)
	{
		pxController->release();
		pxController = nullptr;
	}
}

CharacterControllerCollisionFlags CharacterController::Move(DirectX::XMFLOAT3 displacement, float deltaTime, bool applyGravity)
{
	//Apply gravity if we want to
	if (applyGravity)
		displacement.y += physicsManager->GetGravity() * (deltaTime * 2);

	//Move controller
	PxControllerCollisionFlags colFlags = pxController->move(
		Float3ToVec3(displacement),
		0,
		deltaTime,
		filters);

	//Setup flags
	CharacterControllerCollisionFlags flags;
	if (colFlags.isSet(PxControllerCollisionFlag::eCOLLISION_UP))
		flags.Set(CharacterControllerCollisionFlag::Above);
	if (colFlags.isSet(PxControllerCollisionFlag::eCOLLISION_SIDES))
		flags.Set(CharacterControllerCollisionFlag::Sides);
	if (colFlags.isSet(PxControllerCollisionFlag::eCOLLISION_DOWN))
		flags.Set(CharacterControllerCollisionFlag::Below);
	if (flags.flags == 0)
		flags.Set(CharacterControllerCollisionFlag::None);

	return flags;
}

// Update debug view
void CharacterController::Update(float deltaTime)
{
	if (debug)
	{
		Renderer::GetInstance()->AddDebugCapsule(radius, height, gameObject()->GetPosition(),
			gameObject()->GetRotation(), ShapeDrawType::SingleFrame);
	}
}

// Update collisions
void CharacterController::FixedUpdate(float deltaTime)
{
	collisionResolver->ResolveCollisions(gameObject());
}

// Set the filter data of the controller
void CharacterController::SetFilterData(physx::PxShape* shape)
{
	filterData.word0 = 1 << (PxU32)layerType.value();
	filterData.word1 = layers.flags;
	shape->setSimulationFilterData(filterData);
	filters = PxControllerFilters(&filterData, NULL, NULL);
}

// Get the foot position of the character controller
DirectX::XMFLOAT3 CharacterController::GetFootPosition()
{
	return ExtVec3ToFloat3(pxController->getFootPosition());
}

// Set the max height the controller can step up objects
void CharacterController::SetStepOffset(float offset)
{
	pxController->setStepOffset(offset);
}

// Get the max height the controller can step up objects
float CharacterController::GetStepOffset()
{
	return pxController->getStepOffset();
}

// Resize the character controller.
void CharacterController::Resize(float height)
{
	pxController->resize(height);
}
