#include "Game.h"
#include "DebugMovement.h"
#include "TestCallbacks.h"
#include "MAT_Skybox.h"
#include "MAT_Basic.h"
#include "TestBullet.h"
#include "ShipyardCrane.h"

using namespace DirectX;

// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::LoadAssets()
{
	Job* root = JobSystem::CreateJob(&EmptyJob);

	//Create meshes
	resourceManager->LoadMeshAsync("Assets\\Models\\Basic\\cube.obj", device, root);
	resourceManager->LoadMeshAsync("Assets\\Models\\Basic\\sphere.obj", device, root);
	resourceManager->LoadMeshAsync("Assets\\Models\\Basic\\cylinder.obj", device, root);
	resourceManager->LoadMeshAsync("Assets\\Models\\Shipyard\\shipyard_container.obj", device, root);

	//Load shaders
	resourceManager->LoadVertexShaderAsync("VertexShader.cso", device, context, root);
	resourceManager->LoadPixelShaderAsync("PixelShader.cso", device, context, root);

	resourceManager->LoadVertexShaderAsync("VS_ColDebug.cso", device, context, root);
	resourceManager->LoadPixelShaderAsync("PS_ColDebug.cso", device, context, root);

	resourceManager->LoadVertexShaderAsync("VS_Sky.cso", device, context, root);
	resourceManager->LoadPixelShaderAsync("PS_Sky.cso", device, context, root);

	resourceManager->LoadVertexShaderAsync("VS_Shadow.cso", device, context, root);

	//Load textures
	resourceManager->LoadTexture2DAsync("Assets/Textures/white.png", device, root);
	resourceManager->LoadTexture2DAsync("Assets/Textures/normals_flat.png", device, root);
	resourceManager->LoadTexture2DAsync("Assets/Textures/Shipyard/shipyard_container_diffuse.jpeg", device, context, root);
	resourceManager->LoadTexture2DAsync("Assets/Textures/Shipyard/shipyard_concrete_diffuse.jpg", device, context, root);
	resourceManager->LoadTexture2DAsync("Assets/Textures/Shipyard/shipyard_concrete_normals.jpg", device, context, root);
	resourceManager->LoadTexture2DAsync("Assets/Textures/Shipyard/blue.png", device, root);
	resourceManager->LoadTexture2DAsync("Assets/Textures/Shipyard/yellow.png", device, root);
	resourceManager->LoadTexture2DAsync("Assets/Textures/Shipyard/gray.png", device, root);
	resourceManager->LoadTexture2DAsync("Assets/Textures/Shipyard/shipyard_crate_diffuse.png", device, context, root);
	resourceManager->LoadTexture2DAsync("Assets/Textures/Shipyard/shipyard_crate_normals.png", device, context, root);

	//Load cubemaps
	resourceManager->LoadCubeMapAsync("Assets/Textures/Sky/SunnyCubeMap.dds", device, root);

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
	
	//Wait for assets to load
	JobSystem::Run(root);
	JobSystem::Wait(root);

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
		resourceManager->GetTexture2D("Assets/Textures/normals_flat.png"), 
		shadowSampler,
		0, 52
	);
	resourceManager->AddMaterial("white", mat_white);
	//Blue material
	Material* blue = new MAT_Basic(vs, ps_basic, XMFLOAT2(10, 10), samplerState,
		resourceManager->GetTexture2D("Assets/Textures/Shipyard/blue.png"),
		resourceManager->GetTexture2D("Assets/Textures/normals_flat.png"),
		shadowSampler,
		0.5f, 52
	);
	resourceManager->AddMaterial("blue", blue);
	//Yellow material
	Material* yellow = new MAT_Basic(vs, ps_basic, XMFLOAT2(10, 10), samplerState,
		resourceManager->GetTexture2D("Assets/Textures/Shipyard/yellow.png"),
		resourceManager->GetTexture2D("Assets/Textures/normals_flat.png"), 
		shadowSampler, 
		0.5f, 52
	);
	resourceManager->AddMaterial("yellow", yellow);
	//Gray material
	Material* gray = new MAT_Basic(vs, ps_basic, XMFLOAT2(10, 10), samplerState,
		resourceManager->GetTexture2D("Assets/Textures/Shipyard/gray.png"),
		resourceManager->GetTexture2D("Assets/Textures/normals_flat.png"), 
		shadowSampler, 
		0.5f, 52
	);
	resourceManager->AddMaterial("gray", gray);

	//Shipyard container material
	Material* mat_cont = new MAT_Basic(vs, ps_basic, XMFLOAT2(1, 1), samplerState,
		resourceManager->GetTexture2D("Assets/Textures/Shipyard/shipyard_container_diffuse.jpeg"),
		resourceManager->GetTexture2D("Assets/Textures/normals_flat.png"), 
		shadowSampler, 
		0.6f, 10
	);
	resourceManager->AddMaterial("shipyard_container", mat_cont);

	//Shipyard concrete material
	Material* mat_conc = new MAT_Basic(vs, ps_basic, XMFLOAT2(10, 10), samplerState,
		resourceManager->GetTexture2D("Assets/Textures/Shipyard/shipyard_concrete_diffuse.jpg"),
		resourceManager->GetTexture2D("Assets/Textures/Shipyard/shipyard_concrete_normals.jpg"),
		shadowSampler, 
		0.5f, 52
	);
	resourceManager->AddMaterial("shipyard_concrete", mat_conc);

	//Shipyard crate material
	Material* mat_crate = new MAT_Basic(vs, ps_basic, XMFLOAT2(1, 1), samplerState,
		resourceManager->GetTexture2D("Assets/Textures/Shipyard/shipyard_crate_diffuse.png"),
		resourceManager->GetTexture2D("Assets/Textures/Shipyard/shipyard_crate_normals.png"),
		shadowSampler, 
		1, 80
	);
	resourceManager->AddMaterial("shipyard_crate", mat_crate);

	//Bouncy PhysicsMaterial
	PhysicsMaterial* pMat_bouncy = new PhysicsMaterial(0.1f, 0.1f, 1.0f);
	resourceManager->AddPhysicsMaterial("bouncy", pMat_bouncy);

	//Solid PhysicsMaterial
	PhysicsMaterial* pMat_solid = new PhysicsMaterial(0.1f, 0.1f, 0.0f);
	resourceManager->AddPhysicsMaterial("solid", pMat_solid);
}

//Create crane
static GameObject* CreateCraneObj(ResourceManager* rm, const char* name, const char* mat, GameObject* parent)
{
	GameObject* go = new GameObject(name);
	go->AddComponent<MeshRenderer>(
		rm->GetMesh("Assets\\Models\\Basic\\cube.obj"),
		rm->GetMaterial(mat));
	go->SetParent(parent);
	return go;
}
static void CreateCrane(ResourceManager* rm)
{
	GameObject* craneGO = new GameObject("Crane");
	//Create pillars
	for (int i = 1; i < 5; i++)
	{
		GameObject* pillar = CreateCraneObj(rm, "Pillar" + i, "blue", craneGO);
		pillar->SetScale(3, 30, 3);
		switch (i)
		{
		case 1:
			pillar->SetLocalPosition(XMFLOAT3(-20, 0, -15));
			break;
		case 2:
			pillar->SetLocalPosition(XMFLOAT3(-20, 0, 15));
			break;
		case 3:
			pillar->SetLocalPosition(XMFLOAT3(20, 0, 15));
			break;
		case 4:
			pillar->SetLocalPosition(XMFLOAT3(20, 0, -15));
			break;
		default: break;
		}
		pillar->AddComponent<RigidBody>(1.0f)->SetKinematic(true);
		pillar->AddComponent<BoxCollider>(pillar->GetScale());
	}

	//Create tops
	GameObject* top1 = CreateCraneObj(rm, "Top1", "blue", craneGO);
	top1->SetScale(43, 2, 3);
	top1->SetLocalPosition(0, 15, -15);
	top1->AddComponent<RigidBody>(1.0f)->SetKinematic(true);
	top1->AddComponent<BoxCollider>(top1->GetScale());
	GameObject* top2 = CreateCraneObj(rm, "Top2", "blue", craneGO);
	top2->SetScale(43, 2, 3);
	top2->SetLocalPosition(0, 15, 15);
	top2->AddComponent<RigidBody>(1.0f)->SetKinematic(true);
	top2->AddComponent<BoxCollider>(top2->GetScale());

	//Create sides
	GameObject* side1 = CreateCraneObj(rm, "Side1", "blue", craneGO);
	side1->SetScale(1, 3, 30);
	side1->SetLocalPosition(20, 6, 0);
	side1->AddComponent<RigidBody>(1.0f)->SetKinematic(true);
	side1->AddComponent<BoxCollider>(side1->GetScale());
	GameObject* side2 = CreateCraneObj(rm, "Side1", "blue", craneGO);
	side2->SetScale(1, 3, 30);
	side2->SetLocalPosition(-20, 6, 0);
	side2->AddComponent<RigidBody>(1.0f)->SetKinematic(true);
	side2->AddComponent<BoxCollider>(side2->GetScale());

	//Create moveable crane part
	GameObject* moveable = CreateCraneObj(rm, "CraneMoveable", "blue", craneGO);
	moveable->SetScale(14, 3, 30);
	moveable->SetLocalPosition(0, 16.5f, 0);
	moveable->AddComponent<RigidBody>(1.0f)->SetKinematic(true);
	moveable->AddComponent<BoxCollider>(moveable->GetScale());

	for (int i = 1; i < 5; i++)
	{
		GameObject* rope = new GameObject("Rope"+i);
		rope->AddComponent<MeshRenderer>(
			rm->GetMesh("Assets\\Models\\Basic\\cylinder.obj"),
			rm->GetMaterial("gray"));
		rope->SetParent(moveable);
		rope->SetScale(0.2f, 10, 0.2f);
		switch (i)
		{
		case 1:
			rope->SetLocalPosition(XMFLOAT3(5, -5, -13));
			rope->SetLocalRotation(0, 0, -5);
			break;
		case 2:
			rope->SetLocalPosition(XMFLOAT3(-5, -5, 13));
			rope->SetLocalRotation(0, 0, 5);
			break;
		case 3:
			rope->SetLocalPosition(XMFLOAT3(5, -5, 13));
			rope->SetLocalRotation(0, 0, -5);
			break;
		case 4:
			rope->SetLocalPosition(XMFLOAT3(-5, -5, -13));
			rope->SetLocalRotation(0, 0, 5);
			break;
		default: break;
		}
	}

	//Create hook
	GameObject* hookBase = CreateCraneObj(rm, "HookBase", "yellow", moveable);
	hookBase->SetScale(10, 1, 28);
	hookBase->SetLocalPosition(0, -10, 0);
	hookBase->AddComponent<RigidBody>(1.0f)->SetKinematic(true);
	hookBase->AddComponent<BoxCollider>(hookBase->GetScale());
	GameObject* hook1 = CreateCraneObj(rm, "Hook1", "yellow", hookBase);
	hook1->SetScale(10, 2, 1);
	hook1->SetLocalPosition(0, -1, 13.75f);
	hook1->AddComponent<RigidBody>(1.0f)->SetKinematic(true);
	hook1->AddComponent<BoxCollider>(hook1->GetScale());
	GameObject* hook2 = CreateCraneObj(rm, "Hook2", "yellow", hookBase);
	hook2->SetScale(10, 2, 1);
	hook2->SetLocalPosition(0, -1, -13.75f);
	hook2->AddComponent<RigidBody>(1.0f)->SetKinematic(true);
	hook2->AddComponent<BoxCollider>(hook2->GetScale());

	craneGO->MoveAbsolute(XMFLOAT3(-50, 13, 40));
	craneGO->Rotate(0, 90, 0);
	craneGO->AddComponent<ShipyardCrane>(moveable);
}

static GameObject* CreateContainer(ResourceManager* rm, const char* name)
{
	GameObject* cont = new GameObject(name);
	cont->AddComponent<MeshRenderer>(
		rm->GetMesh("Assets\\Models\\Shipyard\\shipyard_container.obj"),
		rm->GetMaterial("shipyard_container"));
	cont->AddComponent<RigidBody>(1.0f)->SetKinematic(true);
	cont->AddComponent<BoxCollider>(XMFLOAT3(25.5f, 10, 10), nullptr, XMFLOAT3(-0.2f, 5.1f, 0))->SetDebug(true);
	return cont;
}

static GameObject* CreateCrate(ResourceManager* rm, const char* name, float size)
{
	GameObject* crate = new GameObject(name);
	crate->AddComponent<MeshRenderer>(
		rm->GetMesh("Assets\\Models\\Basic\\cube.obj"),
		rm->GetMaterial("shipyard_crate")
		);
	crate->SetScale(size, size, size);
	crate->AddComponent<RigidBody>(1.0f);
	crate->AddComponent<BoxCollider>(crate->GetScale());
	return crate;
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
	floor->AddComponent<BoxCollider>(floor->GetScale());

	//Create horizontal shipping container
	GameObject* cont1 = CreateContainer(resourceManager, "Container1");
	cont1->MoveAbsolute(XMFLOAT3(25, -1.5f, 50));
	cont1->Rotate(0, -180, 0);

	//Create crane shipping container
	GameObject* cont2 = CreateContainer(resourceManager, "Container2");
	cont2->MoveAbsolute(XMFLOAT3(-50, -1.5f, 50));

	//Create horizontal shipping container
	GameObject* cont3 = CreateContainer(resourceManager, "Container3");
	cont3->MoveAbsolute(XMFLOAT3(-10, -1.5f, 50));

	//Create slanted shipping container
	GameObject* cont4 = CreateContainer(resourceManager, "Container4");
	cont4->MoveAbsolute(XMFLOAT3(30, -1.5f, -5));
	cont4->Rotate(0, 135, 0);

	//Create slanted shipping container
	GameObject* cont5 = CreateContainer(resourceManager, "Container5");
	cont5->MoveAbsolute(XMFLOAT3(0, -1.5f, -20));
	cont5->Rotate(0, -150, 0);

	//Create top shipping container
	GameObject* cont6 = CreateContainer(resourceManager, "Container6");
	cont6->MoveAbsolute(XMFLOAT3(15, 8.6f, -15));
	cont6->Rotate(0, -30, 0);

	CreateCrate(resourceManager, "Crate1", 1)->SetPosition(35, 0, 20);
	CreateCrate(resourceManager, "Crate2", 3)->SetPosition(39, 0, 24);
	CreateCrate(resourceManager, "Crate3", 3)->SetPosition(32, 0, 18);
	CreateCrate(resourceManager, "Crate4", 3)->SetPosition(20, 0, 19);
	CreateCrate(resourceManager, "Crate5", 4)->SetPosition(11, 2, 24);
	CreateCrate(resourceManager, "Crate6", 2)->SetPosition(-4, 0, 9);
	CreateCrate(resourceManager, "Crate7", 3)->SetPosition(-9, 0, 11);
	CreateCrate(resourceManager, "Crate8", 3)->SetPosition(-30, 0, 8);
	CreateCrate(resourceManager, "Crate9", 1)->SetPosition(-18, 0, 26);

	GameObject* crate10 = CreateCrate(resourceManager, "Crate10", 5);
	crate10->SetPosition(-20, 3, 17);
	crate10->AddComponent<TestCallbacks>();
	GameObject* crate10C = new GameObject("Crate10Child");
	crate10C->AddComponent<MeshRenderer>(
		resourceManager->GetMesh("Assets\\Models\\Basic\\cube.obj"),
		resourceManager->GetMaterial("shipyard_crate")
		);
	crate10C->SetScale(2, 2, 2);
	crate10C->SetParent(crate10);
	crate10C->SetLocalPosition(0, 4, 0);
	crate10C->SetLocalRotation(0, 0, 45);
	crate10C->AddComponent<BoxCollider>(crate10C->GetScale())->SetDebug(true);

	CreateCrane(resourceManager);
}