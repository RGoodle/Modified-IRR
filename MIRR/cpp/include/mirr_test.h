#pragma once

#include <chrono>
#include <vector>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <algorithm>

#include "date_math.h"
#include "modified_irr.h"
#include "roots.h"

using namespace std;

//----------------------------------------------------------------------------------
// Test the list of cash flows and their dates.
bool	TestCashFlowList() 
{
	mirr::CashFlowList		cash_flows;
	std::time_t				starting_date = std::time(0);

	cout << "Max cash flow" << RAND_MAX << endl;

	for (int i = 0; i < 10; i++) {
		std::time_t		cash_flow_date(dates::AddMonths(starting_date, i));
		cash_flows.push_back(mirr::CashFlow(cash_flow_date, (std::rand())));
	}

	cout << "Test CashFlowList:" << endl;
	for (mirr::CashFlow& cash_flow : cash_flows) {
		cout << cash_flow.ToString() << endl;
	}

	return true;
}

//----------------------------------------------------------------------------------
// Test the calculation of NPV based on a list of cash flows.
bool	TestNPV() 
{
	mirr::CashFlowList		cash_flows;

	// Set up the list of cash flows.

	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2007-05-31"), 9978.82));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2007-06-14"), 15000.0));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2009-10-26"), 20439.95)); 
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2009-11-09"), 5000.0));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2010-02-11"), 3000.0)); 
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-10-24"), -112961.67));

	cout << "Test CashFlowList:" << endl;
	for (mirr::CashFlow& cash_flow : cash_flows) {
		cout << cash_flow.ToString() << endl;
	}

	// Calculate the net present value of the cash flows given a daily discount rate.

	cout << "NPV = " << cash_flows.calculateNPV(0.000394780648885) << endl;

	return true;
}

//----------------------------------------------------------------------------------
// Test searching for the IRR that will make the NPV of the series of cash flows = 0.
bool	TestMIRR()
{
	mirr::CashFlowList	cash_flows;
	mirr::Calculator	calculator;
	mirr::Rate_t		result;
	
	// Set up the list of cash flows.
	
	//Test Case 0:

	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2007-05-31"), 9978.82));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2007-06-14"), 15000.0));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2009-10-26"), 20439.95));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2009-11-09"), -5000.0));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2010-02-11"), 3000.0));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-10-24"), 49190.0));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2015-02-13"), -122444.29));

	cout << "Test TestMIRR:" << endl;
	for (mirr::CashFlow& cash_flow : cash_flows) {
		cout << cash_flow.ToString() << endl;
	}

	// Solve for the modified internal rate of return that makes those cash flows have an NPV = 0.

	result = calculator.GetRate(cash_flows);

	cout << "IRR=" << std::fixed << std::setw(15) << std::setprecision(30) << result << endl;
	cout << "Expected IRR= 0.6935541782410140" << endl;

	cout << endl << endl;

	//Test Case 1: IRR = 0.57068992946099172768520

	calculator.calc_log.clear();
	cash_flows.clear();

	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-12-31"), 27));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-01-02"), 1092));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-02-25"), 1354.8));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-03-25"), -429.28));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-04-07"), -85.05));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-05-26"), -1415));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-06-02"), -1188));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-06-16"), -489.5));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-06-25"), -62.25));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-07-28"), 500.39));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-08-25"), 1532.79));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-09-02"), 75.7));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-09-22"), 35.5));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-10-20"), 3035.8));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-10-30"), -4627));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-10-31"), 109.8));

	for (mirr::CashFlow& cash_flow : cash_flows) {
		cout << cash_flow.ToString() << endl;
	}

	result = calculator.GetRate(cash_flows);
	cout << "IRR=" << std::fixed << std::setw(15) << std::setprecision(30) << result << endl;
	cout << "Expected IRR= 0.57068992946099172768520" << endl;

	//Test Case 2: IRR = 54.52564034284328783070

	calculator.calc_log.clear();
	cash_flows.clear();

	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-02-07"), 323.28));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-02-12"), 6193.87));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-02-13"), 12958.49));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-03-25"), -5880.88));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-04-10"), 7433.3));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-04-25"), -14451.17));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-04-26"), 3541.24));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-05-08"), -6829.46));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-05-29"), 560.8));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-06-07"), 611.1));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-06-21"), -4485.53));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-07-09"), -9991.02));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-07-23"), -7387.22));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-10-22"), 219.55));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-11-13"), 8673.57));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-11-22"), -15306.6));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-12-16"), 8461.69));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-12-17"), 1563.95));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-01-14"), 3556.8));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-01-22"), -32427.98));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-01-28"), 3130.5));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-03-03"), 1200));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-03-24"), -646.53));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-03-26"), 33894));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-04-24"), -8793.99));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-05-01"), -12599.94));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-05-06"), 6193.61));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-05-12"), 5055.24));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-08-28"), 114.69));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-10-02"), -32467.25));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-10-24"), 809.82));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-10-31"), 0));

	for (mirr::CashFlow& cash_flow : cash_flows) {
		cout << cash_flow.ToString() << endl;
	}

	result = calculator.GetRate(cash_flows);
	cout << "IRR=" << std::fixed << std::setw(15) << std::setprecision(30) << result << endl;
	cout << "Expected IRR= 54.52564034284328783070" << endl;
	
	//Test Case 3: 

	calculator.calc_log.clear();
	cash_flows.clear();

	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2011-02-04"), 444));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2011-02-10"), 177300.25));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2011-02-16"), 1593162.55));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2011-03-10"), 21600));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2011-03-11"), 14400));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2011-03-29"), 112595));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2011-03-31"), 455950));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2011-04-01"), -51276.3));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2011-04-15"), 1504.77));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2011-05-02"), -45514.7));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2011-05-27"), 30100));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2011-06-20"), -119818));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2011-06-30"), 32225));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2011-07-14"), 20448));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2011-07-20"), 50178.81));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2011-08-12"), 54222.2));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2011-09-14"), 70860.76));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2011-09-21"), 100366.7));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2011-09-23"), -104663.2));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2011-09-27"), 38143));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2011-09-30"), -33170));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2011-10-07"), 19430));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2011-10-18"), 50958.68));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2011-10-26"), -65940));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2011-11-04"), 61703.4));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2011-11-09"), 31480));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2011-11-17"), 32515.4));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2011-11-22"), 511.2));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2011-12-31"), 0));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2012-01-30"), 15092.98));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2012-02-02"), 265586.2));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2012-02-08"), -218030));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2012-02-09"), 156750));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2012-02-27"), 32210.4));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2012-03-07"), 82921.13));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2012-03-12"), 224200));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2012-03-15"), -225232));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2012-03-30"), 0));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2012-04-19"), -35420.43));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2012-04-30"), 115850));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2012-05-03"), -120275.2));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2012-05-11"), 34009.6));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2012-05-22"), -44722.22));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2012-05-30"), -71468));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2012-06-07"), 106334.51));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2012-06-12"), -110030));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2012-06-29"), 0));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2012-07-24"), -5769.55));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2012-07-31"), -87962.5));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2012-08-20"), 93008.78));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2012-08-28"), 32681.1));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2012-09-10"), -95229.5));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2012-09-28"), 47350));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2012-10-02"), -50723.82));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2012-10-05"), -3072999.35));

	for (mirr::CashFlow& cash_flow : cash_flows) {
		cout << cash_flow.ToString() << endl;
	}

	result = calculator.GetRate(cash_flows);
	cout << "IRR=" << std::fixed << std::setw(15) << std::setprecision(30) << result << endl;
	cout << "Expected IRR= 0.15577775610447013648378" << endl;
	
	//Test Case 4:

	calculator.calc_log.clear();
	cash_flows.clear();

	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-01-24"), 320.8));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-01-29"), 352.6));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-02-01"), -92.15));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-02-28"), 740));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-03-26"), 655));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-04-25"), 2707.75));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-05-13"), -1159.59));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-05-27"), -3921.1));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-06-27"), 2290.05));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-07-16"), -279.81));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-07-29"), -1117.92));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-08-15"), -457.25));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-08-28"), -1809));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-09-25"), -934));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-10-30"), 590));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-11-28"), -842));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-01-02"), 1092));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-02-25"), 1354.8));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-03-25"), -429.28));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-04-07"), -85.05));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-05-26"), -1415));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-06-02"), -1188));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-06-16"), -489.5));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-06-25"), -62.25));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-07-28"), 500.39));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-08-25"), 1532.79));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-09-02"), 75.7));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-09-22"), 35.5));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-10-20"), 3035.8));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-10-30"), -4627));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-10-31"), 109.8));

	for (mirr::CashFlow& cash_flow : cash_flows) {
		cout << cash_flow.ToString() << endl;
	}

	result = calculator.GetRate(cash_flows);
	cout << "IRR=" << std::fixed << std::setw(15) << std::setprecision(30) << result << endl;
	cout << "Expected IRR= 17.823529759677437485977" << endl;

	//Test Case 5:

	calculator.calc_log.clear();
	cash_flows.clear();

	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2007-05-31"), 9978.82));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2007-06-14"), 15000));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2009-10-26"), 20439.95));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2009-11-09"), -5000));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2010-02-11"), 3000));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-10-24"), 49190));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-02-28"), -112961.67));

	for (mirr::CashFlow& cash_flow : cash_flows) {
		cout << cash_flow.ToString() << endl;
	}

	result = calculator.GetRate(cash_flows);
	cout << "IRR=" << std::fixed << std::setw(15) << std::setprecision(30) << result << endl;
	cout << "Expected IRR= 0.5391053430857646636078" << endl;

	//Test Case 6: 

	calculator.calc_log.clear();
	cash_flows.clear();

	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2015-01-01"), 100));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2016-01-01"), -75));

	for (mirr::CashFlow& cash_flow : cash_flows) {
		cout << cash_flow.ToString() << endl;
	}

	result = calculator.GetRate(cash_flows);
	cout << "IRR=" << std::fixed << std::setw(15) << std::setprecision(30) << result << endl;
	cout << "Expected IRR= -0.25" << endl;

	//Test Case 7 (RBCNullCase):

	calculator.calc_log.clear();
	cash_flows.clear();

	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2007-05-31"), 9978.82));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2007-06-14"), 15000));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2009-10-26"), 20439.95));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2009-11-09"), -5000));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2010-02-11"), 3000));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2013-10-24"), 49190));
	cash_flows.push_back(mirr::CashFlow(dates::MakeDate("2014-02-28"), -112961.67));

	for (mirr::CashFlow& cash_flow : cash_flows) {
		cout << cash_flow.ToString() << endl;
	}

	result = calculator.GetRate(cash_flows);
	cout << "IRR=" << std::fixed << std::setw(15) << std::setprecision(30) << result << endl;
	cout << "Expected IRR= 0.53910534308576466360784" << endl;
	
	return true;
}

