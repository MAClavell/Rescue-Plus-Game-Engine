#pragma once
#include "Component.h"
#include "Mesh.h"
#include "Material.h"
#include "InputManager.h"

class TestBullet :
	public UserComponent
{
private:
	Mesh* sphereMesh;
	Material* whiteMat;
	InputManager* inputManager;
	bool fPressed = false;

public:
	TestBullet(GameObject* gameObject);
	~TestBullet();

	void Update(float deltaTime) override;
};

