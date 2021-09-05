#pragma once

// Library includes
#include "allocator.h"

// External includes
#include <new>

namespace bento {
	// SFNIAE based member for allocation based class
	#define ALLOCATOR_BASED typedef int allocator_based;

	struct NoAllocator
	{
	};

	// Convert integer to type.
	template <int v>
	struct Int2Type 
	{ 
		enum
		{
			value = v
		}; 
	};

	// Determines if a class is based on the allocation system
	template <class T>
	struct is_allocator_based {

		template <typename C>
		static char dummy_func(typename C::allocator_based *);

		template <typename C>
		static int dummy_func(...);

	public:
		enum {
			value = (sizeof(dummy_func<T>(0)) == sizeof(char))
		};
	};

	// Helper macros for allocation detection
	#define IS_ALLOCATOR_BASED(T) is_allocator_based<T>::value
	#define IS_ALLOCATOR_BASED_TYPE(T) Int2Type< IS_ALLOCATOR_BASED(T) >

	template<typename T>
	inline T* make_new(IAllocator& allocator)
	{
		void* ptr = allocator.allocate(sizeof(T), 4);
		return ptr != nullptr ? new (ptr) T() : nullptr;
	}

	template<typename T, typename P1>
	inline T* make_new(IAllocator& allocator, P1& p1)
	{
		void* ptr = allocator.allocate(sizeof(T), 4);
		return ptr != nullptr ? new (ptr) T(p1) : nullptr;
	}	

	template<typename T, typename P1, typename P2>
	inline T* make_new(IAllocator& allocator, P1& p1, P2& p2)
	{
		void* ptr = allocator.allocate(sizeof(T), 4);
		return  ptr != nullptr ? new (ptr) T(p1, p2) : nullptr;
	}	

	template<typename T>
	inline void make_delete(IAllocator& allocator, T* target_ptr)
	{
		target_ptr->~T();
		allocator.deallocate(target_ptr);
	}

	template<typename THeader>
	THeader& header_from_pointer(void* ptr)
	{
		THeader* headerPtr = (THeader*)ptr;
		return *(headerPtr);
	}

	template<typename THeader>
	void* memory_from_pointer(void* ptr)
	{
		return (void*)((uint8_t*)ptr + sizeof(THeader));
	}

	template<typename THeader>
	const THeader& header_from_memory(void* ptr)
	{
		THeader* headerPtr = (THeader*)ptr;
		return *(headerPtr - 1);
	}

	template<typename THeader>
	void* pointer_from_memory(void* ptr)
	{
		THeader* headerPtr = (THeader*)ptr;
		return (headerPtr - 1);
	}

	IAllocator* common_allocator();
}
