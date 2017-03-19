#pragma once

#include <functional>
#include <iostream>
#include "log.h"

using namespace logging;
using namespace std;

//----------------------------------------------------------------------------------
// Provide capabilities to find solutions/roots for equations.

namespace roots {

	//----------------------------------------------------------------------------------
	// This exception is thrown when the solution does not lie between the estimates 
	// provided.  It will indicate if the results were too high or too low.
	class RangeException : public std::runtime_error {
	public:
		//----------------------------------------------------------------------------------
		// Identify where the estimates produced results that were too high, too low, or
		// within the valid range.
		enum relative_to_solution_e
		{
			within_range = 0,
			too_low = 1,
			too_high = 2,
			unknown = 3
		};

		//----------------------------------------------------------------------------------
		// Constructors
		RangeException(const string& _Message)
			: std::runtime_error(_Message)
			, relative_position_(relative_to_solution_e::unknown)
		{}
		RangeException(const string& _Message, const relative_to_solution_e& in_position)
			: std::runtime_error(_Message)
			, relative_position_(in_position)
		{}
		RangeException(const RangeException& in_from)
			: std::runtime_error(in_from)
			, relative_position_(in_from.relative_position_)
		{}

		RangeException&	operator=(const RangeException& in_from)
		{
			*this = RangeException(in_from);
			return *this;
		}

		relative_to_solution_e	relative_position_ = relative_to_solution_e::unknown;
	};

	//----------------------------------------------------------------------------------
	// Given a function of the form 0 = f(x), searching for a value of x that will 
	// make the result 0.
	template <class RESULT_T>
	class RootFinder {

	public:

		using function_t = std::function < RESULT_T(const RESULT_T&) > ;

		enum methods_available
		{
			unknown = 0,
			quadratic_interpolation = 1,
			secant = 2,
			bisection = 3
		};

		//----------------------------------------------------------------------------------
		// Define a log that the calculations can use to record their steps.
		logging::Log	calc_log = Log(logging::Control(info));

		// ----------------------------------------------------------------------------------
		// This method uses a close variation on the Brent's/Brent-Dekker algorithm for finding
		// a root for some function given the function and two estimates for the solution.  The
		// estimates need to bracket the actual solution so that they can be brought together
		// on it.  If they do not brackket the solution, an InvalidRangeException will be raised
		// indicating whether the estimate range was too high or low.
		//
		// The algorithm uses a combination of inverse quadratic interpolation, a secant approximation
		// and bisection depending on which of them is the can provide the best estimate
		// for each iteration.
		//
		// Note that some of the conditions for choosing one estimation method over the others
		// are slightly different from the traditional algorithm to account for its specific
		// application in this case.
		RESULT_T	SearchForRoot(RESULT_T in_best_estimate, RESULT_T in_counter_estimate, function_t in_function) {

			static const	int	kBest = 3; // best
			static const	int	kCounter = 2; // counter
			static const	int	kMinus1 = 1; // prev
			static const	int	kMinus2 = 0; // earlier

			RESULT_T	estimate[4] = { 0.0, 0.0, 0.0, 0.0 };
			RESULT_T	result[4] = { 0.0, 0.0, 0.0, 0.0 };
			RESULT_T	parabolic_estimate[4] = { 0.0, 0.0, 0.0, 0.0 };
			RESULT_T	parabolic_result[4] = { 0.0, 0.0, 0.0, 0.0 };
			RESULT_T	step_size = 0.0;

			RESULT_T	estimate_tolerance = 0.000000001;
			RESULT_T	result_tolerance = 0.000000001;

			// Discount the cash fcounters based on the counter and curr estimates.

			estimate[kBest] = in_best_estimate;
			estimate[kCounter] = in_counter_estimate;
			estimate[kMinus1] = estimate[kCounter];
			estimate[kMinus2] = 0.0;

			result[kBest] = (in_function)(estimate[kBest]);		//f(b);
			result[kCounter] = (in_function)(estimate[kCounter]); //f(a)
			result[kMinus1] = result[kCounter];
			result[kMinus2] = result[kCounter];

			RESULT_T	new_estimate = estimate[kBest]; // s
			RESULT_T	new_result = result[kBest]; // f(s)

			long	count = 0;
			static	const	long	kMaxIterations = 100;
			
			methods_available	method_to_use = methods_available::unknown;

			// The solution is not between the counter and curr estimates so the root
			// would not be found.

			if ((result[kCounter] * result[kBest]) >= 0)
			{
				if (result[kBest] < 0)
				{
					throw RangeException("Results are below the solution.", RangeException::relative_to_solution_e::too_low);
				}
				else
				{
					throw RangeException("Results are above the solution.", RangeException::relative_to_solution_e::too_high);
				}
			}

			// If the result using the counter - estimate is closer to 0, swap it
			// with the estimate to be used.

			if (std::abs(result[kCounter]) < std::abs(result[kBest]))
			{
				std::swap(estimate[kBest], estimate[kCounter]);
				std::swap(result[kBest], result[kCounter]);
			}

			estimate[kMinus1] = estimate[kCounter];

			calc_log.log(debug) << "Count        CurrEstimate   NPV                  CounterEstimate    NPV            Method";
			calc_log.log(debug) << "-----        ------------   ---                  ---------------    ---            ------";
			calc_log.log(debug) << count << "     "
				<< std::fixed << std::setw(15) << std::setprecision(6) << estimate[kBest] << "   "
				<< std::fixed << std::setw(15) << std::setprecision(6) << result[kBest] << "   "
				<< std::fixed << std::setw(15) << std::setprecision(6) << estimate[kCounter] << "        "
				<< std::fixed << std::setw(15) << std::setprecision(6) << result[kCounter];

			// Include a safety condition to prevent excessive looping.

			while (count < kMaxIterations)
			{
				count++;

				LogEntry	log_entry(debug);

				// Prefer using the inverse quadratic interpolation over the secant method
				// (linear interpolation) because it is slightly more efficient and producing
				// an accurate estimate despite increased calculation complexity.

				if ((result[kCounter] != result[kMinus1]) && (result[kBest] != result[kMinus1]))
				{
					new_estimate = QuadraticInterpolation(estimate[kCounter], estimate[kBest], estimate[kMinus1],
															result[kCounter], result[kBest], result[kMinus1]);
					method_to_use = methods_available::quadratic_interpolation;
				}
				else
				{
					new_estimate = SecantEstimate(estimate[kCounter], estimate[kBest], result[kCounter], result[kBest]);
					method_to_use = methods_available::secant;
				}

				parabolic_estimate[kMinus2] = parabolic_estimate[kMinus1];
				parabolic_estimate[kMinus1] = new_estimate;
				parabolic_estimate[kBest] = new_estimate;

				RESULT_T		under_shoot_threshold = (((0.75) 
															* std::abs(estimate[kMinus1] - estimate[kBest])) 
															- std::abs(estimate[kBest]));

				// Determine whether or not to estimate using a bisection method.

				if (parabolic_estimate[kBest] <= under_shoot_threshold)
				{
					method_to_use = methods_available::bisection;
				}
				else
				{
					//if (prev_method_used == methods_available::bisection) 
					//{
						RESULT_T	estimate_delta_2 = std::abs(parabolic_estimate[kMinus2] - estimate[kMinus2]);
						RESULT_T	estimate_delta_1 = std::abs(parabolic_estimate[kBest] - estimate[kBest]);

						RESULT_T	prev_earlier_midpoint = std::abs(estimate[kMinus1] - estimate[kMinus2]) / 2.0;
	
						if ((estimate_delta_1 > prev_earlier_midpoint) &&
							(estimate_delta_2 > (estimate_tolerance / 2.0)))
						{
							method_to_use = methods_available::bisection;
						}
					//}
				}

				// If the iteration should use bisection instead, calculate using that
				// approach.

				if (method_to_use == methods_available::bisection)
				{
					new_estimate = (estimate[kBest] + estimate[kCounter]) / 2.0;
				}

				// Calculate the result of the function given the new estimate for a solution.

				new_result = (in_function)(new_estimate);

				estimate[kMinus2] = estimate[kMinus1];
				result[kMinus2] = result[kMinus1];
				estimate[kMinus1] = estimate[kBest];
				result[kMinus1] = result[kBest];

				// If the result of using the counter estimate and the new estimate have opposite signs, 
				// then the root still lies between them so use the same counter estimate.  If they have
				// the same sign then the current estimate crossed zero so use it as the next counter 
				// estimate.

				if ((new_result * result[kCounter]) < 0)
				{
					estimate[kBest] = new_estimate;
					result[kBest] = new_result;
				}
				else
				{
					estimate[kCounter] = new_estimate;
					result[kCounter] = new_result;
				}

				// If the result using the counter-estimate is closer to 0, swap it
				// with the estimate to be used.

				if (std::abs(result[kCounter]) < std::abs(result[kBest]))
				{
					std::swap(estimate[kBest], estimate[kCounter]);
					std::swap(result[kBest], result[kCounter]);
				}

				// Debug messages

				log_entry << count << "     "
					<< std::fixed << std::setw(15) << std::setprecision(6) << estimate[kBest] << "   "
					<< std::fixed << std::setw(15) << std::setprecision(6) << result[kBest] << "   "
					<< std::fixed << std::setw(15) << std::setprecision(6) << estimate[kCounter] << "        "
					<< std::fixed << std::setw(15) << std::setprecision(6) << result[kCounter];

				switch (method_to_use)
				{
				case methods_available::quadratic_interpolation:
					log_entry << "   quadratic_interpolation_estimate method";
					break;
				case methods_available::secant:
					log_entry << "   secant method";
					break;
				case methods_available::bisection:
					log_entry << "   bisection method";
					break;
				case methods_available::unknown:
					log_entry << "   unknown method";
					break;
				}
				
				calc_log.log(log_entry);
				//cout << calc_log.flush();

				// Stop the loop if the estimates are no longer changing by more than 
				// the tolerance or if the result is close enough to zero.

				step_size = std::abs(estimate[kBest] - estimate[kCounter]);

				if (step_size < estimate_tolerance)
				{
					break;
				}

				if (std::abs(result[kBest]) < result_tolerance)
				{
					break;
				}

			}

			return estimate[kBest];
		}

		private:

			//----------------------------------------------------------------------------------
			// Return the point at which a secant of an arc crosses the x-axis when it
			// contains two of the function points.
			RESULT_T	SecantEstimate(const RESULT_T& in_prev_estimate, const RESULT_T& in_earlier_estimate,
										const RESULT_T& in_prev_result, const RESULT_T& in_earlier_result)
			{
				RESULT_T	new_estimate = 0;

				// x_n2 = x_n1 - { f(x_n1) * [{x_n1 - x_n0) / ( f(x_n1) - f(x_n0) )] }

				new_estimate = in_prev_estimate - (in_prev_result *
					((in_prev_estimate - in_earlier_estimate) / (in_prev_result - in_earlier_result)));

				return new_estimate;
			}

			//----------------------------------------------------------------------------------
			// Fit an inverse arc of a parabola that goes through the points f(a), f(b) and f(c).  To
			// find the next estimate, find the solution, x, to that parabola when y = 0.
			RESULT_T	QuadraticInterpolation(const RESULT_T& in_prev_estimate, const RESULT_T& in_curr_estimate, const RESULT_T& in_earlier_estimate,
												const RESULT_T& in_prev_result, const RESULT_T& in_curr_result, const RESULT_T& in_earlier_result)
			{
				RESULT_T	new_estimate = 0;

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

				new_estimate = (in_earlier_estimate * in_curr_result * in_prev_result)
								 / ((in_earlier_result - in_curr_result) * (in_earlier_result - in_prev_result));

				new_estimate += (in_curr_estimate * in_earlier_result * in_prev_result)
								/ ((in_curr_result - in_earlier_result) * (in_curr_result - in_prev_result));

				new_estimate += (in_prev_estimate * in_earlier_result * in_curr_result)
								/ ((in_prev_result - in_earlier_result) * (in_prev_result - in_curr_result));

				return new_estimate;
			}


	};


}