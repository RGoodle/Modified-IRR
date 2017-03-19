#include "date_math.h"

//----------------------------------------------------------------------------------
// This file provides functions for adding/subtracting dates.
//----------------------------------------------------------------------------------

namespace dates {

	//----------------------------------------------------------------------------------
	// Return whether or not a year is a leap year.
	bool IsLeapYear(int year) {
		if (year % 4 != 0) return false;
		if (year % 400 == 0) return true;
		if (year % 100 == 0) return false;
		return true;
	}

	//----------------------------------------------------------------------------------
	// Given a particular month, return the number of days in that month.
	int GetDaysInMonth(int year, int month)	{
		assert(month >= 0);
		assert(month < 12);

		int days = daysInMonths[month];

		if (month == 1 && IsLeapYear(year)) // February of a leap year
			days += 1;

		return days;
	}

	//----------------------------------------------------------------------------------
	// Return the date n months after the date provided.
	tm AddMonths(const tm& in_date, int months)	{

		bool isLastDayInMonth = (in_date.tm_mday == GetDaysInMonth(in_date.tm_year, in_date.tm_mon));

		int year = in_date.tm_year + months / 12;
		int month = in_date.tm_mon + months % 12;

		if (month > 11) {
			year += 1;
			month -= 12;
		}

		int day;

		if (isLastDayInMonth) {
			day = GetDaysInMonth(year, month); // Last day of month maps to last day of result month
		} else {
			day = std::min(in_date.tm_mday, GetDaysInMonth(year, month));
		}

		tm result = tm();

		result.tm_year = year;
		result.tm_mon = month;
		result.tm_mday = day;

		result.tm_hour = in_date.tm_hour;
		result.tm_min = in_date.tm_min;
		result.tm_sec = in_date.tm_sec;

		return result;
	}

	//----------------------------------------------------------------------------------
	// Return the date n months after the date provided.
	time_t AddMonths(const time_t &in_date, int months)
	{
		tm date = tm();
		localtime_s(&date, &in_date);
		tm result = AddMonths(date, months);

		return mktime(&result);
	}

	//----------------------------------------------------------------------------------
	// Return the difference between two date/times in seconds.
	double GetDifferenceSeconds(const time_t& in_lhs, const time_t& in_rhs)
	{
		return std::difftime(in_lhs, in_rhs);
	}

	//----------------------------------------------------------------------------------
	// Return the difference between two date/times in seconds.
	double GetDifferenceDays(const time_t& in_lhs, const time_t& in_rhs)
	{
		return (GetDifferenceSeconds(in_lhs, in_rhs) / kSecondsPerDay);
	}

	//----------------------------------------------------------------------------------
	// Return a string representation of a time.
	std::string	Date2String(const time_t& in_date)
	{
		char buffer[20];
		struct tm time_parts;
		localtime_s(&time_parts, &in_date);

		strftime(buffer, 20, "%Y-%m-%d", &time_parts);
		return std::string(buffer);
	}

	//----------------------------------------------------------------------------------
	// Return a string representation of a time.
	std::string	Time2String(const time_t& in_date)
	{
		char buffer[20];
		struct tm time_parts;
		localtime_s(&time_parts, &in_date);

		strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", &time_parts);
		return std::string(buffer);
	}

	//----------------------------------------------------------------------------------
	// Given a string date (as YYYY-MM-DD), return a time structure for it.
	time_t	MakeDate(const std::string& in_date)
	{
		std::tm t = {};
		std::istringstream ss(in_date);
		ss.imbue(std::locale());
		ss >> std::get_time(&t, "%Y-%m-%d");

		std::time_t new_date = std::mktime(&t);

		return new_date;
	}


}

