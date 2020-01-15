#include "Collider.h"
#include "PhysicsManager.h"
#include "PhysicsHelper.h"
#include "Renderer.h"
#include "PhysicsHelper.h"

#define DEFAULT_PHYSICS_MAT (physics->createMaterial(0.6f, 0.6f, 0))

using namespace DirectX;
using namespace physx;

#pragma region Base Collider
// ----------------------------------------------------------------------------
//									BASE COLLIDER
// ----------------------------------------------------------------------------

// Create a collider and try to find a rigidbody
Collider::Collider(GameObject* gameObject, ColliderType type, bool isTrigger,
	PhysicsMaterial* physicsMaterial, XMFLOAT3 center)
	: Component(gameObject)
{
	this->type = type;
	this->physicsMaterial = physicsMaterial;
	this->center = center;
	this->isTrigger = isTrigger;

	debug = false;
	collisionResolver = new CollisionResolver();
	staticActor = nullptr;
	attachedRigidBody = nullptr;

	gameObject->AddListenerOnPositionChanged(std::bind(&Collider::OnPositionChanged, this,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	gameObject->AddListenerOnRotationChanged(std::bind(&Collider::OnRotationChanged, this,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

Collider::~Collider()
{
	//Detach shape first
	if (attachedRigidBody != nullptr)
		DeAttachFromRB(false);
	else
		DeAttachFromStatic();

	if (collisionResolver != nullptr)
		delete collisionResolver;

	//gameObject()->RemoveListenerOnPositionChanged(std::bind(&Collider::OnPositionChanged, this,
	//	std::placeholders::_1, std::placeholders::_2));
}

// Get the rigidbody this collider is attached to (null if none)
RigidBody* Collider::GetAttachedRigidBody()
{
	return attachedRigidBody;
}

// Search to see if there is already a rigidbody we can attach to
void Collider::FindParentRigidBody()
{
	//Try to find a rigidbody to attach to
	GameObject* go = gameObject();
	bool isChild = false;
	while (go != nullptr)
	{
		RigidBody* rigidBody = go->GetComponent<RigidBody>();
		if (rigidBody != nullptr)
		{
			AttachToRB(rigidBody, isChild);
			return;
		}
		go = go->GetParent();
		isChild = true;
	}
	AttachToStatic();
}

// Update collisions
void Collider::FixedUpdate(float deltaTime)
{
	if(staticActor != nullptr)
		collisionResolver->ResolveCollisions(gameObject());
}

void Collider::OnPositionChanged(DirectX::XMFLOAT3 position, bool fromParent, bool fromRigidBody)
{
	if (!fromRigidBody && ((attachedRigidBody != nullptr && !fromParent && isInChildObj)
		|| (staticActor != nullptr)))
		ReAttach();
}

void Collider::OnRotationChanged(DirectX::XMFLOAT4 rotation, bool fromParent, bool fromRigidBody)
{
	if (!fromRigidBody && ((attachedRigidBody != nullptr && !fromParent)
		|| (staticActor != nullptr)))
		ReAttach();
}

void Collider::OnScaleChanged(DirectX::XMFLOAT3 scale)
{
	ReAttach();
}

void Collider::SetFilterData(physx::PxShape* shape)
{
	PxFilterData filterData;
	filterData.word0 = 1 << (PxU32)layerType.value();
	filterData.word1 = layers.layers;
	shape->setSimulationFilterData(filterData);
}

// DeAttach this collider from it's rigidbody and make it a static collider
void Collider::DeAttachFromRB(bool makeStatic)
{
	if (attachedRigidBody != nullptr)
	{
		attachedRigidBody->GetRigidBody()->detachShape(*shape);
		attachedRigidBody = nullptr;
	}
	shape = nullptr;

	if (makeStatic)
		AttachToStatic();
}

// DeAttach this collider to a rigidbody
void Collider::DeAttachFromStatic()
{
	if (staticActor != nullptr)
	{
		PhysicsManager::GetInstance()->RemoveActor(staticActor);
		staticActor->release();
		staticActor = nullptr;
	}
	shape = nullptr;
}

// Re-calculate the shape and re-attach this collider to a rigidbody
void Collider::ReAttach()
{
	if (attachedRigidBody != nullptr)
	{
		RigidBody* rb = attachedRigidBody;
		DeAttachFromRB(false);
		AttachToRB(rb, isInChildObj);
	}
	else
	{
		DeAttachFromStatic();
		AttachToStatic();
	}
}

// Attach this collider to a rigidbody
void Collider::AttachToRB(RigidBody* rigidBody, bool isChildObj)
{
	if (staticActor != nullptr)
		DeAttachFromStatic();

	PxPhysics* physics = PhysicsManager::GetInstance()->GetPhysics();

	//Create shape
	isInChildObj = isChildObj;
	attachedRigidBody = rigidBody;
	PxShape* tempShape = GenerateShape(physics);
	tempShape->userData = this;
	
	//Set filter data
	if (!layerType.has_value())
		layerType = CollisionLayer::WorldDynamic;
	SetFilterData(tempShape);

	//Attach
	PxRigidDynamic* rb = rigidBody->GetRigidBody();
	rb->attachShape(*tempShape);

	if (!(rb->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC))
		rb->wakeUp();

	//Get the actual shape and store it
	PxShape** shapes = new PxShape*[rb->getNbShapes()];
	PxU32 nbShapes = rb->getShapes(shapes, rb->getNbShapes());
	for (PxU32 i = 0; i < nbShapes; i++)
	{
		if (shapes[i] == tempShape)
			shape = shapes[i];
	}
	tempShape->release();
	delete[] shapes;
}

// Attach this collider to a static shape
void Collider::AttachToStatic()
{
	if (attachedRigidBody != nullptr)
		DeAttachFromRB(false);

	PxPhysics* physics = PhysicsManager::GetInstance()->GetPhysics();

	//Create shape
	PxShape* tempShape = GenerateShape(physics);
	tempShape->userData = this;
	
	//Set filter data
	if (!layerType.has_value())
		layerType = CollisionLayer::WorldStatic;
	SetFilterData(tempShape);

	//Calc transform
	PxTransform tr = PxTransform(Float3ToVec3(gameObject()->GetPosition()),
		Float4ToQuat(gameObject()->GetRotation()));

	//Attach
	if (staticActor != nullptr)
		staticActor->release();
	staticActor = PxCreateStatic(*physics, tr, *tempShape);
	PhysicsManager::GetInstance()->AddActor(staticActor);
	
	//Get the actual shape and store it
	PxShape** shapes = new PxShape*[staticActor->getNbShapes()];
	PxU32 nbShapes = staticActor->getShapes(shapes, staticActor->getNbShapes());
	for (PxU32 i = 0; i < nbShapes; i++)
	{
		if (shapes[i] == tempShape)
			shape = shapes[i];
	}
	tempShape->release();
	delete[] shapes;
}

// Get this shape's transform based on its position from the parent
PxTransform Collider::GetChildTransform()
{
	//Rotation difference
	XMVECTOR rotDiffVec = XMQuaternionMultiply(
		XMLoadFloat4(&gameObject()->GetRotation()),
		XMQuaternionInverse(XMLoadFloat4(&attachedRigidBody->gameObject()->GetRotation())));
	XMFLOAT4 rotDiff;
	XMStoreFloat4(&rotDiff, rotDiffVec);
	
	//Math to get the local position
	//Unrotate difference between positions
	XMVECTOR V = XMVectorSubtract(XMLoadFloat3(&gameObject()->GetPosition()), XMLoadFloat3(&attachedRigidBody->gameObject()->GetPosition()));
	XMVECTOR Q = XMQuaternionInverse(XMLoadFloat4(&attachedRigidBody->gameObject()->GetRotation()));
	XMVECTOR T = XMVectorScale(XMVector3Cross(Q, V), 2.0f);
	XMVECTOR posDiff = XMVectorAdd(V, XMVectorAdd(XMVectorScale(T, attachedRigidBody->gameObject()->GetRotation().w), XMVector3Cross(Q, T)));

	//Rotated center added to posDiff
	XMFLOAT3 pos;
	XMStoreFloat3(&pos, XMVectorAdd(posDiff,
		XMVector3Rotate(XMLoadFloat3(&center), rotDiffVec)));

	//Make transform
	return PxTransform(Float3ToVec3(pos), Float4ToQuat(rotDiff));
}

// Get the center of the Collider
XMFLOAT3 Collider::GetCenter()
{
	return center;
}
// Set the center of the Collider
void Collider::SetCenter(DirectX::XMFLOAT3 center)
{
	this->center = center;
	ReAttach();
}
// Set the center of the Collider
void Collider::SetCenter(float x, float y, float z)
{
	this->center = XMFLOAT3(x, y, z);
	ReAttach();
}

// Get the physics material of the Collider
PhysicsMaterial * Collider::GetPhysicsMaterial()
{
	return physicsMaterial;
}
// Set the physics material of the Collider
void Collider::SetPhysicsMaterial(PhysicsMaterial* physicsMaterial)
{
	this->physicsMaterial = physicsMaterial;
	ReAttach();
}

// Get if this collider is a trigger shape
bool Collider::GetTrigger()
{
	return isTrigger;
}
// Set if this collider is a trigger shape
void Collider::SetTrigger(bool isTrigger)
{
	if (this->isTrigger != isTrigger)
	{
		this->isTrigger = isTrigger;

		//Triggers cannot be attached to rigidbodies
		//Must be recalculated
		if (attachedRigidBody != nullptr)
			DeAttachFromRB(true);
		else
		{
			shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
			shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
		}
	}
}

// Get this collider's collision layer type 
// (what type of layer this collider belongs to)
CollisionLayer Collider::GetCollisionLayerType()
{
	//Will always have a value, it is just empty for initial purposes
	return layerType.value();
}
// Set this collider's collision layer type
// (what type of layer this collider belongs to)
void Collider::SetCollisionLayerType(CollisionLayer layerType)
{
	this->layerType = layerType;
	SetFilterData(shape);
}

// Get this collider's collision layers 
// (what layers this collider will collide with)
CollisionLayers Collider::GetCollisionLayers()
{
	return layers;
}
// Get if this collider has the collision layer set 
// (what layers this collider will collide with)
bool Collider::GetIfCollisionLayerSet(CollisionLayer layer)
{
	return layers.IsSet(layer);
}

// Set a SINGLE collision layer for this collider 
// (what layers this collider will collide with)
void Collider::SetCollisionLayers(CollisionLayer layer)
{
	layers.Set(layer);
	SetFilterData(shape);
}
// Set this collider's collision layers
// (what layers this collider will collide with)
void Collider::SetCollisionLayers(CollisionLayers layers)
{
	this->layers = layers;
	SetFilterData(shape);
}
// Set ALL COLLISION LAYERS for this collider
// Based on the parameter, this will set all layers to collide
// or ignore collisions
void Collider::SetCollisionLayers(bool ignoreCollisions)
{
	CollisionLayers layers;
	for (uint32_t lay = (uint32_t)CollisionLayer::WorldStatic; lay <= (uint32_t)CollisionLayer::WorldDynamic; lay++)
	{
		if (ignoreCollisions)
			layers.Unset((CollisionLayer)lay);
		else layers.Set((CollisionLayer)lay);
	}
	this->layers = layers;
	SetFilterData(shape);
}

// WARNING: THIS IS FOR INTERNAL ENGINE USE ONLY. DO NOT USE
// Get the collision resolver for this collider.
CollisionResolver* Collider::GetCollisionResolver()
{
	return collisionResolver;
}

// Get the debug status of this collider
bool Collider::GetDebug()
{
	return debug;
}
// Set the debug status of this collider
void Collider::SetDebug(bool debug)
{
	this->debug = debug;
}

#pragma endregion

#pragma region Box Collider
// ----------------------------------------------------------------------------
//									BOX COLLIDER
// ----------------------------------------------------------------------------

BoxCollider::BoxCollider(GameObject* gameObject, DirectX::XMFLOAT3 size, bool isTrigger,
	PhysicsMaterial* physicsMaterial, XMFLOAT3 center)
	: Collider(gameObject, ColliderType::Box, isTrigger, physicsMaterial, center)
{
	this->size = size;
	FindParentRigidBody(); //has to be in derived constructor because of GenerateShape pure virtual
	SetCollisionLayers(false);
}

// Get the size of the BoxCollider
DirectX::XMFLOAT3 BoxCollider::GetSize()
{
	return size;
}
// Set the size of the BoxCollider
void BoxCollider::SetSize(DirectX::XMFLOAT3 size)
{
	this->size = size;
	ReAttach();
}
// Set the size of the BoxCollider
void BoxCollider::SetSize(float x, float y, float z)
{
	this->size = XMFLOAT3(x, y, z);
	ReAttach();
}

// Implementation of the abstract function for generating the collider's physx shape
physx::PxShape* BoxCollider::GenerateShape(PxPhysics* physics)
{
	XMFLOAT3 half;
	XMStoreFloat3(&half, XMVectorScale(XMLoadFloat3(&size), 1/2.0f));
	PxBoxGeometry box = PxBoxGeometry(Float3ToVec3(half));
	
	//Get material
	PxMaterial* mat;
	if (physicsMaterial == nullptr)
		mat = DEFAULT_PHYSICS_MAT;
	else mat = physicsMaterial->GetPxMaterial();

	//Get transform
	PxTransform tr;
	if (isInChildObj && attachedRigidBody != nullptr)
		tr = GetChildTransform();
	else tr = PxTransform(Float3ToVec3(center));

	//Make shape
	PxShape* shape = physics->createShape(box, *mat, true);
	shape->setLocalPose(tr);

	if (isTrigger)
	{
		shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	}
	return shape;
}

// Update debug view
void BoxCollider::Update(float deltaTime)
{
	if (debug)
	{
		XMFLOAT3 pos;
		XMStoreFloat3(&pos, XMVectorAdd(XMLoadFloat3(&gameObject()->GetPosition()),
			XMVector3Rotate(XMLoadFloat3(&center), XMLoadFloat4(&gameObject()->GetRotation()))));
		Renderer::GetInstance()->AddDebugCube(
			pos,
			gameObject()->GetRotation(),
			size, ShapeDrawType::SingleFrame);
	}
}
#pragma endregion

#pragma region Sphere Collider
// ----------------------------------------------------------------------------
//									SPHERE COLLIDER
// ----------------------------------------------------------------------------

SphereCollider::SphereCollider(GameObject* gameObject, float radius, bool isTrigger,
	PhysicsMaterial* physicsMaterial, XMFLOAT3 center)
	: Collider(gameObject, ColliderType::Sphere, isTrigger, physicsMaterial, center)
{
	this->radius = radius;
	FindParentRigidBody();
	SetCollisionLayers(false);
}

// Implementation of the abstract function for generating the collider's physx shape
physx::PxShape* SphereCollider::GenerateShape(physx::PxPhysics * physics)
{
	PxSphereGeometry sphere = PxSphereGeometry(radius);

	//Get material
	PxMaterial* mat;
	if (physicsMaterial == nullptr)
		mat = DEFAULT_PHYSICS_MAT;
	else mat = physicsMaterial->GetPxMaterial();

	//Get transform
	PxTransform tr;
	if (isInChildObj && attachedRigidBody != nullptr)
		tr = GetChildTransform();
	else tr = PxTransform(Float3ToVec3(center));

	//Make shape
	PxShape* shape = physics->createShape(sphere, *mat, true);
	shape->setLocalPose(tr);

	if (isTrigger)
	{
		shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	}
	return shape;
}

// Get the radius of the SphereCollider
float SphereCollider::GetRadius()
{
	return radius;
}
// Set the radius of the SphereCollider
void SphereCollider::SetRadius(float radius)
{
	this->radius = radius;
	ReAttach();
}

// Update debug view
void SphereCollider::Update(float deltaTime)
{
	if (debug)
	{
		XMFLOAT3 pos;
		XMStoreFloat3(&pos, XMVectorAdd(XMLoadFloat3(&gameObject()->GetPosition()),
			XMVector3Rotate(XMLoadFloat3(&center), XMLoadFloat4(&gameObject()->GetRotation()))));
		Renderer::GetInstance()->AddDebugSphere(
			pos,
			gameObject()->GetRotation(),
			radius, ShapeDrawType::SingleFrame);
	}
}
#pragma endregion

#pragma region Capsule Collider
// ----------------------------------------------------------------------------
//									CAPSULE COLLIDER
// ----------------------------------------------------------------------------
CapsuleCollider::CapsuleCollider(GameObject* gameObject, float radius, float height,
	CapsuleDirection dir, bool isTrigger,
	PhysicsMaterial* physicsMaterial, XMFLOAT3 center)
	: Collider(gameObject, ColliderType::Capsule, isTrigger, physicsMaterial, center)
{
	this->radius = radius;
	this->height = height;
	this->dir = dir;
	rot = XMFLOAT4(0, 0, 0, 1);
	FindParentRigidBody();
	SetCollisionLayers(false);
}

// Implementation of the abstract function for generating the collider's physx shape
physx::PxShape* CapsuleCollider::GenerateShape(physx::PxPhysics * physics)
{
	PxCapsuleGeometry capsule = PxCapsuleGeometry(radius, height / 2);

	//Get material
	PxMaterial* mat;
	if (physicsMaterial == nullptr)
		mat = DEFAULT_PHYSICS_MAT;
	else mat = physicsMaterial->GetPxMaterial();

	//Get transform
	PxTransform tr;
	if (isInChildObj && attachedRigidBody != nullptr)
		tr = GetChildTransform();
	else tr = PxTransform(Float3ToVec3(center));

	//Create shape
	PxShape* shape = physics->createShape(capsule, *mat, true);

	//Rotate capsule
	switch (dir)
	{
		case CapsuleDirection::Y:
			tr.q = tr.q * PxQuat(PxHalfPi, PxVec3(0, 0, 1));
			shape->setLocalPose(tr);
			break;

		case CapsuleDirection::Z:
			tr.q = tr.q * PxQuat(PxHalfPi, PxVec3(0, 1, 0));
			shape->setLocalPose(tr);
			break;

		default:
			tr.q = tr.q * PxQuat(PxHalfPi, PxVec3(1, 0, 0));
			shape->setLocalPose(tr);
			break;
	}
	rot = QuatToFloat4(tr.q);
	
	if (isTrigger)
	{
		shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		shape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
	}
	return shape;
}

// Get the radius of the CapsuleCollider
float CapsuleCollider::GetRadius()
{
	return radius;
}
// Set the radius of the CapsuleCollider
void CapsuleCollider::SetRadius(float radius)
{
	this->radius = radius;
	ReAttach();
}

// Get the height of the CapsuleCollider
float CapsuleCollider::GetHeight()
{
	return height;
}
// Set the height of the CapsuleCollider
void CapsuleCollider::SetHeight(float height)
{
	this->height = height;
	ReAttach();
}

// Get the direction of the CapsuleCollider
CapsuleDirection CapsuleCollider::GetCapsuleDirection()
{
	return dir;
}
// Set the direction of the CapsuleCollider
void CapsuleCollider::SetCapsuleDirection(CapsuleDirection dir)
{
	this->dir = dir;
	ReAttach();
}

// Update debug view
void CapsuleCollider::Update(float deltaTime)
{
	if (debug)
	{
		Renderer* renderer = Renderer::GetInstance();
		
		//Cylinder
		XMFLOAT4 cylRot;
		XMVECTOR rotVec = XMQuaternionMultiply(XMLoadFloat4(&rot), 
			XMLoadFloat4(&gameObject()->GetRotation()));
		XMStoreFloat4(&cylRot, rotVec);
		XMFLOAT3 pos;
		XMVECTOR posVec = XMVectorAdd(XMLoadFloat3(&gameObject()->GetPosition()),
			XMVector3Rotate(XMLoadFloat3(&center), rotVec));
			XMStoreFloat3(&pos, posVec);

		renderer->AddDebugCapsule(radius, height, pos, cylRot, ShapeDrawType::SingleFrame);
	}
}
#pragma endregion