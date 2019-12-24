// SDK includes
#include "bento_base/log.h"
#include "bento_tools/file_system.h"
#pragma warning(push)
#pragma warning(disable: 4505)
#include "bento_base/dirent.h"
#pragma warning(pop)


// External includes
#include <stdio.h>

namespace bento {

	// character used to separate all the paths in the SDK
	const char PATH_SEPARATOR = '/';
	const char EXTENSION_SEPARATOR = '.';

	namespace path {

		// Joing a root path with a sub-path
		inline DynamicString internal_join(const char* root, const char* sub_path, const char separator, IAllocator& allocator)
		{	
			// Compute the path lengths
			uint32_t root_length = strlen32(root);
			uint32_t subpath_length = strlen32(sub_path);
			uint32_t required_size = root_length + 1 + subpath_length;

			// Create the output container
			DynamicString result(allocator, required_size);

			// Fetch the raw buffer
			char* raw_buffer = result.c_str();

			// Copy the sub_paths
			memcpy(raw_buffer, root, root_length);
			raw_buffer[root_length] = separator;
			memcpy(raw_buffer + root_length + 1, sub_path, subpath_length);

			// We are done
			return result;
		}
		
		DynamicString join(const char* root, const char* sub_path, IAllocator& allocator)
		{
			return internal_join(root, sub_path, PATH_SEPARATOR, allocator);
		}

		DynamicString add_extension(const char *path, const char *extension, IAllocator& allocator)
		{
			return internal_join(path, extension, EXTENSION_SEPARATOR, allocator);
		}

		DynamicString strip_extension(const char *file_path, IAllocator& allocator)
		{
			uint32_t separator = find_last_of(file_path, EXTENSION_SEPARATOR);
			DynamicString result(allocator, separator);
			if(separator) {
				memcpy(result.c_str(), file_path, separator);
			}
			return result;
		}

		DynamicString extension(const char *file_path, IAllocator& allocator)
		{
			uint32_t path_size = strlen32(file_path);
			uint32_t separator = find_last_of(file_path, EXTENSION_SEPARATOR);
			DynamicString result(allocator, path_size  - separator);
			uint32_t extension_size = path_size  - separator;
			if(extension_size) {
				memcpy(result.c_str(), file_path + separator + 1, extension_size);
			}
			return result;
		}

		DynamicString directory(const char *file_path, IAllocator& allocator)
		{
			uint32_t separator = find_last_of(file_path, PATH_SEPARATOR);
			DynamicString result(allocator, separator);
			if(separator) {
				memcpy(result.c_str(), file_path, separator);
			}
			return result;
		}

		DynamicString filename(const char *file_path, IAllocator& allocator)
		{
			uint32_t path_size = strlen32(file_path);
			uint32_t dir_separator = find_last_of(file_path, PATH_SEPARATOR);
			dir_separator = dir_separator != path_size ? dir_separator : 0;
			uint32_t extension = find_last_of(file_path, EXTENSION_SEPARATOR);
			uint32_t filename_size = extension - dir_separator - 1;
			DynamicString result(allocator, filename_size);
			if(filename_size){
				memcpy(result.c_str(), file_path + dir_separator + 1, filename_size);
			}
			return result;
		}
	}

	namespace file_system
	{
		bool path_is_file(const char* path)
		{
		    struct stat path_stat;
		    stat(path, &path_stat);
		    return S_ISREG(path_stat.st_mode) != 0;
		}

		void collect_files_with_extension(const char* root_path, const char* target_extension, Vector<DynamicString>& output_list)
		{
	        // Opening the directory
	        DIR * directory;
	        directory = opendir (root_path);

	        if (! directory) 
	        {
				default_logger()->log(LogLevel::error, "FILE_SYSTEM", "Error in directory.");
				return;
	        }

	        uint32_t extensionSize = (uint32_t)strlen(target_extension);

	        // For each entry in the directory
	        while (1) 
	        {
	        	// If each new entry
	            struct dirent* newEntry;
	            newEntry = readdir (directory);
	            if (! newEntry)
	            {
	            	// The entry is not valid, we done
	                break;
	            }

	            // Get its filename
	            DynamicString entry_name(*common_allocator(), newEntry->d_name);

	            // Reject dummy entries
	            if((entry_name=="..") || (entry_name=="."))
	            {
	                continue;
	            }
				DynamicString absolute_path(*common_allocator(), root_path);
	            absolute_path += "/";
	            absolute_path += entry_name;

	            // Check if the entry is a directory or not
	            bool is_file = path_is_file(absolute_path.c_str());

	            if(is_file)
	            {
					const DynamicString& extension = path::extension(entry_name.c_str(), *common_allocator());
	            	if( entry_name.size() < extensionSize || extension != target_extension)
		            {
		                continue;
		            }

		            // We found a nice fella over here
					output_list.push_back(absolute_path);
	            }
	            else
	            {
	            	// We need to go recursive
					collect_files_with_extension(absolute_path.c_str(), target_extension, output_list);
	            }
	        }

	        if (closedir (directory)) 
	        {
				default_logger()->log(LogLevel::error, "FILE_SYSTEM", "Error while closing directory.");
	            return;
	        }
		}
	}

	// Write a buffer to a file
	bool write_file(const char *file_name, const char *buffer, uint32_t buffer_size, FileType type)
	{	
		bool success = false;
		if (file_name != nullptr) {
			const char * open_mode = type == FileType::Binary ? "wb" : "w";
			FILE* file_pointer = fopen(file_name, open_mode);
			if (file_pointer != nullptr) {
				// Write and ensure everything has been written
				if (fwrite(buffer, sizeof(char), buffer_size, file_pointer) == buffer_size)
					success = true;
				// Close the target file
				fclose(file_pointer);
			}
		}
		return success;
	}

	bool read_file(const char *file_name, Vector<char>& buffer, FileType type)
	{
		bool success = false;
		if (file_name != nullptr) {
			const char * open_mode = "rb";
			FILE* file_pointer = fopen(file_name, open_mode);
			if (file_pointer != nullptr) {
				fseek(file_pointer, 0L, SEEK_END);
				int buffer_size = ftell(file_pointer);
				fseek(file_pointer, 0L, SEEK_SET);
				buffer.resize(buffer_size);
				if (fread(buffer.begin(), sizeof(char), buffer_size, file_pointer) == buffer_size)
					success = true;
				fclose(file_pointer);
				// If this is supposed to be read as a string, we need to add an end chracter
				if (type == FileType::Text) buffer.push_back('\0');
			}
		}
		return success;
	}
}