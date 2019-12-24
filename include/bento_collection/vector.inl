
namespace bento
{
	template <typename T>
	Vector<T>::Vector(IAllocator& allocator)
	: _allocator(&allocator)
	, _size(0)
	, _capacity(0)
	, _data(nullptr)
	{

	}

	template <typename T>
	Vector<T>::Vector(NoAllocator&)
	: _allocator(nullptr)
	, _size(0)
	, _capacity(0)
	, _data(nullptr)
	{

	}

	template <typename T>
	Vector<T>::Vector(IAllocator& allocator, uint32_t size)
	: _allocator(&allocator)
	, _size(0)
	, _capacity(0)
	, _data(nullptr)
	{
		resize(size);
	}

	template <typename T>
	Vector<T>::~Vector()
	{
		free();
	}

	template <typename T>
	void Vector<T>::resize(uint32_t size)
	{
		if(size ==  0)
		{
			clear();
		}
		else if(size < _size)
		{
			if(!std::is_trivially_constructible<T>())
			{
				// Extra elements in the vector remove them
				for(uint32_t ele_idx = size; ele_idx < _size; ++ele_idx)
				{
					_data[ele_idx].~T();
				}
			}

			_size = size;
		}
		else if (size < _capacity)
		{
			if(!std::is_trivially_constructible<T>())
			{
				// Enough capacity to handle this, construct the new elements
				for(uint32_t ele_idx = _size; ele_idx < size; ++ele_idx)
				{
					construct(&_data[ele_idx], IS_ALLOCATOR_BASED_TYPE(T)());
				}
			}
			_size = size;
		}
		else
		{
			// We need to increase our capacity, it is not big enough
			reserve(size);
			if(!std::is_trivially_constructible<T>())
			{
				for(uint32_t ele_idx = _size; ele_idx < size; ++ele_idx)
				{
					construct(&_data[ele_idx], IS_ALLOCATOR_BASED_TYPE(T)());
				}
			}
			_size = size;
		}
	}

	template <typename T>
	void Vector<T>::clear()
	{
		if(!std::is_trivially_constructible<T>())
		{
			for(uint32_t ele_idx = 0; ele_idx < _size; ++ele_idx)
			{
				_data[ele_idx].~T();
			}
		}
		_size = 0;
	}

	template <typename T>
	void Vector<T>::free()
	{
		if(_capacity)
		{
			clear();
			_allocator->deallocate(_data);
			_capacity = 0;
		}
	}

	template <typename T>
	void Vector<T>::reserve(uint32_t size)
	{
		size_t allocCount = (size_t)(_capacity + size);
		void* ptr = _allocator->allocate(sizeof(T) * allocCount, 4);
		memcpy(ptr, _data, sizeof(T) * _size);
		if(_data != nullptr)
		{
			_allocator->deallocate(_data);
		}
		_data = static_cast<T*>(ptr);
		_capacity = (_capacity + size);
	}


	template <typename T>
	void Vector<T>::push_back(const T& _value)
	{
		if(_size == _capacity)
		{
			reserve(_capacity * 2 + 10);
		}
		
		if(!std::is_trivially_constructible<T>())
		{
			// construct the new element
			construct(&_data[_size], IS_ALLOCATOR_BASED_TYPE(T)());
		}
		
		_data[_size] = _value;

		// Increase the size
		_size++;
	}

	// Copy operator
	template <typename T>
	void Vector<T>::operator=(const Vector<T>& vec)
	{
		uint32_t final_size = vec.size();
		resize(final_size);
		for(uint32_t ele_idx = 0; ele_idx < final_size; ++ele_idx)
		{
			_data[ele_idx] = vec[ele_idx];
		}
	}

	// Creates a new element and returns a pointer to it
	template <typename T>
	T& Vector<T>::extend()
	{
		if (_size == _capacity)
		{
			reserve(_capacity * 2 + 10);
		}

		if (!std::is_trivially_constructible<T>())
		{
			// construct the new element
			construct(&_data[_size], IS_ALLOCATOR_BASED_TYPE(T)());
		}

		// Increase the size
		_size++;

		return _data[_size - 1];
	}
}