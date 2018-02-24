#pragma once

// SDK includes
#include "bento_memory/common.h"
#include "bento_collection/dynamic_string.h"
#include "bento_tools/file_system.h"

namespace bento {

	class DiskSerializer
	{
		ALLOCATOR_BASED;
	public:
		// Cst
		DiskSerializer(IAllocator& allocator, const char* root_path);

		// Function that produces a binary file from a given entry
		template<typename T>
		bool write_binary(const T& entry, const char* name, const char* extension)
		{
			Vector<char> buffer(_allocator);
			pack_type(buffer, entry);
			return internal_write_buffer(buffer, name, extension);
		}

		// Function that fills an entry from a binary file
		template<typename T>
		bool read_binary(T& entry, const char* name, const char* extension)
		{
			Vector<char> buffer(_allocator);
			bool buffer_read = internal_read_buffer(buffer, name, extension);
			if (!buffer_read) return false;
			const char* stream = buffer.begin();
			return unpack_type(stream, entry);
		}

		template<typename T>
		bool write_string(T& type, const char* name, const char* extension)
		{
			DynamicString str(_allocator);
			to_string(str, type);
			return internal_write_string(str, name, extension);
		}

		template<typename T>
		bool read_string(T& type, const char* name, const char* extension)
		{
			DynamicString str(_allocator);
			bool buffer_read = internal_read_string(str, name, extension);
			if (!buffer_read) return false;
			return from_string(str, type);
		}

	private:
		bool internal_write_buffer(const Vector<char>& buffer, const char *name, const char* extension);
		bool internal_read_buffer(Vector<char>& buffer, const char *name, const char* extension);

		bool internal_write_string(const DynamicString& str, const char *name, const char* extension);
		bool internal_read_string(DynamicString& str, const char *name, const char* extension);
		
	private:
		DynamicString _root;
		IAllocator& _allocator;
	};
}