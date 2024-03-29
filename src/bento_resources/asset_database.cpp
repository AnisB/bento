// Bento includes
#include <bento_base/stream.h>
#include <bento_resources/asset_database.h>
#include <bento_tools/json.hpp>

namespace bento
{
	const uint32_t DATABASE_VERSION = 1;

	TAssetDatabase::TAssetDatabase(bento::IAllocator& alloc)
	: _assets(alloc)
	, allocator(alloc)
	{

	}

	TAssetDatabase::~TAssetDatabase()
	{

	}

	void TAssetDatabase::insert_asset(const char* name, const char* path, uint32_t resourceType, bento::Vector<char>& data)
	{
		// Compute the new index
		uint32_t new_asset_idx = _assets.size();
		
		// Create and fetch the new asset		
		_assets.resize(new_asset_idx + 1);

		// Assign the data of to the asset
		TAsset& asset = _assets[new_asset_idx];
		asset.id = bento::murmur_hash_64(name, string::strlen32(name), 0);
		asset.name = name;
		asset.path = path;
		asset.type = resourceType;
		asset.data = data;
	}

	const TAsset* TAssetDatabase::request_asset(const char* name) const
	{
		uint64_t id = bento::murmur_hash_64(name, string::strlen32(name), 0);
		return request_asset(id);
	}

	const TAsset* TAssetDatabase::request_asset(uint64_t id) const
	{
		uint32_t num_assets = _assets.size();
		for (uint32_t asset_idx = 0; asset_idx < num_assets; ++asset_idx)
		{
			const TAsset& current_asset = _assets[asset_idx];
			if (current_asset.id == id)
			{
				return &current_asset;
			}
		}
		return nullptr;
	}

	void pack_type(Vector<char>& buffer, const TAsset& asset)
	{
		pack_bytes(buffer, asset.id);
		pack_type(buffer, asset.name);
		pack_type(buffer, asset.path);
		pack_bytes(buffer, asset.type);
		pack_vector_bytes(buffer, asset.data);
	}

	void unpack_type(const char*& stream, TAsset& asset)
	{
		unpack_bytes(stream, asset.id);
		unpack_type(stream, asset.name);
		unpack_type(stream, asset.path);
		unpack_bytes(stream, asset.type);
		unpack_vector_bytes(stream, asset.data);
	}

	void pack_type(Vector<char>& buffer, const TAssetDatabase& database)
	{
		pack_bytes(buffer, DATABASE_VERSION);
		pack_vector_types(buffer, database._assets);
	}

	bool unpack_type(const char*& stream, TAssetDatabase& database)
	{
		// Read the version
		uint32_t database_version;
		unpack_bytes(stream, database_version);

		// Stop if this does not match the current version
		if (database_version != DATABASE_VERSION) return false;
		unpack_vector_types(stream, database._assets);
		return true;
	}

	void to_string(DynamicString& str, const TAssetDatabase& target_database)
	{
		// Serialize the db to a json struct
		nlohmann::json db;
		uint32_t num_assets = target_database._assets.size();
		for (uint32_t asset_idx = 0; asset_idx < num_assets; ++asset_idx)
		{
			const bento::TAsset& current_asset = target_database._assets[asset_idx];
			db[std::to_string(current_asset.id)] = { { "name", current_asset.name.c_str() },
			{ "path", current_asset.path.c_str() },
			{ "type", std::to_string(current_asset.type) },
			{ "size", current_asset.data.size() } };
		}

		// Append it to the string
		str += db.dump(2).c_str();
	}
}