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
		int32_t char_idx = path_size - 1;
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
		resize(0);
	}

	DynamicString::DynamicString(IAllocator& allocator, const char* str)
	: _allocator(allocator)
	, _data(allocator)
	{
		uint32_t str_len = strlen32(str);
		resize(str_len);
		if(str_len)
		{
			memcpy(_data.begin(), str,  str_len);
		}
	}

	DynamicString::DynamicString(IAllocator& allocator, uint32_t str_size)
	: _allocator(allocator)
	, _data(allocator)
	{
		resize(str_size);
		if(str_size)
		{
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
		if (size)
		{
			_data.resize(size + 1);
			_data[size] = 0;
		}
		else
		{
			_data.resize(1);
			_data[0] = 0;
		}
	}

	uint32_t DynamicString::size() const
	{
		return _data.size() - 1;
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

	DynamicString& DynamicString::operator+=(const DynamicString& str)
	{
		(*this) += str.c_str();
		return *this;
	}

	bool DynamicString::operator==(const char* str) const
	{
		return strcmp(_data.begin(), str) == 0;
	}

	bool DynamicString::operator!=(const char* str) const
	{
		return !((*this) == str);
	}

	void DynamicString::append(const char* str, uint32_t sizeP)
	{
		uint32_t prevSize = size();
		resize(prevSize + sizeP);
		memcpy(_data.begin() + prevSize, str, sizeP);
	}

	namespace string
	{
		void to_lower_case(DynamicString& target_string)
		{
			uint32_t num_chars = target_string.size();
			char* character_array = target_string.c_str();
			for(uint32_t char_idx = 0; char_idx < num_chars; ++char_idx)
			{
				character_array[char_idx] = (char)tolower(character_array[char_idx]);
			}
		}

		DynamicString substr(const DynamicString& source_string, uint32_t first_idx, uint32_t size)
		{
			DynamicString result_string(source_string._allocator, size);
			memcpy(result_string.c_str(), source_string.c_str() + first_idx, size);
			return result_string;
		}
	}

	void pack_type(Vector<char>& buffer, const DynamicString& str)
	{
		pack_vector_bytes(buffer, str._data);
	}

	void unpack_type(const char*& stream, DynamicString& str)
	{
		unpack_vector_bytes(stream, str._data);
	}
}