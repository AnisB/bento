// Bento includes
#include <bento_math/vector3.h>
#include <bento_rt/intersect.h>

// External includes
#include <algorithm>

namespace bento
{
	bool intersect(const Vector3& ray_origin, const Vector3& inv_dir, const Vector3& box_min, const Vector3& box_max, float& dist)
	{
		// X Component
		float t1 = (box_min.x - ray_origin.x) * inv_dir.x;
		float t2 = (box_max.x - ray_origin.x) * inv_dir.x;
		float tmin = std::min(t1, t2);
		float tmax = std::max(t1, t2);
 
		// Y Component
		t1 = (box_min.y - ray_origin.y) * inv_dir.y;
		t2 = (box_max.y - ray_origin.y) * inv_dir.y;
		tmin = std::max(tmin, std::min(t1, t2));
		tmax = std::min(tmax, std::max(t1, t2));

		// Z Component
		t1 = (box_min.z - ray_origin.z) * inv_dir.z;
		t2 = (box_max.z - ray_origin.z) * inv_dir.z;
		tmin = std::max(tmin, std::min(t1, t2));
		tmax = std::min(tmax, std::max(t1, t2));

		// If valid, override and return
		bool valid = tmax > std::max(tmin, 0.0f);
		dist = valid ? tmin : dist;
		return valid;
	}

	bool intersect(const Vector3& ray_origin, const Vector3& ray_dir, const Vector3& v0, const Vector3& v1, const Vector3& v2, float& dist)
	{
		const Vector3& e_1 = v1 - v0;
		const Vector3& e_2 = v2 - v0;
		const Vector3& u_normal = cross(e_1, e_2);
		const Vector3& q = cross(ray_dir, e_2);
		const float det = dot(e_1, q);

		if ((dot(u_normal, ray_dir) >= 0) || (std::abs(det) <= FLT_EPSILON)) return false;
		const Vector3& s = (ray_origin - v0) / det;
		const Vector3& r = cross(s, e_1);

		float u = dot(s, q);
		float v = dot(r, ray_dir);
		float w = 1.0f - u - v;

		if ((u < 0.0f) || (v < 0.0f) || (w < 0.0f)) return false;

		float candidate_dist = dot(e_2, r);

		if (candidate_dist >= 0.0f && candidate_dist <= dist) {
			dist = candidate_dist;
			return true;
		}
		return false;
	}

	bool intersect_simd(const float4& ray_orig, const float4& ray_inv_dir, const float4& box_min, const float4& box_max, float dist)
	{
		float4 d0 = (box_min - ray_orig) * ray_inv_dir;
		float4 d1 = (box_max - ray_orig) * ray_inv_dir;

		float4 v0 = min(d0, d1);
		float4 v1 = max(d0, d1);

		float tmin = max_xyz(v0);
		float tmax = min_xyz(v1);

		bool hit = (tmax >= 0) && (tmax >= tmin) && (tmin <= dist);
		if (hit)
			dist = tmin;
		return hit;
	}
}