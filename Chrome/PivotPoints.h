#pragma once

namespace Technical {
	class PivotPoints {
	private:
		char* _symbol;
		int _timeFrame;
		double _pp;
		double _r1;
		double _r2;
		double _r3;
		double _s1;
		double _s2;
		double _s3;
	public:
		PivotPoints(char* symbol, int timeFrame);
		bool Calculate();
		double PP() { return _pp; };
		double R1() { return _r1; };
		double R2() { return _r2; };
		double R3() { return _r3; };
		double S1() { return _s1; };
		double S2() { return _s2; };
		double S3() { return _s3; };
	};
}