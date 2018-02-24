#pragma once

#include "types.h"

namespace bento
{
	// inline methods
		// Constructors
	inline Vector3 vector3(const float _x,const float _y, const float _z);
	inline Vector3 vector3(const float _v);

		// Index access
	inline float& at_index(Vector3& _val, int _index);
	inline const float& at_index(const Vector3& _val, int _index);

		// Operators
	inline Vector3 operator*(const Vector3& _v1, float parFactor);
	inline Vector3 operator*(const Vector3& _v1, const Vector3& _v2);
	inline Vector3 operator/(const Vector3& _v1, float parFactor);
	inline Vector3 operator+(const Vector3& _v1, const Vector3& _v2);
	inline Vector3 operator-(const Vector3& _v1, const Vector3& _v2);
	inline Vector3 operator-(const Vector3& _v1);

		// Cannonic operations
	inline float dot(const Vector3& _v1, const Vector3& _v2);
	inline Vector3 cross(const Vector3& _v1, const Vector3& _v2);
	inline float len(const Vector3& _v1);
	inline float len_squared(const Vector3& _v1);
	inline Vector3 normalize(const Vector3& _v1);

	// Default values;
	extern const Vector3 v3_ZERO;
	extern const Vector3 v3_X;
	extern const Vector3 v3_Y;
	extern const Vector3 v3_Z;
}

#include "vector3.inl"
