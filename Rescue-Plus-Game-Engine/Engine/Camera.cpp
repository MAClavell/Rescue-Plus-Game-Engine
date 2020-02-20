#include "Camera.h"

using namespace DirectX;

// Constructor - Set up the camera
Camera::Camera(GameObject* gameObject, float fovDegrees,
	float aspectRatio, float nearClip, float farClip) : Component(gameObject)
{
	//Default transformation values
	up = XMFLOAT3(0, 1, 0);
	this->fovDegrees = fovDegrees;
	this->aspectRatio = aspectRatio;
	this->nearClip = nearClip;
	this->farClip = farClip;
	projectionDirty = true;
	CreateViewMatrix();
}

// Destructor for when an instance is deleted
Camera::~Camera()
{ }

// Create the camera's view matrix from fields
void Camera::CreateViewMatrix()
{
	//Rotate the forward vector
	XMVECTOR forward = XMLoadFloat3(&gameObject()->GetForwardAxis());

	//Create the up vector from the forward
	XMVECTOR up = XMVector3Cross(XMVector3Cross(forward, XMLoadFloat3(&(this->up))), forward);

	//Create view matrix (transpose for HLSL)
	XMMATRIX viewMat = XMMatrixLookToLH(XMLoadFloat3(&gameObject()->GetPosition()), forward, up);
	XMStoreFloat4x4(&rawView, viewMat);
	XMStoreFloat4x4(&view, XMMatrixTranspose(viewMat));
}
// Get the camera's view matrix
XMFLOAT4X4 Camera::GetViewMatrix()
{
	CreateViewMatrix();
	return view;
}
// Get the camera's raw (untransposed) view matrix
XMFLOAT4X4 Camera::GetRawViewMatrix()
{
	CreateViewMatrix();
	return rawView;
}

// Create the camera's projection matrix from parameters
void Camera::CreateProjectionMatrix()
{
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(fovDegrees),			// Field of View Y Angle
		aspectRatio,							// Aspect ratio
		nearClip,								// Near clip plane distance
		farClip);								// Far clip plane distance
	XMStoreFloat4x4(&rawProjection, P);
	XMStoreFloat4x4(&projection, XMMatrixTranspose(P)); // Transpose for HLSL!
	projectionDirty = false;
}
// Get the camera's projection matrix
XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	if (projectionDirty)
		CreateProjectionMatrix();
	return projection;
}
// Get the camera's raw (untransposed) projection matrix
XMFLOAT4X4 Camera::GetRawProjectionMatrix()
{
	if (projectionDirty)
		CreateProjectionMatrix();
	return rawProjection;
}

// Set the FOV of the camera (in degrees)
void Camera::SetFOV(float fovDegrees)
{
	this->fovDegrees = fovDegrees;
	projectionDirty = true;
}
// Get the FOV of the camera (in degrees)
float Camera::GetFOV()
{
	return fovDegrees;
}

// Set the aspect ratio of the camera (width / height)
void Camera::SetAspectRatio(float aspectRatio)
{
	this->aspectRatio = aspectRatio;
	projectionDirty = true;
}
// Get the aspect ratio of the camera (width / height)
float Camera::GetAspectRatio()
{
	return aspectRatio;
}

// Set the near clip plane distance
void Camera::SetNearClip(float nearClip)
{
	this->nearClip = nearClip;
	projectionDirty = true;
}
// Get the near clip plane distance
float Camera::GetNearClip()
{
	return nearClip;
}

// Set the far clip plane distance
void Camera::SetFarClip(float farClip)
{
	this->farClip = farClip;
	projectionDirty = true;
}
// Get the far clip plane distance
float Camera::GetFarClip()
{
	return farClip;
}
