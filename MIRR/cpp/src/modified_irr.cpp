#include "modified_irr.h"
#include "date_math.h"
#include "roots.h"

namespace mirr {

	//----------------------------------------------------------------------------------
	// Support copy constructor and assignment.
	CashFlow::CashFlow(const CashFlow& in_rhs)
	{
		CopyFrom(in_rhs);
	}

	//----------------------------------------------------------------------------------
	// Copy the values from a right-hand side to this object.
	CashFlow&	CashFlow::operator=(const CashFlow& in_rhs)
	{
		CopyFrom(in_rhs);
		return *this;
	}

	//----------------------------------------------------------------------------------
	// Copy the values from a right-hand side to this object.
	void	CashFlow::CopyFrom(const CashFlow& in_rhs)
	{
		date_ = in_rhs.date_;
		amount_ = in_rhs.amount_;
		days_from_start_ = in_rhs.days_from_start_;
	}

	//----------------------------------------------------------------------------------
	// Return whether or not this object is less than (earlier) than
	// another.
	bool	CashFlow::operator<(const CashFlow& in_rhs)
	{
		double	difference = dates::GetDifferenceSeconds(date_, in_rhs.date_);
		return (difference < 0);
	}

	//----------------------------------------------------------------------------------
	// Return whether or not this object is greater than (later) than
	// another.
	bool	CashFlow::operator>(const CashFlow& in_rhs)
	{
		double	difference = dates::GetDifferenceSeconds(date_, in_rhs.date_);
		return (difference > 0);
	}

	//----------------------------------------------------------------------------------
	// Return a string representation of this obejct for debugging.
	std::string	CashFlow::ToString()
	{
		std::stringstream	buffer;
		buffer << dates::Date2String(date_) << "[day " << days_from_start_ << "]=" 
			<< std::fixed << std::setw(15) << std::setprecision(3) << amount_;
		return buffer.str();
	}

	//----------------------------------------------------------------------------------
	// Support copy constructor and assignment.
	CashFlowList::CashFlowList(const CashFlowList& in_rhs)
	{
		CopyFrom(in_rhs);
	}

	//----------------------------------------------------------------------------------
	// Copy the values from a right-hand side to this object.
	CashFlowList&	CashFlowList::operator=(const CashFlowList& in_rhs)
	{
		CopyFrom(in_rhs);
		return *this;
	}

	//----------------------------------------------------------------------------------
	// Copy the values from a right-hand side to this object.
	void	CashFlowList::CopyFrom(const CashFlowList& in_rhs)
	{
		start_date_ = in_rhs.start_date_;
		clear();
		insert(begin(), in_rhs.begin(), in_rhs.end());
		start_date_ = in_rhs.start_date_;
		end_date_ = in_rhs.end_date_;
	}

	//----------------------------------------------------------------------------------
	// Add a cash flow entry to the list.  Note, for simplicity and to avoid
	// recalculating, calculate the days from start for each cash flow.
	void	CashFlowList::push_back(CashFlow& in_new)
	{
		if (size() == 0)
		{
			start_date_ = in_new.date_;
		} else
		{
			long	days_from_start = std::lround(dates::GetDifferenceDays(in_new.date_, start_date_));
			if (days_from_start < 0)
			{
				start_date_ = in_new.date_;

				if (size() > 0)
				{
					// If the new cash flow is out of order and earlier than the 
					// previous start date, recalculate the other cash flow's
					// number of days since that new earlier start date.

					for (CashFlow& cash_flow : (*this))
					{
						cash_flow.days_from_start_ = std::lround(dates::GetDifferenceDays(cash_flow.date_, start_date_));
					}
				}
			}
			in_new.days_from_start_ = days_from_start;
		}

		// Add theh new entry to the collection.
		std::vector<CashFlow>::push_back(in_new);

	}

	//----------------------------------------------------------------------------------
	// Return the number of days between the start and end dates of the list.
	long	CashFlowList::GetDaysInRange()
	{
		long	result = 0;

		if (size() > 0)
		{
			std::vector<CashFlow>::const_iterator start = std::min_element(std::begin(*this), std::end(*this));
			std::vector<CashFlow>::const_iterator end = std::max_element(std::begin(*this), std::end(*this));

			start_date_ = (*start).date_;
			end_date_ = (*end).date_;
			result = std::lround(dates::GetDifferenceDays((*end).date_, (*start).date_));
		}

		return result;
	}

	//----------------------------------------------------------------------------------
	// Given a discount rate, calculate the value of the serie sof 
	// cash flows discounted by that rate.
	NPV_t	CashFlowList::calculateNPV(const Rate_t& in_daily_discount_rate)
	{
		NPV_t	result = 0.0;
		Rate_t	power_rate = (1.0 + in_daily_discount_rate);
		Rate_t	discount_exponent = 0.0;
		Rate_t	discount_denom = 1.0;
		NPV_t	discounted_cash_flow = 0.0;

		static const double	kDaysInYear = 365.0;

		// If the discount rate is -100% that implies that all cash flows were
		// entirely lost.  That means that all have a net present value = 0.
		// Returning zero directly is faster and also prevents divide by 0 later.

		if (in_daily_discount_rate == -1.0)
		{
			return 0.0;
		}

		CashFlowList::iterator	last_cash_flow = std::max_element(begin(), end());

		for (CashFlow& cash_flow : *this)
		{
			// Calculate a since inception rate.
			
			discount_exponent = static_cast<Rate_t>(cash_flow.days_from_start_) 
									/ static_cast<Rate_t>((*last_cash_flow).days_from_start_);

			// Calculate an annual rate (like XIRR in Excel).
			//discount_exponent = static_cast<Rate_t>(cash_flow.days_from_start_) / static_cast<Rate_t>(kDaysInYear);

			// Calculate an daily rate.
			//discount_exponent = static_cast<Rate_t>(cash_flow.days_from_start_);

			// Calculate the denominator as the compounded discount rate raised 
			// to the power of the number of subperiods.
			
			discount_denom = std::pow(power_rate, discount_exponent);

			if (discount_denom != 0.0) // For divide by zero
			{
				discounted_cash_flow = static_cast<NPV_t>(cash_flow.amount_) / discount_denom;
				result += discounted_cash_flow;
			}
		}

		return result;
	}

	//----------------------------------------------------------------------------------
	// Using a root finding routine to iteratively search for the solution/root 
	// to make the series of cash flows equal zero.
	Rate_t Calculator::GetRate(CashFlowList& in_cash_flows)
	{
		Rate_t	result = 0.0;
		Rate_t	low_estimate = -0.99999;
		Rate_t	high_estimate = +1.0;
		
		static const	int	kMaxIterations = 100;

		int				count = 0;
		bool			searching = true;

		roots::RootFinder<Rate_t>	root_finder;
		roots::RangeException::relative_to_solution_e	err_cause = roots::RangeException::relative_to_solution_e::unknown;

		// Wrap the call to the root finding in a loop.  The root finding
		// algorithm expects initial estimates that are on either side (+ and -)
		// of the eventual solution.  This loop is to adjust the estimates
		// passed to the routine until bracketing estimates are found.

		while (searching)
		{
			try
			{
				err_cause = roots::RangeException::relative_to_solution_e::unknown;

				result = root_finder.SearchForRoot(low_estimate, high_estimate,
								[this, &in_cash_flows](const Rate_t& in_rate) -> Rate_t
								{
									return in_cash_flows.calculateNPV(in_rate);
								}
							);
				calc_log = root_finder.calc_log;
			}
			catch (roots::RangeException err)
			{
				// If the root was not within the range attempted, shift the range up or down
				// depending on the exception and estimate again.

				Rate_t	difference = std::abs(high_estimate - low_estimate);
				
				err_cause = err.relative_position_;
				if (err_cause == roots::RangeException::relative_to_solution_e::too_low)
				{
					low_estimate = high_estimate;
					high_estimate += difference;
				}
				else
				{
					high_estimate = low_estimate;
					low_estimate -= difference;
				}
			}

			// Check if another iteration is required.

			searching = ((count < kMaxIterations) &&
						(err_cause != roots::RangeException::relative_to_solution_e::unknown) &&
						(err_cause != roots::RangeException::relative_to_solution_e::within_range));
		}

		calc_log.log(info) << "IRR = " << result << endl;
		cout << calc_log.flush();

		return result;
	}

};

