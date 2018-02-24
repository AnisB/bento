#pragma once

// Library includes
#include "allocator.h"

namespace bento {
	class SystemAllocator : public IAllocator
	{
	public:
		SystemAllocator();
		~SystemAllocator();

		void* allocate(size_t size, size_t alignment) override;
		void* reallocate(void* old_ptr, size_t old_size, size_t new_size, size_t alignment) override;
		void deallocate(void* _ptr) override;
	};
}