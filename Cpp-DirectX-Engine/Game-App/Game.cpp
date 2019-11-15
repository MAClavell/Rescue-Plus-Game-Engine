#include "Game.h"
#include <fstream>
#include <sstream>
#include "StringHelper.h"
#include "MAT_PBRTexture.h"
#include "MAT_Skybox.h"
#include "MAT_Basic.h"
#include "DebugMovement.h"
#include "JobSystem.h"

// For the DirectX Math library
using namespace DirectX;
using namespace std;

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
	//Release singletons
	entityManager->Release();
	lightManager->Release();
	physicsManager->Release();
	resourceManager->Release();
	inputManager->Release();
	renderer->Release();

	//Delete sampler states
	samplerState->Release();
	shadowSampler->Release();

	//Release jobs system
	JobSystem::Release();
}

// --------------------------------------------------------
// Read the config file for data
// --------------------------------------------------------
void Game::ReadConfig(string path)
{
	ifstream config(path, ifstream::in);
	
	if (config.fail())
	{
		printf("Could not find or open config file... Assuming defaults and generating file.");
		return;
	}

	//Loop until EoF
	string key;
	while(!getline(config, key, ':').fail())
	{
		//Read token
		key = trim(key);

		//Read arguments
		string arg;
		getline(config, arg);
		arg = trim(arg);

		//Parse key
		if (key == "FPS")
		{
			//Calculate max FPS from file
			float max = 60;
			try
			{
				stringstream argSS(arg);
				argSS >> max;
			}
			catch(exception e)
			{
				printf("Error parsing 'FPS' from config file");
			}
			//If it's we read in less than 0, then set it to 0
			if (max <= 0)
			{
				maxFrameRate = 0;
			}
			else maxFrameRate = 1 / max;
		}

	}

	config.close();
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	ReadConfig("Assets/rescue+.txt");

	//Initialize job system
	JobSystem::Init();

	//Load all needed assets
	resourceManager = ResourceManager::GetInstance();
	LoadAssets();

	//Initialize singletons
	inputManager = InputManager::GetInstance();
	renderer = Renderer::GetInstance();
	renderer->Init(device, width, height);
	entityManager = EntityManager::GetInstance();
	physicsManager = PhysicsManager::GetInstance();

	//Initialize singleton data
	inputManager->Init(hWnd);

	//Create game entities
	CreateEntities();

	//Initialize transformation modifiers
	position = -2;
	rotation = 0;
	rotSpeed = 20;
	scale = 1;

	//Water
	translate = 0.0f;

	//Initialize lights
	//Set ambient light
	lightManager = LightManager::GetInstance();
	lightManager->SetAmbientColor(0.35f, 0.19f, 0.02f);

	//Directional lights
	DirectionalLight* dLight = lightManager->CreateDirectionalLight(true, XMFLOAT3(1, 1, 1), 1);
	dLight->gameObject()->SetRotation(60, -45, 0);

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::LoadAssets()
{
	//Load shaders
	resourceManager->LoadVertexShader("VertexShader.cso", device, context);
	resourceManager->LoadPixelShader("PixelShader.cso", device, context);

	resourceManager->LoadVertexShader("VS_ColDebug.cso", device, context);
	resourceManager->LoadPixelShader("PS_ColDebug.cso", device, context);

	resourceManager->LoadVertexShader("VS_Sky.cso", device, context);
	resourceManager->LoadPixelShader("PS_Sky.cso", device, context);

	resourceManager->LoadVertexShader("VS_Shadow.cso", device, context);

	//Create meshes
	resourceManager->LoadMesh("Assets\\Models\\Basic\\cube.obj", device);
	resourceManager->LoadMesh("Assets\\Models\\Basic\\sphere.obj", device);
	resourceManager->LoadMesh("Assets\\Models\\Basic\\cylinder.obj", device);

	//Load textures
	resourceManager->LoadTexture2D("Assets/Textures/white.png", device, context);
	resourceManager->LoadTexture2D("Assets/Textures/normals_flat.png", device, context);

	//Load cubemaps
	resourceManager->LoadCubeMap("Assets/Textures/Sky/SunnyCubeMap.dds", device);

	//Create sampler state
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC; //Anisotropic filtering
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&samplerDesc, &samplerState);

	// Create the special "comparison" sampler state for shadows
	D3D11_SAMPLER_DESC shadowSampDesc = {};
	shadowSampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR; // Could be anisotropic
	shadowSampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	shadowSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.BorderColor[0] = 1.0f;
	shadowSampDesc.BorderColor[1] = 1.0f;
	shadowSampDesc.BorderColor[2] = 1.0f;
	shadowSampDesc.BorderColor[3] = 1.0f;
	device->CreateSamplerState(&shadowSampDesc, &shadowSampler);

	SimpleVertexShader* vs = resourceManager->GetVertexShader("VertexShader.cso");
	SimplePixelShader* ps_basic = resourceManager->GetPixelShader("PixelShader.cso");

	//Skybox material
	Material* mat_skybox = new MAT_Skybox(ResourceManager::GetInstance()->GetVertexShader("VS_Sky.cso"),
		ResourceManager::GetInstance()->GetPixelShader("PS_Sky.cso"),
		ResourceManager::GetInstance()->GetCubeMap("Assets/Textures/Sky/SunnyCubeMap.dds"),
		samplerState
	);
	resourceManager->AddMaterial("skybox", mat_skybox);

	//White material
	Material* mat_white = new MAT_Basic(vs, ps_basic, XMFLOAT2(1, 1), samplerState,
		resourceManager->GetTexture2D("Assets/Textures/white.png"),
		resourceManager->GetTexture2D("Assets/Textures/normals_flat.png"), 0, 52, shadowSampler
	);
	resourceManager->AddMaterial("white", mat_white);

	//Bouncy PhysicsMaterial
	PhysicsMaterial* pMat_bouncy = new PhysicsMaterial(0.1f, 0.1f, 1.0f);
	resourceManager->AddPhysicsMaterial("bouncy", pMat_bouncy);

	//Solid PhysicsMaterial
	PhysicsMaterial* pMat_solid = new PhysicsMaterial(0.1f, 0.1f, 0.0f);
	resourceManager->AddPhysicsMaterial("solid", pMat_solid);
}

//TODO: REMOVE AFTER TESTING
void OnCol(Collision c)
{
	printf(c.other->gameObject()->GetName().c_str());
}

void Game::CreateEntities()
{
	//Create the camera and initialize matrices
	GameObject* cameraGO = new GameObject();
	camera = cameraGO->AddComponent<Camera>();
	camera->CreateProjectionMatrix(0.25f * XM_PI, (float)width / height, 0.1f, 10000.0f);
	cameraGO->AddComponent<DebugMovement>();

	//Create the floor
	floor = new GameObject("Floor");
	floor->AddComponent<MeshRenderer>(
		resourceManager->GetMesh("Assets\\Models\\Basic\\cube.obj"),
		resourceManager->GetMaterial("white")
	);
	floor->MoveAbsolute(XMFLOAT3(0, -2, 0));
	floor->SetScale(300, 1, 300);
	floor->AddComponent<RigidBody>(0.0f)->SetKinematic(true);
	floor->AddComponent<BoxCollider>(floor->GetScale());

	//Create box1
	GameObject* box1 = new GameObject("Box1");
	box1->AddComponent<MeshRenderer>(
		resourceManager->GetMesh("Assets\\Models\\Basic\\cube.obj"),
		resourceManager->GetMaterial("white")
	);
	box1->MoveAbsolute(XMFLOAT3(0, 0, 8));
	box1->SetScale(2, 2, 2);
	box1->AddComponent<RigidBody>(20.0f);
	box1->AddComponent<BoxCollider>(box1->GetScale(), 
		resourceManager->GetPhysicsMaterial("solid"));

	//Create box2
	GameObject* box2 = new GameObject("Box2");
	box2->AddComponent<MeshRenderer>(
		resourceManager->GetMesh("Assets\\Models\\Basic\\cube.obj"),
		resourceManager->GetMaterial("white")
		);
	box2->MoveAbsolute(XMFLOAT3(0, 2, 8));
	box2->SetScale(1, 1, 1);
	rb = box2->AddComponent<RigidBody>(1.0f);
	box2->AddComponent<BoxCollider>(box2->GetScale(),
		resourceManager->GetPhysicsMaterial("solid"))
		->AddCallbackCollisionEnter(std::function<void(Collision)>(OnCol));

	//Create box3
	GameObject* box3 = new GameObject("Box3");
	box3->AddComponent<MeshRenderer>(
		resourceManager->GetMesh("Assets\\Models\\Basic\\cube.obj"),
		resourceManager->GetMaterial("white")
		);
	box3->SetScale(0.5f, 0.5f, 0.5f);
	box3->SetParent(box2);
	box3->SetLocalPosition(0, 2, 0);
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
// Update your game at a fixed time interval here - physics
// --------------------------------------------------------
void Game::FixedUpdate(float constantStepSize, float totalTime)
{
	//Update physics
	physicsManager->Simulate(constantStepSize);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	inputManager->UpdateFocus();
	if (!inputManager->IsWindowFocused())
		return;

	//The only call to UpdateMousePos() for the InputManager
	//Get the current mouse position
	inputManager->UpdateMousePos();
	// --------------------------------------------------------
	//All game code goes below


	// Quit if the escape key is pressed
	if (inputManager->GetKey(VK_ESCAPE))
		Quit();

	//Update all entities
	entityManager->Update(deltaTime);

	if (inputManager->GetKey('F'))
	{
		rb->AddForce(0, 10, 0);
	}

	if (inputManager->GetKey('G'))
	{
		GameObject* box2 = new GameObject("Box2");
		box2->AddComponent<MeshRenderer>(
			resourceManager->GetMesh("Assets\\Models\\Basic\\cube.obj"),
			resourceManager->GetMaterial("white")
			);
		box2->MoveAbsolute(XMFLOAT3(0, 8, 8));
		box2->SetScale(1, 2, 2);
		box2->AddComponent<RigidBody>(1.0f);
		box2->AddComponent<BoxCollider>(box2->GetScale());
	}


	//All game code goes above
	// --------------------------------------------------------

	//The only call to Update() for the InputManager
	//Update for next frame
	inputManager->UpdateStates();

	//Delete finished jobs
	JobSystem::DeleteFinishedJobs();
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	//Draw all entities in the renderer
	renderer->SetClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Needed for clearing the post process buffer texture and the back buffer.
	renderer->Draw(context, device, camera, backBufferRTV, depthStencilView, samplerState, width, height);

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