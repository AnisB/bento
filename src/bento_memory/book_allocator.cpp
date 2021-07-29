// Library includes
#include "bento_base/platform.h"
#include "bento_memory/book_allocator.h"
#include "bento_base/security.h"

// External includes
#include <algorithm>

namespace bento {
    // This is unsafe and relies on the fact that the pointer 
    // was actually allocated by this exact of allocator to work
    uint32_t get_pointer_header(void* ptr)
    {
        uint32_t* uint32Ptr = (uint32_t*)ptr;
        return *(uint32Ptr - 1);
    }

    BookAllocator::BookAllocator()
    : _pages(*common_allocator())
    {
    }

    BookAllocator::~BookAllocator()
    {
    }

    // Allocate a memory chunk give a particular alignment
    void* BookAllocator::allocate(size_t size, size_t alignment)
    {
        // Loop through all the pages
        uint32_t numPages = _pages.size();
        for(uint32_t pageIdx = 0; pageIdx < numPages; ++pageIdx)
        {
            // Get the current page
            PageAllocator& currentPage = _pages[pageIdx];

            // Would this allocation fit inside the page? 
            if (currentPage.chunk_size() <= size && !currentPage.is_full())
            {
                // We allocate 4 additional bytes for us to store the book allocator's information
                uint32_t* data = (uint32_t*)currentPage.allocate(4 + size, alignment);
                
                // We store out additional information (page idx for now)
                data[0] = pageIdx;

                // Return the pointer (shifted by the header)
                return (void*)(data + 1);
            }
        }
        return nullptr;
    }

    void* BookAllocator::reallocate(void* old_ptr, size_t old_size, size_t new_size, size_t alignment)
    {
        // Grab the page index
        uint32_t pageIdx = get_pointer_header(old_ptr);
        PageAllocator& originPage = _pages[pageIdx];
        // If the required size is inferior to the previously allocated size, we can keep the same pointer
        if (new_size < originPage.chunk_size())
            return old_ptr;

        // Try to allocate a new pointer as the previous one cannot be used
        void* newPtr = allocate(new_size, alignment);
        if (newPtr != nullptr)
        {
            deallocate(old_ptr);
            return newPtr;
        }

        // This allocator cannot provide the allocation
        return nullptr;
    }

    void BookAllocator::deallocate(void* ptr)
    {
        // Grab the page index
        uint32_t pageIdx = get_pointer_header(ptr);
        _pages[pageIdx].deallocate(ptr);
    }

    bool BookAllocator::is_multi_thread_safe()
    {
        return false;
    }

    void BookAllocator::initialize(uint64_t min_chunk_size, uint64_t max_chunk_size)
    {
        _minChunkSize = min_chunk_size;
        _maxChunkSize = max_chunk_size;

        // Make sure everything is a multiple of 4
        assert(_minChunkSize != 0 && _minChunkSize % 4 == 0);
        assert(_maxChunkSize > _minChunkSize && _maxChunkSize % 4 == 0);

        // Define the number of pages we'll need
        uint32_t numPages = (_maxChunkSize - _minChunkSize) / 4 + 1;
        _pages.resize(numPages);

        for (uint32_t pageIdx = 0; pageIdx < numPages; ++pageIdx)
        {
            // Get the current page
            PageAllocator& currentPage = _pages[pageIdx];
            currentPage.initialize(_minChunkSize + 4 * pageIdx);
        }
    }

    size_t BookAllocator::min_chunk_size()
    {
        return _minChunkSize;
    }

    size_t BookAllocator::max_chunk_size()
    {
        return _minChunkSize;
    }
}