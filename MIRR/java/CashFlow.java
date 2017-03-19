package mirr.java;

import java.time.LocalDate;

/** ----------------------------------------------------------------------------------
 * The properties of a cash flow that occurred on a particular date.
 */
public class CashFlow implements Comparable<Object> {

	public CashFlow(LocalDate inDate, double inAmount)
	{ 
		date_ = inDate;
		amount_ = inAmount;
		daysFromStart_ = 0;
	}
	
	// Support copy constructor and assignment.
	public CashFlow(CashFlow inRhs)
	{
		CopyFrom(inRhs);
	}
	
	// Copy the values from a right-hand side to this object.
	public CashFlow clone()
	{
		CashFlow result = new CashFlow(date_, amount_);
		return result;
	}
	
	// Copy the values from a right-hand side to this object.
	public void	CopyFrom(CashFlow inRhs)
	{
		date_ = inRhs.date_;
		amount_ = inRhs.amount_;
		daysFromStart_ = inRhs.daysFromStart_;
	}
	
	@Override
	public int compareTo(Object o) {
		if (o instanceof CashFlow)
		{
			return compareTo((CashFlow) o);
		}
		return 0;
	}
	// Return whether or not this object is less than (earlier) than
	// another.
	public int	compareTo(CashFlow inRhs)
	{
		return date_.compareTo(inRhs.date_);
	}
	
	// Return a string representation of this obejct for debugging.
	public String	ToString()
	{
		String result = date_.toString() + "[day " + String.valueOf(daysFromStart_) + "]=" + String.valueOf(amount_);
		return result;
	}

	// Properties

	LocalDate	date_;
	double		amount_ = 0;
	long		daysFromStart_ = 0;

}
