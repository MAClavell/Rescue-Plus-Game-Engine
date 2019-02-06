#pragma once

#include "DXCore.h"
#include <DirectXMath.h>
#include "Renderer.h"
#include "InputManager.h"
#include "Camera.h"

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	//Singletons
	Renderer* renderer;
	InputManager* inputManager;
	
	//3 Demo meshes
	Mesh* meshes[3];
	Entity* entities[5];

	//Camera
	Camera* camera;

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	// Overridden mouse input helper methods
	void OnMouseDown (WPARAM buttonState, int x, int y);
	void OnMouseUp	 (WPARAM buttonState, int x, int y, int button);
	void OnMouseMove (WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta,   int x, int y);
private:
	//Transformation modifiers
	float position;
	float rotation;
	float rotSpeed;
	float scale;

	// Initialization helper methods - feel free to customize, combine, etc.
	void CreateBasicGeometry();
	void CreateEntities();
};

