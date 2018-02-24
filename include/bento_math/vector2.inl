#include <math.h>

namespace bento{

	// Constructors;
	Vector2 vector2(const float _x, const float _y)
	{
		Vector2 out;
		out.x = _x;
		out.y = _y;
		return out;
	}
	Vector2 vector2(const float _v)
	{
		Vector2 out;
		out.x = _v;
		out.y = _v;
		return out;
	}

	// Operators
	Vector2 operator*(const Vector2& _v1, float _factor)
	{
		Vector2 out(_v1);
		out.x *= _factor;
		out.y *= _factor;
		return out;
	}
	Vector2 operator/(const Vector2& _v1, float _factor)
	{
		Vector2 out(_v1);
		out.x /= _factor;
		out.y /= _factor;
		return out;
	}

	Vector2 operator+(const Vector2& _v1, const Vector2& _v2)
	{
		return vector2(_v1.x + _v2.x, _v1.y + _v2.y);
	}

	Vector2 operator-(const Vector2& _v1, const Vector2& _v2)
	{
		return vector2(_v1.x - _v2.x, _v1.y - _v2.y);
	}

	// Cannonic operations
	float dotProd(const Vector2& _v1, const Vector2& _v2)
	{
		return _v1.x * _v2.x + _v1.y * _v2.y;
	}

	float length(const Vector2& _v1)
	{
		return sqrtf(_v1.x * _v1.x + _v1.y * _v1.y);
	}

	float length2(const Vector2& _v1)
	{
		return (_v1.x * _v1.x + _v1.y * _v1.y);
	}
}