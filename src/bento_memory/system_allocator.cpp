// Library includes
#include "bento_base/platform.h"
#include "bento_memory/system_allocator.h"

// External includes
#include <algorithm>

namespace bento {

	SystemAllocator::SystemAllocator() {
	}

	SystemAllocator::~SystemAllocator() {
	}

	// Allocate a memory chunk give a particular alignment
	void* SystemAllocator::allocate(size_t size, size_t alignment)
	{
		return platform_allocate(size, alignment);
	}

	void* SystemAllocator::reallocate(void* old_ptr, size_t old_size, size_t new_size, size_t alignment)
	{
		void* ptr = allocate(new_size, alignment);
		memcpy(ptr, old_ptr, old_size > new_size ? new_size : old_size);
		deallocate(old_ptr);
		return ptr;
	}

	void SystemAllocator::deallocate(void* ptr)
	{
		platform_free(ptr);
	}

	bool SystemAllocator::is_multi_thread_safe()
	{
		return true;
	}
}