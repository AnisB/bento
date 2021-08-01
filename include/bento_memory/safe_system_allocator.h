#pragma once

// Library includes
#include "allocator.h"

namespace bento {
    class SafeSystemAllocator : public IAllocator
    {
    public:
		SafeSystemAllocator();
        ~SafeSystemAllocator();

        void* allocate(size_t size, size_t alignment) override;
        void* reallocate(void* old_ptr, size_t old_size, size_t new_size, size_t alignment) override;
        void deallocate(void* _ptr) override;
        bool is_multi_thread_safe() override;
		uint32_t header_size() override;

		uint32_t total_memory_allocated();
		uint32_t total_freed_memory();
		uint32_t current_allocated_memory();

	private:
		uint32_t _totalMemoryAllocated;
		uint32_t _totalFreedMemory;
		uint32_t _currentAllocatedMemory;
	};
}
