#pragma once

#include "DXCore.h"
#include <DirectXMath.h>
#include "Renderer.h"

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	//Singletons
	Renderer* renderer;

	//3 Demo meshes
	Mesh* meshes[3];
	Entity* entities[5];

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	// Overridden mouse input helper methods
	void OnMouseDown (WPARAM buttonState, int x, int y);
	void OnMouseUp	 (WPARAM buttonState, int x, int y);
	void OnMouseMove (WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta,   int x, int y);
private:
	//Transformation modifiers
	float position;
	float rotation;
	float rotSpeed;
	float scale;

	// Initialization helper methods - feel free to customize, combine, etc.
	void CreateMatrices();
	void CreateBasicGeometry();
	void CreateEntities();

	// The matrices to go from model space to screen space
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;
};

