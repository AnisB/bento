// Library includes
#include "bento_base/platform.h"
#include "bento_memory/page_allocator.h"

// External includes
#include <algorithm>

namespace bento {

	// Given that we are using a uint64_t for tracking the usage flag, the number of chunks per page is 64
	#define CHUNKS_PER_PAGE 64

    PageAllocator::PageAllocator()
    {
        _usageFlags = 0;
        _chunkSize = 0;
        _rawMemory = nullptr;
    }

    PageAllocator::~PageAllocator()
    {
        platform_free(_rawMemory);
    }

    void PageAllocator::initialize(uint64_t chunkSize)
    {
        _usageFlags = 0;
        _chunkSize = chunkSize;
        _rawMemory = platform_allocate(_chunkSize * CHUNKS_PER_PAGE, 4);
    }

    // Allocate a memory chunk give a particular alignment
    void* PageAllocator::allocate(size_t size, size_t)
    {
        if (size > _chunkSize|| UINT64_MAX == _usageFlags)
            return nullptr;

    	uint8_t* memoryAsUint8 = (uint8_t*) _rawMemory;
        for (uint64_t chunkIdx = 0; chunkIdx < CHUNKS_PER_PAGE; ++chunkIdx)
        {
        	uint64_t chunkMask = (uint64_t)1 << chunkIdx;
        	if ((chunkMask & _usageFlags) == 0)
        	{
        		// This chunk is free, we can mark it as full and return it
        		_usageFlags |= chunkMask;
        		return (void*)(memoryAsUint8 + _chunkSize * chunkIdx);
        	}
        }
        return nullptr;
    }

    void* PageAllocator::reallocate(void* old_ptr, size_t, size_t new_size, size_t)
    {
        // If the required size is inferior to the chunk size, we can keep the same pointer
        if (new_size < _chunkSize)
            return old_ptr;
        // Otherwise, this allocator cannot provide the allocation
        return nullptr;
    }

    void PageAllocator::deallocate(void* ptr)
    {
    	// Convert the pointer to a relative adress
        size_t relativeLocation = (uint8_t*)ptr - (uint8_t*) _rawMemory;
        // Compute the chunk index
        size_t chunkIndex = relativeLocation / _chunkSize;
        // Compute the mask of the chunk
       	uint64_t chunkMask = (uint64_t)1 << (uint64_t)chunkIndex;
       	// Force the chunk bit index to 0
       	_usageFlags &= ~(chunkMask);
    }

    bool PageAllocator::is_multi_thread_safe()
    {
        return false;
    }

    size_t PageAllocator::chunk_size()
    {
        return _chunkSize;
    }

    uint64_t PageAllocator::usage_flags()
    {
        return _usageFlags;
    }

    bool PageAllocator::is_full()
    {
        return _usageFlags == UINT64_MAX;
    }
}