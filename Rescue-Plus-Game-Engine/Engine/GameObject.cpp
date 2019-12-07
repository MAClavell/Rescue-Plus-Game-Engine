#include "GameObject.h"
#include "Renderer.h"
#include "EntityManager.h"
#include "RigidBody.h"

// For the DirectX Math library
using namespace DirectX;

//Construct a component
Component::Component(GameObject* gameObject)
{
	this->attatchedGameObject = gameObject;
}

//Have to put these here for the Collision definition
void UserComponent::OnCollisionEnter(Collision collision) {}
void UserComponent::OnCollisionStay(Collision collision) {}
void UserComponent::OnCollisionExit(Collision collision) {}

// Constructor - Set up the gameobject.
GameObject::GameObject()
{
	//Set default transformation values
	parent = nullptr;
	world = XMFLOAT4X4();
	position = XMFLOAT3(0, 0, 0);
	localPosition = XMFLOAT3(0, 0, 0);
	SetRotation(0, 0, 0);
	scale = XMFLOAT3(1, 1, 1);
	worldDirty = false;
	RebuildWorld();

	enabled = true;
	name = "GameObject";

	EntityManager::GetInstance()->AddEntity(this);
}

// Constructor - Set up the gameobject.
GameObject::GameObject(std::string name)
	: GameObject()
{
	this->name = name;
}

// Destructor for when an instance is deleted
// Destroys all children too
GameObject::~GameObject()
{ 
	//Delete all components
	for (auto c : components)
	{
		delete c;
	}
}

// Get the enabled state of the gameobject
bool GameObject::GetEnabled()
{
	return enabled;
}

// Enable or disable the gameobject
void GameObject::SetEnabled(bool enabled)
{
	this->enabled = enabled;
}

// Set the name of this gameobject
void GameObject::SetName(std::string name)
{
	this->name = name;
}

// Get the name of this gameobject
std::string GameObject::GetName()
{
	return name;
}

// Set the parent of this GameObject
void GameObject::SetParent(GameObject* parent)
{
	if (this->parent != nullptr)
		this->parent->RemoveChild(this);

	this->parent = parent;

	if (this->parent != nullptr)
		this->parent->AddChild(this);

	SetPosition(position, true);
	SetRotation(rotation, true);
	SetScale(scale);
}

// Get the parent of this GameObject
GameObject* GameObject::GetParent()
{
	return this->parent;
}

// Remove a child from a gameobject
void GameObject::RemoveChild(GameObject* child)
{
	for (auto iter = children.begin(); iter != children.end(); iter++)
	{
		if (*iter == child)
		{
			//Swap with the last and pop
			std::iter_swap(iter, children.end());
			children.pop_back();

			//Delete
			delete child;
			break;
		}
	}
}

// Get the children of this gameobject
std::vector<GameObject*> GameObject::GetChildren()
{
	return children;
}

// --------------------------------------------------------
// Add a child to a gameobject
// --------------------------------------------------------
void GameObject::AddChild(GameObject* child)
{
	children.push_back(child);
}

// Get a readonly list of all user components
const std::vector<UserComponent*>& GameObject::GetAllUserComponents()
{
	return userComponents;
}

// Update this component
void GameObject::Update(float deltaTime)
{ 
	for (auto c : components)
	{
		c->Update(deltaTime);
	}
}

// Update this component at the fixed timestep
void GameObject::FixedUpdate(float deltaTime)
{
	for (auto c : components)
	{
		c->FixedUpdate(deltaTime);
	}
}

// Get the world matrix for this GameObject (rebuilding if necessary)
XMFLOAT4X4 GameObject::GetWorldMatrix()
{
	//Rebuild the world if it is not current
	if (worldDirty)
		RebuildWorld();

	return world;
}

DirectX::XMFLOAT4X4 GameObject::GetRawWorldMatrix()
{
	if (worldDirty)
		RebuildWorld();

	return worldRaw;
}

// Get the inverse transpose of the world matrix for this entity (rebuilding if necessary)
XMFLOAT4X4 GameObject::GetWorldInvTransMatrix()
{
	//Rebuild the world if it is not current
	if (worldDirty)
		RebuildWorld();

	return worldInvTrans;
}

// Rebuild the world matrix from the different components
void GameObject::RebuildWorld()
{
	//Get translation matrix
	XMMATRIX translation = XMMatrixTranslationFromVector(XMLoadFloat3(&position));

	//Get rotation matrix
	XMMATRIX rotating = XMMatrixRotationQuaternion(XMLoadFloat4(&rotation));

	//Get scale matrix
	XMMATRIX scaling = XMMatrixScalingFromVector(XMLoadFloat3(&scale));

	//Calculate matrix and store
	//Transpose for DirectX
	XMMATRIX newWorldRaw = scaling * rotating * translation;
	XMMATRIX newWorld = XMMatrixTranspose(newWorldRaw);
	XMStoreFloat4x4(&world, newWorld);
	XMStoreFloat4x4(&worldRaw, newWorldRaw);

	//Calculate inverse transpose
	XMStoreFloat4x4(&worldInvTrans, XMMatrixInverse(&XMVectorSet(-1, -1, -1, -1), XMMatrixTranspose(newWorld)));

	worldDirty = false;
}

//TODO: Add removal to the messenger

// Add a listener to onPositionChanged
void GameObject::AddListenerOnPositionChanged(std::function<void(DirectX::XMFLOAT3, bool, bool)> function)
{
	onPositionChanged.AddListener(function);
}
// Remove a listener from onPositionChanged
void GameObject::RemoveListenerOnPositionChanged(std::function<void(DirectX::XMFLOAT3, bool, bool)> function)
{
	onPositionChanged.RemoveListener(function);
}

// Add a listener to onRotationChanged
void GameObject::AddListenerOnRotationChanged(std::function<void(DirectX::XMFLOAT4, bool, bool)> function)
{
	onRotationChanged.AddListener(function);
}
// Remove a listener from onScaleChanged
void GameObject::RemoveListenerOnRotationChanged(std::function<void(DirectX::XMFLOAT4, bool, bool)> function)
{
	onRotationChanged.RemoveListener(function);
}

// Remove a listener from onRotationChanged
void GameObject::AddListenerOnScaleChanged(std::function<void(DirectX::XMFLOAT3)> function)
{
	onScaleChanged.AddListener(function);
}
// Add a listener to onScaleChanged
void GameObject::RemoveListenerOnScaleChanged(std::function<void(DirectX::XMFLOAT3)> function)
{
	onScaleChanged.RemoveListener(function);
}

// Get the position for this GameObject
XMFLOAT3 GameObject::GetPosition()
{
	return position;
}

// Get the local position for this GameObject
DirectX::XMFLOAT3 GameObject::GetLocalPosition()
{
	return localPosition;
}

// Set the position for this GameObject
void GameObject::SetPosition(XMFLOAT3 newPosition, bool setLocal,
	bool fromParent, bool fromRigidBody)
{
	worldDirty = true;
	position = newPosition;

	//Update the local position
	if (setLocal)
	{
		if (parent != nullptr)
		{
			//Math to get the local position
			//Unrotate difference between positions
			XMVECTOR V = XMVectorSubtract(XMLoadFloat3(&position), XMLoadFloat3(&parent->GetPosition()));
			XMVECTOR Q = XMQuaternionInverse(XMLoadFloat4(&parent->GetRotation()));
			XMVECTOR T = XMVectorScale(XMVector3Cross(Q, V), 2.0f);
			XMVECTOR newLoc = XMVectorAdd(V, XMVectorAdd(XMVectorScale(T, parent->GetRotation().w), XMVector3Cross(Q, T)));
			XMStoreFloat3(&localPosition, newLoc);
		}
		else localPosition = position;
	}

	//Run event
	onPositionChanged.Invoke(position, fromParent, fromRigidBody);

	//Update transforms of all children
	for (auto c : children)
	{
		c->SetLocalPosition(c->GetLocalPosition(), true);
	}
}

// Set the position for this GameObject from a rigidbody
void GameObject::SetPositionFromRigidBody(XMFLOAT3 newPosition)
{
	SetPosition(newPosition, true, false, true);
}

// Set the position for this GameObject
void GameObject::SetPosition(XMFLOAT3 newPosition)
{
	SetPosition(newPosition, true);
}

// Set the position for this GameObject
void GameObject::SetPosition(float x, float y, float z)
{
	SetPosition(XMFLOAT3(x, y, z), true);
}

// Set the local position for this GameObject
void GameObject::SetLocalPosition(XMFLOAT3 newLocalPosition,
	bool fromParent)
{
	if (parent != nullptr)
	{
		localPosition = newLocalPosition;
		XMFLOAT3 rotatedWorldPos;
		XMVECTOR rotated = XMVector3Rotate(XMLoadFloat3(&localPosition),
			XMLoadFloat4(&parent->GetRotation()));
		XMStoreFloat3(&rotatedWorldPos,
			XMVectorAdd(XMLoadFloat3(&parent->GetPosition()), rotated));
		SetPosition(rotatedWorldPos, false, fromParent);
	}
	else this->SetPosition(newLocalPosition, true, fromParent);
}

// Set the local position for this GameObject
void GameObject::SetLocalPosition(XMFLOAT3 newLocalPosition)
{
	SetLocalPosition(newLocalPosition, false);
}

// Set the local position for this GameObject
void GameObject::SetLocalPosition(float x, float y, float z)
{
	SetLocalPosition(XMFLOAT3(x, y, z), false);
}

// Moves this GameObject in absolute space by a given vector.
// Does not take rotation into account
void GameObject::MoveAbsolute(XMFLOAT3 moveAmnt)
{
	//Add the vector to the position
	XMFLOAT3 newPos;
	XMStoreFloat3(&newPos, XMVectorAdd(XMLoadFloat3(&position),
		XMLoadFloat3(&moveAmnt)));
	SetPosition(newPos, true);
}

// Moves this GameObject in relative space by a given vector.
// Does take rotation into account
void GameObject::MoveRelative(XMFLOAT3 moveAmnt)
{
	// Rotate the movement vector
	XMVECTOR move = XMVector3Rotate(XMLoadFloat3(&moveAmnt),
		XMLoadFloat4(&rotation));

	//Add to position
	XMFLOAT3 newPos;
	XMStoreFloat3(&newPos, XMVectorAdd(XMLoadFloat3(&position), move));
	SetPosition(newPos, true);
}

// Get the rotated forward axis of this gameobject
XMFLOAT3 GameObject::GetForwardAxis()
{
	return forwardAxis;
}

// Get the rotated right axis of this gameobject
XMFLOAT3 GameObject::GetRightAxis()
{
	return rightAxis;
}

// Get the rotated up axis of this gameobject
XMFLOAT3 GameObject::GetUpAxis()
{
	return upAxis;
}

// Get the quaternion rotation for this entity (Quaternion)
DirectX::XMFLOAT4 GameObject::GetRotation()
{
	return rotation;
}

DirectX::XMFLOAT4 GameObject::GetLocalRotation()
{
	return localRotation;
}

// Set the rotation for this GameObject (Quaternion)
void GameObject::SetRotation(DirectX::XMFLOAT4 newQuatRotation, bool setLocal, 
	bool fromParent, bool fromRigidBody)
{
	worldDirty = true;
	rotation = newQuatRotation;

	CalculateAxis();

	//Update the local position
	if (setLocal)
	{
		if (parent != nullptr)
		{
			XMStoreFloat4(&localRotation,
				XMQuaternionMultiply(XMLoadFloat4(&rotation),
					XMQuaternionInverse(XMLoadFloat4(&parent->GetLocalRotation()))));
		}
		else localRotation = rotation;
	}

	onRotationChanged.Invoke(rotation, fromParent, fromRigidBody);

	//Update transforms of all children
	for (auto c : children)
	{
		c->SetLocalRotation(c->GetLocalRotation(), true);
		c->SetLocalPosition(c->GetLocalPosition(), true);
	}
}

// Set the rotation for this GameObject (Quaternion) from a rigidbody
void GameObject::SetRotationFromRigidBody(DirectX::XMFLOAT4 newQuatRotation)
{
	SetRotation(newQuatRotation, true, false, true);
}

// Set the rotation for this GameObject (Quaternion)
void GameObject::SetRotation(DirectX::XMFLOAT4 newQuatRotation)
{
	SetRotation(newQuatRotation, true);
}

// Set the rotation for this GameObject (Quaternion)
void GameObject::SetRotation(XMFLOAT3 newRotation)
{
	//Convert to quaternions and store
	XMVECTOR angles = XMVectorScale(XMLoadFloat3(&newRotation), XM_PI / 180.0f);
	XMFLOAT4 newRot;
	XMStoreFloat4(&newRot, XMQuaternionRotationRollPitchYawFromVector(angles));
	SetRotation(newRot, true);
}

// Set the rotation for this GameObject using euler angles (Quaternion)
void GameObject::SetRotation(float x, float y, float z)
{
	//Convert to quaternions and store
	XMVECTOR angles = XMVectorScale(XMVectorSet(x, y, z, 0), XM_PI / 180.0f);
	XMFLOAT4 newRot;
	XMStoreFloat4(&newRot, XMQuaternionRotationRollPitchYawFromVector(angles));
	SetRotation(newRot, true);
}

// Set the local rotation for this GameObject (Quaternion)
void GameObject::SetLocalRotation(XMFLOAT4 newLocalQuatRotation, bool fromParent)
{
	if (parent != nullptr)
	{
		localRotation = newLocalQuatRotation;
		XMFLOAT4 newRot;
		XMStoreFloat4(&newRot,
			XMQuaternionMultiply(XMLoadFloat4(&localRotation), (XMLoadFloat4(&parent->GetRotation()))));
		SetRotation(newRot, false, fromParent);
	}
	else this->SetRotation(newLocalQuatRotation, true, fromParent);
}

// Set the local rotation for this GameObject (Quaternion)
void GameObject::SetLocalRotation(XMFLOAT4 newLocalQuatRotation)
{
	this->SetLocalRotation(newLocalQuatRotation,false);
}

// Set the local rotation for this GameObject (Angles)
void GameObject::SetLocalRotation(XMFLOAT3 newLocalRotation)
{
	//Convert to quaternions and store
	XMVECTOR angles = XMVectorScale(XMLoadFloat3(&newLocalRotation), XM_PI / 180.0f);
	XMFLOAT4 newRot;
	XMStoreFloat4(&newRot, XMQuaternionRotationRollPitchYawFromVector(angles));
	SetLocalRotation(newRot, false);
}

// Set the local rotation for this GameObject using angles
void GameObject::SetLocalRotation(float x, float y, float z)
{
	//Convert to quaternions and store
	XMVECTOR angles = XMVectorScale(XMVectorSet(x, y, z, 0), XM_PI / 180.0f);
	XMFLOAT4 newRot;
	XMStoreFloat4(&newRot, XMQuaternionRotationRollPitchYawFromVector(angles));
	SetLocalRotation(newRot, false);
}


// Rotate this GameObject (Angles)
void GameObject::Rotate(DirectX::XMFLOAT3 newRotation)
{
	XMVECTOR angles = XMVectorScale(XMLoadFloat3(&newRotation), XM_PI / 180.0f);
	XMVECTOR quat = XMQuaternionRotationRollPitchYawFromVector(angles);

	XMFLOAT4 rot;
	XMStoreFloat4(&rot, XMQuaternionMultiply(XMLoadFloat4(&rotation), quat));
	SetRotation(rot, true);
}

// Rotate this GameObject using angles
void GameObject::Rotate(float x, float y, float z)
{
	XMVECTOR angles = XMVectorScale(XMVectorSet(x, y, z, 0), XM_PI / 180.0f);
	XMVECTOR quat = XMQuaternionRotationRollPitchYawFromVector(angles);

	XMFLOAT4 rot;
	XMStoreFloat4(&rot, XMQuaternionMultiply(XMLoadFloat4(&rotation), quat));
	SetRotation(rot, true);
}

// Calculate the local axis for the gameobject
void GameObject::CalculateAxis()
{
	//Rotate the forward axis
	XMVECTOR forward = XMVector3Normalize(
		XMVector3Rotate(XMVectorSet(0, 0, 1, 0), XMLoadFloat4(&rotation)));
	XMStoreFloat3(&forwardAxis, forward);

	//Rotate the right axis
	XMVECTOR right = XMVector3Normalize(
		XMVector3Rotate(XMVectorSet(1, 0, 0, 0), XMLoadFloat4(&rotation)));
	XMStoreFloat3(&rightAxis, right);

	//Rotate the up axis
	XMVECTOR up = XMVector3Normalize(
		XMVector3Rotate(XMVectorSet(0, 1, 0, 0), XMLoadFloat4(&rotation)));
	XMStoreFloat3(&upAxis, up);
}

// Get the scale for this GameObject
XMFLOAT3 GameObject::GetScale()
{
	return scale;
}

// Set the scale for this GameObject
void GameObject::SetScale(XMFLOAT3 newScale)
{
	worldDirty = true;

	scale = newScale;
	onScaleChanged.Invoke(scale);

	//Update transforms of all children
	for (auto c : children)
	{
		XMFLOAT3 newScale;
		XMStoreFloat3(&newScale, XMVectorMultiply(XMLoadFloat3(&scale), XMLoadFloat3(&c->GetScale())));
		c->SetScale(newScale);
	}

}

// Set the scale for this GameObject
void GameObject::SetScale(float x, float y, float z)
{
	SetScale(XMFLOAT3(x, y, z));
}