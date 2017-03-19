#include "log.h"

//----------------------------------------------------------------------------------
// Provide a facility to record events in a log.

namespace logging {

	//----------------------------------------------------------------------------------
	// Constructor 
	LogEntry::LogEntry(const std::string& in_msg)
	{
		*this << in_msg;
	}

	//----------------------------------------------------------------------------------
	// Copy constructor since the stream is not directly copy-able.
	LogEntry::LogEntry(const LogEntry& in_from)
	{
		CopyFrom(in_from);
	}

	//----------------------------------------------------------------------------------
	// Perform an operation on the log including f lushing/clearing it.
	LogEntry&	LogEntry::operator=(const LogEntry& in_rhs)
	{
		CopyFrom(in_rhs);
		return *this;
	}

	//----------------------------------------------------------------------------------
	// Perform an operation on the log including f lushing/clearing it.
	void	LogEntry::CopyFrom(const LogEntry& in_from)
	{
		*this << in_from.str();
		level_logged_ = in_from.level_logged_;
	}

	//----------------------------------------------------------------------------------
	// A collection of log entries that can either be buffered (kept in memory) or written
	// to an output (e.g. file) as each entry is logged.  Note that even if the priority 
	// is lower than the level being logged, the entry gets included in the log list but
	// excluded when the log is flushed so the caller can change the level later (e.g. 
	// when an exception is detected).
	LogEntry&	Log::log(LogEntry& in_entry)
	{
		std::vector<LogEntry>::push_back(in_entry);
		return at(size() - 1);
	}
	LogEntry&	Log::log(const level_logged_e in_level)
	{
		std::vector<LogEntry>::push_back(LogEntry(in_level));
		return at(size() - 1);
	}

	//----------------------------------------------------------------------------------
	// Perform an operation on the log including flushing/clearing it.
	std::string	Log::flush()
	{
		std::stringstream	result;

		result << *this;
		this->clear();
		
		return result.str();
	}

	//----------------------------------------------------------------------------------
	// Print the log entries to the output stream.
	std::ostream& operator<<(std::ostream& output, Log& in_log)
	{
		for (LogEntry& entry : in_log)
		{
			if (entry.GetLevel() >= in_log.GetLevel())
			{
				output << entry.ToString() << std::endl;
			}
		}
		return output;
	}

	//----------------------------------------------------------------------------------
	// Copy the contents of the log from another log.
	Log::Log(const Log& in_from)
	{
		CopyFrom(in_from);
	}

	//----------------------------------------------------------------------------------
	// Copy the contents of the log from another log.
	Log&	Log::operator = (const Log& in_rhs)
	{
		CopyFrom(in_rhs);
		return *this;
	}

	//----------------------------------------------------------------------------------
	// Copy the contents of the log from another log.
	void	Log::CopyFrom(const Log& in_from)
	{
		clear();
		for (const LogEntry& from_entry : in_from)
		{
			push_back(LogEntry(from_entry));
		}
		level_logged_ = in_from.level_logged_;
	}


}
