// Library includes
#include "bento_collection/dynamic_string.h"
#include "bento_base/stream.h"

// External includes
#include <string.h>

namespace bento
{
	uint32_t strlen32(const char* str)
	{
		return (uint32_t)strlen(str);
	}

	// Find the last occurent of a character
	uint32_t find_last_of(const char *str, const char s)
	{
		uint32_t path_size = strlen32(str);
		uint32_t char_idx = path_size - 1;
		while( char_idx >= 0 && str[char_idx] != s)
		{
			--char_idx;
		}
		return char_idx != UINT32_MAX ? char_idx : path_size;
	}

	DynamicString::DynamicString(IAllocator& allocator)
	: _allocator(allocator)
	, _data(allocator)
	{
	}

	DynamicString::DynamicString(IAllocator& allocator, const char* str)
	: _allocator(allocator)
	, _data(allocator)
	{
		uint32_t str_len = strlen32(str);
		if(str_len) {
			_data.resize(str_len + 1);
			memcpy(_data.begin(), str,  str_len + 1);
		}
	}

	DynamicString::DynamicString(IAllocator& allocator, uint32_t str_size)
	: _allocator(allocator)
	, _data(allocator)
	{
		if(str_size) {
			_data.resize(str_size + 1);
			_data[str_size] = 0;
		}
	}

	DynamicString::DynamicString(const DynamicString& str)
	: _allocator(str._allocator)
	, _data(str._allocator)
	{
		// Set them to be the same size
		uint32_t str_size = str.size();
		resize(str_size);

		// memcpy the buffer
		if (str_size)
			memcpy(_data.begin(), str.c_str(), str_size);
	}

	void DynamicString::resize(uint32_t size)
	{
		if (size) {
			_data.resize(size + 1);
			_data[size] = 0;
		} else {
			_data.resize(0);
		}
	}

	uint32_t DynamicString::size() const
	{
		uint32_t size = _data.size();
		return size ? size - 1 : 0;
	}

	DynamicString& DynamicString::operator=(const DynamicString& str)
	{
		// Set them to be the same size
		uint32_t str_size = str.size();
		resize(str_size);

		// memcpy the buffer
		if (str_size)
			memcpy(_data.begin(), str.c_str(), str_size);

		return *this;
	}

	DynamicString& DynamicString::operator=(const char* str)
	{
		// Set them to be the same size
		uint32_t str_size = strlen32(str);
		resize(str_size);

		// memcpy the buffer
		if (str_size)
			memcpy(_data.begin(), str, str_size);

		return *this;
	}

	DynamicString& DynamicString::operator+=(const char* str)
	{
		uint32_t current_size = size();
		uint32_t str_size = strlen32(str);
		uint32_t new_size = current_size + str_size;
		resize(new_size);
		memcpy(_data.begin() + current_size, str, str_size);
		return *this;
	}

	bool DynamicString::operator==(const char* str) const
	{
		return strcmp(_data.begin(), str) == 0;
	}

	void pack_type(Vector<char>& buffer, const DynamicString& str)
	{
		pack_bytes(buffer, str.size());
		pack_buffer(buffer, str.size(), str.c_str());
	}

	void unpack_type(const char*& stream, DynamicString& str)
	{
		uint32_t str_size;
		unpack_bytes(stream, str_size);
		str.resize(str_size);
	}
}