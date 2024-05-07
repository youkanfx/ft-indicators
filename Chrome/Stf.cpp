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
		// ���݂̃G���A�݂̂Ŕ���
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

		// ���O�̃G���A���݂Ŕ���
		if (_beforeStfArea == areaUnknown) {
			return stfOrbitUnknown;
		}

		BBPrices bbPrices = _bb->GetPrices(1);
		// ��C�P�E
		if (_beforeStfArea == areaUpperIkeIke) {
			double low = iLow(Symbol(), _timeFrame, 1);
			// �`�n�l���~�h����
			if (currentArea == areaUpperMiddle) {
				// �~�h���^�b�`���Ă���ꍇ
				if (bbPrices.middle >= low) {
					// �~�h���Ȃ�
					_beforeStfArea = currentArea;
					return stfOrbitUp;
				}
				else {
					// �~�h���G��ɍs��
					_beforeStfArea = currentArea;
					return stfOrbitDown;
				}
			}
			// �`�n�l���~�h����
			else if (currentArea == areaLowerMiddle) {
				// -1�Ѓ^�b�`���Ă���ꍇ
				if (bbPrices.lower1 >= low) {
					// -1�ЋȂ�
					_beforeStfArea = currentArea;
					return stfOrbitUp;
				}
				else {
					// -1�АG��ɍs��
					_beforeStfArea = currentArea;
					return stfOrbitDown;
				}
			}
		}
		// ���C�P�E
		else if (_beforeStfArea == areaLowerIkeIke) {
			double high = iHigh(Symbol(), _timeFrame, 1);
			// �`�n�l���~�h����
			if (currentArea == areaLowerMiddle) {
				// �~�h���^�b�`���Ă���ꍇ
				if (bbPrices.middle <= high) {
					// �~�h���Ȃ�
					_beforeStfArea = currentArea;
					return stfOrbitDown;
				}
				else {
					// �~�h���G��ɍs��
					_beforeStfArea = currentArea;
					return stfOrbitUp;
				}
			}
			// �`�n�l���~�h����
			else if (currentArea == areaUpperMiddle) {
				// +1�Ѓ^�b�`���Ă���ꍇ
				if (bbPrices.upper1 <= high) {
					// +1�ЋȂ�
					_beforeStfArea = currentArea;
					return stfOrbitDown;
				}
				else {
					// +1�АG��ɍs��
					_beforeStfArea = currentArea;
					return stfOrbitUp;
				}
			}
		}
		// �~�h����
		else if (_beforeStfArea == areaUpperMiddle) {
			// 1�ЊO�͔���ς݂Ȃ̂ł����ł͖���
			double low = iLow(Symbol(), _timeFrame, 1);
			double high = iHigh(Symbol(), _timeFrame, 1);
			// �`���������n�l���~�h����
			if (currentArea == areaUpperMiddle) {
				// �~�h���G���Ă��炸�A+1�Ђ�G���Ă���ꍇ
				if (high >= bbPrices.upper1 && low > bbPrices.middle) {
					// 1�Г��J�`�J�`
					_beforeStfArea = currentArea;
					return stfOrbitDown;
				}
				// +1�АG���Ă��炸�A�~�h����G���Ă���ꍇ
				else if (high < bbPrices.upper1 && low <= bbPrices.middle) {
					// �~�h���Ȃ�
					_beforeStfArea = currentArea;
					return stfOrbitUp;
				}

				// ����ȊO�͂킩��Ȃ�
				_beforeStfArea = currentArea;
				return stfOrbitUnknown;
			}
			// �`�n�l���~�h����
			else if (currentArea == areaLowerMiddle) {
				// -1�Ђ�G���Ă���ꍇ
				if (low <= bbPrices.lower1) {
					// -1�ЋȂ�
					_beforeStfArea = currentArea;
					return stfOrbitUp;
				}

				// ����ȊO�͉�
				_beforeStfArea = currentArea;
				return stfOrbitDown;
			}
		}
		// �~�h����
		else if (_beforeStfArea == areaLowerMiddle) {
			// 1�ЊO�͔���ς݂Ȃ̂ł����ł͖���
			double low = iLow(Symbol(), _timeFrame, 1);
			double high = iHigh(Symbol(), _timeFrame, 1);
			// �`���������n�l���~�h����
			if (currentArea == areaLowerMiddle) {
				// �~�h���G���Ă��炸�A-1�Ђ�G���Ă���ꍇ
				if (low <= bbPrices.lower1 && high < bbPrices.middle) {
					// 1�Г��J�`�J�`
					_beforeStfArea = currentArea;
					return stfOrbitUp;
				}
				// -1�АG���Ă��炸�A�~�h����G���Ă���ꍇ
				else if (low > bbPrices.lower1 && high >= bbPrices.middle) {
					// �~�h���Ȃ�
					_beforeStfArea = currentArea;
					return stfOrbitDown;
				}

				// ����ȊO�͂킩��Ȃ�
				_beforeStfArea = currentArea;
				return stfOrbitUnknown;
			}
			// �`�n�l���~�h����
			else if (currentArea == areaUpperMiddle) {
				// +1�Ђ�G���Ă���ꍇ
				if (high >= bbPrices.upper1) {
					// -1�ЋȂ�
					_beforeStfArea = currentArea;
					return stfOrbitDown;
				}

				// ����ȊO�͏�
				_beforeStfArea = currentArea;
				return stfOrbitUp;
			}
		}

		return stfOrbitUnknown;
	}
}