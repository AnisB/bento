// Library includes
#include "bento_compute/compute_api.h"
#include "bento_base/security.h"
#include "bento_collection/vector.h"
#include "bento_collection/dynamic_string.h"

#if defined(OPENCL_SUPPORTED)
#include <CL/cl.h>
#include <fstream>
#include <algorithm>

namespace bento
{
	struct OpenCLContext
	{
		ALLOCATOR_BASED;
		OpenCLContext(IAllocator& allocator): vendorName(allocator), _allocator(allocator) {}

		// Context / device
		cl_context context;
		cl_device_id device_id;
		
		// Debug info
		DynamicString vendorName;

		// Dispatch parameters
		size_t tileDimension;
		size_t tileSize;
		IVector3 tileResolution;
		IAllocator& _allocator;
	};

	struct OpenCLCommandList
	{
		cl_command_queue commandList;
		size_t tileSize;
		IVector3 tileResolution;
		bento::IAllocator* _allocator;
	};

	struct OpenCLBuffer
	{
		cl_mem buffer;
		uint64_t size;
		ComputeBufferType::Type type;
		bento::IAllocator* _allocator;
	};

	uint64_t evaluate_platforms(bento::Vector<cl_platform_id>& platform_ids)
	{
		uint64_t final_platform = 0;
		size_t global_dim = 0;
		uint64_t num_platforms = platform_ids.size();
		for (uint32_t plat_idx = 0; plat_idx < num_platforms; ++plat_idx)
		{
			cl_device_id device_id;
			clGetDeviceIDs(platform_ids[plat_idx], CL_DEVICE_TYPE_ALL, 1, &device_id, NULL);
			size_t candidate_dim;
			clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &candidate_dim, NULL);
			if (candidate_dim > global_dim)
			{
				final_platform = plat_idx;
				global_dim = candidate_dim;
			}
		}
		return final_platform;
	}

	ComputeContext create_compute_context(bento::IAllocator& allocator)
	{
		// Instantiate a cl context
		OpenCLContext* new_context = bento::make_new<OpenCLContext>(allocator, allocator);

		// Get the number of platforms
		cl_uint platformIdCount = 0;
		clGetPlatformIDs(0, nullptr, &platformIdCount);

		// Get all the platforms
		bento::Vector<cl_platform_id> platformIds(allocator, platformIdCount);
		clGetPlatformIDs(platformIdCount, platformIds.begin(), nullptr);

		uint64_t best_platform = evaluate_platforms(platformIds);

		// Fetch the id of the first available device
		cl_int error_flag = clGetDeviceIDs(platformIds[(uint32_t)best_platform], CL_DEVICE_TYPE_GPU, 1, &new_context->device_id, NULL);
	    if (error_flag != CL_SUCCESS)
	    {
	        delete new_context;
			assert_fail_msg("Can't fetch devices");
			return 0;
	    }

	    // Create the context
		new_context->context = clCreateContext(0, 1, &new_context->device_id, NULL, NULL, &error_flag);
	  	if (!new_context->context)
	    {
       		delete new_context;
			assert_fail_msg("Can't create context");
	        return 0;
	    }

		// Get the vendor name
		size_t vendorLength;
		error_flag = clGetDeviceInfo(new_context->device_id, CL_DEVICE_VENDOR, 0, NULL, &vendorLength);
		new_context->vendorName.resize((uint32_t)vendorLength);
		error_flag = clGetDeviceInfo(new_context->device_id, CL_DEVICE_VENDOR, vendorLength, new_context->vendorName.c_str(), NULL);

	    // Grab the maximal work group size
		error_flag = clGetDeviceInfo(new_context->device_id, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), (void*)&new_context->tileSize, NULL);

		// Grab the maximal size of each individual tile
		size_t tileResolutions[3];
		error_flag = clGetDeviceInfo(new_context->device_id, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * 3, (void*)&tileResolutions, NULL);
		new_context->tileResolution.x = (uint32_t)tileResolutions[0];
		new_context->tileResolution.y = (uint32_t)tileResolutions[1];
		new_context->tileResolution.z = (uint32_t)tileResolutions[2];

		// Grab the max work dimension
		error_flag = clGetDeviceInfo(new_context->device_id, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(size_t), (void*)&new_context->tileDimension, NULL);

	    // All done lets go fella
	    return (ComputeContext)new_context;
	}

	void destroy_compute_context(ComputeContext context)
	{
		OpenCLContext* target_context = (OpenCLContext*) context;
		clReleaseContext(target_context->context);
		bento::make_delete<OpenCLContext>(target_context->_allocator, target_context);
	}

	ComputeCommandList create_command_list(ComputeContext context, bento::IAllocator& allocator)
	{
		OpenCLCommandList* newCommandList = bento::make_new<OpenCLCommandList>(allocator);
		newCommandList->_allocator = &allocator;
		OpenCLContext* target_context = (OpenCLContext*)context;
		newCommandList->tileSize = target_context->tileSize;
		newCommandList->tileResolution = target_context->tileResolution;

		cl_int errorFlag;
		newCommandList->commandList = clCreateCommandQueue(target_context->context, target_context->device_id, 0, &errorFlag);
		if (!newCommandList->commandList || errorFlag != CL_SUCCESS)
		{
			assert_fail_msg("Can't create command list");
			return 0;
		}
		return (ComputeCommandList)newCommandList;
	}

	void destroy_command_list(ComputeCommandList computeCommandList)
	{
		OpenCLCommandList* currentCommandList = (OpenCLCommandList*)computeCommandList;
		clReleaseCommandQueue(currentCommandList->commandList);
		bento::make_delete<OpenCLCommandList>(*currentCommandList->_allocator, currentCommandList);
	}

	ComputeProgram create_program_source(ComputeContext context, const char* programSource)
	{
		// Fetch the context
		OpenCLContext* target_context = (OpenCLContext*)context;

		// Create a program
		cl_program program;
		cl_int error_flag;
		program = clCreateProgramWithSource(target_context->context, 1, &programSource, NULL, &error_flag);
		if (error_flag || !program)
		{
			assert_fail_msg("Can't create program with source");
			return 0;
		}
		error_flag = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
		if (error_flag != CL_SUCCESS)
		{
			// Get the length of the log buffer
			size_t len;
			error_flag = clGetProgramBuildInfo(program, target_context->device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &len);

			// Read the build errors
			bento::DynamicString errorString(target_context->_allocator);
			errorString.resize((uint32_t)len);
			error_flag = clGetProgramBuildInfo(program, target_context->device_id, CL_PROGRAM_BUILD_LOG, len, errorString.c_str(), NULL);

			// Release the program
			clReleaseProgram(program);

			// Fail
			assert_fail_msg(errorString.c_str());
			return 0;
		}
		return (ComputeProgram)program;
	}

	void destroy_program(ComputeProgram gpu_program)
	{
		clReleaseProgram((cl_program)gpu_program);
	}

	ComputeKernel create_kernel(ComputeProgram program, const char* kernel_name)
	{
		cl_int error_flag;
		cl_kernel kernel = clCreateKernel((cl_program)program, kernel_name, &error_flag);
		if (!kernel || error_flag != CL_SUCCESS)
		{
			assert_fail_msg("Can't create kernel");
			return 0;
		}
		return (ComputeKernel)kernel;
	}

	uint32_t dispatch_tile_size(ComputeContext context)
	{
		OpenCLContext* target_context = (OpenCLContext*)context;
		return (uint32_t)target_context->tileSize;
	}

	IVector3 dispatch_tile_resolution(ComputeContext context)
	{
		OpenCLContext* target_context = (OpenCLContext*)context;
		return target_context->tileResolution;
	}

	bool dispatch_kernel(ComputeCommandList commandList, ComputeKernel kernel, IVector3 numTilesParam, IVector3 tileSize)
	{
		OpenCLCommandList* currentCommandList = (OpenCLCommandList*)commandList;

		// The tile sizes
		size_t tilesSizes[3];
		tilesSizes[0] = std::min((size_t)tileSize.x, (size_t)currentCommandList->tileResolution.x);
		tilesSizes[1] = std::min((size_t)tileSize.y, (size_t)currentCommandList->tileResolution.y);
		tilesSizes[2] = std::min((size_t)tileSize.z, (size_t)currentCommandList->tileResolution.z);
		size_t totalTileSize = tilesSizes[0] * tilesSizes[1] * tilesSizes[2];
		assert(totalTileSize <= currentCommandList->tileSize);

		// Compute the global workload per dimension
		size_t globalWorkLoad[3];
		globalWorkLoad[0] = numTilesParam.x * tilesSizes[0];
		globalWorkLoad[1] = numTilesParam.y * tilesSizes[1];
		globalWorkLoad[2] = numTilesParam.z * tilesSizes[2];

		cl_int error_flag = clEnqueueNDRangeKernel(currentCommandList->commandList, (cl_kernel)kernel, 3, NULL, globalWorkLoad, tilesSizes, 0, NULL, NULL);

		if (error_flag != CL_SUCCESS)
		{
			assert_fail_msg("Kernel cannot be dispatched");
			return false;
		}
		return true;
	}

	void flush_command_list(ComputeCommandList commandList)
	{
		OpenCLCommandList* currentCommandList = (OpenCLCommandList*)commandList;
		clFinish(currentCommandList->commandList);
	}

	void destroy_kernel(ComputeKernel kernel)
	{
		clReleaseKernel((cl_kernel)kernel);
	}

	ComputeBuffer create_buffer(ComputeContext context, uint64_t buffer_size, ComputeBufferType::Type buffer_type, bento::IAllocator& allocator)
	{
		OpenCLBuffer* buffer = bento::make_new<OpenCLBuffer>(allocator);
		buffer->_allocator = &allocator;
		buffer->size = buffer_size;
		buffer->type = buffer_type;

		OpenCLContext* target_context = (OpenCLContext*)context;

		cl_int error_flag;
		buffer->buffer = clCreateBuffer(target_context->context, buffer_type, buffer_size, NULL, &error_flag);
		// Copy data to input buffer
		if (error_flag != CL_SUCCESS)
		{
			delete buffer;
			assert_fail_msg("Can't create buffer");
			return 0;
		}
		return (ComputeBuffer)buffer;
	}

	bool read_buffer(ComputeCommandList commandList, ComputeBuffer buffer, void* output_data)
	{
		OpenCLCommandList* currentCommandList = (OpenCLCommandList*)commandList;
		OpenCLBuffer* target_buffer = (OpenCLBuffer*)buffer;
		cl_int error_flag = clEnqueueReadBuffer(currentCommandList->commandList, target_buffer->buffer, CL_TRUE, 0, target_buffer->size, output_data, 0, NULL, NULL);
		if (error_flag != CL_SUCCESS)
		{
			assert_fail_msg("Can't read buffer");
			return false;
		}
		return true;
	}

	bool write_buffer(ComputeCommandList commandList, ComputeBuffer buffer, void* intput_data)
	{
		OpenCLCommandList* currentCommandList = (OpenCLCommandList*)commandList;
		OpenCLBuffer* target_buffer = (OpenCLBuffer*)buffer;
		cl_int error_flag = clEnqueueWriteBuffer(currentCommandList->commandList, target_buffer->buffer, CL_TRUE, 0, target_buffer->size, intput_data, 0, NULL, NULL);
		if (error_flag != CL_SUCCESS)
		{
			assert_fail_msg("Can't write buffer");
			return false;
		}
		return true;
	}

	void destroy_buffer(ComputeBuffer buffer)
	{
		OpenCLBuffer* current_buffer = (OpenCLBuffer*)buffer;
		clReleaseMemObject((cl_mem)current_buffer->buffer);
		bento::make_delete<OpenCLBuffer>(*current_buffer->_allocator, current_buffer);
	}

	bool kernel_argument(ComputeKernel kernel, uint32_t idx, uint32_t size, const void* raw_data)
	{
		cl_int error_flag = clSetKernelArg((cl_kernel)kernel, idx, size, raw_data);
		if (error_flag != CL_SUCCESS)
		{
			assert_fail_msg("Can't set kernel argument");
			return false;
		}
		return true;
	}

	bool kernel_argument(ComputeKernel kernel, uint32_t idx, ComputeBuffer buffer)
	{
		OpenCLBuffer* current_buffer = (OpenCLBuffer*)buffer;
		cl_int error_flag = clSetKernelArg((cl_kernel)kernel, idx, sizeof(cl_mem), &(current_buffer->buffer));
		if (error_flag != CL_SUCCESS)
		{
			assert_fail_msg("Can't set kernel argument");
			return false;
		}
		return true;
	}
}
#endif