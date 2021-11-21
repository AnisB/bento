// bento includes
#include <bento_rt/ray.h>

namespace bento
{
	Ray ray(const bento::Vector3& orig, const bento::Vector3& dir)
	{
		Ray ray;
		ray.orig = orig;
		ray.dir = dir;
		ray.inv_dir = { 1.0f / dir.x, 1.0f / dir.y, 1.0f / dir.z };
		return ray;
	}
}