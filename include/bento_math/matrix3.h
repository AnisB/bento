#pragma once

// nyx inludes
#include "types.h"

namespace bento {
	void set_identity(Matrix3& _mat);
	void set_zero(Matrix3& _mat);
	Matrix3 matrix3(const Matrix4& _mat);

	float det(Matrix3& _mat);
	Matrix3 inverse(const Matrix3& mat);
	Matrix3 transpose(const Matrix3& mat);

	// Operators
	Vector3 operator*(const Matrix3& _matrix, const Vector3& _vector);
}