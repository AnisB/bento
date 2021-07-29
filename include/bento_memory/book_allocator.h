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

        // Allocator specific methods
        void initialize(uint64_t min_chunk_size, uint64_t max_chunk_size);
        size_t min_chunk_size();
        size_t max_chunk_size();
        PageAllocator& get_page_allocator(uint32_t page_index);

    protected:
        Vector<PageAllocator> _pages;
        size_t _maxChunkSize;
        size_t _minChunkSize;
    };
}