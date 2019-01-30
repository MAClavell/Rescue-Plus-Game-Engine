#pragma once
#include <vector>
#include "SimpleShader.h"
#include "Entity.h"

// Basis from: https://stackoverflow.com/questions/1008019/c-singleton-design-pattern

// --------------------------------------------------------
// Singleton
//
// Handles rendering entities to the screen
// --------------------------------------------------------
class Renderer
{
private:
	std::vector<Entity*> renderList;
	
	// Wrappers for DirectX shaders to provide simplified functionality
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;

	// --------------------------------------------------------
	// Singleton Constructor - Set up the singleton instance of the renderer
	// --------------------------------------------------------
	Renderer() { Init(); }

	// --------------------------------------------------------
	// Destructor for when the singleton instance is deleted
	// --------------------------------------------------------
	~Renderer();

	// --------------------------------------------------------
	// Initialize values in the renderer
	// --------------------------------------------------------
	void Init();

public:
	// --------------------------------------------------------
	// Get the singleton instance of the renderer
	// --------------------------------------------------------
	static Renderer* GetInstance()
	{
		static Renderer instance;

		return &instance;
	}

	//Delete this
	Renderer(Renderer const&) = delete;
	void operator=(Renderer const&) = delete;

	// --------------------------------------------------------
	// Loads shaders from compiled shader object (.cso) files using
	// my SimpleShader wrapper for DirectX shader manipulation.
	// - SimpleShader provides helpful methods for sending
	//   data to individual variables on the GPU
	// --------------------------------------------------------
	void LoadShaders(ID3D11Device* device, ID3D11DeviceContext* context);

	// --------------------------------------------------------
	// Draw all entities in the render list
	//
	// context - DirectX device context
	// viewMatrix - the camera's view
	// projectionMatrix - the camera's projection matrix
	// --------------------------------------------------------
	void Draw(ID3D11DeviceContext* context, DirectX::XMFLOAT4X4 viewMatrix, DirectX::XMFLOAT4X4 projectionMatrix);

	// --------------------------------------------------------
	// Check if an entity is in the render list
	// --------------------------------------------------------
	bool IsEntityInRenderList(Entity* e);

	// --------------------------------------------------------
	// Add an entity to the render list
	// --------------------------------------------------------
	void AddEntityToRenderList(Entity* e);

	// --------------------------------------------------------
	// Remove an entity from the render list
	// --------------------------------------------------------
	void RemoveEntityFromRenderList(Entity* e);
};