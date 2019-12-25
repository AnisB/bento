#pragma once

namespace bento {
	// The different log levels that are supported
	namespace LogLevel {
		enum Type
		{
			debug = 0,
			info = 1, 
			warning = 2,
			error = 3,
		};	
	}

	// The logger interface that must be overloaded by your custom logger
	class ILogger
	{
	public:
		// Cst & Dst
		ILogger() {}
		virtual ~ILogger() {}

		// The virtual logging function that must be 
		virtual void log(LogLevel::Type log_level, const char* tag, const char* message) = 0;

		// Insert empty new line
		virtual void new_line() = 0;
	};

	// default logger manipulation
	ILogger* default_logger();
	void set_default_logger(ILogger* _loggerInterface);
	void reset_logger();
}