// nyx includes
#include "bento_base/platform.h"
#include "bento_base/log.h"
#include "bento_base/security.h"

// External includes
#include <assert.h>

namespace bento {
	void print_trace()
	{
		#if defined(LINUXPC) || defined (OSX)
		    void *array[EXCEPTION_STACK_SIZE];
		    size_t size;
		    size = backtrace(array, EXCEPTION_STACK_SIZE);
		    backtrace_symbols_fd(array, size, STDERR_FILENO);
		#endif
	}

	void __handle_fail(const char* msg, const char* file_name, int)
	{
		default_logger()->log(LogLevel::error, "FAILURE", msg);
		default_logger()->log(LogLevel::error, "FAILURE", file_name);
		print_trace();
		assert(false);
	}
}
