#include "Camera.h"

using namespace DirectX;

// Constructor - Set up the camera
Camera::Camera()
{
	//Default transformation values
	forward = XMFLOAT3(0, 0, 1);
	up = XMFLOAT3(0, 1, 0);

	Init();
}

Camera::Camera(XMFLOAT3 forward, XMFLOAT3 up)
{
	//Default transformation values
	this->forward = forward;
	this->up = up;

	Init();
}

//Initialize default values
void Camera::Init()
{
	position = XMFLOAT3();
	EulerAngles(0, 0, 0);
	enableKeyMovement = true;
	inputManager = InputManager::GetInstance();
	CreateViewMatrix();
}

// Destructor for when an instance is deleted
Camera::~Camera()
{ }

// Update the camera (runs every frame)
void Camera::Update(float deltaTime, float moveSpeed)
{
	//Movement
	if (enableKeyMovement)
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
			position.y += moveSpeed * deltaTime;
		}
		else if (inputManager->GetKey('E'))
		{
			position.y -= moveSpeed * deltaTime;
		}

		// Rotate the movement vector
		XMVECTOR move = XMVector3Rotate(XMLoadFloat3(&movement), 
								XMLoadFloat4(&rotation));
		//Add to position and store
		XMStoreFloat3(&position, XMVectorAdd(XMLoadFloat3(&position), move));

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

	CreateViewMatrix();
}

// Enable/disable movement with the keyboard
void Camera::SetKeyMovement(bool enabled)
{
	enableKeyMovement = enabled;
}

// Create the camera's view matrix from fields
void Camera::CreateViewMatrix()
{
	//Rotate the forward vector
	XMVECTOR forward = XMVector3Rotate(XMLoadFloat3(&(this->forward)), 
						XMLoadFloat4(&rotation));

	//Create the up vector from the forward
	XMVECTOR up = XMVector3Cross(XMVector3Cross(forward, XMLoadFloat3(&(this->up))), forward);

	//Create view matrix (transpose for HLSL)
	XMStoreFloat4x4(&view, XMMatrixTranspose(
		XMMatrixLookToLH(XMLoadFloat3(&position), forward, up)));
}

// Get the camera's view matrix
XMFLOAT4X4 Camera::GetViewMatrix()
{
	return view;
}

// Create the camera's projection matrix from parameters
void Camera::CreateProjectionMatrix(float fov, float aspectRatio, 
										float nearClip, float farClip)
{
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		fov,			// Field of View Angle
		aspectRatio,	// Aspect ratio
		nearClip,		// Near clip plane distance
		farClip);		// Far clip plane distance
	XMStoreFloat4x4(&projection, XMMatrixTranspose(P)); // Transpose for HLSL!
}

// Get the camera's projection matrix
XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return projection;
}

// Set the position for this camera
void Camera::SetPosition(XMFLOAT3 newPosition)
{
	position = newPosition;
}

// Set the position for this camera
void Camera::SetPosition(float x, float y, float z)
{
	position.x = x;
	position.y = y;
	position.z = z;
}

// Get the position for this camera
XMFLOAT3 Camera::GetPosition()
{
	return position;
}

// Set the rotation for this camera
void Camera::SetRotation(XMFLOAT4 newRotation)
{
	rotation = newRotation;
}

void Camera::EulerAngles(XMFLOAT3 newRotation)
{
	//Convert to degrees
	XMVECTOR angles = XMVectorScale(XMLoadFloat3(&newRotation), XM_PI / 180.0f);

	//Convert from angles to a quaternion
	XMStoreFloat4(&rotation, XMQuaternionRotationRollPitchYawFromVector(angles));
}

// Set the rotation for this camera
void Camera::EulerAngles(float x, float y, float z)
{
	//Convert to degrees
	XMVECTOR angles = XMVectorScale(XMVectorSet(x, y, z, 0.0f), XM_PI / 180.0f);

	//Convert from angles to a quaternion
	XMStoreFloat4(&rotation, XMQuaternionRotationRollPitchYawFromVector(angles));
}

// Get the rotation for this camera (degrees)
XMFLOAT4 Camera::GetRotation()
{
	return rotation;
}

// Calculate the camera's rotation when moving
void Camera::CalculateCameraRotFromMouse()
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
	EulerAngles(xRot, yRot, rotation.z);
	SetCursorPos(centerX, centerY); //Position the mouse in the center
}