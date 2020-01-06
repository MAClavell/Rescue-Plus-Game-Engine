#include "Camera.h"

using namespace DirectX;

// Constructor - Set up the camera
Camera::Camera(GameObject* gameObject) : Component(gameObject)
{
	//Default transformation values
	up = XMFLOAT3(0, 1, 0);
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
void Camera::CreateProjectionMatrix(float fov, float aspectRatio, 
										float nearClip, float farClip)
{
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		fov,			// Field of View Angle
		aspectRatio,	// Aspect ratio
		nearClip,		// Near clip plane distance
		farClip);		// Far clip plane distance
	XMStoreFloat4x4(&rawProjection, P);
	XMStoreFloat4x4(&projection, XMMatrixTranspose(P)); // Transpose for HLSL!
}

// Get the camera's projection matrix
XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return projection;
}

// Get the camera's raw (untransposed) projection matrix
XMFLOAT4X4 Camera::GetRawProjectionMatrix()
{
	return rawProjection;
}