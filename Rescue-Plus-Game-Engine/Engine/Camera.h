#pragma once
#include <DirectXMath.h>
#include "GameObject.h"

// --------------------------------------------------------
// A camera definition.
// --------------------------------------------------------
class Camera : public Component
{
private:
	//Matrices
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 rawView;
	DirectX::XMFLOAT4X4 projection;
	DirectX::XMFLOAT4X4 rawProjection;

	//Transformation data
	DirectX::XMFLOAT3 up;

	//Projection data
	bool projectionDirty;
	float fovDegrees;
	float aspectRatio;
	float nearClip;
	float farClip;

	// --------------------------------------------------------
	// Create the camera's projection matrix from the class fields
	// --------------------------------------------------------
	void CreateProjectionMatrix();

public:
	// --------------------------------------------------------
	// Constructor - Set up the camera
	//
	// fovDegrees - Y FOV of the camera in degrees
	// aspectRatio - Aspect ratio of the camera (width / height)
	// nearClip - Near clip plane distance
	// farClip - Far clip plane distance
	// --------------------------------------------------------
	Camera(GameObject* gameObject, float fovDegrees,
		float aspectRatio, float nearClip, float farClip);

	// --------------------------------------------------------
	// Destructor for when an instance is deleted
	// --------------------------------------------------------
	~Camera();

	// --------------------------------------------------------
	// Create the camera's view matrix from fields
	// --------------------------------------------------------
	void CreateViewMatrix();

	// --------------------------------------------------------
	// Get the camera's view matrix
	// --------------------------------------------------------
	DirectX::XMFLOAT4X4 GetViewMatrix();

	// --------------------------------------------------------
	// Get the camera's raw (untransposed) view matrix
	// --------------------------------------------------------
	DirectX::XMFLOAT4X4 GetRawViewMatrix();

	// --------------------------------------------------------
	// Get the camera's projection matrix
	// --------------------------------------------------------
	DirectX::XMFLOAT4X4 GetProjectionMatrix();

	// --------------------------------------------------------
	// Get the raw (untransposed) camera's projection matrix
	// --------------------------------------------------------
	DirectX::XMFLOAT4X4 GetRawProjectionMatrix();

	// --------------------------------------------------------
	// Set the FOV of the camera (in degrees)
	// --------------------------------------------------------
	void SetFOV(float fovDegrees);
	// --------------------------------------------------------
	// Get the FOV of the camera (in degrees)
	// --------------------------------------------------------
	float GetFOV();

	// --------------------------------------------------------
	// Set the aspect ratio of the camera (width / height)
	// --------------------------------------------------------
	void SetAspectRatio(float ratio);
	// --------------------------------------------------------
	// Get the aspect ratio of the camera (width / height)
	// --------------------------------------------------------
	float GetAspectRatio();

	// --------------------------------------------------------
	// Set the near clip plane distance
	// --------------------------------------------------------
	void SetNearClip(float nearClip);
	// --------------------------------------------------------
	// Get the near clip plane distance
	// --------------------------------------------------------
	float GetNearClip();

	// --------------------------------------------------------
	// Set the far clip plane distance
	// --------------------------------------------------------
	void SetFarClip(float farClip);
	// --------------------------------------------------------
	// Get the far clip plane distance
	// --------------------------------------------------------
	float GetFarClip();


};

