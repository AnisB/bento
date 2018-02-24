namespace bento
{
	__forceinline float4 build_float4(float v)
	{
		_mm_set_ps(v, v, v, v);
	}

	__forceinline float4 build_float4(float x, float y, float z)
	{
		_mm_set_ps(0.0f, z, y, x);
	}

	__forceinline float4 build_float4(float x, float y, float z, float w)
	{
		_mm_set_ps(w, z, y, x);
	}

	__forceinline float& element(float4& v, uint32_t i) 
	{ 
		return v.m.m128_f32[i]; 
	}

	__forceinline const float& element(const float4& v, uint32_t i)
	{ 
		return v.m.m128_f32[i];
	}

	__forceinline float x_element(float4 v)
	{
		return _mm_cvtss_f32(v.m);
	}

	__forceinline float y_element(float4 v)
	{ 
		return _mm_cvtss_f32(_mm_shuffle_ps(v.m, v.m, _MM_SHUFFLE(1, 1, 1, 1)));
	}

	__forceinline float z_element(float4 v)
	{
		return _mm_cvtss_f32(_mm_shuffle_ps(v.m, v.m, _MM_SHUFFLE(2, 2, 2, 2))); 
	}

	__forceinline float w_element(float4 v)
	{ 
		return _mm_cvtss_f32(_mm_shuffle_ps(v.m, v.m, _MM_SHUFFLE(3, 3, 3, 3)));
	}

	__forceinline float4 operator+ (const float4& a, const float4& b)
	{ 
		return {_mm_add_ps(a.m, b.m)};
	}

	__forceinline float4 operator- (const float4& a, const float4& b)
	{ 
		return { _mm_sub_ps(a.m, b.m) };
	}

	__forceinline float4 operator* (const float4& a, const float4& b)
	{ 
		return { _mm_mul_ps(a.m, b.m)};
	}

	__forceinline float4 operator/ (const float4& a, const float4& b)
	{ 
		return { _mm_div_ps(a.m, b.m)};
	}

	__forceinline float4 operator* (const float4& a, float b)
	{ 
		return { _mm_mul_ps(a.m, _mm_set1_ps(b))};
	}

	__forceinline float4 operator/ (const float4& a, float b)
	{ 
		return { _mm_div_ps(a.m, _mm_set1_ps(b))};
	}

	__forceinline float4 operator* (float a, const float4& b)
	{ 
		return { _mm_mul_ps(_mm_set1_ps(a), b.m) };
	}

	__forceinline float4 operator/ (float a, const float4& b)
	{ 
		return{ _mm_div_ps(_mm_set1_ps(a), b.m) };
	}

	__forceinline float4& operator+= (float4 &a, const float4& b)
	{ 
		a = a + b; return a;
	}

	__forceinline float4& operator-= (float4 &a, const float4& b)
	{ 
		a = a - b; return a;
	}

	__forceinline float4& operator*= (float4 &a, const float4& b)
	{ 
		a = a * b; return a;
	}

	__forceinline float4& operator/= (float4 &a, const float4& b)
	{ 
		a = a / b; return a;
	}

	__forceinline float4& operator*= (float4 &a, float b)
	{ 
		a = a * b; return a;
	}

	__forceinline float4& operator/= (float4 &a, float b)
	{ 
		a = a / b; return a;
	}

	__forceinline float4 operator==(const float4& a, const float4& b)
	{ 
		return { _mm_cmpeq_ps(a.m, b.m)};
	}

	__forceinline float4 operator!=(const float4& a, const float4& b)
	{ 
		return { _mm_cmpneq_ps(a.m, b.m)};
	}

	__forceinline float4 operator< (const float4& a, const float4& b)
	{ 
		return { _mm_cmplt_ps(a.m, b.m)};
	}

	__forceinline float4 operator> (const float4& a, const float4& b)
	{ 
		return { _mm_cmpgt_ps(a.m, b.m)};
	}

	__forceinline float4 operator<=(const float4& a, const float4& b)
	{ 
		return { _mm_cmple_ps(a.m, b.m)};
	}

	__forceinline float4 operator>=(const float4& a, const float4& b)
	{ 
		return { _mm_cmpge_ps(a.m, b.m)};
	}

	__forceinline float4 min(float4 a, float4 b)
	{ 
		return { _mm_min_ps(a.m, b.m)};
	}

	__forceinline float4 max(float4 a, float4 b)
	{ 
		return { _mm_max_ps(a.m, b.m)};
	}

	__forceinline float min_xyz(float4 v)
	{
		v = min(v, SHUFFLE3(v, 1, 0, 2));
		return x_element((min(v, SHUFFLE3(v, 2, 0, 1))));
	}

	__forceinline float max_xyz(float4 v)
	{
		v = max(v, SHUFFLE3(v, 1, 0, 2));
		return x_element((v, SHUFFLE3(v, 2, 0, 1)));
	}
}