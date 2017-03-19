#include "roots.h"

namespace roots {

	//----------------------------------------------------------------------------------
	// Using Brent's method which combines an inverse quadratic method with the secant method
	// and the bisection method, iteratively search for the solution/root to make the series 
	/// of calcualtions equal zero.
	// Iteratively try to narrow the low/curr rate estimates until their difference
	// resolves into a solution for the equation.  Note that low and curr estimates provided as input
	// should cause the NPV function to return values with opposite signs so they bracket the solution.

	template<class RESULT_T>
	RESULT_T	RootFinder<RESULT_T>::SearchForRoot(RESULT_T in_counter_estimate, RESULT_T in_curr_estimate, function_t in_function) {

		RESULT_T	counter_estimate = in_counter_estimate;
		RESULT_T	curr_estimate = in_curr_estimate;
		RESULT_T	prev_estimate = 0;
		RESULT_T	earlier_estimate = 0;

		RESULT_T	tolerance = 0.0000000000000000000001;

		enum methods_available
		{
			unknown = 0,
			quadratic_interpolation = 1,
			secant = 2,
			bisection = 3
		};

		// Discount the cash fcounters based on the counter and curr estimates.

		RESULT_T	counter_result = (in_function)(counter_estimate); //f(a)
		RESULT_T	curr_result = (in_function)(curr_estimate);		//f(b)

		RESULT_T	new_estimate = 0; // s
		RESULT_T	new_result = curr_result; // f(s)
		RESULT_T	prev_result = 0; // f(c)

		long long	count = 0;

		calc_log.log() << "Count  Method         CurrEstimate  NPV     CounterEstimate  NPV";
		calc_log.log() << "-----  ------         ------------  ---     ---------------  ---";
		calc_log.log() << count << "              " << curr_estimate << "   " << curr_result << "   " << counter_estimate << "   " << counter_result;

		// The solution is not between the counter and curr estimates
		// so return.

		if ((counter_result * curr_result) >= 0)
		{
			throw std::runtime_error("Rate not bounded by estimates");
		}

		// If the curr is less than the counter estimate, swap them.

		if (std::abs(counter_result) < std::abs(curr_result))
		{
			RESULT_T	temp = counter_result;
			counter_result = curr_result;
			curr_result = temp;
		}

		RESULT_T			curr_rate = counter_result;
		methods_available	prev_method_used = methods_available::unknown;

		while (true)
		{
			count++;

			if (std::abs(curr_result) < tolerance) {
				calc_log.log() << "Stopping: [" << count << "] new_result = " << new_result;
				break;
			}
			if ((std::abs(curr_estimate - prev_estimate) < tolerance) && (std::abs(curr_estimate - counter_estimate) < tolerance)) {
				calc_log.log() << "Stopping: [" << count << "] curr_estimate [" << curr_estimate
					<< "] - counter_estimate [" << counter_estimate << "] < tolerance [" << tolerance << "]";
				break;
			}

			LogEntry	log_entry;
			RESULT_T	quadratic_interpolation_estimate = 0.0;
			RESULT_T	secant_estimate = 0.0;
			RESULT_T	bisection_estimate = 0.0;
			methods_available	method_to_use = methods_available::unknown;

			// Prefer using the inverse quadratic interpolation over the secant method
			// (linear interpolation) because it is slightly more efficient and producing
			// an accurate estimate despite increased calculation complexity.

			if ((counter_result != prev_result) && (curr_result != prev_result))
			{
				//(inverse quadratic interpolation)
				quadratic_interpolation_estimate = (counter_estimate * curr_result * prev_result)
					/ ((counter_result - curr_result) * (counter_result - prev_result));
				quadratic_interpolation_estimate += (curr_estimate * counter_result * prev_result)
					/ ((curr_result - counter_result) * (curr_result - prev_result));
				quadratic_interpolation_estimate += (prev_estimate * counter_result * curr_result)
					/ ((prev_result - counter_result) * (prev_result - curr_result));

				method_to_use = methods_available::quadratic_interpolation;
			} else
			{
				// (secant method)
				secant_estimate = curr_estimate
					- (curr_result * ((curr_estimate - prev_estimate) / (curr_result - prev_result)));
				method_to_use = methods_available::secant;
			}

			RESULT_T		allowed_min = (((3.0 * counter_estimate) + curr_estimate) / 4.0);
			RESULT_T		curr_prev_avg = (std::abs(curr_estimate - prev_estimate) / 2.0);
			RESULT_T		prev_earlier_avg = (std::abs(prev_estimate - earlier_estimate) / 2.0);
			RESULT_T		new_curr_diff = std::abs(new_estimate - curr_estimate);
			RESULT_T		curr_prev_diff = std::abs(curr_estimate - prev_estimate);
			RESULT_T		prev_earlier_diff = std::abs(prev_estimate - earlier_estimate);

			// Determine whether or not to estimate using a bisection method.

			if (((quadratic_interpolation_estimate < allowed_min) || (quadratic_interpolation_estimate > curr_estimate)) ||
				((secant_estimate < allowed_min) || (secant_estimate > curr_estimate)))
			{
				method_to_use = methods_available::bisection;
			} else
			{
				if (prev_method_used == methods_available::bisection)
				{
					if (curr_prev_diff > std::abs(tolerance))
					{
						method_to_use = methods_available::quadratic_interpolation;
					} else
					{
						method_to_use = methods_available::bisection;
					}

					if (new_curr_diff >= curr_prev_avg)
					{
						method_to_use = methods_available::bisection;
					}
				} else
				{
					if (prev_method_used == methods_available::quadratic_interpolation)
					{
						if (std::abs(prev_earlier_diff) > std::abs(tolerance))
						{
							method_to_use = methods_available::quadratic_interpolation;
						} else
						{
							method_to_use = methods_available::bisection;
						}

						if (new_curr_diff >= prev_earlier_avg)
						{
							method_to_use = methods_available::bisection;
						}
					}
				}
			}

			// If using bisection, calculate the new estimate using that.

			switch (method_to_use)
			{
			case methods_available::quadratic_interpolation:
				new_estimate = quadratic_interpolation_estimate;
				log_entry << count << "   quadratic_interpolation_estimate method";
				break;
			case methods_available::secant:
				new_estimate = secant_estimate;
				log_entry << count << "   secant method";
				break;
			case methods_available::bisection:
				new_estimate = (counter_estimate + curr_estimate) / 2.0;
				log_entry << count << "   bisection method";
				break;
			case methods_available::unknown:
				new_estimate = 0.0;
				log_entry << count << "   unknown method";
				break;
			}

			// Calculate the result of the function given the new estimate for a solution.

			new_result = (in_function)(new_estimate);

			// Set up the values for the next iteration to save 
			earlier_estimate = prev_estimate;
			prev_estimate = curr_estimate;
			prev_method_used = method_to_use;

			// If the result of using the counter estiamte and the new estiamte have opposite signs, 
			// then the root still lies between them so use the same counter estimate.  If they have
			// the same sign then the current estimate crossed zero so use it as the next counter 
			// estimate.

			if ((counter_result * new_result) > 0.0)
			{
				counter_estimate = curr_estimate;
				counter_result = curr_result;
			}

			curr_estimate = new_estimate;
			curr_result = new_result;

			// Calcualte the results using the new estimate and counter estimate.

			// If the result using the counter-estimate is closer to 0, swap it
			// with the estimate to be used.

			if (std::abs(counter_result) < std::abs(curr_result)) {
				RESULT_T	temp = counter_estimate;
				counter_estimate = curr_estimate;
				curr_estimate = temp;
			}

			calc_log.log(log_entry) << "   " << curr_estimate << "   " << curr_result << "   " << counter_estimate << "   " << counter_result;
		}

		cout << calc_log;

		return curr_estimate;
	}

};