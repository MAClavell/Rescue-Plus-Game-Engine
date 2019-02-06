#include "Game.h"
#include "Vertex.h"

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		// The application's handle
		"DirectX Game",	   	// Text for the window's title bar
		1280,			// Width of the window's client area
		720,			// Height of the window's client area
		true)			// Show extra stats (fps) in title bar?
{

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif
	
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	//Delete demo meshes
	for (int i = 0; i < 3; i++)
	{
		if (meshes[i]) { delete meshes[i]; }
	}

	//Delete demo entities
	for (int i = 0; i < 5; i++)
	{
		if (entities[i]) { delete entities[i]; }
	}

	//Delete the camera
	if (camera) { delete camera; }
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	//Initialize singletons
	inputManager = InputManager::GetInstance();
	renderer = Renderer::GetInstance();

	//Initialize singleton data
	inputManager->Init(hWnd);
	renderer->LoadShaders(device, context);

	//Create the camera and initialize matrices
	camera = new Camera();
	camera->CreateProjectionMatrix(0.25f * XM_PI, (float)width / height, 0.1f, 100.0f);
	camera->SetPosition(0, 0, -5);

	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	CreateBasicGeometry();
	CreateEntities();

	//Initialize transformation modifiers
	position = -2;
	rotation = 0;
	rotSpeed = 20;
	scale = 1;

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	// Set up the vertices of the triangle we would like to draw
	// - We're going to copy this array, exactly as it exists in memory
	//    over to a DirectX-controlled data structure (the vertex buffer)
	Vertex vertices0[] =
	{ 
		{ XMFLOAT3(+0.0f, +1.0f, +0.0f), red },
		{ XMFLOAT3(+1.5f, -1.0f, +0.0f), blue },
		{ XMFLOAT3(-1.5f, -1.0f, +0.0f), green }
	};
	Vertex vertices1[] =
	{
		{ XMFLOAT3(-1.0f, +1.0f, +0.0f), green },
		{ XMFLOAT3(+1.0f, +1.0f, +0.0f), blue },
		{ XMFLOAT3(+1.0f, -1.0f, +0.0f), green },
		{ XMFLOAT3(-1.0f, -1.0f, +0.0f), red }
	};
	Vertex vertices2[] =
	{
		{ XMFLOAT3(-0.0f, +0.0f, +0.0f), green },
		{ XMFLOAT3(-0.0f, +1.0f, +0.0f), green },
		{ XMFLOAT3(+1.0f, +0.2f, +0.0f), blue },
		{ XMFLOAT3(+0.75f, -1.0f, +0.0f), green },
		{ XMFLOAT3(-0.75f, -1.0f, +0.0f), red },
		{ XMFLOAT3(-1.0f, +0.2f, +0.0f), red }
	};

	// Set up the indices, which tell us which vertices to use and in which order
	unsigned indices0[] = { 0, 1, 2 }; //triangle
	unsigned indices1[] = { 0, 1, 2, 0, 2, 3 }; //square
	unsigned indices2[] = { 0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 5, 0, 5, 1 }; //pentagon

	//Create meshes
	meshes[0] = new Mesh(vertices0, 3, indices0, 3, device);
	meshes[1] = new Mesh(vertices1, 4, indices1, 6, device);
	meshes[2] = new Mesh(vertices2, 6, indices2, 15, device);
}

void Game::CreateEntities()
{
	//Triangle at origin
	entities[0] = new Entity(meshes[0]);
	
	//Square offset by (2, 1, 0)
	entities[1] = new Entity(meshes[1]);
	entities[1]->SetPosition(2, 1, 0);

	//Pentagon offset by (-2, 1, 0)
	entities[2] = new Entity(meshes[2]);
	entities[2]->SetPosition(-2, 1, 0);
	entities[2]->SetScale(0.75f, 0.75f, 0.75f);

	//Moving triangle
	entities[3] = new Entity(meshes[0]);
	entities[3]->SetPosition(position, -1, 0);
	entities[3]->SetScale(0.5f, 0.5f, 0.5f);

	//Upside down triangle
	entities[4] = new Entity(meshes[0]);
	entities[4]->SetPosition(0, 1.70f, 0);
	entities[4]->EulerAngles(0, 0, 180);
	entities[4]->SetScale(0.25f, 0.25f, 0.25f);
}

// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	// Update our projection matrix since the window size changed
	camera->CreateProjectionMatrix(
		0.25f * XM_PI,			// Field of View Angle
		(float)width / height,	// Aspect ratio
		0.1f,				  	// Near clip plane distance
		100.0f);			  	// Far clip plane distance
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	//The only call to UpdateMousePosition() for the InputManager
	//Get the current mouse position
	inputManager->UpdateMousePos();

	// Quit if the escape key is pressed
	if (inputManager->GetKey(VK_ESCAPE))
		Quit();

	//Update the camera
	camera->Update(deltaTime);

	//Move position around
	position = sin(totalTime / 2) * 2.5f;
	entities[3]->SetPosition(position, -1, 0);

	//Rotate
	rotation += rotSpeed * deltaTime;
	entities[2]->EulerAngles(0, 0, rotation);

	//Scale
	scale = (sin(totalTime / 2) + 1) / 2;
	entities[1]->SetScale(scale, scale, scale);

	//The only call to Update() for the InputManager
	//Update for next frame
	inputManager->UpdateStates();
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	//Draw all entities in the renderer
	renderer->Draw(context, camera);

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);
}


#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void Game::OnMouseDown(WPARAM buttonState, int x, int y)
{
	inputManager->OnMouseDown(buttonState, x, y);
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void Game::OnMouseUp(WPARAM buttonState, int x, int y, int button)
{
	inputManager->OnMouseUp(buttonState, x, y, button);
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void Game::OnMouseMove(WPARAM buttonState, int x, int y)
{
	inputManager->OnMouseMove(buttonState, x, y);
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	inputManager->OnMouseWheel(wheelDelta, x, y);
}
#pragma endregion