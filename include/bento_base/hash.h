#pragma once

// Library includes
#include "bento_base/platform.h"

namespace bento {

	// Implementation taken from https://sites.google.com/site/murmurhash/
	uint32_t murmur_hash(const void * key, uint32_t len, uint32_t seed);
	uint64_t murmur_hash_64(const void * key, uint32_t len, uint32_t seed);
}