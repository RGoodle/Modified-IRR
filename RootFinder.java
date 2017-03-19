package mirr.java;

import java.math.BigDecimal;
import java.util.function.Function;

/* ------------------------------------------------------------------------------------------
 * Implement an algorithm to find a root/solution of a formula using an iterative
 * approach.  This algorithm uses Brent's algorithm which combines inverse quadratic
 * interpolation, the secant method, and bisection method to iteratively narrow in
 * on a particular solution of the formula.  The solution is one that will make it
 * equal zero.
 ------------------------------------------------------------------------------------------ */
public class RootFinder {

	// Thresholds for narrowing in on the solution.  Once the iteration passes either
	// of these thresholds the search will stop.
	
	public	static	final	BigDecimal	ESTIMATE_TOLERANCE = new BigDecimal("0.000000001");
	public	static	final	BigDecimal	RESULT_TOLERANCE = new BigDecimal("0.000000001");
	public	static	final	long	MAX_ITERATIONS = 100;

	private	static final	int	BEST = 3; // best
	private	static final	int	COUNTER = 2; // counter
	private	static final	int	MINUS1 = 1; // prev
	private	static final	int	MINUS2 = 0; // earlier
	
	// Constants to avoid redefining these all the time and to make the code easier to read.
	
	private	static final	BigDecimal	TWO = new BigDecimal("2.0");
	private	static final	BigDecimal	THREE_QUARTERS = new BigDecimal("0.75");
	
	// The types of methods that are used to produce estimates when searching for
	// a solution.
	
	private	enum MethodsAvailable
	{
		UNKNOWN,
		QUADRATIC_INTERPOLATION,
		SECANT,
		BISECTION
	};
	
	/* ------------------------------------------------------------------------------------------
	 * Calculate a new estimate by finding the secant joining two points on
	 * either side of the solution axis.
	 * @param inPrevEstimate		The estimate last used for the solution.
	 * @param inEarlierEstimate	The estimate before the last one used for the solution.
	 * @param inPrevResult		The result returned by the function using the previous estimate.
	 * @param inEarlierResult		The result returned by the function using the earlier estimate.
	 * @return The x coordinate where the secant of the circle crosses the x axis when 
	 * 			drawn from the former estimates.
	 ------------------------------------------------------------------------------------------ */
	private	BigDecimal	SecantEstimate(BigDecimal inPrevEstimate, BigDecimal inEarlierEstimate,
													BigDecimal inPrevResult, BigDecimal inEarlierResult)
	{
		BigDecimal	newEstimate = BigDecimal.ZERO;

		// x_n2 = x_n1 - { f(x_n1) * [{x_n1 - x_n0) / ( f(x_n1) - f(x_n0) )] }
		
		newEstimate = inPrevEstimate.subtract(
							inPrevResult.multiply(
									(inPrevEstimate.subtract(inEarlierEstimate))
										.divide(inPrevResult.subtract(inEarlierResult), 100, BigDecimal.ROUND_HALF_UP)
									)
							);
		
		return newEstimate;
	}
	
	/* ------------------------------------------------------------------------------------------
	 * Fit an inverse arc of a parabola that goes through the points f(a), f(b) and f(c).  To
	 * find the next estimate, find the solution, x, to that parabola when y = 0.
	 * @param inPrevEstimate		The estimate used from previous iteration. 
	 * @param inCurrEstimate		The best estimate of the solution from this iteration.
	 * @param inEarlierEstimate	The estimate used two iterations ago.
	 * @param inPrevResult		The result of using the previous estimate with the function.
	 * @param inCurrResult		The result of using the best estimate with the function.
	 * @param inEarlierResult		The result of using the earlier estimate with the function.
	 * @return						The x coordinate as the estimated root where the parabola
	 * 								crosses the x axis when y is zero.
	 ------------------------------------------------------------------------------------------ */
	private	BigDecimal	InverseQuadraticInterpolation(BigDecimal inPrevEstimate, BigDecimal inCurrEstimate, BigDecimal inEarlierEstimate,
													BigDecimal inPrevResult, BigDecimal inCurrResult, BigDecimal inEarlierResult)
	{
		BigDecimal	newEstimate = BigDecimal.ZERO;
		
		/* The formula for the inverse parabola is:
		x = {[(y-f_b)(y-f_c) ] / [(f_a-f_b)*(f_a-f_c)]} * a
				+ {[(y-f_a)(y-f_c) ] / [(f_b-f_a)*(f_b-f_c)]} * b
				+ {[(y-f_a)(y-f_b) ] / [(f_c-f_a)*(f_b-f_b)]} * c
		
		Find the point at which y = 0.  As the parabola moves closer to the actual
		solution, the point at which it crosses the x-axis (when y = 0) will move
		closer to that solution.
		
		x becomes 
			x = b + P / Q
			where
				R = f_b / f_c
				S = f_b / f_a
				T = f_a / f_c
				P = S[T(R-T)(c-b)-(1-R)(b-a)]
				Q = (T-1)(R-1)(S-1)
		*/
		/*
		BigDecimal	r = inCurrResult.divide(inPrevResult, 100, BigDecimal.ROUND_HALF_UP);
		BigDecimal	s = inCurrResult.divide(inEarlierResult, 100, BigDecimal.ROUND_HALF_UP);
		BigDecimal	t = inPrevResult.divide(inPrevResult, 100, BigDecimal.ROUND_HALF_UP);
		BigDecimal	p = s.multiply(t.multiply(r.subtract(t).multiply(inPrevEstimate.subtract(inCurrEstimate))
							.subtract(BigDecimal.ONE.subtract(r).multiply(inCurrEstimate.subtract(inEarlierEstimate)))));
		BigDecimal	q = t.subtract(BigDecimal.ONE)
							.multiply(r.subtract(BigDecimal.ONE))
							.multiply(s.subtract(BigDecimal.ONE));
		
		newEstimate = inCurrEstimate.add(p.divide(q, 100, BigDecimal.ROUND_HALF_UP));
		*/
		//(inverse quadratic interpolation)
		newEstimate = (inEarlierEstimate.multiply(inCurrResult).multiply(inPrevResult))
				.divide((inEarlierResult.subtract(inCurrResult)).multiply(inEarlierResult.subtract(inPrevResult)), 100, BigDecimal.ROUND_HALF_UP);
				
		newEstimate = newEstimate
				.add((inCurrEstimate.multiply(inEarlierResult).multiply(inPrevResult))
				.divide((inCurrResult.subtract(inEarlierResult)).multiply(inCurrResult.subtract(inPrevResult)), 100, BigDecimal.ROUND_HALF_UP));
		
		newEstimate = newEstimate
				.add((inPrevEstimate.multiply(inEarlierResult).multiply(inCurrResult))
				.divide((inPrevResult.subtract(inEarlierResult)).multiply(inPrevResult.subtract(inCurrResult)), 100, BigDecimal.ROUND_HALF_UP));

		
		return newEstimate;		
	}

	/** ----------------------------------------------------------------------------------
	 * This method uses a close variation on the Brent's/Brent-Dekker algorithm for finding
	 * a root for some function given the function and two estimates for the solution.  The
	 * estimates need to bracket the actual solution so that they can be brought together
	 * on it.  If they do not brackket the solution, an InvalidRangeException will be raised
	 * indicating whether the estimate range was too high or low.
	 * 
	 * The algorithm uses a combination of inverse quadratic interpolation, a secant approximation
	 * and bisection depending on which of them is the can provide the best estimate 
	 * for each iteration.
	 * 
	 * Note that some of the conditions for choosing one estimation method over the others
	 * are slightly different from the traditional algorithm to account for its specific
	 * application in this case.
	 * 
	 * @param inBestEstimate			The best estimate of the root that will cause the function
	 * 									to return zero.
	 * @param inCounterEstimate		Another estimate of the root chosen so that it will be
	 * 									on the opposite side of the x axis as the best estiate 
	 * 									(i.e. result in the	opposite sign).
	 * @param inCalculator				The function that implements the formula for which the
	 * 									root is being sought.
	 * @return							The solution that causes the function to be 0, or at least
	 * 									to be within a tolerance range of 0.
	 * @throws InvalidRangeException
	 ------------------------------------------------------------------------------------------ */
	public	BigDecimal FindRoot(BigDecimal inBestEstimate, BigDecimal inCounterEstimate, 
			Function<BigDecimal, BigDecimal> inCalculator) 
	throws InvalidRangeException
	{
		
		BigDecimal[]	estimate = { BigDecimal.ZERO, BigDecimal.ZERO, BigDecimal.ZERO, BigDecimal.ZERO };
		BigDecimal[]	results = { BigDecimal.ZERO, BigDecimal.ZERO, BigDecimal.ZERO, BigDecimal.ZERO };
		BigDecimal[]	parabolicEstimate = { BigDecimal.ZERO, BigDecimal.ZERO, BigDecimal.ZERO, BigDecimal.ZERO };
		BigDecimal		stepSize = BigDecimal.ZERO;
		
		estimate[BEST]	= inBestEstimate;	// best = 'b'
		estimate[COUNTER] = inCounterEstimate; // counter = 'a'
		estimate[MINUS1] = estimate[COUNTER];	// prev = 'c'
		estimate[MINUS2] = BigDecimal.ZERO; // earlier = 'd'
		
		results[BEST] = inCalculator.apply(estimate[BEST]);
		results[COUNTER] = inCalculator.apply(estimate[COUNTER]);
		results[MINUS1] = results[COUNTER];
		results[MINUS2] = results[COUNTER];
		
		BigDecimal	newEstimate = estimate[BEST];
		BigDecimal	newResult = results[BEST];
		
		long 	count = 0;
		
		//MethodsAvailable	prevMethodUsed = MethodsAvailable.BISECTION;
		MethodsAvailable	methodToUse = MethodsAvailable.UNKNOWN;

		// If the solution is not bracketed by the target range, notify the 
		// caller so they can either stop or shift the range.
		
		if (results[BEST].signum() == results[COUNTER].signum())
		{
			if (results[BEST].compareTo(BigDecimal.ZERO) < 0)
			{
				throw new InvalidRangeException("Results are below the solution.", InvalidRangeException.RelativeToSolution.TOO_LOW);
			}
			else 
			{
				throw new InvalidRangeException("Results are above the solution.", InvalidRangeException.RelativeToSolution.TOO_HIGH);
			}
		}
		
		// If the result using the counter-estimate is closer to 0, swap it
		// with the estimate to be used.

		if (results[COUNTER].abs().compareTo(results[BEST].abs()) < 0) {
			BigDecimal	temp = estimate[COUNTER];
			estimate[COUNTER] = estimate[BEST];
			estimate[BEST] = temp;
			temp = results[COUNTER];
			results[COUNTER] = results[BEST];
			results[BEST] = temp;
		}
		
		estimate[MINUS1] = estimate[COUNTER];

		// Include a safety condition to prevent excessive looping.
		
		while (count < MAX_ITERATIONS)
		{
			count++;
			
			// If not going to have divide by 0, get the result of the inverse quadratic interpolation.
			
			if ((results[COUNTER].compareTo(results[MINUS1]) != 0) && 
				(results[MINUS1].compareTo(results[BEST]) != 0) 
				)
			{
				newEstimate = InverseQuadraticInterpolation(estimate[COUNTER], estimate[BEST], estimate[MINUS1], 
															results[COUNTER], results[BEST], results[MINUS1]);
				methodToUse = MethodsAvailable.QUADRATIC_INTERPOLATION;
			}
			else
			{
				// Use linear estimate (secant method) to find a point that crosses
				// the solution (x) axis when the quadratic cannot be calculated.

				newEstimate = SecantEstimate(estimate[COUNTER], estimate[BEST], results[COUNTER], results[BEST]);		
				methodToUse = MethodsAvailable.SECANT;
			}
			
			parabolicEstimate[MINUS2] = parabolicEstimate[MINUS1];
			parabolicEstimate[MINUS1] = newEstimate;
			parabolicEstimate[BEST] = newEstimate;

			// If quadratic interpolation provides a new estimate that is more than
			// 3/4 of the way from the previous estimate and the counter estimate,
			// switch to using the bisection method.
			
			BigDecimal		underShootThreshold = (THREE_QUARTERS
													.multiply(estimate[MINUS1].subtract(estimate[BEST]).abs()))
													.subtract(estimate[BEST]).abs(); 

			if (parabolicEstimate[BEST].compareTo(underShootThreshold) <= 0)
			{
				methodToUse = MethodsAvailable.BISECTION;
			} 
			else
			{	
				// Purposefully used some conditions that differ from the commonly described
				// approach for the Brent-Dekker/Brent's algorithm.  For the specific application
				// they seemed to cause more iterations rather than fewer.
				
				//if (prevMethodUsed == MethodsAvailable.BISECTION) 
				//{
					BigDecimal	estimateDelta2 = parabolicEstimate[MINUS2].subtract(estimate[MINUS2]).abs();
					BigDecimal	estimateDelta1 = parabolicEstimate[BEST].subtract(estimate[BEST]).abs();
					
					BigDecimal	prev_earlier_midpoint = estimate[MINUS1].subtract(estimate[MINUS2]).abs().divide(TWO, 100, BigDecimal.ROUND_HALF_UP);
					
					
					if ((estimateDelta1.compareTo(prev_earlier_midpoint) > 0) &&
							(estimateDelta2.compareTo(ESTIMATE_TOLERANCE.divide(TWO, 100, BigDecimal.ROUND_HALF_UP)) > 0)
							)
					{
						methodToUse = MethodsAvailable.BISECTION;
					} 
				//}
			}
			
			// If the iteration should use bisection instead, calculate using that
			// approach.
			
			if (methodToUse == MethodsAvailable.BISECTION)
			{
				newEstimate = estimate[BEST].add(estimate[COUNTER]).divide(TWO, 100, BigDecimal.ROUND_HALF_UP);
			}
			
			// Calculate the result of the function given the new estimate for a solution.

			newResult = inCalculator.apply(newEstimate);

			estimate[MINUS2] = estimate[MINUS1];
			results[MINUS2] = results[MINUS1];
			estimate[MINUS1] = estimate[BEST];
			results[MINUS1] = results[BEST];

			// If the result of using the counter estimate and the new estimate have opposite signs, 
			// then the root still lies between them so use the same counter estimate.  If they have
			// the same sign then the current estimate crossed zero so use it as the next counter 
			// estimate.
			
			if (newResult.signum() != results[COUNTER].signum())
			{
				estimate[BEST] = newEstimate;
				results[BEST] = newResult;
			}
			else 
			{
				estimate[COUNTER] = newEstimate;
				results[COUNTER] = newResult;
			}

			// If the result using the counter-estimate is closer to 0, swap it
			// with the estimate to be used.

			if (results[COUNTER].abs().compareTo(results[BEST].abs()) < 0) {
				BigDecimal	temp = estimate[COUNTER];
				estimate[COUNTER] = estimate[BEST];
				estimate[BEST] = temp;
				temp = results[COUNTER];
				results[COUNTER] = results[BEST];
				results[BEST] = temp;
			}
			
			// Debug messages
			
			System.out.print("   " + BDtoString(estimate[BEST]) + "   " + BDtoString(results[BEST]) 
			+ "   " + BDtoString(estimate[COUNTER]) + "   " + BDtoString(results[COUNTER]));

			switch (methodToUse)
			{
			case QUADRATIC_INTERPOLATION:
				System.out.println(String.valueOf(count) + "[" + String.valueOf(count) + "] quadratic_interpolation");
				break;
			case SECANT:
				System.out.println(String.valueOf(count) + "[" + String.valueOf(count) + "] secant");
				break;
			case BISECTION:
				System.out.println(String.valueOf(count) + "[" + String.valueOf(count) + "] bisection");
				break;
			case UNKNOWN:
				System.out.println(String.valueOf(count) + "[" + String.valueOf(count) + "] unknown");
				break;
			}
			
			// Stop the loop if the estimates are no longer changing by more than 
			// the tolerance or if the result is close enough to zero.
			
			stepSize = estimate[BEST].subtract(estimate[COUNTER]).abs();
			
			if (stepSize.compareTo(ESTIMATE_TOLERANCE) < 0) 
			{
				break;
			}
			
			if (results[BEST].abs().compareTo(RESULT_TOLERANCE) < 0)
			{
				break;
			}
			
		}
		
		return newEstimate;
	}
	
	/*------------------------------------------------------------------------------------------
	 * For tracing/debugging, format a big decimal as a compact string.
	 ------------------------------------------------------------------------------------------*/
	private String BDtoString(BigDecimal inNum)
	{
		String	result;
		
		result = String.format("%20s", inNum.toPlainString()).replace(' ', '0');
		if (result.length() > 20) {
			result = result.substring(0, 20);
		}
		
		return result;
	}
	

}
