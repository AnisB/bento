// Library includes
#include "bento_base/hash.h"

namespace bento {

	// Implementation taken from https://sites.google.com/site/murmurhash/
	uint32_t murmur_hash ( const void * key, uint32_t len, uint32_t seed )
	{
		// 'm' and 'r' are mixing constants generated offline.
		// They're not really 'magic', they just happen to work well.

		const unsigned int m = 0x5bd1e995;
		const uint32_t r = 24;

		// Initialize the hash to a 'random' value

		uint32_t h = seed ^ len;

		// Mix 4 bytes at a time into the hash

		const unsigned char * data = (const unsigned char *)key;

		while(len >= 4)
		{
			unsigned int k = *(unsigned int *)data;

			k *= m; 
			k ^= k >> r; 
			k *= m; 
			
			h *= m; 
			h ^= k;

			data += 4;
			len -= 4;
		}
		
		// Handle the last few bytes of the input array

		switch(len)
		{
		case 3: h ^= data[2] << 16;
		case 2: h ^= data[1] << 8;
		case 1: h ^= data[0];
		        h *= m;
		};

		// Do a few final mixes of the hash to ensure the last few
		// bytes are well-incorporated.

		h ^= h >> 13;
		h *= m;
		h ^= h >> 15;

		return h;
	}

	uint64_t murmur_hash_64(const void * key, uint32_t len, uint32_t seed)
	{
		const uint64_t m = 0xc6a4a7935bd1e995;
		const int r = 47;

		uint64_t h = seed ^ (len * m);

		const uint64_t * data = (const uint64_t *)key;
		const uint64_t * end = data + (len / 8);

		while (data != end)
		{
			uint64_t k = *data++;

			k *= m;
			k ^= k >> r;
			k *= m;

			h ^= k;
			h *= m;
		}

		const unsigned char * data2 = (const unsigned char*)data;

		switch (len & 7)
		{
		case 7: h ^= uint64_t(data2[6]) << 48;
		case 6: h ^= uint64_t(data2[5]) << 40;
		case 5: h ^= uint64_t(data2[4]) << 32;
		case 4: h ^= uint64_t(data2[3]) << 24;
		case 3: h ^= uint64_t(data2[2]) << 16;
		case 2: h ^= uint64_t(data2[1]) << 8;
		case 1: h ^= uint64_t(data2[0]);
			h *= m;
		};

		h ^= h >> r;
		h *= m;
		h ^= h >> r;

		return h;
	}
}