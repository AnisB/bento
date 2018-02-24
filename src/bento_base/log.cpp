// Library includes
#include "bento_base/log.h"
#include "bento_base/system_logger.h"

namespace bento {
	// This the default logger we provide for the user
	SystemLogger __default_system_logger;
	ILogger* __default_logger = &__default_system_logger;

	ILogger* default_logger()
	{
		return __default_logger;
	}

	void set_default_logger(ILogger* new_default_logger)
	{
		__default_logger = new_default_logger;
	}

	void reset_logger()
	{
		__default_logger = &__default_system_logger;
	}
}