#pragma once

// SDK includes
#include "bento_collection/vector.h"

namespace bento {
	// Functions to pack/unpack a raw buffer (knwoing its size in both cases
	void pack_buffer(Vector<char>& buffer, uint32_t write_size, const char* data);
	void unpack_buffer(const char*& stream, uint32_t read_size, char* data);

	// Functions to pack/unpack a type T as bytes
	template<typename T>
	void pack_bytes(Vector<char>& buffer, const T& type);
	template<typename T>
	void unpack_bytes(const char*& stream, T& type);

	// Functions to pack/unpack a vector and its elements as types
	template<typename T>
	void pack_vector_types(Vector<char>& buffer, const Vector<T>& data);
	template<typename T>
	void unpack_vector_types(const char*& stream, Vector<T>& data);

	// Function to pack/unpack a buffer and its content as bytes
	template<typename T>
	void pack_vector_bytes(Vector<char>& buffer, const Vector<T>& data);
	template<typename T>
	void unpack_vector_bytes(const char*& stream, Vector<T>& data);
}

#include "stream.inl"