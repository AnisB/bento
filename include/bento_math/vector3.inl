#include <math.h>

namespace bento
{
	// Constructors;
	Vector3 vector3(const float _x, const float _y, const float _z)
	{
		Vector3 out;
		out.x = _x;
		out.y = _y;
		out.z = _z;
		return out;
	}
	Vector3 vector3(const float _v)
	{
		Vector3 out;
		out.x = _v;
		out.y = _v;
		out.z = _v;
		return out;
	}

	// Operators
	Vector3 operator*(const Vector3& _v1, float _factor)
	{
		return vector3(_v1.x*_factor,  _v1.y*_factor, _v1.z*_factor);
	}
	Vector3 operator*(const Vector3& _v1, const Vector3& _v2)
	{
		return vector3(_v1.x*_v2.x,  _v1.y*_v2.y, _v1.z*_v2.z);
	}

	Vector3 operator/(const Vector3& _v1, float _factor)
	{
		return vector3(_v1.x/_factor,  _v1.y/_factor, _v1.z/_factor);
	}

	Vector3 operator+(const Vector3& _v1, const Vector3& _v2)
	{
		return vector3(_v1.x + _v2.x, _v1.y + _v2.y, _v1.z + _v2.z);
	}

	Vector3 operator-(const Vector3& _v1, const Vector3& _v2)
	{
		return vector3(_v1.x - _v2.x, _v1.y - _v2.y, _v1.z - _v2.z);
	}
	Vector3 operator-(const Vector3& _v1)
	{
		return vector3(-_v1.x,  -_v1.y, -_v1.z);
	}
	
	// Cannonic operations
	float dot(const Vector3& _v1, const Vector3& _v2)
	{
		return _v1.x * _v2.x + _v1.y * _v2.y + _v1.z * _v2.z;
	}

	Vector3 cross(const Vector3& _v1, const Vector3& _v2)
	{
		return vector3(_v1.y*_v2.z - _v1.z * _v2.y, _v1.z*_v2.x - _v1.x * _v2.z, _v1.x*_v2.y - _v1.y * _v2.x);
	}

	float len(const Vector3& _v1)
	{
		return sqrtf(_v1.x * _v1.x + _v1.y * _v1.y + _v1.z * _v1.z);
	}

	float len_squared(const Vector3& _v1)
	{
		return (_v1.x * _v1.x + _v1.y * _v1.y + _v1.z * _v1.z);
	}

	Vector3 normalize(const Vector3& _v1)
	{
		return _v1 / len(_v1);
	}
		// Index access
	float& at_index(Vector3& _val, int _index)
	{
		return _index == 0 ? _val.x : (_index == 1 ? _val.y : _val.z);
	}

	const float& at_index(const Vector3& _val, int _index)
	{
		return _index == 0 ? _val.x : (_index == 1 ? _val.y : _val.z);
	}
}