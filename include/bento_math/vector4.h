#pragma once
// nyx includes
#include "types.h"

namespace bento
{
	// inline methods
		// Constructors
	inline Vector4 vector4(const float _x, const float _y, const float _z, const float _w);
	inline Vector4 vector4(const Vector3& vec);
	inline Vector4 vector4(const float _v);

		// Operators
	inline Vector4 operator*(const Vector4& _v1, float parFactor);
	inline Vector4 operator/(const Vector4& _v1, float parFactor);
	inline Vector4 operator+(const Vector4& _v1, const Vector4& _v2);
	inline Vector4 operator-(const Vector4& _v1, const Vector4& _v2);

		// Canonic operations
	inline float dot(const Vector4& _v1, const Vector4& _v2);
	inline Vector4 cross(const Vector4& _v1, const Vector4& _v2);
	inline float length(const Vector4& _v1);
	inline float length2(const Vector4& _v1);
	inline Vector4 normalize(const Vector4& _vec);

	// Default values;
	extern const Vector4 v4_ZERO;
	extern const Vector4 v4_X;
	extern const Vector4 v4_Y;
	extern const Vector4 v4_Z;
	extern const Vector4 v4_W;
}

#include "vector4.inl"
