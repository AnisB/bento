// Library includes
#include "bento_memory/common.h"
#include "bento_memory/system_allocator.h"

namespace bento {
	static SystemAllocator __common_allocator;

	// Return the common allocator ptr
	IAllocator* common_allocator()
	{
		return &__common_allocator;
	}
}