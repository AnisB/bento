#pragma once

// SDK includes
#include <bento_base/platform.h>

// SIMD include
#include <xmmintrin.h>

// Remove those shitty macros
#undef min
#undef max

namespace bento {

	// Shuffle macros
	#define SHUFFLE3(V, X,Y,Z) float4({_mm_shuffle_ps((V).m, (V).m, _MM_SHUFFLE(Z,Z,Y,X))})
	#define SHUFFLE4(V, X,Y,Z,W) float4({_mm_shuffle_ps((V).m, (V).m, _MM_SHUFFLE(W,Z,Y,X))})

	struct float4
	{
		__m128 m;
	};

	// Constructors
	float4 build_float4(float v);
	float4 build_float4(float x, float y, float z);
	float4 build_float4(float x, float y, float z, float w);

	// Direct access to elements
	__forceinline float& element(float4& v, uint32_t i);
	__forceinline const float& element(const float4& v, uint32_t i);
	__forceinline float x_element(float4 v);
	__forceinline float y_element(float4 v);
	__forceinline float z_element(float4 v);
	__forceinline float w_element(float4 v);

	// Operators
	__forceinline float4 operator+ (const float4& a, const float4& b);
	__forceinline float4 operator- (const float4& a, const float4& b);
	__forceinline float4 operator* (const float4& a, const float4& b);
	__forceinline float4 operator/ (const float4& a, const float4& b);
	__forceinline float4 operator* (const float4& a, float b);
	__forceinline float4 operator/ (const float4& a, float b);
	__forceinline float4 operator* (float a, const float4& b);
	__forceinline float4 operator/ (float a, const float4& b);
	__forceinline float4& operator+= (float4 &a, const float4& b);
	__forceinline float4& operator-= (float4 &a, const float4& b);
	__forceinline float4& operator*= (float4 &a, const float4& b);
	__forceinline float4& operator/= (float4 &a, const float4& b);
	__forceinline float4& operator*= (float4 &a, float b);
	__forceinline float4& operator/= (float4 &a, float b);
	__forceinline float4 operator==(const float4& a, const float4& b);
	__forceinline float4 operator!=(float4 a, const float4& b);
	__forceinline float4 operator< (const float4& a, const float4& b);
	__forceinline float4 operator> (const float4& a, const float4& b);
	__forceinline float4 operator<=(const float4& a, const float4& b);
	__forceinline float4 operator>=(const float4& a, const float4& b);
	__forceinline float4 min(float4 a, float4 b);
	__forceinline float4 max(float4 a, float4 b);

	__forceinline float min_xyz(float4 v);
	__forceinline float max_xyz(float4 v);
}

#include "float4.inl"
