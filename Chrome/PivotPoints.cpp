#include "pch.h"
#include "StrategyInterfaceUnit.h"
#include "TechnicalFunctions.h"
#include "PivotPoints.h"

namespace Technical {
	PivotPoints::PivotPoints(char* symbol, int timeFrame) {
		_symbol = symbol;
		_timeFrame = timeFrame;
		_pp = 0;
		_r1 = 0;
		_r2 = 0;
		_r3 = 0;
		_s1 = 0;
		_s2 = 0;
		_s3 = 0;
	}

	bool PivotPoints::Calculate() {
		double close = iClose(Symbol(), _timeFrame, 1);
		double high = iHigh(Symbol(), _timeFrame, 1);
		double low = iLow(Symbol(), _timeFrame, 1);

		if (close <= 0 || high <= 0 || low <= 0) {
			return false;
		}

		_pp = (close + high + low) / 3;
		_r1 = _pp + (_pp - low);
		_r2 = _pp + (high - low);
		_r3 = _r1 + (high - low);
		_s1 = _pp - (high - _pp);
		_s2 = _pp - (high - low);
		_s3 = _s1 - (high - low);

		return true;
	}
}