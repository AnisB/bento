// nyx includes
#include "bento_math/matrix3.h"

// External includes
#include <algorithm>

namespace bento {

	void set_identity(Matrix3& mat)
	{
		mat.m[0] = 1.0;
		mat.m[1] = 0.0;
		mat.m[2] = 0.0;
		
		mat.m[3] = 0.0;
		mat.m[4] = 1.0;
		mat.m[5] = 0.0;
		
		mat.m[6] = 0.0;
		mat.m[7] = 0.0;
		mat.m[8] = 1.0;
	}

	void set_zero(Matrix3& _mat)
	{
		std::fill_n(_mat.m, 9, 0.0f);
	}

	Matrix3 matrix3(const Matrix4& mat)
	{
		Matrix3 result;
		result.m[0] = mat.m[0];
		result.m[1] = mat.m[1];
		result.m[2] = mat.m[2];

		result.m[3] = mat.m[4];
		result.m[4] = mat.m[5];
		result.m[5] = mat.m[6];

		result.m[6] = mat.m[8];
		result.m[7] = mat.m[9];
		result.m[8] = mat.m[10];
		return result;
	}

	float det(const Matrix3& _mat)
	{
		return _mat.m[0]*(_mat.m[4]*_mat.m[8]-_mat.m[7]*_mat.m[5])
	           -_mat.m[1]*(_mat.m[3]*_mat.m[8]-_mat.m[5]*_mat.m[6])
	           +_mat.m[2]*(_mat.m[3]*_mat.m[7]-_mat.m[4]*_mat.m[6]);
	}

	Matrix3 inverse(const Matrix3& _mat)
	{
		float invdet = 1.0f/det(_mat);

		Matrix3 inverseMatrix;
		inverseMatrix.m[0] = (_mat.m[4] * _mat.m[8] - _mat.m[7] * _mat.m[5])*invdet;
		inverseMatrix.m[1] = -(_mat.m[3] * _mat.m[8] - _mat.m[5] * _mat.m[6])*invdet;
		inverseMatrix.m[2] = (_mat.m[3] * _mat.m[7] - _mat.m[6] * _mat.m[4])*invdet;

		inverseMatrix.m[3] = -(_mat.m[1] * _mat.m[8] - _mat.m[2] * _mat.m[7])*invdet;
		inverseMatrix.m[4] = (_mat.m[0] * _mat.m[8] - _mat.m[2] * _mat.m[6])*invdet;
		inverseMatrix.m[5] = -(_mat.m[0] * _mat.m[7] - _mat.m[6] * _mat.m[1])*invdet;

		inverseMatrix.m[6] = (_mat.m[1] * _mat.m[5] - _mat.m[2] * _mat.m[4])*invdet;
		inverseMatrix.m[7] = -(_mat.m[0]*_mat.m[5]-_mat.m[3]*_mat.m[2])*invdet;
		inverseMatrix.m[8] =  (_mat.m[0]*_mat.m[4]-_mat.m[3]*_mat.m[1])*invdet;
		
		return transpose(inverseMatrix);
	}

	Matrix3 transpose(const Matrix3& mat)
	{
		Matrix3 result = mat;
		std::swap(result.m[1], result.m[3]);
		std::swap(result.m[2], result.m[6]);
		std::swap(result.m[5], result.m[7]);
		return result;

	}

	Vector3 operator*(const Matrix3& _mat, const Vector3& _vec)
	{
		Vector3 result;
		result.x = _mat.m[0]*_vec.x + _mat.m[1]*_vec.y + _mat.m[2]*_vec.z;
		result.y = _mat.m[3]*_vec.x + _mat.m[4]*_vec.y + _mat.m[5]*_vec.z;
		result.z = _mat.m[6]*_vec.x + _mat.m[7]*_vec.y + _mat.m[8]*_vec.z;
		return result;
	}
}