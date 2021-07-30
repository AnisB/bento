#pragma once

// Library includes
#include "allocator.h"

namespace bento {
    class PageAllocator : public IAllocator
    {
    public:
        PageAllocator();
        ~PageAllocator();

        // Methods that need to be overriden by the allocator
        void* allocate(size_t size, size_t alignment) override;
        void* reallocate(void* old_ptr, size_t old_size, size_t new_size, size_t alignment) override;
        void deallocate(void* ptr) override;
        bool is_multi_thread_safe() override;
        uint32_t header_size() override;

        // Allocator specific methods
        void initialize(uint64_t chunkSize);
        uint64_t memory_footprint();
        bool is_full();
        size_t chunk_size();
        uint64_t usage_flags();

    protected:
        uint64_t _usageFlags;
        size_t _chunkSize;
        void* _rawMemory;
    };
}