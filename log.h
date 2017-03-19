#pragma once

#include <string>
#include <vector>
#include <sstream>

//----------------------------------------------------------------------------------
// Provide a facility to record events in a log.

namespace logging {

	// Set the level at which messages will be logged.  Each log entry
	// can be assigned a level.  It will only be included in the log
	// if the log has the same level of higher.

	enum level_logged_e
	{
		debug,
		trace,
		info,
		warning,
		error
	};

	//----------------------------------------------------------------------------------
	// Define a class for passing control information to the log.  Passing objects of
	// the class allows the compiler to distinguish the control commands from data
	// being logged.
	class Control {
	public:
		Control(const level_logged_e& in_level) 
			: level_(in_level)
		{}

		level_logged_e	level_ = error;
	};

	//----------------------------------------------------------------------------------
	// This class represents a single log entry stored as a string.  The intention of 
	// supporting stream operators is for the log entry to be created and written to
	// much like a stream.
	class LogEntry : public std::stringstream {
	public:

		LogEntry(level_logged_e in_level_logged = error)
			: level_logged_(in_level_logged)
		{}
		LogEntry(const LogEntry& in_from);
		LogEntry(const std::string& in_msg);

		LogEntry&	operator=(const LogEntry& in_rhs);

		level_logged_e	GetLevel() const { return level_logged_; }

		std::string	ToString() { return std::stringstream::str(); }
		
	private:

		void	CopyFrom(const LogEntry& in_from);

		level_logged_e	level_logged_ = error;
	};
	
	//----------------------------------------------------------------------------------
	// A collection of log entries that can either be buffered (kept in memory) or written
	// to an output (e.g. file) as each entry is logged.
	class Log : public std::vector<LogEntry> {
	public:

		Log()
			: level_logged_(error)
		{}

		Log(const Control& in_level_logged)
			: level_logged_(in_level_logged.level_)
		{}

		Log(const Log& in_from);
		
		Log&	operator=(const Log& in_rhs);

		LogEntry&	log(LogEntry& in_entry);
		LogEntry&	log(const level_logged_e in_level = error);

		std::string	flush();

		level_logged_e	GetLevel() const { return level_logged_; }
		void			SetLevel(level_logged_e in_level) { level_logged_ = in_level; }

		bool	buffer_ = true; // Hold a collection of the log entries rather than writing each separately.

	private:

		void	CopyFrom(const Log& in_from);

		level_logged_e	level_logged_;
	};

	// Print the log entries to the output stream.
	std::ostream& operator<<(std::ostream& output, Log& in_log);

}
