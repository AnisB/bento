#pragma once

// Library includes
#include "allocator.h"
#include "bento_collection/vector.h"
#include "bento_memory/page_allocator.h"

namespace bento {
    class BookAllocator : public IAllocator
    {
    public:
        BookAllocator();
        ~BookAllocator();

        // Methods that need to be overriden by the allocator
        void* allocate(size_t size, size_t alignment) override;
        void* reallocate(void* old_ptr, size_t old_size, size_t new_size, size_t alignment) override;
        void deallocate(void* ptr) override;
        bool is_multi_thread_safe() override;
        uint32_t header_size() override;

        // Allocator specific methods
        size_t min_chunk_size();
        size_t max_chunk_size();
        uint64_t memory_footprint();
        PageAllocator& get_page_allocator(uint32_t page_index);
        void set_num_pages(uint32_t num_pages);
        uint32_t num_pages();

    protected:
        Vector<PageAllocator> _pages;
    };

    namespace book_allocator
    {
        void initialize(BookAllocator& allocator, uint64_t min_chunk_size, uint64_t max_chunk_size);
    }
}