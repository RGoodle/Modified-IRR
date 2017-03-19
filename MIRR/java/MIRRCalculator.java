package mirr.java;

import java.math.BigDecimal;
import java.util.concurrent.atomic.AtomicReference;

/** ------------------------------------------------------------------------------------------
 * This class implements the logic necessary to calculate modified internal rate of return
 * performance based on a set of cash flows and the dates on which each of those cash flows
 * occurred.  Note that the market value at the beginning of the period should be the first
 * cash flow.  The ending value at the end of the period should be negated and included as 
 * the last cash flow in the series.
------------------------------------------------------------------------------------------ */
public class MIRRCalculator {

	public	MIRRCalculator() {}
	
	/** ------------------------------------------------------------------------------------------
	 * Calculate the modified internal rate of return for a list of cash flows. 
	 * 
	 * @param inCashFlows				The cash flows for which the MIRR is to be calculated.
	 * @return							The rate of return that causes the discounted cash flows
	 * 									to equal the final cash flow.
	 * @throws InvalidRangeException
	 * @throws Exception
	------------------------------------------------------------------------------------------ */
	public	BigDecimal	GetRate(CashFlowList inCashFlows) 
			throws InvalidRangeException, Exception
	{
		BigDecimal	result = BigDecimal.ZERO;
		BigDecimal	lowEstimate = new BigDecimal("-0.99999");
		BigDecimal	highEstimate = new BigDecimal("1.0");
		
		RootFinder	rootFinder = new RootFinder();
		
		final	int	kMaxIterations = 100;
		int			count = 0;
		boolean		searching = true;
		
		AtomicReference<InvalidRangeException.RelativeToSolution>	err_cause 
			= new AtomicReference<InvalidRangeException.RelativeToSolution>(InvalidRangeException.RelativeToSolution.WITHIN_RANGE);

		// Call the routine to find the rate that makes the cash flows NOV = 0.
		// Iterate until a solution is found or the max iterations is exceeded (as safety).
		
		while (searching)
		{
			count++;
			
			try {
				err_cause.set(InvalidRangeException.RelativeToSolution.WITHIN_RANGE);
				result = rootFinder.FindRoot(
										highEstimate, lowEstimate, 
										(BigDecimal inRate) -> {
	
											try {
												return 	inCashFlows.CalculateNPV(inRate);
											}
											catch (Exception err)
											{
												err_cause.set(InvalidRangeException.RelativeToSolution.UNKNOWN);
											}
											return inRate;
										}
							);
			}
			catch (InvalidRangeException err)
			{
				err_cause.set(err.relativePosition_);
			}
			
			// If a solution was found, return it.
			
			if ((err_cause.get() == InvalidRangeException.RelativeToSolution.WITHIN_RANGE) ||
				(err_cause.get() == InvalidRangeException.RelativeToSolution.UNKNOWN))
			{
				searching = false;
				return result;
			}
			
			// If the root was not within the range attempted, shift the range up or down
			// depending on the exception and estimate again.
			
			BigDecimal	difference = highEstimate.subtract(lowEstimate);
			
			if (err_cause.get() == InvalidRangeException.RelativeToSolution.TOO_LOW)
			{
				lowEstimate = highEstimate;
				highEstimate = highEstimate.add(difference);
				searching = (count < kMaxIterations);
			}
			else
			{
				if (err_cause.get() == InvalidRangeException.RelativeToSolution.TOO_HIGH)
				{
					highEstimate = lowEstimate;
					lowEstimate = lowEstimate.subtract(difference);
					searching = (count < kMaxIterations);
				}
			}
		}
				
		return result;
	}

}
