#pragma once

// SDK includes
#include "bento_base/platform.h"
#include "bento_math/types.h"
#include "bento_memory/common.h"
#include "bento_compute/compute_types.h"

#if defined(OPENCL_SUPPORTED)
namespace bento
{
	// Compute context functions
	ComputeContext create_compute_context(bento::IAllocator& allocator);
	void destroy_compute_context(ComputeContext context);

	// Program create and destruction
	ComputeProgram create_program_source(ComputeContext context, const char* program_source);
	void destroy_program(ComputeProgram program);

	// Command list creation and destruction
	ComputeCommandList create_command_list(ComputeContext context, bento::IAllocator& allocator);
	void destroy_command_list(ComputeCommandList computeCommandList);

	// Kernel creation and destruction
	ComputeKernel create_kernel(ComputeProgram program, const char* kernel_name);
	void destroy_kernel(ComputeKernel kernel);

	// Buffer creation
	ComputeBuffer create_buffer(ComputeContext context, uint64_t buffer_size, ComputeBufferType::Type buffer_type, bento::IAllocator& allocator);
	void destroy_buffer(ComputeBuffer buffer);
	
	// Dispatch dimensions
	uint32_t dispatch_tile_size(ComputeContext context);
	IVector3 dispatch_tile_resolution(ComputeContext context);
	bool dispatch_kernel(ComputeCommandList commandList, ComputeKernel kernel, IVector3 numTilesParam, IVector3 tileSize);

	// Read and writing to the GPU memory
	bool read_buffer(ComputeCommandList commandList, ComputeBuffer buffer, void* output_data);
	bool write_buffer(ComputeCommandList commandList, ComputeBuffer buffer, void* intput_data);

	// Flushing the command list
	void flush_command_list(ComputeCommandList commandList);

	// Setting parameters to kernels
	bool kernel_argument(ComputeKernel kernel, uint32_t idx, ComputeBuffer buffer);
	bool kernel_argument(ComputeKernel kernel, uint32_t idx, uint32_t size, const void* raw_data);
}
#endif