#include "DebugMovement.h"
#include "ExtendedMath.h"

using namespace DirectX;

// Constructor - Set up the focus camera
DebugMovement::DebugMovement(GameObject* gameObject) : UserComponent(gameObject)
{
	inputManager = InputManager::GetInstance();
}

// Destructor for when an instance is deleted
DebugMovement::~DebugMovement()
{ }

// Update the camera (runs every frame)
void DebugMovement::Update(float deltaTime)
{
	Movement(deltaTime);
}

// Move the GameObject
void DebugMovement::Movement(float deltaTime)
{
	XMFLOAT3 movement = XMFLOAT3();

	//Relative Z movement
	if (inputManager->GetKey('W'))
	{
		movement.z += moveSpeed * deltaTime;
	}
	else if (inputManager->GetKey('S'))
	{
		movement.z -= moveSpeed * deltaTime;
	}

	//Relative X movement
	if (inputManager->GetKey('D'))
	{
		movement.x += moveSpeed * deltaTime;
	}
	else if (inputManager->GetKey('A'))
	{
		movement.x -= moveSpeed * deltaTime;
	}

	//Absolute Y movement
	if (inputManager->GetKey('Q'))
	{
		gameObject()->MoveAbsolute(XMFLOAT3(0, +moveSpeed * deltaTime, 0));
	}
	else if (inputManager->GetKey('E'))
	{
		gameObject()->MoveAbsolute(XMFLOAT3(0, -moveSpeed * deltaTime, 0));
	}

	// Rotate the movement vector
	XMVECTOR move = XMVector3Rotate(XMLoadFloat3(&movement),
		XMLoadFloat4(&gameObject()->GetRotation()));
	//Add to position and store
	XMStoreFloat3(&movement, XMVectorAdd(XMLoadFloat3(&gameObject()->GetPosition()), move));
	gameObject()->SetPosition(movement);

	//Check if the right mouse button is held down
	if (inputManager->GetMouseButtonDown(MouseButtons::R))
	{
		SetCursorPos(inputManager->GetWindowCenterX(), inputManager->GetWindowCenterY());
	}
	else if (inputManager->GetMouseButton(MouseButtons::R))
	{
		CalculateCameraRotFromMouse();
	}
}

// Calculate the camera's rotation when moving
void DebugMovement::CalculateCameraRotFromMouse()
{
	float speed = 0.25f;

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
		fAngleY += fDeltaMouse * speed;
	}
	else if (mouseX < centerX)
	{
		fDeltaMouse = static_cast<float>(centerX - mouseX);
		fAngleY -= fDeltaMouse * speed;
	}

	if (mouseY > centerY)
	{
		fDeltaMouse = static_cast<float>(centerY - mouseY);
		fAngleX -= fDeltaMouse * speed;
	}
	else if (mouseY < centerY)
	{
		fDeltaMouse = static_cast<float>(mouseY - centerY);
		fAngleX += fDeltaMouse * speed;
	}

	static float xRot = 0;
	static float yRot = 0;

	xRot += fAngleX;
	yRot += fAngleY;

	//Keep camera from reversing when looking up/down
	if (xRot > 89.9f)
		xRot = 89.9f;
	if (xRot < -89.9f)
		xRot = -89.9f;

	//Change the Yaw and the Pitch of the camera
	gameObject()->SetRotation(xRot, yRot, 0);
	SetCursorPos(centerX, centerY); //Position the mouse in the center
}