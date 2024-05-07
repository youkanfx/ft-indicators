#include "pch.h"
#include "StrategyInterfaceUnit.h"
#include "TechnicalFunctions.h"
#include "Stf.h"

namespace Technical {
	Stf::Stf(char* symbol, int timeFrame, int period) {
		_symbol = symbol;
		_timeFrame = timeFrame;
		_period = period;
		_bb = new BollingerBands(symbol, timeFrame, period);
		_beforeStfArea = StfArea::areaUnknown;
	}

	Stf::~Stf() {
		delete _bb;
	}

	StfVector Stf::GetVector() {
		int barCount = iBars(Symbol(), _timeFrame);
		if (_period > barCount) {
			return stfVecNone;
		}
		for (int i = 1; i < barCount; i++) {
			BBPrices bbPrices = _bb->GetPrices(i);
			double dayClose = iClose(Symbol(), _timeFrame, i);

			if (dayClose > bbPrices.upper1) {
				return stfVecUp;
			}
			else if (dayClose < bbPrices.lower1) {
				return stfVecLow;
			}
		}
		return stfVecNone;
	}

	StfArea Stf::GetArea() {
		int barCount = iBars(Symbol(), _timeFrame);
		if (_period > barCount) {
			return StfArea::areaUnknown;
		}

		BBPrices bbPrices = _bb->GetPrices(0);
		double open = iOpen(Symbol(), _timeFrame, 0);
		if (bbPrices.upper1 < open) {
			return StfArea::areaUpperIkeIke;
		}
		else if (bbPrices.lower1 > open) {
			return StfArea::areaLowerIkeIke;
		}
		else if (bbPrices.upper1 > open && bbPrices.middle < open) {
			return StfArea::areaUpperMiddle;
		}
		else if (bbPrices.lower1 < open && bbPrices.middle > open) {
			return StfArea::areaLowerMiddle;
		}

		return StfArea::areaUnknown;
	}

	StfOrbit Stf::GetOrbit() {
		StfArea currentArea = GetArea();
		// 現在のエリアのみで判定
		if (currentArea == areaUnknown) {
			_beforeStfArea = currentArea;
			return stfOrbitUnknown;
		}
		else if (currentArea == areaUpperIkeIke) {
			_beforeStfArea = currentArea;
			return stfOrbitUp;
		}
		else if (currentArea == areaLowerIkeIke) {
			_beforeStfArea = currentArea;
			return stfOrbitDown;
		}

		// 直前のエリア込みで判定
		if (_beforeStfArea == areaUnknown) {
			return stfOrbitUnknown;
		}

		BBPrices bbPrices = _bb->GetPrices(1);
		// 上イケ脱
		if (_beforeStfArea == areaUpperIkeIke) {
			double low = iLow(Symbol(), _timeFrame, 1);
			// ～始値がミドル上
			if (currentArea == areaUpperMiddle) {
				// ミドルタッチしている場合
				if (bbPrices.middle >= low) {
					// ミドル曲げ
					_beforeStfArea = currentArea;
					return stfOrbitUp;
				}
				else {
					// ミドル触りに行く
					_beforeStfArea = currentArea;
					return stfOrbitDown;
				}
			}
			// ～始値がミドル下
			else if (currentArea == areaLowerMiddle) {
				// -1σタッチしている場合
				if (bbPrices.lower1 >= low) {
					// -1σ曲げ
					_beforeStfArea = currentArea;
					return stfOrbitUp;
				}
				else {
					// -1σ触りに行く
					_beforeStfArea = currentArea;
					return stfOrbitDown;
				}
			}
		}
		// 下イケ脱
		else if (_beforeStfArea == areaLowerIkeIke) {
			double high = iHigh(Symbol(), _timeFrame, 1);
			// ～始値がミドル下
			if (currentArea == areaLowerMiddle) {
				// ミドルタッチしている場合
				if (bbPrices.middle <= high) {
					// ミドル曲げ
					_beforeStfArea = currentArea;
					return stfOrbitDown;
				}
				else {
					// ミドル触りに行く
					_beforeStfArea = currentArea;
					return stfOrbitUp;
				}
			}
			// ～始値がミドル上
			else if (currentArea == areaUpperMiddle) {
				// +1σタッチしている場合
				if (bbPrices.upper1 <= high) {
					// +1σ曲げ
					_beforeStfArea = currentArea;
					return stfOrbitDown;
				}
				else {
					// +1σ触りに行く
					_beforeStfArea = currentArea;
					return stfOrbitUp;
				}
			}
		}
		// ミドル上
		else if (_beforeStfArea == areaUpperMiddle) {
			// 1σ外は判定済みなのでここでは無視
			double low = iLow(Symbol(), _timeFrame, 1);
			double high = iHigh(Symbol(), _timeFrame, 1);
			// ～引き続き始値がミドル上
			if (currentArea == areaUpperMiddle) {
				// ミドル触っておらず、+1σを触っている場合
				if (high >= bbPrices.upper1 && low > bbPrices.middle) {
					// 1σ内カチカチ
					_beforeStfArea = currentArea;
					return stfOrbitDown;
				}
				// +1σ触っておらず、ミドルを触っている場合
				else if (high < bbPrices.upper1 && low <= bbPrices.middle) {
					// ミドル曲げ
					_beforeStfArea = currentArea;
					return stfOrbitUp;
				}

				// それ以外はわからない
				_beforeStfArea = currentArea;
				return stfOrbitUnknown;
			}
			// ～始値がミドル下
			else if (currentArea == areaLowerMiddle) {
				// -1σを触っている場合
				if (low <= bbPrices.lower1) {
					// -1σ曲げ
					_beforeStfArea = currentArea;
					return stfOrbitUp;
				}

				// それ以外は下
				_beforeStfArea = currentArea;
				return stfOrbitDown;
			}
		}
		// ミドル下
		else if (_beforeStfArea == areaLowerMiddle) {
			// 1σ外は判定済みなのでここでは無視
			double low = iLow(Symbol(), _timeFrame, 1);
			double high = iHigh(Symbol(), _timeFrame, 1);
			// ～引き続き始値がミドル下
			if (currentArea == areaLowerMiddle) {
				// ミドル触っておらず、-1σを触っている場合
				if (low <= bbPrices.lower1 && high < bbPrices.middle) {
					// 1σ内カチカチ
					_beforeStfArea = currentArea;
					return stfOrbitUp;
				}
				// -1σ触っておらず、ミドルを触っている場合
				else if (low > bbPrices.lower1 && high >= bbPrices.middle) {
					// ミドル曲げ
					_beforeStfArea = currentArea;
					return stfOrbitDown;
				}

				// それ以外はわからない
				_beforeStfArea = currentArea;
				return stfOrbitUnknown;
			}
			// ～始値がミドル上
			else if (currentArea == areaUpperMiddle) {
				// +1σを触っている場合
				if (high >= bbPrices.upper1) {
					// -1σ曲げ
					_beforeStfArea = currentArea;
					return stfOrbitDown;
				}

				// それ以外は上
				_beforeStfArea = currentArea;
				return stfOrbitUp;
			}
		}

		return stfOrbitUnknown;
	}
}