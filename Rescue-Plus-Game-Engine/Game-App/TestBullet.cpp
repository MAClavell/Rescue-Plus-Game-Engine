#include "TestBullet.h"
#include "ResourceManager.h"
#include "MeshRenderer.h"
#include "Collider.h"

using namespace DirectX;

TestBullet::TestBullet(GameObject* gameObject) : UserComponent(gameObject)
{
	sphereMesh = ResourceManager::GetInstance()->GetMesh("Assets\\Models\\Basic\\sphere.obj");
	whiteMat = ResourceManager::GetInstance()->GetMaterial("white");
	inputManager = InputManager::GetInstance();
}

void TestBullet::Update(float deltaTime)
{
	//Temp workaround until I get input manager working fully
	if (inputManager->GetKey('F'))
	{
		if (!fPressed)
		{
			fPressed = true;

			//Spawn bullet
			GameObject* bullet = new GameObject("Bullet");
			bullet->AddComponent<MeshRenderer>(
				sphereMesh,
				whiteMat);

			//Set initial transforms
			XMFLOAT3 pos = gameObject()->GetPosition();
			bullet->SetPosition(pos.x, pos.y - 0.5f, pos.z);
			bullet->SetScale(0.5f, 0.5f, 0.5f);

			//Physics, add a force
			bullet->AddComponent<SphereCollider>(0.25f);
			XMFLOAT3 force;
			XMStoreFloat3(&force, XMVectorScale(XMLoadFloat3(&gameObject()->GetForwardAxis()), 3000.0f));
			bullet->AddComponent<RigidBody>(1.0f)->AddForce(force);
		}
	}
	else fPressed = false;
}

TestBullet::~TestBullet()
{
}
