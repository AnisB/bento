#pragma once

// bento includes
#include <bento_collection/vector.h>
#include <bento_math/types.h>
#include <bento_rt/ray.h>
#include <bento_rt/box3.h>

namespace bento
{
	namespace bvh
	{
		// Opaque pointer for the target structure to be manipulated by the accessor
		struct Container;

		// Structure of callbacks that should be implemented for manipulating a container
		struct Accessor
		{
			// Target data structure
			const Container* container;

			// Accessor to the number of primitives that should be processed in the structure
			uint32_t(*num_primitives)(const Container* container);

			// Returns the bounding box of a given primitive
			bento::Box3 (*box)(const Container* container, uint32_t primitive_idx);

			// Intersection function (ray with a given primitive)
			bool(*intersect)(const Container* container, const Ray& ray, uint32_t primitive_idx, float& dist);
		};
	}

	// Reference by index on an primitive that shall be manipulated by the accessor
	struct BvhPrimitive
	{
		// The id of the primitive
		uint32_t _primitiveID;
		// Is it stored in a right or left primitive?
		uint8_t _data;
	};

	// Node of the bounding volume hierarchy
	struct BvhNode
	{
		// The number of primitives that are owned by this node
		uint32_t _numPrimitives;
		// Offset of the first primitive
		uint32_t _offset;
		// Bounding box of the node
		bento::Box3 _box;
	};

	// Structure that holds the data of the bounding volume hierarchy acceleration structure
	struct Bvh
	{
		ALLOCATOR_BASED;
		Bvh(bento::IAllocator& allocator) : primitives(allocator), nodes(allocator) {}
		bento::Vector<BvhPrimitive> primitives;
		bento::Vector<BvhNode> nodes;
	};

	// API to manipulate the bvh structure
	namespace bvh
	{
		// Function that builds the bvh from an accessor
		void build(Bvh& b, Accessor& accessor);

		// Intersect a container with a ray using the bvh
		uint32_t intersect(const Ray& ray, Accessor& accessor, const Bvh& b, float& dist);

		// Serialization and Deserialization methods for the BVH
		void pack(Vector<char>& v, const Bvh& b);
		void unpack(const char *& v, Bvh& b);
	}
}
