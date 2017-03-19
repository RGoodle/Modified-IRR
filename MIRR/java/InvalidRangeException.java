package mirr.java;


/** ----------------------------------------------------------------------------------
 * This class indicates that the range of values searched for a solution to a formula
 * does not contain the solution.
 */
public class InvalidRangeException extends Exception {

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	
	public	InvalidRangeException(String in_message, RelativeToSolution inRelativePosition)
	{
		super(in_message);
		relativePosition_ = inRelativePosition;
	}
	
	enum RelativeToSolution
	{
		WITHIN_RANGE,
		TOO_LOW,
		TOO_HIGH,
		UNKNOWN
	};
	
	public	RelativeToSolution	relativePosition_;
	

}
