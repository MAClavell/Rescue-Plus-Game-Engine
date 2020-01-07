#include "DebugShapes.h"
#include <algorithm>

//Logic from: https://github.com/microsoft/DirectXTK/wiki/DebugDraw

using namespace DirectX;

void XM_CALLCONV DrawRing(PrimitiveBatch<VertexPositionColor>* batch,
	FXMVECTOR origin,
	FXMVECTOR majorAxis,
	FXMVECTOR minorAxis,
	GXMVECTOR color)
{
	static const size_t c_ringSegments = 32;

	VertexPositionColor verts[c_ringSegments + 1];

	FLOAT fAngleDelta = XM_2PI / float(c_ringSegments);
	// Instead of calling cos/sin for each segment we calculate
	// the sign of the angle delta and then incrementally calculate sin
	// and cosine from then on.
	XMVECTOR cosDelta = XMVectorReplicate(cosf(fAngleDelta));
	XMVECTOR sinDelta = XMVectorReplicate(sinf(fAngleDelta));
	XMVECTOR incrementalSin = XMVectorZero();
	static const XMVECTORF32 s_initialCos =
	{
		1.f, 1.f, 1.f, 1.f
	};
	XMVECTOR incrementalCos = s_initialCos.v;
	for (size_t i = 0; i < c_ringSegments; i++)
	{
		XMVECTOR pos = XMVectorMultiplyAdd(majorAxis, incrementalCos, origin);
		pos = XMVectorMultiplyAdd(minorAxis, incrementalSin, pos);
		XMStoreFloat3(&verts[i].position, pos);
		XMStoreFloat4(&verts[i].color, color);
		// Standard formula to rotate a vector.
		XMVECTOR newCos = incrementalCos * cosDelta - incrementalSin * sinDelta;
		XMVECTOR newSin = incrementalCos * sinDelta + incrementalSin * cosDelta;
		incrementalCos = newCos;
		incrementalSin = newSin;
	}
	verts[c_ringSegments] = verts[0];

	batch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, verts, c_ringSegments + 1);
}

void XM_CALLCONV DrawCube(PrimitiveBatch<VertexPositionColor>* batch,
	const ShapeXMFloat3Data& drawData,
	FXMVECTOR color)
{
	XMMATRIX matWorld = XMMatrixRotationQuaternion(XMLoadFloat4(&drawData.rotation));
	matWorld = XMMatrixMultiply(XMMatrixScaling(drawData.data.x / 2, drawData.data.y / 2, drawData.data.z / 2),
		matWorld);
	matWorld.r[3] = XMVectorSelect(matWorld.r[3], XMLoadFloat3(&drawData.position), g_XMSelect1110);

	static const XMVECTORF32 s_verts[8] =
	{
		{ -1.f, -1.f, -1.f, 0.f },
		{  1.f, -1.f, -1.f, 0.f },
		{  1.f, -1.f,  1.f, 0.f },
		{ -1.f, -1.f,  1.f, 0.f },
		{ -1.f,  1.f, -1.f, 0.f },
		{  1.f,  1.f, -1.f, 0.f },
		{  1.f,  1.f,  1.f, 0.f },
		{ -1.f,  1.f,  1.f, 0.f }
	};

	static const WORD s_indices[] =
	{
		0, 1,
		1, 2,
		2, 3,
		3, 0,
		4, 5,
		5, 6,
		6, 7,
		7, 4,
		0, 4,
		1, 5,
		2, 6,
		3, 7
	};

	VertexPositionColor verts[8];
	for (size_t i = 0; i < 8; ++i)
	{
		XMVECTOR v = XMVector3Transform(s_verts[i], matWorld);
		XMStoreFloat3(&verts[i].position, v);
		XMStoreFloat4(&verts[i].color, color);
	}

	batch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_LINELIST, s_indices, _countof(s_indices), verts, 8);
}


void XM_CALLCONV DrawSphere(PrimitiveBatch<VertexPositionColor>* batch,
	const ShapeFloat1Data& drawData,
	FXMVECTOR color)
{
	XMVECTOR origin = XMLoadFloat3(&drawData.position);

	const float radius = drawData.data;

	XMVECTOR xaxis = g_XMIdentityR0 * radius;
	XMVECTOR yaxis = g_XMIdentityR1 * radius;
	XMVECTOR zaxis = g_XMIdentityR2 * radius;

	DrawRing(batch, origin, xaxis, zaxis, color);
	DrawRing(batch, origin, xaxis, yaxis, color);
	DrawRing(batch, origin, yaxis, zaxis, color);
}

void XM_CALLCONV DrawCapsule(PrimitiveBatch<VertexPositionColor>* batch,
	const ShapeXMFloat2Data& drawData,
	FXMVECTOR color)
{
}

void XM_CALLCONV DrawRay(PrimitiveBatch<VertexPositionColor>* batch,
	const ShapeFloat1Data& drawData,
	FXMVECTOR color)
{
	VertexPositionColor verts[3];
	verts[0].position = drawData.position;

	XMVECTOR rayDirection = XMVectorScale(XMLoadFloat4(&drawData.rotation), drawData.data);
	XMVECTOR normDirection = XMVector3Normalize(rayDirection);

	XMVECTOR perpVector = XMVector3Cross(normDirection, g_XMIdentityR1);

	if (XMVector3Equal(XMVector3LengthSq(perpVector), g_XMZero))
	{
		perpVector = XMVector3Cross(normDirection, g_XMIdentityR2);
	}
	perpVector = XMVector3Normalize(perpVector);

	XMVECTOR origin = XMLoadFloat3(&drawData.position);
	XMStoreFloat3(&verts[1].position, XMVectorAdd(rayDirection, origin));
	perpVector = XMVectorScale(perpVector, 0.0625f);
	normDirection = XMVectorScale(normDirection, -0.25f);
	rayDirection = XMVectorAdd(perpVector, rayDirection);
	rayDirection = XMVectorAdd(normDirection, rayDirection);
	XMStoreFloat3(&verts[2].position, XMVectorAdd(rayDirection, origin));

	XMStoreFloat4(&verts[0].color, color);
	XMStoreFloat4(&verts[1].color, color);
	XMStoreFloat4(&verts[2].color, color);

	batch->Draw(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, verts, 2);
}