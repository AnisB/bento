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

    inline void* platform_allocate(size_t size, size_t alignment)
    {
        return memalign(alignment, size);
    }

    inline void platform_free(void* ptr)
    {
        free(ptr);
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

    inline void* platform_allocate(size_t size, size_t alignment)
    {
        return _aligned_malloc(size, alignment);
    }

    inline void platform_free(void* ptr)
    {
        _aligned_free(ptr);
    }
    // defines
    #define FUNCTION_NAME __func__
#else
    #error Unsupported platfrom
#endif

// To avoid collision with of the macros with the functions
#undef min
#undef max