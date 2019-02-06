#include "Renderer.h"

// For the DirectX Math library
using namespace DirectX;

// Constructor - Set up the entity
Entity::Entity(Mesh* mesh)
{
	this->mesh = mesh;
	worldDirty = false;

	//Set default transformation values
	world = XMFLOAT4X4();
	position = XMFLOAT3(0, 0, 0);
	EulerAngles(0, 0, 0);
	scale = XMFLOAT3(1, 1, 1);
	RebuildWorld();

	Renderer::GetInstance()->AddEntityToRenderList(this);
}

// Destructor for when an instance is deleted
Entity::~Entity()
{ 
	Renderer::GetInstance()->RemoveEntityFromRenderList(this);
}

// Get the world matrix for this entity (rebuilding if necessary)
XMFLOAT4X4 Entity::GetWorldMatrix()
{
	//Rebuild the world if it is not current
	if (worldDirty)
		RebuildWorld();

	return world;
}

// Set the world matrix for this entity
void Entity::SetWorldMatrix(XMFLOAT4X4 newWorld)
{
	world = newWorld;
}

// Rebuild the world matrix from the different components
void Entity::RebuildWorld()
{
	//Get translation matrix
	XMMATRIX translation = XMMatrixTranslationFromVector(XMLoadFloat3(&position));

	//Get rotation matrix
	XMMATRIX rotating = XMMatrixRotationQuaternion(XMLoadFloat4(&rotation));

	//Get scale matrix
	XMMATRIX scaling = XMMatrixScalingFromVector(XMLoadFloat3(&scale));

	//Calculate matrix and store
	XMStoreFloat4x4(&world, XMMatrixTranspose(scaling * rotating * translation));
}

// Get the position for this entity
XMFLOAT3 Entity::GetPosition()
{
	return position;
}

// Set the position for this entity
void Entity::SetPosition(XMFLOAT3 newPosition)
{
	worldDirty = true;
	position = newPosition;
}

// Set the position for this entity
void Entity::SetPosition(float x, float y, float z)
{
	worldDirty = true;

	//Set values
	position.x = x;
	position.y = y;
	position.z = z;
}

// Moves this entity in absolute space by a given vector.
// Does not take rotation into account
void Entity::MoveAbsolute(XMFLOAT3 moveAmnt)
{
	worldDirty = true;
	
	//Add the vector to the position
	XMStoreFloat3(&position, XMVectorAdd(XMLoadFloat3(&position),
											XMLoadFloat3(&moveAmnt)));
}

// Moves this entity in relative space by a given vector.
// Does take rotation into account
void Entity::MoveRelative(XMFLOAT3 moveAmnt)
{
	worldDirty = true;

	// Rotate the movement vector
	XMVECTOR move = XMVector3Rotate(XMLoadFloat3(&moveAmnt), 
									XMLoadFloat4(&rotation));

	//Add to position and
	XMStoreFloat3(&position, XMVectorAdd(XMLoadFloat3(&position), move));
}

// Get the rotation for this entity (Quaternion)
XMFLOAT4 Entity::GetRotation()
{
	return rotation;
}

// Set the rotation for this entity (Quaternion)
void Entity::SetRotation(XMFLOAT4 newRotation)
{
	worldDirty = true;
	rotation = newRotation;
}

// Set the rotation for this entity using euler angles (Quaternion)
void Entity::EulerAngles(DirectX::XMFLOAT3 angleVector)
{
	worldDirty = true;
	
	//Convert to degrees
	XMVECTOR angles = XMVectorScale(XMLoadFloat3(&angleVector), XM_PI / 180.0f);

	//Convert from angles to a quaternion
	XMStoreFloat4(&rotation, XMQuaternionRotationRollPitchYawFromVector(angles));
}

// Set the rotation for this entity using euler angles (Quaternion)
void Entity::EulerAngles(float x, float y, float z)
{
	worldDirty = true;

	//Convert to degrees
	XMVECTOR angles = XMVectorScale(XMVectorSet(x, y, z, 0.0f), XM_PI / 180.0f);

	//Convert from angles to a quaternion
	XMStoreFloat4(&rotation, XMQuaternionRotationRollPitchYawFromVector(angles));
}

// Get the scale for this entity
XMFLOAT3 Entity::GetScale()
{
	return scale;
}

// Set the scale for this entity
void Entity::SetScale(XMFLOAT3 newScale)
{
	worldDirty = true;
	scale = newScale;
}

// Set the scale for this entity
void Entity::SetScale(float x, float y, float z)
{
	worldDirty = true;

	//Set values
	scale.x = x;
	scale.y = y;
	scale.z = z;
}

// Get the mesh this entity uses
Mesh* Entity::GetMesh()
{
	return mesh;
}

// Get the vertex buffer the entity's mesh uses
ID3D11Buffer* Entity::GetVertexBuffer()
{
	return mesh->GetVertexBuffer();
}

// Get the index buffer the entity's mesh uses
ID3D11Buffer* Entity::GetIndexBuffer()
{
	return mesh->GetIndexBuffer();
}

// Get the number of indicies this entity's mesh uses
int Entity::GetIndexCount()
{
	return mesh->GetIndexCount();
}

// Add this entity to the render list
void Entity::AddThisEntityToRenderList()
{
	Renderer::GetInstance()->AddEntityToRenderList(this);
}

// Remove this entity from the render list
void Entity::RemoveThisEntityFromRenderList()
{
	Renderer::GetInstance()->RemoveEntityFromRenderList(this);
}