#pragma once

// Bento includes
#include <bento_collection/dynamic_string.h>
#include <bento_base/hash.h>

namespace bento
{
	struct TAsset
	{
		ALLOCATOR_BASED;
		TAsset(bento::IAllocator& alloc)
		: name(alloc)
		, path(alloc)
		, type(UINT32_MAX)
		, data(alloc)
		, id(UINT64_MAX)
		{
		}

		uint64_t				id;
		bento::DynamicString	name;
		bento::DynamicString	path;
		uint32_t				type;
		bento::Vector<char>		data;
	};

	class TAssetDatabase
	{
	public:
		TAssetDatabase(bento::IAllocator& alloc);
		~TAssetDatabase();

		// Insert an asset into the database
		void insert_asset(const char* name, const char* path, uint32_t resourceType, bento::Vector<char>& data);

		// Request an asset either using its name or id
		const TAsset* request_asset(const char* name) const;
		const TAsset* request_asset(uint64_t id) const;

		template<typename T>
		bool unpack_asset_to_type(const char* name, T& outputType) const
		{
			// Hash and call the right model
			uint64_t id = bento::murmur_hash_64(name, strlen32(name), 0);
			return unpack_asset_to_type(id, outputType);
		}

		template<typename T>
		bool unpack_asset_to_type(uint64_t assetId, T& outputType) const
		{
			// First try to request the asset from the database
			const TAsset* targetAsset = request_asset(assetId);
			if (targetAsset == nullptr) return false;

			// Unpack the type and return the result
			const char* targetAssetData = targetAsset->data.begin();
			return bento::unpack_type(targetAssetData, outputType);
		}

	public:
		bento::Vector<TAsset> _assets;
		bento::IAllocator& allocator;
	};

	void pack_type(Vector<char>& buffer, const TAssetDatabase& database);
	bool unpack_type(const char*& stream, TAssetDatabase& database);

	void to_string(DynamicString& str, const TAssetDatabase& database);
}