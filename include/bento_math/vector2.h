#pragma once

#include "types.h"

namespace bento
{
	// inline methods
		// Constructors
	inline Vector2 vector2(const float _x,const float _y);
	inline Vector2 vector2(const float _v);

		// Operators
	inline Vector2 operator*(const Vector2& _v1, float parFactor);
	inline Vector2 operator/(const Vector2& _v1, float parFactor);
	inline Vector2 operator+(const Vector2& _v1, const Vector2& _v2);
	inline Vector2 operator-(const Vector2& _v1, const Vector2& _v2);

		// Cannonic operations
	inline float dotProd(const Vector2& _v1, const Vector2& _v2);
	inline float length(const Vector2& _v1);
	inline float length2(const Vector2& _v1);

	// Default values;
	extern const Vector2 v2_ZERO;
	extern const Vector2 v2_X;
	extern const Vector2 v2_Y;
	extern const Vector2 v2_Z;
}

#include "vector2.inl"