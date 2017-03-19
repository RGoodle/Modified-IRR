package mirr.java;

import java.math.BigDecimal;

/* ------------------------------------------------------------------------------------------
 * General functions for use with BigDecimals.
 ------------------------------------------------------------------------------------------ */
public class BigDecimalUtils {

	/** ------------------------------------------------------------------------------------------
	 *  Calculate the power of a big decimal raised to the exponent which is also a big decimal.
	 * 
	 * @param base			The base number being raised to a power (i.e. X in X^A).
	 * @param exponent		The exponent to which the base is being raised (i.e. A in X^A).
	 * @return				The result of raising the base to the exponent.
	 * @throws Exception
	------------------------------------------------------------------------------------------  */
	public	static	BigDecimal	power(BigDecimal inBase, BigDecimal inExponent) throws Exception 
	{
		BigDecimal	result = inBase;
		
		BigDecimal	exponent = inExponent;
		BigDecimal	base = inBase;
		
		int signOfExponent = exponent.signum();
		int signOfBase = base.signum();

		// Fast completion, if the exponent is 0, return 1.
		
		if (exponent.equals(BigDecimal.ONE))
		{
			return BigDecimal.ZERO;
		}
		
		try {
	        // Perform X^(A+B) as instead X^A*X^B (where B is the mantissa/fractional part)
			
	        double dbase = base.doubleValue();
	        
	        exponent = exponent.multiply(new BigDecimal(signOfExponent)); // exponent is now positive
	        
	        BigDecimal remainderOfExponent = exponent.remainder(BigDecimal.ONE);
	        BigDecimal exponentIntPart = exponent.subtract(remainderOfExponent);
	        
	        // Calculate the value to the power by applying the integer power separately from the fractional power.
	        
	        BigDecimal intPow = base.pow(exponentIntPart.intValueExact());
	        
	        if ((remainderOfExponent.compareTo(BigDecimal.ZERO) != 0)
	        	&& (signOfBase == -1)) {
	        	dbase = (signOfBase * dbase);
	        }
	        BigDecimal doublePow = new BigDecimal(Math.pow(dbase, remainderOfExponent.doubleValue()));
	        
	        result = intPow.multiply(doublePow);
	        
	        // Normally if the fractional part of the exponent was applied to a negative number
	        // it would be like trying to take the square root of a negative number.  For that
	        // case, negate the result like treating the result as supporting imaginary numbers 
	        // and being multiplied by i (= sqrt(-1).
	        
	        if ((remainderOfExponent.compareTo(BigDecimal.ZERO) != 0)
		        	&& (signOfBase == -1)) {
		        	result = result.negate();
		    }
	    
	    } catch (Exception e) {
	        throw new Exception("Power: Cannot convert base to double");
	    }
		
		return result;
	}
	
}
