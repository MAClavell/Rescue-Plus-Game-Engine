#include "CharacterController.h"
#include "PhysicsManager.h"

using namespace physx;

CharacterController::CharacterController(GameObject* gameObject) : Component(gameObject)
{
	//Create description for controller
	PxCapsuleControllerDesc desc;

	//Create controller
	pxController = PhysicsManager::GetInstance()->GetControllerManager()->createController(desc);
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
