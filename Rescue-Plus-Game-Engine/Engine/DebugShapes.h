#pragma once
#include <DirectXCollision.h>
#include <PrimitiveBatch.h>
#include <VertexTypes.h>
#include <DirectXColors.h>

void XM_CALLCONV DrawShape(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
	const DirectX::BoundingSphere& sphere,
	DirectX::FXMVECTOR color = DirectX::Colors::White);

void XM_CALLCONV DrawShape(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
	const DirectX::BoundingBox& bb,
	DirectX::FXMVECTOR color = DirectX::Colors::White);

void XM_CALLCONV DrawShape(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
	DirectX::XMFLOAT4X4 world,
	DirectX::FXMVECTOR color = DirectX::Colors::White);

void XM_CALLCONV DrawShape(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
	const DirectX::BoundingFrustum& frustum,
	DirectX::FXMVECTOR color = DirectX::Colors::White);

void XM_CALLCONV DrawGrid(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
	DirectX::FXMVECTOR xAxis, DirectX::FXMVECTOR yAxis,
	DirectX::FXMVECTOR origin, size_t xdivs, size_t ydivs,
	DirectX::GXMVECTOR color = DirectX::Colors::White);

void XM_CALLCONV DrawRing(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
	DirectX::FXMVECTOR origin, DirectX::FXMVECTOR majorAxis, DirectX::FXMVECTOR minorAxis,
	DirectX::GXMVECTOR color = DirectX::Colors::White);

void XM_CALLCONV DrawRay(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
	DirectX::FXMVECTOR origin, DirectX::FXMVECTOR direction, bool normalize = true,
	DirectX::FXMVECTOR color = DirectX::Colors::White);

void XM_CALLCONV DrawTriangle(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
	DirectX::FXMVECTOR pointA, DirectX::FXMVECTOR pointB, DirectX::FXMVECTOR pointC,
	DirectX::GXMVECTOR color = DirectX::Colors::White);

void XM_CALLCONV DrawQuad(DirectX::PrimitiveBatch<DirectX::VertexPositionColor>* batch,
	DirectX::FXMVECTOR pointA, DirectX::FXMVECTOR pointB,
	DirectX::FXMVECTOR pointC, DirectX::GXMVECTOR pointD,
	DirectX::HXMVECTOR color = DirectX::Colors::White);