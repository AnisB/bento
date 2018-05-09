// SDK includes
#include "bento_tools/disk_serializer.h"

namespace bento {
	DiskSerializer::DiskSerializer(IAllocator& allocator, const char* root)
	: _allocator(allocator)
	, _root(allocator, root)
	{
	}

	bool DiskSerializer::internal_write_buffer(const Vector<char>& buffer, const char *name, const char* extension)
	{	
		// Create the final path of the buffers
		DynamicString total_path = path::join(_root.c_str(), name, _allocator);
		total_path = path::add_extension(total_path.c_str(), extension, _allocator);

		// Write the target buffer
		return write_file(total_path.c_str(), buffer.begin(), buffer.size(), FileType::Binary);
	}

	bool DiskSerializer::internal_read_buffer(Vector<char>& buffer, const char *name, const char* extension)
	{
		// Create the final path of the buffers
		DynamicString total_path = path::join(_root.c_str(), name, _allocator);
		total_path = path::add_extension(total_path.c_str(), extension, _allocator);

		// Write the target buffer
		return read_file(total_path.c_str(), buffer, FileType::Binary);
	}

	bool DiskSerializer::internal_write_string(const DynamicString& str, const char *name, const char* extension)
	{
		// Create the final path of the buffers
		DynamicString total_path = path::join(_root.c_str(), name, _allocator);
		total_path = path::add_extension(total_path.c_str(), extension, _allocator);

		return write_file(total_path.c_str(), str.c_str(), str.size(), FileType::Text);
	}

	bool DiskSerializer::internal_read_string(DynamicString& str, const char *name, const char* extension)
	{
		// Create the final path of the buffers
		DynamicString total_path = path::join(_root.c_str(), name, _allocator);
		total_path = path::add_extension(total_path.c_str(), extension, _allocator);

		// Write the target buffer
		Vector<char> raw_data(_allocator);
		bool result = read_file(total_path.c_str(), raw_data, FileType::Text);
		if (result) {
			str.resize(raw_data.size());
			memcpy(str.c_str(), raw_data.begin(), raw_data.size());
		}
		return result;
	}
}