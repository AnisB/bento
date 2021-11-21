// Bento includes
#include "bento_math/vector3.h"
#include "bento_math/matrix4.h"
#include "bento_rt/box3.h"

// External includes
#include <float.h>

namespace bento
{
	namespace box3
	{
		void reset(Box3& box)
		{
			box.min = bento::vector3(FLT_MAX, FLT_MAX, FLT_MAX);
			box.max = -bento::vector3(FLT_MAX, FLT_MAX, FLT_MAX);
		}

		bool valid(const Box3& box)
		{
			return box.min.x < box.max.x && box.min.y < box.max.y && box.min.z < box.max.z;
		}

		Vector3 center(const Box3& box)
		{
			return (box.max + box.min) / 2.0f;
		}

		Vector3 diagonal(const Box3& box)
		{
			return box.max - box.min;
		}

		float surface_area(const Box3& box)
		{
			if (box.max.x < box.min.x || box.max.y < box.min.y || box.max.z < box.min.z)
				return 0.0f;
			const bento::Vector3& diag = diagonal(box);
			return 2.0f * diag.x * diag.y + 2.0f * diag.x * diag.z + 2.0f * diag.z * diag.y;
		}

		void include_point(Box3& box, const bento::Vector3& _pos)
		{
			// Override the minimum
			box.min.x = _pos.x < box.min.x ? _pos.x : box.min.x;
			box.min.y = _pos.y < box.min.y ? _pos.y : box.min.y;
			box.min.z = _pos.z < box.min.z ? _pos.z : box.min.z;

			// Override the maximum
			box.max.x = _pos.x > box.max.x ? _pos.x : box.max.x;
			box.max.y = _pos.y > box.max.y ? _pos.y : box.max.y;
			box.max.z = _pos.z > box.max.z ? _pos.z : box.max.z;
		}

		void include_points(Box3& box, const bento::Vector3* point_array, uint32_t num_points)
		{
			for (uint32_t idx = 0; idx < num_points; ++idx)
			{
				include_point(box, point_array[idx]);
			}
		}

		void include_box(Box3& host, const Box3& target)
		{
			if (valid(target))
				include_points(host, &target.min, 2);
		}

		Box3 transform(const Box3 & _inputBox, const bento::Matrix4 & _mat)
		{
			// Get the translation vector
			const bento::Vector3& translate = GetTranslate(_mat);

			// Create the output box
			Box3 new_box;
			new_box.min = translate;
			new_box.max = translate;

			float av, bv;
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					av = _mat.m[i * 4 + j] * at_index(_inputBox.min, j);
					bv = _mat.m[i * 4 + j] * at_index(_inputBox.max, j);

					if (av < bv)
					{
						at_index(new_box.min, i) += av;
						at_index(new_box.max, i) += bv;

					}
					else
					{
						at_index(new_box.min, i) += bv;
						at_index(new_box.max, i) += av;
					}
				}
			}
			return new_box;
		}
	}
}