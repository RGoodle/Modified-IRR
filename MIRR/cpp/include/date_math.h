#pragma once

#include <ctime>
#include <assert.h>
#include <algorithm>
#include <iomanip>
#include <string>
#include <sstream> 

//----------------------------------------------------------------------------------
// This file provides functions for adding/subtracting dates.
//----------------------------------------------------------------------------------

namespace dates {

	static int daysInMonths[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	static const long	kSecondsPerDay = (60 * 60 * 24);

	//----------------------------------------------------------------------------------
	// Return whether or not a year is a leap year.
	bool IsLeapYear(int year);

	//----------------------------------------------------------------------------------
	// Given a particular month, return the number of days in that month.
	int GetDaysInMonth(int year, int month);

	//----------------------------------------------------------------------------------
	// Return the date n months after the date provided.
	tm AddMonths(const tm& in_date, int months);

	//----------------------------------------------------------------------------------
	// Return the date n months after the date provided.
	time_t AddMonths(const time_t &in_date, int months);

	//----------------------------------------------------------------------------------
	// Return the difference between two date/times in seconds.
	double GetDifferenceSeconds(const time_t& in_lhs, const time_t& in_rhs);

	//----------------------------------------------------------------------------------
	// Return the difference between two date/times in seconds.
	double GetDifferenceDays(const time_t& in_lhs, const time_t& in_rhs);

	//----------------------------------------------------------------------------------
	// Return a string representation of a time.
	std::string	Date2String(const time_t& in_date);

	//----------------------------------------------------------------------------------
	// Return a string representation of a time.
	std::string	Time2String(const time_t& in_date);

	//----------------------------------------------------------------------------------
	// Given a string date (as YYYY-MM-DD), return a time structure for it.
	time_t	MakeDate(const std::string& in_date);

}

