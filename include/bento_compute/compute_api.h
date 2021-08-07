#pragma once

// SDK includes
#include "bento_base/platform.h"
#include "bento_memory/common.h"
#include "bento_compute/compute_types.h"

#if defined(OPENCL_SUPPORTED)
namespace bento
{
	// Context creation and destruction
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

	// Command enqueing
	bool dispatch_kernel_1D(ComputeCommandList commandList, ComputeKernel kernel, uint64_t job_size);
	bool dispatch_kernel_2D(ComputeCommandList commandList, ComputeKernel kernel, uint64_t job_size_0, uint64_t job_size_1);
	bool read_buffer(ComputeCommandList commandList, ComputeBuffer buffer, void* output_data);
	bool write_buffer(ComputeCommandList commandList, ComputeBuffer buffer, void* intput_data);

	// Flushing the command list
	void flush_command_list(ComputeCommandList commandList);

	// Setting parameters to kernels
	bool kernel_argument(ComputeKernel kernel, uint32_t idx, ComputeBuffer buffer);
	bool kernel_argument(ComputeKernel kernel, uint32_t idx, uint32_t size, const void* raw_data);
}
#endif