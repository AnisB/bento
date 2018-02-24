#pragma once

// Library includes
#include "bento_base/platform.h"

namespace bento {
	struct Vector2
	{
		float x,y;
	};

	struct Vector3
	{
		float x,y,z;
	};

	struct Vector4
	{
		float x,y,z,w;
	};

	struct IVector2
	{
		uint32_t x,y;
	};
	
	struct IVector3
	{
		uint32_t x,y,z;
	};
	
	struct IVector4
	{
		uint32_t x,y,z,w;
	};
	
	struct Matrix3
	{
		float m[9];
	};
	
	struct Matrix4
	{
		float m[16];
	};
}