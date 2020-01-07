#pragma once
#include <DirectXCollision.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <DirectXColors.h>

//Logic from: https://github.com/microsoft/DirectXTK/wiki/DebugDraw

enum class ShapeDrawType { None, SingleFrame, ForDuration };

// --------------------------------------------------------
// Data for drawing shapes
// --------------------------------------------------------
struct ShapeDrawData
{
	ShapeDrawType type;
	float duration;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 rotation;
	ShapeDrawData(DirectX::XMFLOAT3 position, DirectX::XMFLOAT4 rotation, ShapeDrawType type, float duration)
		: type(type), duration(duration), position(position), rotation(rotation) {}
};
struct ShapeFloat1Data : public ShapeDrawData
{
	float data;
	ShapeFloat1Data(float data, DirectX::XMFLOAT3 position, DirectX::XMFLOAT4 rotation,
		ShapeDrawType type, float duration) :
		data(data), ShapeDrawData(position, rotation, type, duration) {}
};
struct ShapeXMFloat2Data : public ShapeDrawData
{
	DirectX::XMFLOAT2 data;
	ShapeXMFloat2Data(DirectX::XMFLOAT2 data, DirectX::XMFLOAT3 position, DirectX::XMFLOAT4 rotation,
		ShapeDrawType type, float duration) :
		data(data), ShapeDrawData(position, rotation, type, duration) {}
};
struct ShapeXMFloat3Data : public ShapeDrawData
{
	DirectX::XMFLOAT3 data;
	ShapeXMFloat3Data(DirectX::XMFLOAT3 data, DirectX::XMFLOAT3 position, DirectX::XMFLOAT4 rotation,
		ShapeDrawType type, float duration) :
		data(data), ShapeDrawData(position, rotation, type, duration) {}
};

// --------------------------------------------------------
// Draw a cube
// --------------------------------------------------------
void XM_CALLCONV DrawCube(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
	const ShapeXMFloat3Data& drawData,
	DirectX::FXMVECTOR color = DirectX::Colors::White);

// --------------------------------------------------------
// Draw a sphere
// --------------------------------------------------------
void XM_CALLCONV DrawSphere(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
	const ShapeFloat1Data& drawData,
	DirectX::FXMVECTOR color = DirectX::Colors::White);

// --------------------------------------------------------
// Draw a capsule
// --------------------------------------------------------
void XM_CALLCONV DrawCapsule(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
	const ShapeXMFloat2Data& drawData,
	DirectX::FXMVECTOR color = DirectX::Colors::White);

// --------------------------------------------------------
// Draw a ray
// --------------------------------------------------------
void XM_CALLCONV DrawRay(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
	const ShapeFloat1Data& drawData,
	DirectX::FXMVECTOR color = DirectX::Colors::White);