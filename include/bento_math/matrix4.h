#pragma once

// Library includes
#include "vector3.h"
#include "vector4.h"

// STL includes
#include <iostream>

namespace bento
{
	// Init Functions
	void matrix4(Matrix4& _mat, float* _values);

	// Reset functions
	void SetIdentity(Matrix4& _matrix);
	void ResetToZero(Matrix4& _matrix);
	void AsPerspective(Matrix4& _mat, float parFovy, float parAspect, float parNear, float parFar);

	// Access functions
	Vector3 GetTranslate(const Matrix4& _matrix);
	Vector3 XAxis(const Matrix4& _matrix);
	Vector3 YAxis(const Matrix4& _matrix);
	Vector3 ZAxis(const Matrix4& _matrix);
	const Vector4* XRow(const Matrix4& _matrix);
	const Vector4* YRow(const Matrix4& _matrix);
	const Vector4* ZRow(const Matrix4& _matrix);
	const Vector4* WRow(const Matrix4& _matrix);
	
	// Computing functions
	float Det(const Matrix4& _matrix);
	Matrix3 Inverse3x3(const Matrix4& _mat);
	Matrix4 Inverse(const Matrix4& _target);
	Matrix4 Translate_M4(const Vector4& parVector);
	Matrix4 Translate_M4(const Vector3& parVector);

	Matrix4 Rotate(float parAngle, const Vector3& parAxis);
	void ToTable(const Matrix4& _matrix, float* content);

	Matrix4 RotateXAxis(float parAngle);
	Matrix4 RotateYAxis(float parAngle);
	Matrix4 RotateZAxis(float parAngle);

	Matrix4 transpose(const Matrix4& target_mat);

	Vector4 operator*(const Matrix4& _mat, const Vector4& _fac);
	Vector3 operator*(const Matrix4& _mat, const Vector3& _fac);
	Matrix4 operator*(const Matrix4& _mat1, const Matrix4& _mat2);
	Matrix4& Set(Matrix4& _target, const Matrix4& parMatrix);

	std::ostream& operator<< (std::ostream& os, const Matrix4& obj); 
}