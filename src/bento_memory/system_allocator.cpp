// Library includes
#include "bento_memory/system_allocator.h"
#include "bento_base/platform.h"

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
		#if defined (WINDOWSPC)
			return _aligned_malloc(size, alignment);
		#elif defined(LINUXPC)
			return memalign(alignment, size);
		#else
			#error Unsupported platfrom
		#endif
	}

	void* SystemAllocator::reallocate(void* old_ptr, size_t old_size, size_t new_size, size_t alignment)
	{
		#if defined (WINDOWSPC)
			void* ptr = allocate(new_size, alignment);
			memcpy(ptr, old_ptr, old_size > new_size ? new_size : old_size);
			deallocate(old_ptr);
			return ptr;
		#elif defined(LINUXPC)
			void* ptr = allocate(new_size, alignment);
			memcpy(ptr, old_ptr, old_size > new_size ? new_size : old_size);
			deallocate(old_ptr);
			return ptr;
		#else
			#error Unsupported platfrom
		#endif
	}

	void SystemAllocator::deallocate(void* _ptr)
	{
		#if defined (WINDOWSPC)
			_aligned_free(_ptr);
		#elif defined(LINUXPC)
			free(ptr);
		#else
			#error Unsupported platfrom
		#endif
	}
}