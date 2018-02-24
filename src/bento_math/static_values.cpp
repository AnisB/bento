#include "bento_math/vector2.h"
#include "bento_math/vector3.h"
#include "bento_math/vector4.h"

namespace bento
{
	// Default values;
	const Vector2 v2_ZERO = vector2(0.0, 0.0);
	const Vector2 v2_X = vector2(1.0, 0.0);
	const Vector2 v2_Y = vector2(0.0, 1.0);

	// Default values;
	const Vector3 v3_ZERO = vector3(0.0, 0.0, 0.0);
	const Vector3 v3_X = vector3(1.0, 0.0, 0.0);
	const Vector3 v3_Y = vector3(0.0, 1.0, 0.0);
	const Vector3 v3_Z = vector3(0.0, 0.0, 1.0);

	// Default values;
	const Vector4 v4_ZERO = vector4(0.0, 0.0, 0.0, 0.0);
	const Vector4 v4_X = vector4(1.0, 0.0, 0.0, 0.0);
	const Vector4 v4_Y = vector4(0.0, 1.0, 0.0, 0.0);
	const Vector4 v4_Z = vector4(0.0, 0.0, 1.0, 0.0);
	const Vector4 v4_W = vector4(0.0, 0.0, 0.0, 1.0);
}