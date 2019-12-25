#pragma once

// Library includes
#include "bento_collection/vector.h"
#include "bento_base/platform.h"

namespace bento {

	// returns the length nof a string
	uint32_t strlen32(const char* str);

	// Find the last occurent of a character
	uint32_t find_last_of(const char *str, const char s);

	class DynamicString
	{
	public:
		ALLOCATOR_BASED;
		// Cst
		DynamicString(IAllocator& allocator);
		DynamicString(IAllocator& allocator, const char* str);
		DynamicString(IAllocator& allocator, uint32_t str_size);
		DynamicString(const DynamicString& str);

		inline char* c_str() { return _data.size() ? _data.begin() : ""; }
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
		// Put all the characters of a string to the lower case
		void to_lower_case(DynamicString& target_string);

		// Get a substric from an original string
		DynamicString substr(const DynamicString& source_string, uint32_t first_idx, uint32_t size);
	}

	void pack_type(Vector<char>& buffer, const DynamicString& str);
	void unpack_type(const char*& stream, DynamicString& str);
}