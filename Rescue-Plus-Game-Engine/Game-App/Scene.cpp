#include "Game.h"
#include "DebugMovement.h"
#include "TestCallbacks.h"
#include "MAT_Skybox.h"
#include "MAT_Basic.h"
#include "TestBullet.h"

using namespace DirectX;

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
	resourceManager->LoadMesh("Assets\\Models\\Shipyard\\shipyard_container.obj", device);

	//Load textures
	resourceManager->LoadTexture2D("Assets/Textures/white.png", device, context);
	resourceManager->LoadTexture2D("Assets/Textures/normals_flat.png", device, context);
	resourceManager->LoadTexture2D("Assets/Textures/Shipyard/shipyard_container_diffuse.jpeg", device, context);
	resourceManager->LoadTexture2D("Assets/Textures/Shipyard/shipyard_concrete_diffuse.jpg", device, context);
	resourceManager->LoadTexture2D("Assets/Textures/Shipyard/shipyard_concrete_normal.jpg", device, context);


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
		resourceManager->GetTexture2D("Assets/Textures/normals_flat.png"), 0, 52
	);
	resourceManager->AddMaterial("white", mat_white);

	//Shipyard container material
	Material* mat_cont = new MAT_Basic(vs, ps_basic, XMFLOAT2(1, 1), samplerState,
		resourceManager->GetTexture2D("Assets/Textures/Shipyard/shipyard_container_diffuse.jpeg"),
		resourceManager->GetTexture2D("Assets/Textures/normals_flat.png"), 0.6f, 10
	);
	resourceManager->AddMaterial("shipyard_container", mat_cont);

	//Shipyard concrete material
	Material* mat_conc = new MAT_Basic(vs, ps_basic, XMFLOAT2(10, 10), samplerState,
		resourceManager->GetTexture2D("Assets/Textures/Shipyard/shipyard_concrete_diffuse.jpg"),
		resourceManager->GetTexture2D("Assets/Textures/Shipyard/shipyard_concrete_normals.jpg"), 0, 10
	);
	resourceManager->AddMaterial("shipyard_concrete", mat_conc);

	//Bouncy PhysicsMaterial
	PhysicsMaterial* pMat_bouncy = new PhysicsMaterial(0.1f, 0.1f, 1.0f);
	resourceManager->AddPhysicsMaterial("bouncy", pMat_bouncy);

	//Solid PhysicsMaterial
	PhysicsMaterial* pMat_solid = new PhysicsMaterial(0.1f, 0.1f, 0.0f);
	resourceManager->AddPhysicsMaterial("solid", pMat_solid);
}

void Game::SetupScene()
{
	//Create the camera and initialize matrices
	GameObject* cameraGO = new GameObject();
	camera = cameraGO->AddComponent<Camera>();
	camera->CreateProjectionMatrix(0.25f * XM_PI, (float)width / height, 0.1f, 10000.0f);
	cameraGO->AddComponent<DebugMovement>();
	cameraGO->AddComponent<TestBullet>();

	//Create the floor
	floor = new GameObject("Floor");
	floor->AddComponent<MeshRenderer>(
		resourceManager->GetMesh("Assets\\Models\\Basic\\cube.obj"),
		resourceManager->GetMaterial("shipyard_concrete")
		);
	floor->MoveAbsolute(XMFLOAT3(0, -2, 0));
	floor->SetScale(200, 1, 200);
	floor->AddComponent<RigidBody>(0.0f)->SetKinematic(true);
	floor->AddComponent<BoxCollider>(floor->GetScale());

	//Create slanted shipping container
	GameObject* cont1 = new GameObject("Container1");
	cont1->AddComponent<MeshRenderer>(
		resourceManager->GetMesh("Assets\\Models\\Shipyard\\shipyard_container.obj"),
		resourceManager->GetMaterial("shipyard_container")
		);
	cont1->MoveAbsolute(XMFLOAT3(-25, -1.5f, 50));
	cont1->Rotate(0, -225, 0);

	//Create horizontal shipping container
	GameObject* cont2 = new GameObject("Container2");
	cont2->AddComponent<MeshRenderer>(
		resourceManager->GetMesh("Assets\\Models\\Shipyard\\shipyard_container.obj"),
		resourceManager->GetMaterial("shipyard_container")
		);
	cont2->MoveAbsolute(XMFLOAT3(30, -1.3f, -5));
	cont2->Rotate(0, 180, 0);

}