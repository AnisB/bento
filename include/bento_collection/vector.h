#pragma once

// Library includes
#include "bento_base/platform.h"
#include "bento_memory/common.h"

// External includes
#include <type_traits>

namespace bento {
	
	template <typename T>
	class Vector
	{
	public:
		ALLOCATOR_BASED;

		// Type definition
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef pointer iterator;
		typedef const_pointer const_iterator;

		// Default constructor
		Vector(IAllocator& allocator);

		// Allocator without allocator
		Vector(NoAllocator&);

		// Size aware constructor
		Vector(IAllocator& allocator, uint32_t size);

		// Dst
		~Vector();

		// Accessors
		inline uint32_t size() const {return _size;}
		inline uint32_t capacity() const {return _capacity;}

		// Resize the array
		void resize(uint32_t size);

		// Free the memory
		void free();

		// set the size to 0
		void clear();

		// Reserve memory in the array
		void reserve(uint32_t _space);

		// Copy operator
		void operator=(const Vector<T>& vec);

		inline reference operator[](uint32_t index)
		{
			return _data[index];
		}

		inline const_reference operator[] (uint32_t index) const
		{
			return _data[index];
		}

		// Append an element in the vector
		void push_back(const T& _value);

		// Creates a new element and returns a pointer to it
		reference extend();

		// Iterator access
		inline iterator begin() {return _data;}
		inline const_iterator begin() const {return _data;}
		inline iterator end() {return _data + _size;}
		inline const_iterator end() const {return _data + _size;}
	private:
		void construct(pointer p, const Int2Type<true> &) { new (p) T(*_allocator); }
		void construct(pointer p, const Int2Type<false> &) { new (p) T(); }

	protected:
		T* _data;
		uint32_t _size;
		uint32_t _capacity;

	public:
		IAllocator* _allocator;
	};
}

#include "vector.inl"