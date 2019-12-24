#pragma once

#if defined(LINUXPC)
	// Includes
	#include <unistd.h>
	#include <execinfo.h>
	#include <stdint.h>

	// Defines
	#define FUNCTION_NAME __PRETTY_FUNCTION__
	#define EXCEPTION_STACK_SIZE 20
	#define SLEEP_FUNCTION(time) sleep(time)

#elif defined(WINDOWSPC)
	// Includes
	#include <stdint.h>
	#include <windows.h>
	#define SLEEP_FUNCTION(time) Sleep(time)

	// defines
	#define FUNCTION_NAME __func__
#endif