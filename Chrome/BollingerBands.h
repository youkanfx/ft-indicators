#pragma once

namespace Technical {
	struct BBPrices {
		double middle;
		double upper1;
		double lower1;
		double upper2;
		double lower2;
		double upper3;
		double lower3;
	};

	class BollingerBands {
	private:
		int _timeFrame;
		char* _symbol;
		int _bbPeriod;
		double GetSMA(int index);
	public:
		BollingerBands(char* symbol, int timeFrame, int period);
		BBPrices GetPrices(int index);
	};
}