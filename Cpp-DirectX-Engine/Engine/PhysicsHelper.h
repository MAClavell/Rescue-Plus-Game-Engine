#pragma once
#include <DirectXMath.h>
#include "Bullet3\src\btBulletDynamicsCommon.h"

class PhysicsHelper
{
	public:

		//Turn a XMFLOAT3 vector3 into a btVector3
		static btVector3 XMVec3ToBtVec3(DirectX::XMFLOAT3 vec3)
		{
			return btVector3(vec3.x, vec3.y, vec3.z);
		}

		//Turn a btQuaternion into a XMFLOAT4 quaternion
		static DirectX::XMFLOAT3 BtVec3ToXMVec3(btVector3 vec3)
		{
			return DirectX::XMFLOAT3(vec3.x(), vec3.y(), vec3.z());
		}

		//Turn a XMFLOAT4 quaternion into a btQuaternion
		static btQuaternion XMQuatToBtQuat(DirectX::XMFLOAT4 quat)
		{
			return btQuaternion(quat.x, quat.y, quat.z, quat.w);
		}

		//Turn a btQuaternion into a XMFLOAT4 quaternion
		static DirectX::XMFLOAT4 BtQuatToXMQuat(btQuaternion quat)
		{
			return DirectX::XMFLOAT4(quat.x(), quat.y(), quat.z(), quat.w());
		}
};