#pragma once

// bento includes
#include <bento_math/types.h>
#include <bento_math/vector3.h>

namespace bento
{
	struct Box3
	{
		Vector3 min;
		Vector3 max;
	};

	namespace box3
	{
		// Reset and invalidate a box
		void reset(Box3& box);

		// Evaluate if the box is valid
		bool valid(const Box3& box);

		// Compute the center of the box
		Vector3 center(const Box3& box);

		// Compute the diagonal of the box
		Vector3 diagonal(const Box3& box);

		// Evaluate the surface area of the box
		float surface_area(const Box3& box);

		// include a single point into the box3
		void include_point(Box3& box, const Vector3& _pos);

		// include a point array into the box3
		void include_points(Box3& box, const Vector3* point_array, uint32_t num_points);

		// include a box into this box
		void include_box(Box3& host, const Box3& target);

		// Apply a matrix transformation to a box
		Box3 transform(const Box3& target_box, const Matrix4& tm);
	}
}