// SDK includes
#include "bento_base/stream.h"
#include <cstring>

namespace bento {
	void pack_buffer(Vector<char>& buffer, uint32_t write_size, const char* data)
	{
		if (write_size) {
			uint32_t old_size = buffer.size();
			buffer.resize(old_size + write_size);
			std::memcpy(buffer.begin() + old_size, data, write_size);
		}
	}

	void unpack_buffer(const char*& stream, uint32_t read_size, char* data)
	{
		if (read_size) {
			std::memcpy(data, stream, read_size);
			stream += read_size;
		}
	}
}