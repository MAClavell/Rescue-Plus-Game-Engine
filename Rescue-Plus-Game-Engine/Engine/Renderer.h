#pragma once
#include <vector>
#include <unordered_map>
#include "SimpleShader.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include <CommonStates.h>
#include <Effects.h>
#include <PrimitiveBatch.h>
#include "VertexTypes.h"
#include <wrl/client.h>


// Basis from: https://stackoverflow.com/questions/1008019/c-singleton-design-pattern

// --------------------------------------------------------
// Singleton
//
// Handles rendering entities to the screen
// --------------------------------------------------------
class Renderer
{
public:
	enum class DebugDrawType { None, SingleFrame, ForDuration };

private:
	//Render list management
	//renderMap uses Mat/Mesh identifiers to point to the correct list
	std::unordered_map<std::string, std::vector<MeshRenderer*>> renderMap;
	std::vector<MeshRenderer*> transparentObjList;

	//Debug meshes
	Mesh* cubeMesh;

	//Debugging
	struct DebugDrawData
	{
		DirectX::XMFLOAT4X4 world;
		DebugDrawType type;
		float duration;
		DebugDrawData(DirectX::XMFLOAT4X4 world, DebugDrawType type, float duration)
			: world(world), type(type), duration(duration) {}
	};
	std::vector<DebugDrawData> debugObjs[4]; //0 = cubes, 1 = spheres, 2 = cylinders, 3 = rays 
	std::unique_ptr<DirectX::CommonStates> db_states;
	std::unique_ptr<DirectX::BasicEffect> db_effect;
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> db_batch;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> db_inputLayout;


	//Transparency
	ID3D11BlendState* transparentBlendState;
	ID3D11DepthStencilState* transparentDepthState;

	//Skybox
	Material* skyboxMat;
	ID3D11RasterizerState* skyRasterState;
	ID3D11DepthStencilState* skyDepthState;

	//Shadows
	ID3D11RasterizerState* shadowRasterizer;
	SimpleVertexShader* shadowVS;

	// Clear color.
	float clearColor[4];

	// --------------------------------------------------------
	// Singleton Constructor - Set up the singleton instance of the renderer
	// --------------------------------------------------------
	Renderer() { }

	// --------------------------------------------------------
	// Destructor for when the singleton instance is deleted
	// --------------------------------------------------------
	~Renderer() { };

	// --------------------------------------------------------
	// Render shadow maps for all lights that cast shadows
	// --------------------------------------------------------
	void RenderShadowMaps(ID3D11DeviceContext* context,
		ID3D11Device* device,
		Camera* camera,
		ID3D11RenderTargetView* backBufferRTV,
		ID3D11DepthStencilView* depthStencilView,
		UINT width, UINT height);

	// --------------------------------------------------------
	// Draw opaque objects
	// --------------------------------------------------------
	void DrawOpaqueObjects(ID3D11DeviceContext* context, Camera* camera);

	// --------------------------------------------------------
	// Draw transparent objects
	// --------------------------------------------------------
	void DrawTransparentObjects(ID3D11DeviceContext* context, Camera* camera);



	// --------------------------------------------------------
	// Draw the skybox
	// --------------------------------------------------------
	void DrawSky(ID3D11DeviceContext* context, Camera* camera);

public:

	// --------------------------------------------------------
// Draw debug shapes
// --------------------------------------------------------
	void DrawDebugShapes(ID3D11DeviceContext* context, Camera* camera, float deltaTime);

	// --------------------------------------------------------
	// Get the singleton instance of the renderer
	// --------------------------------------------------------
	static Renderer* GetInstance()
	{
		static Renderer instance;

		return &instance;
	}

	// --------------------------------------------------------
	// Deinitialize values
	// --------------------------------------------------------
	void Release();

	// --------------------------------------------------------
	// Initialize values in the renderer
	// --------------------------------------------------------
	void Init(ID3D11Device* device, ID3D11DeviceContext* context, UINT width, UINT height);

	//Delete this
	Renderer(Renderer const&) = delete;
	void operator=(Renderer const&) = delete;

	// --------------------------------------------------------
	// Draw all entities in the render list
	//
	// context - DirectX device context
	// camera - The active camera object
	// --------------------------------------------------------
	void Draw(ID3D11DeviceContext* context,
			  ID3D11Device* device,
			  Camera* camera,
			  ID3D11RenderTargetView* backBufferRTV,
		      ID3D11DepthStencilView* depthStencilView,
			  ID3D11SamplerState* sampler,
			  UINT width,
		      UINT height,
			  float deltaTIme
	);

	// --------------------------------------------------------
	// Add an entity to the render list
	// --------------------------------------------------------
	void AddMeshRenderer(MeshRenderer* mr);

	// --------------------------------------------------------
	// Remove an entity from the render list
	// --------------------------------------------------------
	void RemoveMeshRenderer(MeshRenderer* mr);

	// --------------------------------------------------------
	// Tell the renderer to render a cube this frame
	//
	// drawType - How long this debug draw should last
	// duration- If the type is 'Duration', amount of seconds it
	//	should last
	// --------------------------------------------------------
	void AddDebugCube(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale,
		DebugDrawType drawType, float duration = 1);
	// --------------------------------------------------------
	// Tell the renderer to render a cube this frame
	//
	// drawType - How long this debug draw should last
	// duration- If the type is 'Duration', amount of seconds it
	//	should last
	// --------------------------------------------------------
	void AddDebugCube(DirectX::XMFLOAT3 position, DirectX::XMFLOAT4 rotation, DirectX::XMFLOAT3 scale,
		DebugDrawType drawType, float duration = 1);
	// --------------------------------------------------------
	// Tell the renderer to render a cube this frame
	//
	// drawType - How long this debug draw should last
	// duration- If the type is 'Duration', amount of seconds it
	//	should last
	// --------------------------------------------------------
	void AddDebugCube(DirectX::XMFLOAT4X4 world, DebugDrawType drawType, float duration = 1);

	// --------------------------------------------------------
	// Tell the renderer to render a sphere this frame
	//
	// drawType - How long this debug draw should last
	// duration- If the type is 'Duration', amount of seconds it
	//	should last
	// --------------------------------------------------------
	void AddDebugSphere(DirectX::XMFLOAT3 position, float radius, DebugDrawType drawType, float duration = 1);
	// --------------------------------------------------------
	// Tell the renderer to render a cube this frame
	//
	// drawType - How long this debug draw should last
	// duration- If the type is 'Duration', amount of seconds it
	//	should last
	// --------------------------------------------------------
	void AddDebugSphere(DirectX::XMFLOAT3 position, DirectX::XMFLOAT4 rotation, float radius,
		DebugDrawType drawType, float duration = 1);
	// --------------------------------------------------------
	// Tell the renderer to render a collider this frame
	//
	// drawType - How long this debug draw should last
	// duration- If the type is 'Duration', amount of seconds it
	//	should last
	// --------------------------------------------------------
	void AddDebugSphere(DirectX::XMFLOAT4X4 world, DebugDrawType drawType, float duration = 1);

	// --------------------------------------------------------
	// Tell the renderer to render a cylinder this frame
	//
	// drawType - How long this debug draw should last
	// duration- If the type is 'Duration', amount of seconds it
	//	should last
	// --------------------------------------------------------
	void AddDebugCylinder(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale,
		DebugDrawType drawType, float duration = 1);
	// --------------------------------------------------------
	// Tell the renderer to render a cylinder this frame
	//
	// drawType - How long this debug draw should last
	// duration- If the type is 'Duration', amount of seconds it
	//	should last
	// --------------------------------------------------------
	void AddDebugCylinder(DirectX::XMFLOAT3 position, DirectX::XMFLOAT4 rotation, DirectX::XMFLOAT3 scale,
		DebugDrawType drawType, float duration = 1);
	// --------------------------------------------------------
	// Tell the renderer to render a cylinder this frame
	//
	// drawType - How long this debug draw should last
	// duration- If the type is 'Duration', amount of seconds it
	//	should last
	// --------------------------------------------------------
	void AddDebugCylinder(DirectX::XMFLOAT4X4 world, 
		DebugDrawType drawType, float duration = 1);

	// --------------------------------------------------------
	// Set clear color.
	// --------------------------------------------------------
	void SetClearColor(const float color[4]);
	void SetClearColor(float r, float g, float b, float a = 1.0);
};