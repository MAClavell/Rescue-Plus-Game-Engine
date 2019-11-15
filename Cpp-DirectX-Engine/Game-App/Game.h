#pragma once

#include "DXCore.h"
#include <DirectXMath.h>
#include "Renderer.h"
#include "InputManager.h"
#include "EntityManager.h"
#include "ResourceManager.h"
#include "PhysicsManager.h"
#include "LightManager.h"

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Config file reading
	void ReadConfig(std::string path);

	//Camera
	Camera* camera;
	GameObject* floor;

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void FixedUpdate(float constantStepSize, float totalTime);
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	// Overridden mouse input helper methods
	void OnMouseDown (WPARAM buttonState, int x, int y);
	void OnMouseUp	 (WPARAM buttonState, int x, int y, int button);
	void OnMouseMove (WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta,   int x, int y);

private:
	//Singletons
	Renderer* renderer;
	InputManager* inputManager;
	ResourceManager* resourceManager;
	EntityManager* entityManager;
	PhysicsManager* physicsManager;
	LightManager* lightManager;

	//Sampler states
	ID3D11SamplerState* samplerState;
	ID3D11SamplerState* shadowSampler;

	RigidBody* rb;
	GameObject* child;

	//Transformation modifiers
	float position;
	float rotation;
	float rotSpeed;
	float scale;

	//Water
	float translate; //used to scroll the water's normal map

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadAssets();
	void CreateEntities();
};

