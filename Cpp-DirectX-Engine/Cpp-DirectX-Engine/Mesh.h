#pragma once

#include <d3d11.h>
#include "Vertex.h"

// --------------------------------------------------------
// A custom mesh definition.
//
// Holds data for a mesh
// --------------------------------------------------------
class Mesh
{
private:
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	int indexCount;

public:
	// --------------------------------------------------------
	// Constructor - Set up fields and buffers
	//
	// vertices	- The array of vertices this mesh uses
	// vertexCount - The number of vertices in this mesh
	// indices - The array of indices this mesh uses
	// indexCount - The number of indices in this mesh
	// device - The ID3D11Device for this mesh
	// --------------------------------------------------------
	Mesh(Vertex* vertices, int vertexCount, unsigned* indices, int indexCount, ID3D11Device* device);\
	// --------------------------------------------------------
	// Destructor for when an instance is deleted
	// --------------------------------------------------------
	~Mesh();
	// --------------------------------------------------------
	// Release all memory used by this mesh
	// --------------------------------------------------------
	void Release();

	// --------------------------------------------------------
	// Get the vertex buffer this mesh uses
	// --------------------------------------------------------
	ID3D11Buffer* GetVertexBuffer();

	// --------------------------------------------------------
	// Get the index buffer this mesh uses
	// --------------------------------------------------------
	ID3D11Buffer* GetIndexBuffer();

	// --------------------------------------------------------
	// Get the number of indicies in this mesh
	// --------------------------------------------------------
	int GetIndexCount();
};

