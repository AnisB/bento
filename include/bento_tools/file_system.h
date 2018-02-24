#pragma once

// SDK includes
#include "bento_collection/dynamic_string.h"

namespace bento {

	// Methods that are specific to path manipulation
	namespace  path {
		// Joing a root path with a sub-path
		DynamicString join(const char* root, const char* sub_path, IAllocator& allocator);
		DynamicString add_extension(const char *path, const char *extension, IAllocator& allocator);
		DynamicString strip_extension(const char *file, IAllocator& allocator);
		DynamicString extension(const char *file, IAllocator& allocator);
		DynamicString directory(const char *file, IAllocator& allocator);
		DynamicString filename(const char *file, IAllocator& allocator);
	}

	// Enum that defines the access mode of a file
	enum class FileType
	{
		Binary,
		Text
	};

	bool write_file(const char *file_name, const char *buffer, uint32_t buffer_size, FileType type);
	bool read_file(const char *file_name, Vector<char>& buffer, FileType type);
}