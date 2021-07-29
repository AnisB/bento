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

		void find_all_occurences(const char* source_str, uint32_t source_str_size, const char* to_find, uint32_t to_find_size, Vector<uint32_t>& results)
		{
			uint32_t charIdx = 0;
			while (charIdx < source_str_size)
			{
				// If there is not enough characters so that we can find the target string, we are done
				if ((source_str_size - charIdx) < to_find_size)
					break;

				bool found = false;
				for (uint32_t toFindCharIdx = 0; toFindCharIdx < to_find_size; ++toFindCharIdx)
				{
					// If any char of the substr is not fullfilled, we are done
					if (source_str[charIdx + toFindCharIdx] != to_find[toFindCharIdx])
						break;

					// We were able to find an occurence
					if (toFindCharIdx == (to_find_size - 1))
					{
						results.push_back(charIdx);
						found = true;
					}
				}

				charIdx += found ? to_find_size : 1;
			}
		}

		void replace_substring(DynamicString& source_string, const char* to_replace, const char* replacement)
		{
			// First we need to count the number of occurences of "to_replace" to know what is the the new length of the string
			uint32_t sourceLength = strlen32(to_replace);
			if (sourceLength == 0)
				return;

			// Find all the occurences of the target to_replace string
			uint32_t stringSize = source_string.size();
			Vector<uint32_t> occurences(source_string._allocator);
			find_all_occurences(source_string.c_str(), stringSize, to_replace, sourceLength, occurences);

			// If no occurences we found we have nothingto do
			uint32_t numOccurences = occurences.size();
			if (numOccurences == 0)
				return;

			// Define the new size of the string
			uint32_t targetLength = strlen32(replacement);
			uint32_t newStringSize = stringSize - sourceLength * numOccurences + targetLength * numOccurences;
			bool replacementIsBigger = newStringSize > stringSize;
			// If the string is bigger than it was, we allocate the space
			if (replacementIsBigger)
				source_string.resize(newStringSize);

			// Actually replace the data
			for (uint32_t occIdx = 0; occIdx < numOccurences; ++occIdx)
			{
				if (occIdx != 0)
				{

				}
			}

			// Now that we are done, resize to the right size
			if (!replacementIsBigger)
				source_string.resize(newStringSize);
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