#pragma once

// bento includes
#include <bento_math/types.h>
#include <bento_math/float4.h>

namespace bento
{
	// Intersection of a ray with a box
	bool intersect(const Vector3& ray_origin, const Vector3& ray_inv_dir, const Vector3& box_min, const Vector3& box_max, float& dist);

	// Intersection of a ray with a triangle
	bool intersect(const Vector3& ray_origin, const Vector3& ray_dir, const Vector3& v0, const Vector3& v1, const Vector3& v2, float& dist);

	// Intersection of a ray with a box (simd)
	bool intersect_simd(const float4& ray_orig, const float4& ray_inv_dir, float4& box_min, const float4& box_max, float dist);
}