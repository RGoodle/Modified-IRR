package mirr.java;

import java.math.BigDecimal;
import java.time.LocalDate;
import java.time.temporal.ChronoUnit;
import java.util.Collections;
import java.util.Vector;

/** ------------------------------------------------------------------------------------------
 * A collection of cash flows over a defined period of time.
 ------------------------------------------------------------------------------------------ */
class CashFlowList extends Vector<CashFlow> {

	private static final long serialVersionUID = -1002226627300168567L;
	
	public CashFlowList() {}
	
	/** ------------------------------------------------------------------------------------------
	 *  Support copy constructor and assignment.
	 * 
	 * @param inRhs	The object that should be copied into this one.
	  ------------------------------------------------------------------------------------------*/
	public CashFlowList(CashFlowList inRhs)
	{
		CopyFrom(inRhs);
	}

	/** ------------------------------------------------------------------------------------------
	 *  Copy the values from a right-hand side to this object.
	------------------------------------------------------------------------------------------ */
	public CashFlowList clone()
	{
		CashFlowList result = new CashFlowList(this);
		return result;
	}

	/** ------------------------------------------------------------------------------------------
	 *  Copy the values from a right-hand side to this object.
	 * 
	 * @param inRhs	The object that should be copied into this one.
	------------------------------------------------------------------------------------------ */
	public void	CopyFrom(CashFlowList inRhs)
	{
		startDate_ = inRhs.startDate_;
		endDate_ = inRhs.endDate_;
		clear();
		for (CashFlow rhd_cash_flow : inRhs) 
		{
			this.add(new CashFlow(rhd_cash_flow));
		}
	}
	
	/** ------------------------------------------------------------------------------------------
	 *  Add a new entry to this collection.  This method overrides the base class
	 *  so it can update the date ranges in this series of cash flows.
	 * 
	 * @param in_new	The cash flow that is being added to this list.
	------------------------------------------------------------------------------------------ */
	@Override
	public	boolean	add(CashFlow in_new)
	{
		if (size() == 0)
		{
			startDate_ = in_new.date_;
		} else
		{
			
	        //double days_from_start = startDate_.dayCount(in_new.date_);
	        
			long	days_from_start = ChronoUnit.DAYS.between(startDate_, in_new.date_);
			
	        if (days_from_start < 0.0)
			{
				startDate_ =  in_new.date_;

				if (size() > 0)
				{
					// If the new cash flow is out of order and earlier than the 
					// previous start date, recalculate the other cash flow's
					// number of days since that new earlier start date.

					for (CashFlow cash_flow : this)
					{
						cash_flow.daysFromStart_ = ChronoUnit.DAYS.between(startDate_, cash_flow.date_);
					}
				}
			}
			in_new.daysFromStart_ = days_from_start;
		}

		// Add the new entry to the collection.
		
		super.add(in_new);
		return true;
	}

	static	final	BigDecimal	DAYS_IN_YEAR = new BigDecimal("365");
	
	/** ------------------------------------------------------------------------------------------
	 *  Given a discount rate, calculate the value of the series of 
	 * cash flows discounted by that rate.
	 * 
	 * @param inDailyDiscountRate	The rate at which each of the cash flows should be discounted.
	 * @return							The net present value as the sum of the discounted cash flows.
	 * @throws Exception
	------------------------------------------------------------------------------------------ */
	public	BigDecimal	CalculateNPV(BigDecimal inDailyDiscountRate)
	throws Exception
	{
		BigDecimal	result = BigDecimal.ZERO;
		BigDecimal	compoundingDiscountRate = BigDecimal.ONE.add(inDailyDiscountRate);
		BigDecimal	discountDenom = BigDecimal.ONE;
		BigDecimal	discountExponent = BigDecimal.ZERO;
		BigDecimal	discountedCashFlow = BigDecimal.ZERO;

		// If the discount rate is -100% that implies that all cash flows were
		// entirely lost.  That means that all have a net present value = 0.
		// Returning zero directly is faster and also prevents divide by 0 later.
		
		if (inDailyDiscountRate.compareTo(BigDecimal.ONE.negate()) == 0)
		{
			return BigDecimal.ZERO;
		}
		
		CashFlow	lastCashFlow = (CashFlow) Collections.max(this);

		for (CashFlow cashFlow : this)
		{
			// Calculate a since inception rate.
			discountExponent = new BigDecimal(String.valueOf(cashFlow.daysFromStart_))
											.divide(new BigDecimal(String.valueOf(lastCashFlow.daysFromStart_)), 100, BigDecimal.ROUND_HALF_UP);
			
			// Calculate an annual rate (like XIRR in Excel).
			//discount_exponent = new BigDecimal(String.valueOf(cash_flow.days_from_start_)).divide(kDaysInYear, 100, BigDecimal.ROUND_HALF_UP);

			// Calculate an daily rate.
			//discount_exponent = new BigDecimal(String.valueOf(cash_flow.days_from_start_));

			discountDenom = BigDecimalUtils.power(compoundingDiscountRate, discountExponent);

			if (discountDenom.compareTo(BigDecimal.ZERO) != 0)
			{
				discountedCashFlow = new BigDecimal(String.valueOf(cashFlow.amount_)).divide(discountDenom, 100, BigDecimal.ROUND_HALF_UP);
				result = result.add(discountedCashFlow);
			}
		}
		
		return result;

		// Test calculations using double.
		
/*		double	exponent = 0.0;
		double	rate = in_daily_discount_rate.doubleValue() + 1.0;
		double	denom = 1.0;
		double	double_result = 0.0;
		double	days_in_range = Double.valueOf(last_cash_flow.days_from_start_);
		
		for (CashFlow cash_flow : cash_flows_)
		{
			exponent = Double.valueOf(cash_flow.days_from_start_) / days_in_range;
			denom =  Math.pow(rate,  exponent);
			double_result += (cash_flow.amount_ / denom);
		}
		result = new BigDecimal(String.valueOf(double_result));
*/
	}
	
	// Properties
	private	LocalDate	startDate_;
	private	LocalDate	endDate_;
		
}
