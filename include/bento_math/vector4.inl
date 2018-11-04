namespace bento {

	// Constructors;
	Vector4 vector4(const float _x, const float _y, const float _z, const float _w)
	{
		Vector4 out;
		out.x = _x;
		out.y = _y;
		out.z = _z;
		out.w = _w;
		return out;
	}

	Vector4 vector4(const Vector3& vec)
	{
		Vector4 out;
		out.x = vec.x;
		out.y = vec.y;
		out.z = vec.z;
		out.w = 0.0f;
		return out;
	}

	Vector4 vector4(const float _v)
	{
		Vector4 out;
		out.x = _v;
		out.y = _v;
		out.z = _v;
		out.w = _v;
		return out;
	}

	// Operators
	Vector4 operator*(const Vector4& _v1, float _factor)
	{
		return vector4(_v1.x*_factor,  _v1.y*_factor, _v1.z*_factor, _v1.w*_factor);
	}

	Vector4 operator/(const Vector4& _v1, float _factor)
	{
		return vector4(_v1.x/_factor,  _v1.y/_factor, _v1.z/_factor, _v1.w/_factor);
	}

	Vector4 operator+(const Vector4& _v1, const Vector4& _v2)
	{
		return vector4(_v1.x + _v2.x, _v1.y + _v2.y, _v1.z + _v2.z,  _v1.w + _v2.w);
	}

	Vector4 operator-(const Vector4& _v1, const Vector4& _v2)
	{
		return vector4(_v1.x - _v2.x, _v1.y - _v2.y, _v1.z - _v2.z, _v1.w - _v2.w);
	}

	// Cannonic operations
	float dot(const Vector4& _v1, const Vector4& _v2)
	{
		return _v1.x * _v2.x + _v1.y * _v2.y + _v1.z * _v2.z+ _v1.w * _v2.w;
	}

	Vector4 cross(const Vector4& _v1, const Vector4& _v2)
	{
		return vector4(_v1.y*_v2.z - _v1.z * _v2.y, _v1.z*_v2.x - _v1.x * _v2.z, _v1.x*_v2.y - _v1.y * _v2.x, 0.0);
	}

	float length(const Vector4& _v1)
	{
		return sqrtf(_v1.x * _v1.x + _v1.y * _v1.y + _v1.z * _v1.z + _v1.w * _v1.w);
	}

	float length2(const Vector4& _v1)
	{
		return (_v1.x * _v1.x + _v1.y * _v1.y + _v1.z * _v1.z + _v1.w * _v1.w);
	}

	inline Vector4 normalize(const Vector4& _vec)
	{
		const float norm = length(_vec);
		return vector4(_vec.x / norm, _vec.y / norm, _vec.z / norm, _vec.w / norm);
	}
}