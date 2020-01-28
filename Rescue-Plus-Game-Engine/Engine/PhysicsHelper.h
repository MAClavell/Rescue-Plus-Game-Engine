#pragma once
#include <DirectXMath.h>
#include <PxPhysicsAPI.h>

// --------------------------------------------------------
// Convert a DirectX XMFLOAT3 to a PhysX Vec3
// --------------------------------------------------------
static physx::PxVec3 Float3ToVec3(DirectX::XMFLOAT3 float3)
{
	return physx::PxVec3(float3.x, float3.y, float3.z);
}

// --------------------------------------------------------
// Convert a PhysX Vec3 to a DirectX XMFLOAT3
// --------------------------------------------------------
static DirectX::XMFLOAT3 Vec3ToFloat3(physx::PxVec3 vec3)
{
	return DirectX::XMFLOAT3(vec3.x, vec3.y, vec3.z);
}

// --------------------------------------------------------
// Convert a DirectX XMFLOAT3 to a PhysX ExtendedVec3
// --------------------------------------------------------
static physx::PxExtendedVec3 Float3ToExtVec3(DirectX::XMFLOAT3 float3)
{
	return physx::PxExtendedVec3(float3.x, float3.y, float3.z);
}

// --------------------------------------------------------
// Convert a PhysX ExtendedVec3 to a DirectX XMFLOAT3
// --------------------------------------------------------
static DirectX::XMFLOAT3 ExtVec3ToFloat3(physx::PxExtendedVec3 vec3)
{
	return DirectX::XMFLOAT3(vec3.x, vec3.y, vec3.z);
}

// --------------------------------------------------------
// Convert a DirectX XMFLOAT4 to a PhysX Vec4
// --------------------------------------------------------
static physx::PxVec4 Float4ToVec4(DirectX::XMFLOAT4 float4)
{
	return physx::PxVec4(float4.x, float4.y, float4.z, float4.w);
}

// --------------------------------------------------------
// Convert a PhysX Vec4 to a DirectX XMFLOAT4
// --------------------------------------------------------
static DirectX::XMFLOAT4 Vec4ToFloat4(physx::PxVec4 vec4)
{
	return DirectX::XMFLOAT4(vec4.x, vec4.y, vec4.z, vec4.w);
}

// --------------------------------------------------------
// Convert a PhysX Quat to a DirectX XMFLOAT4
// --------------------------------------------------------
static DirectX::XMFLOAT4 QuatToFloat4(physx::PxQuat quat)
{
	return DirectX::XMFLOAT4(quat.x, quat.y, quat.z, quat.w);
}

// --------------------------------------------------------
// Convert a DirectX XMFLOAT4 to a PhysX Quat
// --------------------------------------------------------
static physx::PxQuat Float4ToQuat(DirectX::XMFLOAT4 float4)
{
	return physx::PxQuat(float4.x, float4.y, float4.z, float4.w);
}