#pragma once

#include <DirectXMath.h>
#include "Mesh.h"

// --------------------------------------------------------
// A entity definition.
//
// Holds data for a entity
// --------------------------------------------------------
class Entity
{
private:
	//Transformations
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 rotation;
	DirectX::XMFLOAT3 scale;
	bool worldDirty;

	Mesh* mesh;
	bool isInRenderList;

public:
	// --------------------------------------------------------
	// Constructor - Set up the entity.
	// Entities are automatically added to the renderlist
	//
	// mesh	- The mesh that this entity displays
	// --------------------------------------------------------
	Entity(Mesh* mesh);

	// --------------------------------------------------------
	// Destructor for when an instance is deleted
	// --------------------------------------------------------
	~Entity();

	// --------------------------------------------------------
	// Get the world matrix for this entity (rebuilding if necessary)
	// --------------------------------------------------------
	DirectX::XMFLOAT4X4 GetWorldMatrix();

	// --------------------------------------------------------
	// Set the world matrix for this entity
	//
	// newWorld - The new world matrix
	// --------------------------------------------------------
	void SetWorldMatrix(DirectX::XMFLOAT4X4 newWorld);

	// --------------------------------------------------------
	// Rebuild the world matrix from the different components
	// --------------------------------------------------------
	void RebuildWorld();
	
	// --------------------------------------------------------
	// Get the position for this entity
	// --------------------------------------------------------
	DirectX::XMFLOAT3 GetPosition();

	// --------------------------------------------------------
	// Set the position for this entity
	//
	// newPosition - The new position to translate to
	// --------------------------------------------------------
	void SetPosition(DirectX::XMFLOAT3 newPosition);

	// --------------------------------------------------------
	// Set the position for this entity
	//
	// x - new x position
	// y - new y position
	// z - new z position
	// --------------------------------------------------------
	void SetPosition(float x, float y, float z);

	// --------------------------------------------------------
	// Moves this entity in absolute space by a given vector.
	// Does not take rotation into account
	//
	// moveTo - The amount to move for each axis
	// --------------------------------------------------------
	void MoveAbsolute(DirectX::XMFLOAT3 moveAmnt);

	// --------------------------------------------------------
	// Moves this entity in relative space by a given vector.
	// Does take rotation into account
	//
	// moveTo - The amount to move for each axis
	// --------------------------------------------------------
	void MoveRelative(DirectX::XMFLOAT3 moveAmnt);

	// --------------------------------------------------------
	// Get the rotation for this entity (Quaternion)
	// --------------------------------------------------------
	DirectX::XMFLOAT4 GetRotation();

	// --------------------------------------------------------
	// Set the rotation for this entity (Quaternion)
	//
	// newRotation - The new rotation to rotate to
	// --------------------------------------------------------
	void SetRotation(DirectX::XMFLOAT4 newRotation);

	// --------------------------------------------------------
	// Set the rotation for this entity using euler angles (Quaternion)
	//
	// angleVector - The new rotation to rotate to (in degrees)
	// --------------------------------------------------------
	void EulerAngles(DirectX::XMFLOAT3 angleVector);

	// --------------------------------------------------------
	// Set the rotation for this entity using euler angles (Quaternion)
	//
	// x - x angle
	// y - y angle
	// z - z angle
	// --------------------------------------------------------
	void EulerAngles(float x, float y, float z);

	// --------------------------------------------------------
	// Get the scale for this entity
	// --------------------------------------------------------
	DirectX::XMFLOAT3 GetScale();

	// --------------------------------------------------------
	// Set the scale for this entity
	//
	// newScale - The new scale to resize to
	// --------------------------------------------------------
	void SetScale(DirectX::XMFLOAT3 newScale);

	// --------------------------------------------------------
	// Set the scale for this entity
	//
	// x - new x scale
	// y - new y scale
	// z - new z scale
	// --------------------------------------------------------
	void SetScale(float x, float y, float z);

	// --------------------------------------------------------
	// Get the mesh this entity uses
	// --------------------------------------------------------
	Mesh* GetMesh();

	// --------------------------------------------------------
	// Get the vertex buffer the entity's mesh uses
	// --------------------------------------------------------
	ID3D11Buffer* GetVertexBuffer();

	// --------------------------------------------------------
	// Get the index buffer the entity's mesh uses
	// --------------------------------------------------------
	ID3D11Buffer* GetIndexBuffer();

	// --------------------------------------------------------
	// Get the number of indicies this entity's mesh uses
	// --------------------------------------------------------
	int GetIndexCount();

	// --------------------------------------------------------
	// Add this entity to the render list
	// --------------------------------------------------------
	void AddThisEntityToRenderList();

	// --------------------------------------------------------
	// Remove this entity from the render list
	// --------------------------------------------------------
	void RemoveThisEntityFromRenderList();
	
	// --------------------------------------------------------
	// Check if this entity is in the render list
	// --------------------------------------------------------
	bool IsInRenderList();
};

