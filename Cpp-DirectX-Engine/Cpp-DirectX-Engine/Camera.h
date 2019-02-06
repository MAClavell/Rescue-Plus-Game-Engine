#pragma once

#include <DirectXMath.h>
#include "InputManager.h"

// --------------------------------------------------------
// A first person camera definition.
// --------------------------------------------------------
class Camera
{
private:
	//Matrices
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;

	//Transformation data
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 rotation;
	DirectX::XMFLOAT3 forward;
	DirectX::XMFLOAT3 up;

	//Input
	InputManager* inputManager;
	bool enableKeyMovement;

	// --------------------------------------------------------
	// Calculate the camera's rotation when moving
	// --------------------------------------------------------
	void CalculateCameraRotFromMouse();

public:
	// --------------------------------------------------------
	// Constructor - Set up the camera
	// (Remember to create the projection matrix right after!)
	// --------------------------------------------------------
	Camera();

	// --------------------------------------------------------
	// Constructor - Set up the camera
	//
	// forward - The default forward vector for the camera
	// up - The default up vector for the camera
	// --------------------------------------------------------
	Camera(DirectX::XMFLOAT3 forward, DirectX::XMFLOAT3 up);

	// --------------------------------------------------------
	// Destructor for when an instance is deleted
	// --------------------------------------------------------
	~Camera();

	// --------------------------------------------------------
	// Initialize default values
	// --------------------------------------------------------
	void Init();

	// --------------------------------------------------------
	// Update the camera (runs every frame)
	//
	// deltaTime - The time between frames
	// moveSpeed - The speed the camera moves at (defaulted to 1)
	// --------------------------------------------------------
	void Update(float deltaTime, float moveSpeed = 10);

	// --------------------------------------------------------
	// Enable/disable movement with the keyboard
	// (Enabled by default)
	//
	// enabled - Whether movement is allowed
	// --------------------------------------------------------
	void SetKeyMovement(bool enabled);

	// --------------------------------------------------------
	// Create the camera's view matrix from fields
	// --------------------------------------------------------
	void CreateViewMatrix();

	// --------------------------------------------------------
	// Get the camera's view matrix
	// --------------------------------------------------------
	DirectX::XMFLOAT4X4 GetViewMatrix();

	// --------------------------------------------------------
	// Create the camera's projection matrix from parameters
	//
	// fov - FOV of the camera
	// aspectRatio - Aspect ratio of the camera (width / height)
	// nearClip - Near clip plane distance
	// farClip - Far clip plane distance
	// --------------------------------------------------------
	void CreateProjectionMatrix(float fov, float aspectRatio, 
									float nearClip, float farClip);

	// --------------------------------------------------------
	// Get the camera's projection matrix
	// --------------------------------------------------------
	DirectX::XMFLOAT4X4 GetProjectionMatrix();

	// --------------------------------------------------------
	// Set the position for this camera
	//
	// newPosition - The new position to go to
	// --------------------------------------------------------
	void SetPosition(DirectX::XMFLOAT3 newPosition);

	// --------------------------------------------------------
	// Set the position for this camera
	//
	// x - new x position
	// y - new y position
	// z - new z position
	// --------------------------------------------------------
	void SetPosition(float x, float y, float z);

	// --------------------------------------------------------
	// Get the position for this camera
	// --------------------------------------------------------
	DirectX::XMFLOAT3 GetPosition();

	// --------------------------------------------------------
	// Set the rotation for this camera
	//
	// newRotation - The new rotation to rotate to (Quaternion)
	// --------------------------------------------------------
	void SetRotation(DirectX::XMFLOAT4 newRotation);

	// --------------------------------------------------------
	// Set the rotation for this camera using euler angles
	//
	// newRotation - The new rotation to rotate to (euler angles)
	// --------------------------------------------------------
	void EulerAngles(DirectX::XMFLOAT3 newRotation);

	// --------------------------------------------------------
	// Set the rotation for this camera using euler angles
	//
	// x - x angle (degrees)
	// y - y angle (degrees)
	// z - z angle (degrees)
	// --------------------------------------------------------
	void EulerAngles(float x, float y, float z);

	// --------------------------------------------------------
	// Get the rotation for this camera (degrees)
	// --------------------------------------------------------
	DirectX::XMFLOAT4 GetRotation();
};

