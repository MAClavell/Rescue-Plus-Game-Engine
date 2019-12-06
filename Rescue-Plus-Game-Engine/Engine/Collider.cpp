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
Collider::Collider(GameObject* gameObject, ColliderType type,
	PhysicsMaterial* physicsMaterial, XMFLOAT3 center)
	: Component(gameObject)
{
	this->type = type;
	this->physicsMaterial = physicsMaterial;
	this->center = center;
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
		collisionResolver->ResolveCollisions(gameObject()->GetAllUserComponents());
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

// DeAttach this collider from it's rigidbody and make it a static collider
void Collider::DeAttachFromRB(bool makeStatic)
{
	if (attachedRigidBody != nullptr)
	{
		attachedRigidBody->GetRigidBody()->detachShape(*shape);
		attachedRigidBody = nullptr;
	}
	if (shape != nullptr)
	{
		shape->release();
		shape = nullptr;
	}

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
	if (shape != nullptr)
	{
		shape->release();
		shape = nullptr;
	}
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
	shape = GenerateShape(physics);
	shape->userData = this;

	//Attach
	rigidBody->GetRigidBody()->attachShape(*shape);

	if (!(rigidBody->GetRigidBody()->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC))
		rigidBody->GetRigidBody()->wakeUp();
}

// Attach this collider to a static shape
void Collider::AttachToStatic()
{
	if (attachedRigidBody != nullptr)
		DeAttachFromRB(false);

	PxPhysics* physics = PhysicsManager::GetInstance()->GetPhysics();

	//Create shape
	shape = GenerateShape(physics);
	shape->userData = this;

	//Calc transform
	PxTransform tr = PxTransform(Float3ToVec3(gameObject()->GetPosition()),
		Float4ToQuat(gameObject()->GetRotation()));

	//Attach
	if (staticActor != nullptr)
		staticActor->release();
	staticActor = PxCreateStatic(*physics, tr, *shape);
	PhysicsManager::GetInstance()->AddActor(staticActor);
}

// Get this shape's transform based on its position from the parent
PxTransform Collider::GetChildTransform()
{
	//Rotation difference
	XMVECTOR rbRot = XMLoadFloat4(&attachedRigidBody->gameObject()->GetRotation());
	XMFLOAT4 rotDiff;
	XMVECTOR rotDiffVec = XMQuaternionMultiply(
		XMLoadFloat4(&gameObject()->GetRotation()),
		XMQuaternionInverse(rbRot));
	XMStoreFloat4(&rotDiff, rotDiffVec);
	
	//Position difference
	XMVECTOR V = XMVectorSubtract(XMLoadFloat3(&gameObject()->GetPosition()), XMLoadFloat3(&attachedRigidBody->gameObject()->GetPosition()));
	XMVECTOR Q = XMQuaternionInverse(XMLoadFloat4(&attachedRigidBody->gameObject()->GetRotation()));
	XMVECTOR T = XMVectorScale(XMVector3Cross(Q, V), 2.0f);
	XMVECTOR posDiff = XMVectorAdd(V, XMVectorAdd(XMVectorScale(T, attachedRigidBody->gameObject()->GetRotation().w), XMVector3Cross(Q, T)));

	//XMMATRIX mat = XMLoadFloat4x4(&attachedRigidBody->gameObject()->GetRawWorldMatrix());
	//XMVECTOR posDiff = XMVector3TransformCoord(
	//	XMLoadFloat3(&gameObject()->GetPosition()),
	//	XMMatrixInverse(nullptr, mat));

	//Rotated center added to posDiff
	XMFLOAT3 pos;
	XMStoreFloat3(&pos, XMVectorAdd(posDiff,
		XMVector3Rotate(XMLoadFloat3(&center), rotDiffVec)));

	auto loc = gameObject()->GetLocalPosition();
	printf("%.3f, %.3f, %.3f vs. %.3f, %.3f, %.3f\n", loc.x, loc.y, loc.z, pos.x, pos.y, pos.z);

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

BoxCollider::BoxCollider(GameObject* gameObject, DirectX::XMFLOAT3 size,
	PhysicsMaterial* physicsMaterial, XMFLOAT3 center)
	: Collider(gameObject, ColliderType::Box, physicsMaterial, center)
{
	this->size = size;
	FindParentRigidBody(); //has to be in derived constructor because of GenerateShape pure virtual
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
		Renderer::GetInstance()->AddDebugCubeToThisFrame(
			pos,
			gameObject()->GetRotation(),
			size);
	}
}
#pragma endregion

#pragma region Sphere Collider
// ----------------------------------------------------------------------------
//									SPHERE COLLIDER
// ----------------------------------------------------------------------------

SphereCollider::SphereCollider(GameObject* gameObject, float radius,
	PhysicsMaterial* physicsMaterial, XMFLOAT3 center)
	: Collider(gameObject, ColliderType::Sphere, physicsMaterial, center)
{
	this->radius = radius;
	FindParentRigidBody();
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

void SphereCollider::Update(float deltaTime)
{
}
#pragma endregion

#pragma region Capsule Collider
// ----------------------------------------------------------------------------
//									CAPSULE COLLIDER
// ----------------------------------------------------------------------------
CapsuleCollider::CapsuleCollider(GameObject* gameObject, float radius, float height,
	CapsuleDirection dir, 
	PhysicsMaterial* physicsMaterial, XMFLOAT3 center)
	: Collider(gameObject, ColliderType::Capsule, physicsMaterial, center)
{
	this->radius = radius;
	this->height = height;
	this->dir = dir;
	FindParentRigidBody();
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
			shape->setLocalPose(tr);
			break;
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

void CapsuleCollider::Update(float deltaTime)
{
}
#pragma endregion