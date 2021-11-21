#pragma once

// bento includes
#include <bento_math/types.h>

namespace bento
{
	struct Ray
	{
		Vector3 orig;
		Vector3 dir;
		Vector3 inv_dir;
	};
	Ray ray(const Vector3& orig, const Vector3& dir);
}