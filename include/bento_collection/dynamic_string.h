#pragma once

// Library includes
#include "bento_collection/vector.h"
#include "bento_base/platform.h"

namespace bento {

	class DynamicString
	{
	public:
		ALLOCATOR_BASED;
		// Cst
		DynamicString(IAllocator& allocator);
		DynamicString(IAllocator& allocator, const char* str);
		DynamicString(IAllocator& allocator, uint32_t str_size);
		DynamicString(const DynamicString& str);

		inline char* c_str() { return _data.size() ? _data.begin() : nullptr; }
		inline const char* c_str() const { return _data.size() ? _data.begin() : ""; }

		void resize(uint32_t size);
		uint32_t size() const;
		DynamicString& operator=(const DynamicString& str);
		DynamicString& operator=(const char* str);
		DynamicString& operator+=(const char* str);
		DynamicString& operator+=(const DynamicString& str);
		bool operator==(const char* str) const;
		bool operator!=(const char* str) const;
		void append(const char* str, uint32_t sizeP);

	public:
		Vector<char> _data;
		IAllocator& _allocator;
	};

	namespace string
	{
		// returns the length nof a string
		uint32_t strlen32(const char* str);

		// Find the last occurent of a character
		uint32_t find_last_of(const char* str, const char s);

		// Put all the characters of a string to the lower case
		void to_lower_case(DynamicString& target_string);

		// Get a substric from an original string
		DynamicString substr(const DynamicString& source_string, uint32_t first_idx, uint32_t size);

		// Find all occurences of a given string into a bigger tring
		void find_all_occurences(const char* source_str, uint32_t source_str_size, const char* to_find, uint32_t to_find_size, Vector<uint32_t>& results);

		// Replaces all occurences of a given string by an other substring
		void replace_substring(const DynamicString& source_string, const char* to_replace, const char* replacement, DynamicString& output_string);
	}

	void pack_type(Vector<char>& buffer, const DynamicString& str);
	void unpack_type(const char*& stream, DynamicString& str);
}