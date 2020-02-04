#include "FirstPersonMovement.h"
#include "ExtendedMath.h"

using namespace DirectX;

#define MOVE_SPEED 6

FirstPersonMovement::FirstPersonMovement(GameObject* gameObject) : UserComponent(gameObject)
{
	inputManager = InputManager::GetInstance();

	controller = gameObject->GetComponent<CharacterController>();

	auto children = gameObject->GetChildren();

	//Get Camera
	camera = children[0]->GetComponent<Camera>();
	cameraGO = children[0];

	sprinting = false;
	falling = false;
	crouching = false;
	sliding = false;
}
FirstPersonMovement::~FirstPersonMovement()
{ }

// Factory function to safely create a First Person Movement object
FirstPersonMovement* FirstPersonMovement::CreateFirstPersonCharacter(const char* name,
	float radius, float height, int screenWidth, int screenHeight)
{
	//Root object
	GameObject* root = new GameObject(name);
	root->AddComponent<CharacterController>(radius, height);

	//Camera object
	GameObject* camera = new GameObject("FPCamera");
	camera->AddComponent<Camera>()
		->CreateProjectionMatrix(0.25f * XM_PI, (float)screenWidth / screenHeight, 0.1f, 10000.0f);
	camera->SetParent(root);
	camera->SetLocalPosition(0, 2, 0);

	//Add FirstPersonMovement component
	FirstPersonMovement* fps = root->AddComponent<FirstPersonMovement>();
	return fps;
}

void FirstPersonMovement::Update(float deltaTime)
{
	//Rotate the camera to where the user is looking
	CalculateCameraRotFromMouse();

	//Detect Input first
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

	XMVECTOR moveVec = XMVectorSet(0, 0, 0, 0);

	//Normal movement
	if (!sliding)
	{
		//Relative Z movement
		//W
		if (movementZ == 1)
		{
			XMFLOAT3 forward = gameObject()->GetForwardAxis();
			moveVec = XMVectorAdd(moveVec,
				XMVectorScale(XMLoadFloat3(&forward), MOVE_SPEED * speedMult * deltaTime));
		}
		//S
		else if (movementZ == 2)
		{
			XMFLOAT3 forward = gameObject()->GetForwardAxis();
			moveVec = XMVectorSubtract(moveVec,
				XMVectorScale(XMLoadFloat3(&forward), MOVE_SPEED * speedMult * deltaTime));
		}
		//Relative X movement
		//D
		if (movementX == 3)
		{
			XMFLOAT3 right = gameObject()->GetRightAxis();
			moveVec = XMVectorAdd(moveVec,
				XMVectorScale(XMLoadFloat3(&right), MOVE_SPEED * speedMult * deltaTime));
		}
		//A
		else if (movementX == 4)
		{
			XMFLOAT3 right = gameObject()->GetRightAxis();
			moveVec = XMVectorSubtract(moveVec,
				XMVectorScale(XMLoadFloat3(&right), MOVE_SPEED * speedMult * deltaTime));
		}
	}
	//Sliding movement
	else
	{

	}

	XMFLOAT3 move;
	XMStoreFloat3(&move, moveVec);
	controller->Move(move, deltaTime, true);
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
	controller->Resize(0.5f);
}
// Changes for when we end a crouch
void FirstPersonMovement::EndCrouch()
{
	crouching = false;
	controller->Resize(2.0f);
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
		gameObject()->SetRotation(0, yRot, 0);
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
