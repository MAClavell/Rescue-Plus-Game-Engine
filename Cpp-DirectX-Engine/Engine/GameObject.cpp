#include "GameObject.h"
#include "Renderer.h"

// For the DirectX Math library
using namespace DirectX;

//Construct a component
Component::Component(GameObject* gameObject)
{
	this->gameObject = gameObject;
}

// Constructor - Set up the gameobject.
GameObject::GameObject()
{
	//Set default transformation values
	parent = nullptr;
	world = XMFLOAT4X4();
	position = XMFLOAT3(0, 0, 0);
	SetRotation(0, 0, 0);
	scale = XMFLOAT3(1, 1, 1);
	worldDirty = false;
	RebuildWorld();

	enabled = true;
	name = "GameObject";
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
	//Delete all children
	for (auto go : children)
	{
		delete go;
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

// --------------------------------------------------------
// Add a child to a gameobject
// --------------------------------------------------------
void GameObject::AddChild(GameObject* child)
{
	children.push_back(child);
}

// Add a component of a specific type (must derive from component)
template <typename T, typename... Args>
T GameObject::AddComponent(T type, Args... args)
{
	static_assert(std::is_base_of<Component, T>::value, "Can't add a component not derived from Component");

	//Push new T
	components.push_back(new T(args...);
}

// Remove a component of a specific type (must derive from component
//		and be in the gameobject's component list)
template <typename T>
T GameObject::RemoveComponent(T type)
{
	static_assert(std::is_base_of<Component, T>::value, "Can't remove a component not derived from Component");

	//Try to find it
	bool found = false;
	for (auto iter = components.begin(); iter != components.end(); iter++) 
	{
		try
		{
			T& c = dynamic_cast<T&>(**iter); // try to cast
			found = true;

			//Swap with the last and pop
			std::iter_swap(iter, components.end());
			components.pop_back();

			//Delete
			delete c;
			break;
		}
		catch (...)
		{ }
	}

	if(!found)
		printf("Could not find a component of type '%s' in %s", typeid(T).name(), name.c_str());
}

// Update this entity
void GameObject::Update(float deltaTime)
{ 
	for (auto c : components)
	{
		c->Update(deltaTime);
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
	XMMATRIX rotating = XMMatrixRotationQuaternion(XMLoadFloat4(&rotationQuat));

	//Get scale matrix
	XMMATRIX scaling = XMMatrixScalingFromVector(XMLoadFloat3(&scale));

	//Calculate matrix and store
	XMMATRIX newWorld = XMMatrixTranspose(scaling * rotating * translation);
	XMStoreFloat4x4(&world, newWorld);

	//Calculate inverse transpose
	XMStoreFloat4x4(&worldInvTrans, XMMatrixInverse(&XMVectorSet(-1, -1, -1, -1), XMMatrixTranspose(newWorld)));

	worldDirty = false;
}

// Update transformations after parent's transformations changed
void GameObject::ParentPositionChanged()
{
	XMFLOAT3 newPos;
	XMStoreFloat3(&newPos, XMVectorAdd(XMLoadFloat3(&parent->position), XMLoadFloat3(&position)));
	SetPosition(newPos);
}

// Update transformations after parent's transformations changed
void GameObject::ParentRotationChanged()
{
	XMFLOAT4 newRot;
	XMStoreFloat4(&newRot, XMQuaternionMultiply(XMLoadFloat4(&parent->rotationQuat), XMLoadFloat4(&rotationQuat)));
	SetRotation(newRot);
}

// Update transformations after parent's transformations changed
void GameObject::ParentScaleChanged()
{
	XMFLOAT3 newScale;
	XMStoreFloat3(&newScale, XMVectorMultiply(XMLoadFloat3(&parent->scale), XMLoadFloat3(&scale)));
	SetScale(newScale);
}

// Get the position for this GameObject
XMFLOAT3 GameObject::GetPosition()
{
	return position;
}

// Set the position for this GameObject
void GameObject::SetPosition(XMFLOAT3 newPosition)
{
	worldDirty = true;
	position = newPosition;

	//Update transforms of all children
	for (auto c : children)
	{
		c->ParentPositionChanged();
	}
}

// Set the position for this GameObject
void GameObject::SetPosition(float x, float y, float z)
{
	SetPosition(XMFLOAT3(x, y, z));
}

// Moves this GameObject in absolute space by a given vector.
// Does not take rotation into account
void GameObject::MoveAbsolute(XMFLOAT3 moveAmnt)
{
	//Add the vector to the position
	XMFLOAT3 newPos;
	XMStoreFloat3(&newPos, XMVectorAdd(XMLoadFloat3(&position),
		XMLoadFloat3(&moveAmnt)));
	SetRotation(newPos);
}

// Moves this GameObject in relative space by a given vector.
// Does take rotation into account
void GameObject::MoveRelative(XMFLOAT3 moveAmnt)
{
	// Rotate the movement vector
	XMVECTOR move = XMVector3Rotate(XMLoadFloat3(&moveAmnt),
		XMLoadFloat4(&rotationQuat));

	//Add to position
	XMFLOAT3 newPos;
	XMStoreFloat3(&newPos, XMVectorAdd(XMLoadFloat3(&position), move));
	SetPosition(newPos);
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
	return rotationQuat;
}

// Set the rotation for this GameObject (Quaternion)
void GameObject::SetRotation(DirectX::XMFLOAT4 newQuatRotation)
{
	worldDirty = true;
	rotationQuat = newQuatRotation;

	CalculateAxis();

	//Update transforms of all children
	for (auto c : children)
	{
		c->ParentRotationChanged();
	}
}

// Set the rotation for this GameObject (Quaternion)
void GameObject::SetRotation(XMFLOAT3 newRotation)
{
	//Convert to quaternions and store
	XMVECTOR angles = XMVectorScale(XMLoadFloat3(&newRotation), XM_PI / 180.0f);
	XMFLOAT4 newRot;
	XMStoreFloat4(&newRot, XMQuaternionRotationRollPitchYawFromVector(angles));
	SetRotation(newRot);
}

// Set the rotation for this GameObject using euler angles (Quaternion)
void GameObject::SetRotation(float x, float y, float z)
{
	//Convert to quaternions and store
	XMVECTOR angles = XMVectorScale(XMVectorSet(x, y, z, 0), XM_PI / 180.0f);
	XMFLOAT4 newRot;
	XMStoreFloat4(&newRot, XMQuaternionRotationRollPitchYawFromVector(angles));
	SetRotation(newRot);
}

// Rotate this GameObject (Angles)
void GameObject::Rotate(DirectX::XMFLOAT3 newRotation)
{
	XMVECTOR angles = XMVectorScale(XMLoadFloat3(&newRotation), XM_PI / 180.0f);
	XMVECTOR quat = XMQuaternionRotationRollPitchYawFromVector(angles);

	XMFLOAT4 rot;
	XMStoreFloat4(&rot, XMQuaternionMultiply(XMLoadFloat4(&rotationQuat), quat));
	SetRotation(rot);
}

// Rotate this GameObject using angles
void GameObject::Rotate(float x, float y, float z)
{
	XMVECTOR angles = XMVectorScale(XMVectorSet(x, y, z, 0), XM_PI / 180.0f);
	XMVECTOR quat = XMQuaternionRotationRollPitchYawFromVector(angles);

	XMFLOAT4 rot;
	XMStoreFloat4(&rot, XMQuaternionMultiply(XMLoadFloat4(&rotationQuat), quat));
	SetRotation(rot);
}

// Calculate the local axis for the gameobject
void GameObject::CalculateAxis()
{
	//Rotate the forward axis
	XMVECTOR forward = XMVector3Normalize(
		XMVector3Rotate(XMVectorSet(0, 0, 1, 0), XMLoadFloat4(&rotationQuat)));
	XMStoreFloat3(&forwardAxis, forward);

	//Rotate the right axis
	XMVECTOR right = XMVector3Normalize(
		XMVector3Rotate(XMVectorSet(1, 0, 0, 0), XMLoadFloat4(&rotationQuat)));
	XMStoreFloat3(&rightAxis, right);

	//Rotate the up axis
	XMVECTOR up = XMVector3Normalize(
		XMVector3Rotate(XMVectorSet(0, 1, 0, 0), XMLoadFloat4(&rotationQuat)));
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

	//Update transforms of all children
	for (auto c : children)
	{
		c->ParentRotationChanged();
	}
}

// Set the scale for this GameObject
void GameObject::SetScale(float x, float y, float z)
{
	SetScale(XMFLOAT3(x, y, z));
}