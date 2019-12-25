#pragma once

// Library includes
#include "log.h"

namespace bento {
	// Lgger that logs through the standard output
	class SystemLogger : public ILogger
	{
	public:
		SystemLogger();
		~SystemLogger();

		// The virtual logging function that must be 
		void log(LogLevel::Type log_level, const char* tag, const char* message) override;

		// new line
		void new_line();
	};
}