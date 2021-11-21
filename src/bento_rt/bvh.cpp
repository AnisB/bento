// Bento includes
#include <bento_base/log.h>
#include <bento_base/stream.h>
#include <bento_rt/bvh.h>
#include <bento_rt/intersect.h>

// External includes
#include <float.h>
#include <algorithm>

namespace bento
{
	namespace bvh
	{
		// Threshold under which the subdivision will occur
		const uint32_t subdivision_threshold = 10;

		// Structure that is used to sort primitives based on their data while keeping their index properly
		struct PrimitiveData
		{
			uint32_t _idx;
			float _data;
		};

		template<typename T>
		struct DataSorter
		{
			bool operator()(const T& e1, const T& e2)
			{
				return e1._data < e2._data;
			}
		};

		// Structure that optimizes the bounding box calculus
		struct SplitHelper
		{
			ALLOCATOR_BASED;
			SplitHelper(IAllocator& allocator, uint32_t num_primitives)
			: _boxes(allocator)
			, _boxes_split_buffer(allocator)
			, _max_values(allocator)
			{
				_boxes.resize(num_primitives);
				_boxes_split_buffer.resize(num_primitives);
				_max_values.resize(num_primitives * 3);
			}

			Vector<Box3> _boxes;
			Vector<Box3> _boxes_split_buffer;
			Vector<PrimitiveData> _max_values;
		};

		uint32_t allocate_node_array(Bvh& b, uint32_t count)
		{
			uint32_t num_nodes = b.nodes.size();
			b.nodes.resize(num_nodes + count);
			return num_nodes;
		}

		void subdivide_node(Bvh& b, Accessor& accessor, uint32_t node_idx, uint32_t primitive_shift, uint32_t num_primitives, const Box3& _box, SplitHelper& sh)
		{
			if (num_primitives >= subdivision_threshold) {
				// Compute the bounding boxes for all the entries that we must process
				for (uint32_t local_idx = 0; local_idx < num_primitives; ++local_idx)
				{
					uint32_t global_idx = b.primitives[primitive_shift + local_idx]._primitiveID;
					sh._boxes[local_idx] = accessor.box(accessor.container, global_idx);
				}

				// Compute the max values for each entry that we must process on the 3 dimensions 
				for (uint32_t local_idx = 0; local_idx < num_primitives; ++local_idx)
				{
					const Vector3& max_val = sh._boxes[local_idx].max;

					PrimitiveData& max_x_value = sh._max_values[local_idx];
					max_x_value._idx = local_idx;
					max_x_value._data = at_index(max_val, 0);

					PrimitiveData& max_y_value = sh._max_values[local_idx + num_primitives];
					max_y_value._idx = local_idx;
					max_y_value._data = at_index(max_val, 1);

					PrimitiveData& max_z_value = sh._max_values[local_idx + 2 * num_primitives];
					max_z_value._idx = local_idx;
					max_z_value._data = at_index(max_val, 2);
				}

				// Sort the max values of the entries on each dimension
				std::sort(sh._max_values.begin(), sh._max_values.begin() + num_primitives, DataSorter<PrimitiveData>());
				std::sort(sh._max_values.begin() + num_primitives, sh._max_values.begin() + 2 * num_primitives, DataSorter<PrimitiveData>());
				std::sort(sh._max_values.begin() + 2 * num_primitives, sh._max_values.begin() + 3 * num_primitives, DataSorter<PrimitiveData>());

				// Data where that will contain the best candidate
				Box3 final_box[2];
				float final_cost = FLT_MAX;
				PrimitiveData* final_sorted_primitives = nullptr;
				uint32_t final_split_idx = 0;

				// We process each subgroup that he have generated
				for (char dimension = 0; dimension < 3; ++dimension)
				{
					// Fetch the sorted entries for this dimension
					PrimitiveData* current_dimension_sorted_array = &sh._max_values[dimension * num_primitives];

					// Here, we gradually include one element after the other one and generate the bounding box that matches it (is used to avoid recomputing the bbox)
					Box3 right_box_node;
					for (int32_t local_idx = num_primitives - 1; local_idx >= 0; --local_idx)
					{
						box3::include_box(right_box_node, sh._boxes[current_dimension_sorted_array[local_idx]._idx]);
						sh._boxes_split_buffer[local_idx] = right_box_node;
					}

					Box3 tmp_boxes[2];

					for (uint32_t local_idx = 0; local_idx < num_primitives; ++local_idx)
					{
						tmp_boxes[1] = sh._boxes_split_buffer[local_idx];
						float current_cost = local_idx * box3::surface_area(tmp_boxes[0]) + (num_primitives - local_idx) * box3::surface_area(tmp_boxes[1]);

						// Is the current cost better than the previously stated one ?
						if (current_cost < final_cost) {
							// Override the output data
							final_cost = current_cost;
							final_box[0] = tmp_boxes[0];
							final_box[1] = tmp_boxes[1];
							final_sorted_primitives = current_dimension_sorted_array;
							final_split_idx = local_idx;
						}

						// Update the left child's bounding box by adding one more object
						box3::include_box(tmp_boxes[0], sh._boxes[current_dimension_sorted_array[local_idx]._idx]);
					}
				}

				// Did we find a suitable subdivision?
				if (final_split_idx != 0) {
					// For each entry, adjust the ownership data
					for (uint32_t local_idx = 0; local_idx < num_primitives; ++local_idx)
					{
						PrimitiveData& current_primitive_ref = final_sorted_primitives[local_idx];
						b.primitives[primitive_shift + current_primitive_ref._idx]._data = local_idx < final_split_idx ? 0 : 1;
					}

					// Sort the array per owner-id in order to have them ordered for sons
					std::sort(b.primitives.begin() + primitive_shift, b.primitives.begin() + primitive_shift + num_primitives, DataSorter<BvhPrimitive>());

					// Allocate the children
					uint32_t child_idx = allocate_node_array(b, 2);

					// Fetch the current node to fill
					BvhNode& current_node = b.nodes[node_idx];

					// Fill the node data
					current_node._box = _box;
					current_node._numPrimitives = 0;
					current_node._offset = child_idx;

					// Subdivide the node's hierarchy
					subdivide_node(b, accessor, child_idx, primitive_shift, final_split_idx, final_box[0], sh);
					subdivide_node(b, accessor, child_idx + 1, primitive_shift + final_split_idx, num_primitives - final_split_idx, final_box[1], sh);
				} 
				else
				{
					BvhNode& current_node = b.nodes[node_idx];
					current_node._box = _box;
					current_node._numPrimitives = num_primitives;
					current_node._offset = primitive_shift;
					default_logger()->log(LogLevel::warning, "BVH", "Node is not well divided");
				}
			} 
			else
			{
				// Just assign everything to this node
				BvhNode& current_node = b.nodes[node_idx];
				current_node._box = _box;
				current_node._numPrimitives = num_primitives;
				current_node._offset = primitive_shift;
			}
		}

		void build(Bvh& b, Accessor& accessor)
		{
			// Allocate the entries
			uint32_t num_primitives = accessor.num_primitives(accessor.container);
			b.primitives.resize(num_primitives);

			// Fill them
			for (uint32_t primitive_idx = 0; primitive_idx < num_primitives; ++primitive_idx)
			{
				b.primitives[primitive_idx]._primitiveID = primitive_idx;
			}

			// Compute the total box
			Box3 combined_box;
			for (uint32_t primitive_idx = 0; primitive_idx < num_primitives; ++primitive_idx)
			{
				box3::include_box(combined_box, accessor.box(accessor.container, primitive_idx));
			}

			// Allocate the root
			uint32_t root_idx = allocate_node_array(b, 1);

			// Build the box helper
			SplitHelper split_helper(*common_allocator(), num_primitives);

			// Launch the subdivision of this node
			subdivide_node(b, accessor, root_idx, 0, num_primitives, combined_box, split_helper);
		}

		uint32_t intersect_node(const Ray& ray, Accessor& accessor, const Bvh& b, uint32_t node_idx, float& dist)
		{
			uint32_t result = UINT32_MAX;
			const BvhNode& current_node = b.nodes[node_idx];
			float tmp_dist = FLT_MAX;
			if (intersect(ray.orig, ray.inv_dir, current_node._box.min, current_node._box.max, tmp_dist) && tmp_dist < dist)
			{
				// is it a leaf?
				if (current_node._numPrimitives)
				{
					uint32_t last_entry = current_node._offset + current_node._numPrimitives;
					for (uint32_t primitive_idx = current_node._offset; primitive_idx < last_entry; ++primitive_idx)
					{
						uint32_t tmp_result = accessor.intersect(accessor.container, ray, primitive_idx, dist);
						result = tmp_result != UINT32_MAX ? tmp_result : result;
					}
				}
				else
				{
					uint32_t tmp_result = bvh::intersect_node(ray, accessor, b, current_node._offset, dist);
					result = tmp_result != UINT32_MAX ? tmp_result : result;
					tmp_result = bvh::intersect_node(ray, accessor, b, current_node._offset + 1, dist);
					result = tmp_result != UINT32_MAX ? tmp_result : result;
				}
			}

			return result;
		}

		uint32_t intersect(const Ray& ray, Accessor& accessor, const Bvh& b, float& dist)
		{
			return intersect_node(ray, accessor, b, 0, dist);
		}

		// Serialization and Deserialization functions
		void pack(Vector<char>& buffer, const Bvh& b)
		{
			pack_vector_bytes(buffer, b.primitives);
			pack_vector_bytes(buffer, b.nodes);
		}
		
		void unpack(const char *& buffer, Bvh& b)
		{
			unpack_vector_bytes(buffer, b.primitives);
			unpack_vector_bytes(buffer, b.nodes);
		}
	}
}