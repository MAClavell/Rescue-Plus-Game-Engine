#include "CharacterController.h"
#include "PhysicsManager.h"
#include "PhysicsHelper.h"

using namespace physx;

static PhysicsManager* physicsManager;
//TODO: hook up gameobject position with char controller position

CharacterController::CharacterController(GameObject* gameObject) : Component(gameObject)
{
	physicsManager = PhysicsManager::GetInstance();

	//Create description for controller
	PxCapsuleControllerDesc desc;
	

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
		displacement.y -= physicsManager->GetGravity() * (deltaTime * 2);

	//Move controller
	PxControllerCollisionFlags colFlags;/* = pxController->move(
		Float3ToVec3(displacement),
		0,
		deltaTime,
		);*/

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

// Get the foot position of the character controller
DirectX::XMFLOAT3 CharacterController::GetFootPosition()
{
	return ExtVec3ToFloat3(pxController->getFootPosition());
}

float CharacterController::GetStepOffset()
{
	return pxController->getStepOffset();
}

// Resize the character controller.
void CharacterController::Resize(float height)
{
	pxController->resize(height);
}
