// Library includes
#include "bento_base/platform.h"
#include "bento_base/security.h"
#include "bento_memory/common.h"
#include "bento_memory/safe_system_allocator.h"

// External includes
#include <algorithm>

namespace bento {

    struct SafeSystemAllocatorHeader
    {
        uint32_t allocationSize;
		SafeSystemAllocator* ptr;
    };

	SafeSystemAllocator::SafeSystemAllocator()
    : _totalMemoryAllocated(0)
    , _totalFreedMemory(0)
    , _currentAllocatedMemory(0)
    {
    }

	SafeSystemAllocator::~SafeSystemAllocator()
    {
        assert(_currentAllocatedMemory == 0);
    }

    // Allocate a memory chunk give a particular alignment
    void* SafeSystemAllocator::allocate(size_t size, size_t alignment)
    {
        // Compute the total size of the allocation
        uint32_t allocationSize = (uint32_t)size + header_size();

        // allocate the memory
        void* ptr = platform_allocate(size + header_size(), alignment);

        // Grab the header at set the values
		SafeSystemAllocatorHeader& headerPointer = header_from_pointer<SafeSystemAllocatorHeader>(ptr);
        headerPointer.allocationSize = allocationSize;
        headerPointer.ptr = this;

        // Account for the allocation
        _totalMemoryAllocated += allocationSize;
        _currentAllocatedMemory += allocationSize;

        // return the memory
        return (void*)memory_from_pointer<SafeSystemAllocatorHeader>(ptr);
    }

    void* SafeSystemAllocator::reallocate(void* old_ptr, size_t old_size, size_t new_size, size_t alignment)
    {
        // Allocate the memory
        void* ptr = allocate(new_size, alignment);

        // Copy the new memory
        memcpy(ptr, old_ptr, old_size > new_size ? new_size : old_size);

        // Free the previous memory
        deallocate(old_ptr);

        // return the new pointer
        return ptr;
    }

    void SafeSystemAllocator::deallocate(void* ptr)
    {
        // Grab the page header
        const SafeSystemAllocatorHeader& header = header_from_memory<SafeSystemAllocatorHeader>(ptr);

        // Make sure this was allocated by this allocator
        assert(this == header.ptr);

        // Make sure this can be freed
        assert(_currentAllocatedMemory >= header.allocationSize);
        
        // Account for the free
        _totalFreedMemory += header.allocationSize;
        _currentAllocatedMemory -= header.allocationSize;

        // Release the pointer
        platform_free(pointer_from_memory<SafeSystemAllocatorHeader>(ptr));
    }

    bool SafeSystemAllocator::is_multi_thread_safe()
    {
        return false;
    }

    uint32_t SafeSystemAllocator::header_size()
    {
        return sizeof(SafeSystemAllocatorHeader);
    }

	uint32_t SafeSystemAllocator::total_memory_allocated()
	{
		return _totalMemoryAllocated;
	}

	uint32_t SafeSystemAllocator::total_freed_memory()
	{
		return _totalFreedMemory;
	}

	uint32_t SafeSystemAllocator::current_allocated_memory()
	{
		return _currentAllocatedMemory;
	}
}