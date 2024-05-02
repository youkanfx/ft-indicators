#pragma once
#include "BollingerBands.h"

namespace Technical {
	enum StfArea {
		areaUnknown,
		areaUpperIkeIke,
		areaLowerIkeIke,
		areaUpperMiddle,
		areaLowerMiddle,
	};

	enum StfOrbit {
		stfOrbitUp,
		stfOrbitDown,
		stfOrbitUnknown
	};

	enum StfVector {
		stfVecUp,
		stfVecLow,
		stfVecNone
	};

	class Stf {
	private:
		char* _symbol;
		int _timeFrame;
		int _period;
		BollingerBands* _bb;
		StfArea _beforeStfArea;
	public:
		Stf(char* symbol, int timeFrame, int period);
		~Stf();
		StfVector GetVector();
		StfArea GetArea();
		StfOrbit GetOrbit();
	};
}