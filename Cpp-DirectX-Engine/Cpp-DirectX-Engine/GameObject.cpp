#include "GameObject.h"

// For the DirectX Math library
using namespace DirectX;

// Constructor - Set up the gameobject.
GameObject::GameObject()
{
	//Set default transformation values
	world = XMFLOAT4X4();
	position = XMFLOAT3(0, 0, 0);
	SetRotation(0, 0, 0);
	scale = XMFLOAT3(1, 1, 1);
	worldDirty = false;
	RebuildWorld();
}

// Destructor for when an instance is deleted
GameObject::~GameObject()
{
}

// Get the world matrix for this GameObject (rebuilding if necessary)
XMFLOAT4X4 GameObject::GetWorldMatrix()
{
	//Rebuild the world if it is not current
	if (worldDirty)
		RebuildWorld();

	return world;
}

// Set the world matrix for this GameObject
void GameObject::SetWorldMatrix(XMFLOAT4X4 newWorld)
{
	world = newWorld;
}

// Rebuild the world matrix from the different components
void GameObject::RebuildWorld()
{
	//Get translation matrix
	XMMATRIX translation = XMMatrixTranslationFromVector(XMLoadFloat3(&position));

	//Get rotation matrix

	//Get rotation matrix
	XMMATRIX rotating = XMMatrixRotationQuaternion(XMLoadFloat4(&rotationQuat));

	//Get scale matrix
	XMMATRIX scaling = XMMatrixScalingFromVector(XMLoadFloat3(&scale));

	//Calculate matrix and store
	XMStoreFloat4x4(&world, XMMatrixTranspose(scaling * rotating * translation));
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
}

// Set the position for this GameObject
void GameObject::SetPosition(float x, float y, float z)
{
	worldDirty = true;

	//Set values
	position.x = x;
	position.y = y;
	position.z = z;
}

// Moves this GameObject in absolute space by a given vector.
// Does not take rotation into account
void GameObject::MoveAbsolute(XMFLOAT3 moveAmnt)
{
	worldDirty = true;

	//Add the vector to the position
	XMStoreFloat3(&position, XMVectorAdd(XMLoadFloat3(&position),
		XMLoadFloat3(&moveAmnt)));
}

// Moves this GameObject in relative space by a given vector.
// Does take rotation into account
void GameObject::MoveRelative(XMFLOAT3 moveAmnt)
{
	worldDirty = true;

	// Rotate the movement vector
	XMVECTOR move = XMVector3Rotate(XMLoadFloat3(&moveAmnt),
		XMLoadFloat4(&rotationQuat));

	//Add to position and
	XMStoreFloat3(&position, XMVectorAdd(XMLoadFloat3(&position), move));
}

// Get the rotation for this GameObject (Quaternion)
XMFLOAT3 GameObject::GetRotation()
{
	return rotation;
}

// Get the quaternion rotation for this entity (Quaternion)
DirectX::XMFLOAT4 GameObject::GetQuatRotation()
{
	return rotationQuat;
}

// Set the rotation for this GameObject (Quaternion)
void GameObject::SetRotation(XMFLOAT3 newRotation)
{
	worldDirty = true;
	rotation = newRotation;

	//Convert to quaternions and store
	XMVECTOR angles = XMVectorScale(XMLoadFloat3(&rotation), XM_PI / 180.0f);
	XMStoreFloat4(&rotationQuat, XMQuaternionRotationRollPitchYawFromVector(angles));
}

// Set the rotation for this GameObject using euler angles (Quaternion)
void GameObject::SetRotation(float x, float y, float z)
{
	worldDirty = true;

	//Convert to degrees
	rotation.x = x;
	rotation.y = y;
	rotation.z = z;

	//Convert to quaternions and store
	XMVECTOR angles = XMVectorScale(XMLoadFloat3(&rotation), XM_PI / 180.0f);
	XMStoreFloat4(&rotationQuat, XMQuaternionRotationRollPitchYawFromVector(angles));
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
}

// Set the scale for this GameObject
void GameObject::SetScale(float x, float y, float z)
{
	worldDirty = true;

	//Set values
	scale.x = x;
	scale.y = y;
	scale.z = z;
}