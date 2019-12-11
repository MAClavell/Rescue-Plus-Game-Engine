#include "FirstPersonMovement.h"

using namespace DirectX;

FirstPersonMovement::FirstPersonMovement(GameObject* gameObject) : UserComponent(gameObject)
{
	inputManager = InputManager::GetInstance();
	auto children = gameObject->GetChildren();
	go = gameObject;

	//Get Camera
	camera = children[0]->GetComponent<Camera>();
	cameraGO = children[0];

	//Get rigidbody
	rb = gameObject->GetComponent<RigidBody>();

	//Get colliders
	standCol =go->GetComponent<CapsuleCollider>();
	crouchCol = children[1]->GetComponent<CapsuleCollider>();
	slideCol = children[2]->GetComponent<CapsuleCollider>();

	sprinting = false;
	falling = false;
	crouching = false;
	sliding = false;
}
FirstPersonMovement::~FirstPersonMovement()
{ }

// Factory function to safely create a First Person Movement object
FirstPersonMovement* FirstPersonMovement::CreateFirstPersonCharacter(const char* name,
	int screenWidth, int screenHeight)
{
	//Root object
	GameObject* root = new GameObject(name);
	root->AddComponent<RigidBody>(10.0f)->SetContraints(false, false, false, true, true, true);
	root->AddComponent<CapsuleCollider>(1.0f, 2.0f, CapsuleDirection::Y);

	//Camera object
	GameObject* camera = new GameObject("FPCamera");
	camera->SetParent(root);
	camera->AddComponent<Camera>()
		->CreateProjectionMatrix(0.25f * XM_PI, (float)screenWidth / screenHeight, 0.1f, 10000.0f);
	camera->SetLocalPosition(0, 2, 0);

	//Crouching collider
	GameObject* crouchCol = new GameObject("CrouchingCollider");
	crouchCol->SetParent(root);
	crouchCol->SetLocalPosition(0, -1, 0);
	crouchCol->AddComponent<CapsuleCollider>(1.0f, 1.0f, CapsuleDirection::Y)->SetCollisionLayers(true);

	//Sliding collider
	GameObject* slideCol = new GameObject("SlidingCollider");
	slideCol->SetParent(root);
	slideCol->SetLocalPosition(0, -1, 1);
	slideCol->AddComponent<CapsuleCollider>(1.0f, 2.0f, CapsuleDirection::Z)->SetCollisionLayers(true);

	//Add FirstPersonMovement component
	FirstPersonMovement* fps = root->AddComponent<FirstPersonMovement>();

	return fps;
}

void FirstPersonMovement::FixedUpdate(float deltaTime)
{
}

void FirstPersonMovement::Update(float deltaTime)
{
	//Detect Input in the update loop
	
	//Check if the right mouse button is held down
	if (inputManager->GetMouseButtonDown(MouseButtons::R))
	{
		SetCursorPos(inputManager->GetWindowCenterX(), inputManager->GetWindowCenterY());
	}
	else if (inputManager->GetMouseButton(MouseButtons::R))
	{
		CalculateCameraRotFromMouse();
	}

	//Detect movement input
	short movementZ = 0; //0=none, 1=W, 2=S
	short movementX = 0; //0=none, 1=D, 2=A
	if (inputManager->GetKey('W'))
	{
		movementZ = 1;
	}
	else if (inputManager->GetKey('S'))
	{
		movementZ = 2;
	}
	if (inputManager->GetKey('D'))
	{
		movementX = 3;
	}
	else if (inputManager->GetKey('A'))
	{
		movementX = 4;
	}

	//Don't have to hold these buttons down
	if (inputManager->GetKey('Q') && !sprinting)
	{
		if (crouching)
			EndCrouch();
		if (sliding)
			EndSlide();

		StartSprint();
	}
	else if (sprinting && movementZ == 0 && movementX == 0)
		EndSprint();
	if (inputManager->GetKey('C'))
	{
		//If we are sprinting, start a slide
		if (crouching)
		{
			EndCrouch();
		}
		else if (sprinting)
		{
			EndSprint();
			StartSlide();
		}
		else if (sliding)
		{
			EndSlide();
			StartCrouch();
		}
		else StartCrouch();
	}

	//Get correct speed mult
	float speedMult = 1;
	if (sprinting)
		speedMult = 2;
	else if (crouching)
		speedMult = 0.5f;
	//else if (sliding)

	//Normal movement
	if (!sliding)
	{
		XMFLOAT3 velocity = XMFLOAT3(0, rb->GetLinearVelocity().y ,0);
		//Relative Z movement
		if (movementZ == 1)
		{
			velocity.z = baseSpeed * speedMult;
		}
		else if (movementZ == 2)
		{
			velocity.z = -baseSpeed * speedMult;
		}
		//Relative X movement
		if (movementX == 3)
		{
			velocity.x += baseSpeed * speedMult;
		}
		else if (movementX == 4)
		{
			velocity.x = -baseSpeed * speedMult;
		}
		//Rotate and send to rb
		XMStoreFloat3(&velocity, XMVector3Rotate(XMLoadFloat3(&velocity),
			XMLoadFloat4(&go->GetRotation())));
		rb->SetLinearVelocity(velocity);
	}
	//Sliding movement
	else
	{

	}
}

// Changes for when we start a sprint
void FirstPersonMovement::StartSprint()
{
	sprinting = true;
}
// Changes for when we end a sprint
void FirstPersonMovement::EndSprint()
{
	sprinting = false;
}

// Changes for when we start a crouch
void FirstPersonMovement::StartCrouch()
{
	crouching = true;
}
// Changes for when we end a crouch
void FirstPersonMovement::EndCrouch()
{
	crouching = false;
}

// Changes for when we start a slide
void FirstPersonMovement::StartSlide()
{
	sliding = true;
}
// Changes for when we end a slide
void FirstPersonMovement::EndSlide()
{
	sliding = false;
}

// Calculate the camera's rotation when the player moves the mouse
void FirstPersonMovement::CalculateCameraRotFromMouse()
{
	static float cameraSensitivity = 0.25f;

	//Get the center position of the window
	long centerX = inputManager->GetWindowCenterX();
	long centerY = inputManager->GetWindowCenterY();

	//Get the mouse position
	long mouseX = inputManager->GetMouseX();
	long mouseY = inputManager->GetMouseY();

	//Calculate the difference in view with the angle
	float fAngleX = 0.0f;
	float fAngleY = 0.0f;
	float fDeltaMouse = 0.0f;
	if (mouseX > centerX)
	{
		fDeltaMouse = static_cast<float>(mouseX - centerX);
		fAngleY += fDeltaMouse * cameraSensitivity;
	}
	else if (mouseX < centerX)
	{
		fDeltaMouse = static_cast<float>(centerX - mouseX);
		fAngleY -= fDeltaMouse * cameraSensitivity;
	}

	if (mouseY > centerY)
	{
		fDeltaMouse = static_cast<float>(centerY - mouseY);
		fAngleX -= fDeltaMouse * cameraSensitivity;
	}
	else if (mouseY < centerY)
	{
		fDeltaMouse = static_cast<float>(mouseY - centerY);
		fAngleX += fDeltaMouse * cameraSensitivity;
	}

	static float xRot = 0;

	xRot += fAngleX;
	yRot += fAngleY;

	//Keep camera from reversing when looking up/down
	if (xRot > 89.9f)
		xRot = 89.9f;
	if (xRot < -89.9f)
		xRot = -89.9f;

	//Change the Yaw and the Pitch of the camera
	if (!sliding)
	{
		go->SetRotation(0, yRot, 0);
		cameraGO->SetRotation(xRot, yRot, 0);
	}

	SetCursorPos(centerX, centerY); //Position the mouse in the center
	SetCursor(false);
}

// Get the controller's camera
Camera* FirstPersonMovement::GetCamera()
{
	return camera;
}
