#pragma once

#include <ctime>
#include <algorithm>
#include <functional>
#include <sstream>
#include <vector>

#include "log.h"

//----------------------------------------------------------------------------------
//	MIRR (Modified Internal Rate of Return)
namespace mirr {

	using CashFlowAmt_t = long double;
	using NPV_t = long double;
	using Rate_t = long double;


	//----------------------------------------------------------------------------------
	// The properties of a cash flow that occured on a particular date.
	class CashFlow {

	public:

		CashFlow(const std::time_t& in_date, const CashFlowAmt_t& in_amount)
			: date_(in_date)
			, amount_(in_amount)
			, days_from_start_(0)
		{ }

		// Support copy constructor and assignment.
		CashFlow(const CashFlow& in_rhs);

		// Copy the values from a right-hand side to this object.
		CashFlow&	operator=(const CashFlow& in_rhs);

		// Copy the values from a right-hand side to this object.
		void	CopyFrom(const CashFlow& in_rhs);

		// Return whether or not this object is less than (earlier) than
		// another.
		bool	operator<(const CashFlow& in_rhs);

		// Return whether or not this object is greater than (later) than
		// another.
		bool	operator>(const CashFlow& in_rhs);

		// Return a string representation of this obejct for debugging.
		std::string	ToString();

		// Properties

		std::time_t		date_;
		CashFlowAmt_t	amount_ = 0;
		long			days_from_start_ = 0;
	};

	//----------------------------------------------------------------------------------
	// A collection of cash flows over a defined period of time.
	class CashFlowList : public std::vector < CashFlow > {
	public:
		CashFlowList() {}

		// Support copy constructor and assignment.
		CashFlowList(const CashFlowList& in_rhs);

		// Copy the values from a right-hand side to this object.
		CashFlowList&	operator=(const CashFlowList& in_rhs);

		// Copy the values from a right-hand side to this object.
		void	CopyFrom(const CashFlowList& in_rhs);

		// Add a cash flow entry to the list.  Note, for simplicity and to avoid
		// recalculating, calculate the days from start for each cash flow.
		void	push_back(CashFlow& in_new);

		// Return the number of days between the start and end dates of the list.
		long	GetDaysInRange();

		// Given a discount rate, calculate the value of the series of
		// cash flows discounted by that rate.
		NPV_t	calculateNPV(const Rate_t& in_daily_discount_rate);

	private:
		// Properties

		std::time_t		start_date_;
		std::time_t		end_date_;

	};

	//----------------------------------------------------------------------------------
	// Find the rate of return that makes the series of cash flows have an NPV = 0.
	class Calculator {

	public:
		Calculator() {}

		logging::Log	calc_log;

		// Search for the solution/root to make the series of calcualtions equal zero.
		Rate_t GetRate(CashFlowList& in_cash_flows);

	};
};

