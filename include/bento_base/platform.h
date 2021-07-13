#pragma once

#if defined(LINUXPC)
	// Includes
	#include <unistd.h>
	#include <execinfo.h>
	#include <stdint.h>
	#include <ctype.h>
	#include <stdlib.h>
	#include <malloc.h>
	#include <stddef.h>
	#include <string.h>
	#include <dirent.h>
	#include <sys/stat.h>

	// Defines
	#define FUNCTION_NAME __PRETTY_FUNCTION__
	#define EXCEPTION_STACK_SIZE 20
	#define SLEEP_FUNCTION(time) sleep(time * 0.001)

	typedef int errno_t;
	inline errno_t fopen_s(FILE **f, const char *name, const char *mode)
	{
	    *f = fopen(name, mode);
	    return 0;
	}

#elif defined(WINDOWSPC)
	// Includes
	#pragma warning(push)
	#pragma warning(disable: 4505)
	#include "bento_base/dirent.h"
	#pragma warning(pop)
	#include <stdint.h>
	#include <windows.h>
	#define SLEEP_FUNCTION(time) Sleep(time)

	// defines
	#define FUNCTION_NAME __func__
#endif