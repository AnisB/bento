// Library includes
#include "bento_base/platform.h"
#include "bento_memory/book_allocator.h"
#include "bento_base/security.h"

// External includes
#include <algorithm>

namespace bento {

    struct BookAllocatorHeader
    {
        uint32_t pageIdx;
    };

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
        // Compute the total allocationsize
        uint32_t totalAllocationSize = (uint32_t)(sizeof(BookAllocatorHeader) + size);

        // Loop through all the pages
        uint32_t numPages = _pages.size();
        for(uint32_t pageIdx = 0; pageIdx < numPages; ++pageIdx)
        {
            // Get the current page
            PageAllocator& currentPage = _pages[pageIdx];

            // Would this allocation fit inside the page? 
            if (currentPage.chunk_size() >= totalAllocationSize && !currentPage.is_full())
            {
				// Allocate the required memory
				void* rawPtr = currentPage.allocate(totalAllocationSize, alignment);

                // We allocate 4 additional bytes for us to store the book allocator's information
				BookAllocatorHeader& headerPointer = header_from_pointer<BookAllocatorHeader>(rawPtr);
                
                // We store out additional information (page idx for now)
                headerPointer.pageIdx = pageIdx;

                // Return the pointer (shifted by the header)
                return memory_from_pointer<BookAllocatorHeader>(rawPtr);
            }
        }
        return nullptr;
    }

    void* BookAllocator::reallocate(void* old_ptr, size_t, size_t new_size, size_t)
    {
        // Grab the page index
        const BookAllocatorHeader& pointerHeader = header_from_memory<BookAllocatorHeader>(old_ptr);
        PageAllocator& originPage = _pages[pointerHeader.pageIdx];
        // If the required size is inferior to the previously allocated size, we can keep the same pointer
        if (new_size < originPage.chunk_size())
            return old_ptr;

        // Try to allocate a new pointer as the previous one cannot be used
        void* newPtr = allocate(new_size, 4);
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
		const BookAllocatorHeader& header = header_from_memory<BookAllocatorHeader>(ptr);
		_pages[header.pageIdx].deallocate(ptr);
    }

    bool BookAllocator::is_multi_thread_safe()
    {
        return false;
    }

    uint32_t BookAllocator::header_size()
    {
        return sizeof(BookAllocatorHeader);
    }

    size_t BookAllocator::min_chunk_size()
    {
        return _pages[0].chunk_size();
    }

    size_t BookAllocator::max_chunk_size()
    {
        return _pages[_pages.size() - 1].chunk_size();
    }

    PageAllocator& BookAllocator::get_page_allocator(uint32_t page_index)
    {
        return _pages[page_index];
    }

    void BookAllocator::set_num_pages(uint32_t num_pages)
    {
        _pages.resize(num_pages);
    }

    uint32_t BookAllocator::num_pages()
    {
        return _pages.size();
    }

    uint64_t BookAllocator::memory_footprint()
    {
        uint64_t totalFootprint = 0;
        // Loop through all the pages
        uint32_t numPages = _pages.size();
        for (uint32_t pageIdx = 0; pageIdx < numPages; ++pageIdx)
        {
            // Get the current page
            PageAllocator& currentPage = _pages[pageIdx];
            totalFootprint += currentPage.memory_footprint();
        }
        return totalFootprint;
    }

    namespace book_allocator
    {
        void initialize(BookAllocator& bookAllocator, uint64_t min_chunk_size, uint64_t max_chunk_size)
        {
            // Make sure everything is a multiple of 4
            assert(min_chunk_size != 0 && min_chunk_size % 4 == 0);
            assert(max_chunk_size > min_chunk_size && max_chunk_size % 4 == 0);

            // Define the number of pages we'll need
            uint32_t numPages = (uint32_t)((max_chunk_size - min_chunk_size) / 4 + 1);
            bookAllocator.set_num_pages(numPages);

            for (uint32_t pageIdx = 0; pageIdx < numPages; ++pageIdx)
            {
                // Get the current page
                PageAllocator& currentPage = bookAllocator.get_page_allocator(pageIdx);
                currentPage.initialize(min_chunk_size + (uint32_t)4 * pageIdx + 4);
            }
        }
    }
}