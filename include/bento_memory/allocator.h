#pragma once

#include "bento_base/platform.h"

namespace bento
{
	class IAllocator
	{
	public:
		IAllocator();
		virtual ~IAllocator();

		virtual void* allocate(size_t size, size_t alignment) = 0;
		virtual void* reallocate(void* old_ptr, size_t old_size, size_t new_size, size_t alignment) = 0;
		virtual void deallocate(void* _ptr) = 0;
		virtual bool is_multi_thread_safe() = 0;
		virtual uint32_t header_size() = 0;
	};
}