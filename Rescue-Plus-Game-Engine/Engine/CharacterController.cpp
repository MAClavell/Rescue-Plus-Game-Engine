#include "CharacterController.h"
#include "PhysicsManager.h"
#include "PhysicsHelper.h"
#include "Renderer.h"

using namespace DirectX;
using namespace physx;

static PhysicsManager* physicsManager;
//TODO: hook up gameobject position with char controller position

CharacterController::CharacterController(GameObject* gameObject, float radius, float height)
	: ColliderBase(gameObject, ColliderType::Controller)
{
	physicsManager = PhysicsManager::GetInstance();

	//Set members
	this->radius = radius;
	this->height = height;
	isGrounded = false;

	//Create description for controller
	PxCapsuleControllerDesc desc;
	desc.radius = radius;
	desc.height = height;
	desc.material = physicsManager->GetPhysics()->createMaterial(0.6f, 0.6f, 0);
	desc.position = Float3ToExtVec3(gameObject->GetPosition());
	desc.userData = this;
	desc.reportCallback = physicsManager;
	
	//Create controller
	pxController = physicsManager->GetControllerManager()->createController(desc);
	
	//Get the actual shape and assign userdata
	PxShape** shapes = new PxShape * [pxController->getActor()->getNbShapes()];
	PxU32 nbShapes = pxController->getActor()->getShapes(shapes, pxController->getActor()->getNbShapes());
	for (PxU32 i = 0; i < nbShapes; i++)
	{
		shapes[i]->userData = this;
		shapes[i]->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
		shape = shapes[i];
	}
	delete[] shapes;

	//Set filter data
	if (!layerType.has_value())
		layerType = CollisionLayer::WorldDynamic;
	SetCollisionLayers(false);
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

// Move the character by a displacement (in world coordinates)
CharacterControllerCollisionFlags CharacterController::Move(DirectX::XMFLOAT3 displacement,
	float deltaTime, bool applyGravity, float gravityScale)
{
	//Apply gravity if we want to
	if (applyGravity)
		displacement.y += (physicsManager->GetGravity() * gravityScale) * (deltaTime * 2);

	//Move controller
	PxControllerCollisionFlags colFlags = pxController->move(
		Float3ToVec3(displacement),
		0,
		deltaTime,
		filters);

	//Update the gameobject
	gameObject()->SetPositionFromPhysics(ExtVec3ToFloat3(pxController->getPosition()));

	//Setup flags
	isGrounded = false;
	CharacterControllerCollisionFlags flags;
	if (colFlags.isSet(PxControllerCollisionFlag::eCOLLISION_UP))
		flags.Set(CharacterControllerCollisionFlag::Above);
	if (colFlags.isSet(PxControllerCollisionFlag::eCOLLISION_SIDES))
		flags.Set(CharacterControllerCollisionFlag::Sides);
	if (colFlags.isSet(PxControllerCollisionFlag::eCOLLISION_DOWN))
	{
		flags.Set(CharacterControllerCollisionFlag::Below);
		isGrounded = true;
	}
	if (flags.flags == 0)
		flags.Set(CharacterControllerCollisionFlag::None);
	
	return flags;
}

// Update debug view
void CharacterController::Update(float deltaTime)
{
	if (debug)
	{
		XMFLOAT4 rot;
		XMStoreFloat4(&rot, XMQuaternionMultiply(XMLoadFloat4(&gameObject()->GetRotation()),
			XMQuaternionRotationAxis(XMVectorSet(0, 0, 1, 0), PxHalfPi)));
		Renderer::GetInstance()->AddDebugCapsule(radius, height, gameObject()->GetPosition(),
			rot, ShapeDrawType::SingleFrame);
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
	shape->setQueryFilterData(filterData);
	filters = PxControllerFilters(&filterData, physicsManager, NULL);
}

// The attached GameObject's position changed
void CharacterController::OnPositionChanged(XMFLOAT3 position, bool fromParent, bool fromPhysics)
{
	if (fromPhysics)
		return;

	pxController->setPosition(Float3ToExtVec3(position));
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

bool CharacterController::IsGrounded()
{
	return isGrounded;
}

// Resize the character controller.
void CharacterController::Resize(float height)
{
	pxController->resize(height);
}
