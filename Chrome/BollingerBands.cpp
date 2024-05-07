#include "pch.h"
#include <windows.h>
#include <malloc.h>
#include "StrategyInterfaceUnit.h"
#include "TechnicalFunctions.h"
#include "BollingerBands.h"

using namespace std;

namespace Technical {
	double BollingerBands::GetSMA(int index)
	{
		double sum = 0;
		for (int i = index; i < (_bbPeriod + index); i++) {
			sum += iClose(Symbol(), _timeFrame, i);
		}
		return sum / _bbPeriod;
	}

	BollingerBands::BollingerBands(char* symbol, int timeFrame, int period)
	{
		_symbol = symbol;
		_timeFrame = timeFrame;
		_bbPeriod = period;
	}

	BBPrices BollingerBands::GetPrices(int index)
	{
		double middle = GetSMA(index);
		double sum = 0;
		for (int i = index; i < (_bbPeriod + index); i++) {
			double b = iClose(Symbol(), _timeFrame, i) - middle;
			double a = (double)fabs(b);
			sum += pow(a, 2);
		}
		double squareRoot = sqrt(sum / _bbPeriod);
		double sd1 = squareRoot * 1;
		double sd2 = squareRoot * 2;
		double sd3 = squareRoot * 3;

		BBPrices bb;
		bb.middle = middle;
		bb.upper1 = middle + sd1;
		bb.lower1 = middle - sd1;
		bb.upper2 = middle + sd2;
		bb.lower2 = middle - sd2;
		bb.upper3 = middle + sd3;
		bb.lower3 = middle - sd3;
		return bb;
	}
}