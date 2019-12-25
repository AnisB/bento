// Library includes
#include "bento_base/system_logger.h"

// External incldues
#include <iostream>

namespace bento {

	const char* level_to_string(LogLevel::Type log_level) {
		switch(log_level)
		{
			case LogLevel::debug:
				return "[DEBUG]";
			case LogLevel::info:
				return "[INFO]";
			case LogLevel::warning:
				return "[WARNING]";
			case LogLevel::error:
				return "[ERROR]";
		};
		return "";
	}

	SystemLogger::SystemLogger() {
	}

	SystemLogger::~SystemLogger() {
	}

	void SystemLogger::log(LogLevel::Type log_level, const char* tag, const char* message) {
		std::cout<< level_to_string(log_level) << "[" << tag << "]" << message << "\n";
	}

	void SystemLogger::new_line()
	{
		std::cout << "\n";
	}
}