
namespace bento {
	template<typename T>
	void pack_bytes(Vector<char>& buffer, const T& type)
	{
		pack_buffer(buffer, (uint32_t)sizeof(T), (const char*)&type);
	}

	template<typename T>
	void unpack_bytes(const char*& stream, T& type)
	{
		unpack_buffer(stream, (uint32_t)sizeof(T), (char*)&type);
	}

	template<typename T>
	void pack_vector_types(Vector<char>& buffer, const Vector<T>& data)
	{
		uint32_t num_elements = data.size();
		pack_bytes(buffer, num_elements);
		if (num_elements) {
			for (uint32_t ele_idx = 0; ele_idx < num_elements; ++ele_idx)
			{
				pack_type(buffer, data[ele_idx]);
			}
		}
	}

	template<typename T>
	void unpack_vector_types(const char*& stream, Vector<T>& data)
	{
		uint32_t num_elements;
		unpack_bytes(stream, num_elements);
		data.resize(num_elements);
		if (num_elements) {
			for (uint32_t ele_idx = 0; ele_idx < num_elements; ++ele_idx)
			{
				unpack_type(stream, data[ele_idx]);
			}
		}
	}

	template<typename T>
	void pack_vector_bytes(Vector<char>& buffer, const Vector<T>& data)
	{
		uint32_t num_elements = data.size();
		pack_bytes(buffer, num_elements);
		if (num_elements) {
			pack_buffer(buffer, num_elements * (uint32_t)sizeof(T), (const char*)data.begin());
		}
	}

	template<typename T>
	void unpack_vector_bytes(const char*& stream, Vector<T>& data)
	{
		uint32_t num_elements;
		unpack_bytes(stream, num_elements);
		data.resize(num_elements);
		if (num_elements) {
			unpack_buffer(stream, num_elements * (uint32_t)sizeof(T), (char*)data.begin());
		}
	}
}