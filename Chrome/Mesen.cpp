#include "pch.h"
#include "StrategyInterfaceUnit.h"
#include "TechnicalFunctions.h"
#include "Mesen.h"

void PrintStr(string str);

namespace Technical {
	Mesen::Mesen(int timeFrame, int limit, int linecolor, int linestyle, MesenDrawLine drawLine) {
		_mode = MesenMode::Simple;
		_timeFrame = timeFrame;
		_limit = limit;
		_resisapoPrice = 0;
		_mesenPrice = 0;
		_tipPrice = 0;
		_lineColor = linecolor;
		_lineStyle = linestyle;
		_mesenCandidatePrice = 0;
		_inited = false;
		_showHline = true;
		_drawline = drawLine;
	}

	void Mesen::judge(int index) {
		string logprefix = "[ " + to_string(_timeFrame) + "Mesen ]";
		// ���݁A�V���v���ڐ��̎����̂�
		if (_tipPrice == 0 || _mesenPrice == 0) {
			PrintStr(logprefix + "TipPrice And MesenPrice = 0");
			return;
		}

		double open = iOpen(Symbol(), _timeFrame, index);
		double close = iClose(Symbol(), _timeFrame, index);

		if (_currentMesen == mesenUe) {
			// ���l�X�V
			if (_tipPrice < close) {
				PrintStr(logprefix + "Takane Koushin");
				// ���l���C���Ɩڐ����C�����X�V
				_tipPrice = close;
				if (_mesenCandidatePrice > 0) {
					_mesenPrice = _mesenCandidatePrice;
					_mesenCandidatePrice = 0;
				}
			}
			// �������l����
			else if (_mesenPrice > close) {
				_currentMesen = mesenShita;
				// ���߂̗z���̏I�l���ڐ����C��
				bool exists = false;
				for (int i = (index + 1); i < _limit; i++) {
					double open = iOpen(Symbol(), _timeFrame, i);
					double close = iClose(Symbol(), _timeFrame, i);
					if (open < close) {
						_mesenPrice = close;
						exists = true;
						break;
					}
				}
				if (!exists) {
					_mesenPrice = _tipPrice;
				}
				_tipPrice = close;
				_mesenCandidatePrice = 0;
			}
			// ���l�X�V�A�ڐ����C���X�V����
			else if (_tipPrice >= close && _mesenPrice <= close) {
				// �A���̏ꍇ�͏I�l���ڐ����C�����
				if (open > close) {
					PrintStr(logprefix + "Mesen Line Kouho Koushin");
					_mesenCandidatePrice = close;
				}
			}
		}
		else if (_currentMesen == mesenShita) {
			// ���l�X�V
			if (_tipPrice > close) {
				PrintStr(logprefix + "Yasune Koushin");
				// ���l���C���Ɩڐ����C�����X�V
				_tipPrice = close;
				if (_mesenCandidatePrice > 0) {
					_mesenPrice = _mesenCandidatePrice;
					_mesenCandidatePrice = 0;
				}
			}
			// �߂荂�l�z��
			else if (_mesenPrice < close) {
				PrintStr(logprefix + "ModoriTakane Goe");
				_currentMesen = mesenUe;
				// ���߂̉A���̏I�l���ڐ����C��
				bool exists = false;
				for (int i = (index + 1); i < _limit; i++) {
					double open = iOpen(Symbol(), _timeFrame, i);
					double close = iClose(Symbol(), _timeFrame, i);
					if (open > close) {
						_mesenPrice = close;
						exists = true;
						break;
					}
				}
				if (!exists) {
					_mesenPrice = _tipPrice;
				}
				_tipPrice = close;
				_mesenCandidatePrice = 0;
			}
			// ���l�X�V�A�ڐ����C���X�V����
			else if (_tipPrice <= close && _mesenPrice >= close) {
				// �z���̏ꍇ�͏I�l���ڐ����C�����
				if (open < close) {
					PrintStr(logprefix + "Mesen Line Kouho Koushin");
					_mesenCandidatePrice = close;
				}
			}
		}
		else {
			// ���̂Ƃ��떢����
			PrintStr(logprefix + "No Logic");
		}
	}

	void Mesen::resisapo() {
		if (_currentMesen == mesenWakaranai) {
			return;
		}

		if (_tipPrice <= 0 || _mesenPrice <= 0) {
			return;
		}

		int barCount = iBars(Symbol(), _timeFrame);
		if (barCount > _limit) {
			barCount = _limit;
		}

		int maxIndex = barCount - 1;
		bool existsResisapo = false;
		for (int i = 0; i < maxIndex; i++) {
			double open = iOpen(Symbol(), _timeFrame, i);
			double close = iClose(Symbol(), _timeFrame, i);
			if (_currentMesen == mesenUe) {
				if (open < close && close > _tipPrice) {
					_resisapoPrice = close;
					existsResisapo = true;
					break;
				}
			}
			else if (_currentMesen == mesenShita) {
				if (close < open && close < _tipPrice) {
					_resisapoPrice = close;
					existsResisapo = true;
					break;
				}
			}
		}

		if (!existsResisapo) {
			_resisapoPrice = 0;
		}
	}

	void Mesen::firstVector() {
		int barCount = iBars(Symbol(), _timeFrame);
		if (barCount > _limit) {
			barCount = _limit;
		}

		int maxIndex = barCount - 1;

		// �k�������[�\�N���̉A�z�Ŏb��ڐ�������
		double firstOpen = iOpen(Symbol(), _timeFrame, maxIndex);
		double firstClose = iClose(Symbol(), _timeFrame, maxIndex);
		_currentMesen = firstOpen < firstClose ? mesenUe : mesenShita;
		_tipPrice = firstClose;
		_mesenPrice = firstOpen;
		_mesenCandidatePrice = 0;

		PrintStr("[ " + to_string(_timeFrame) + "Mesen ]" +
			"TipPrice:" + to_string(_tipPrice) +
			"MesenPrice:" + to_string(_mesenPrice)
		);

		for (int i = maxIndex; i >= 0; i--) {
			judge(i);
		}

		resisapo();
	}

	string Mesen::periodName() {
		switch (_timeFrame)
		{
		case 60:
			return "1H";
		case 240:
			return "4H";
		case 1440:
			return "D1";
		default:
			return "?";
		}
	}

	bool Mesen::drawHline() {
		if (_drawline == dl_None) {
			return true;
		}

		if (_drawline == dl_All || _drawline == dl_MesenAndTip) {
			if (_tipPrice > 0) {
				string objName = "tip_hline_tf" + to_string(_timeFrame);
				char* tipLineName = const_cast<char*>(objName.c_str());
				if (!ObjectExists(tipLineName)) {
					if (!ObjectCreate(tipLineName, obj_HLine, 0, 0, _tipPrice)) {
						return false;
					}
					ObjectSet(tipLineName, OBJPROP_COLOR, _lineColor);
					ObjectSet(tipLineName, OBJPROP_STYLE, psDash);
				}
				else {
					ObjectSet(tipLineName, OBJPROP_PRICE1, _tipPrice);
				}
			}
		}

		if (_mesenPrice > 0) {
			string objName = "mesen_hline_tf" + to_string(_timeFrame);
			char* mesenLineName = const_cast<char*>(objName.c_str());
			if (!ObjectExists(mesenLineName)) {
				if (!ObjectCreate(mesenLineName, obj_HLine, 0, 0, _mesenPrice)) {
					return false;
				}
				ObjectSet(mesenLineName, OBJPROP_COLOR, _lineColor);
				ObjectSet(mesenLineName, OBJPROP_STYLE, _lineStyle);
			}
			else {
				ObjectSet(mesenLineName, OBJPROP_PRICE1, _mesenPrice);
			}
		}

		if (_drawline == dl_All || _drawline == dl_MesenAndResisapo) {
			string objName = "resisapo_hline_tf" + to_string(_timeFrame);
			char* lineName = const_cast<char*>(objName.c_str());
			if (_resisapoPrice > 0) {
				if (!ObjectExists(lineName)) {
					if (!ObjectCreate(lineName, obj_HLine, 0, 0, _resisapoPrice)) {
						return false;
					}
					ObjectSet(lineName, OBJPROP_COLOR, _lineColor);
					ObjectSet(lineName, OBJPROP_STYLE, psDash);
				}
				else {
					ObjectSet(lineName, OBJPROP_PRICE1, _resisapoPrice);
				}
			}
			else {
				if (ObjectExists(lineName)) {
					ObjectDelete(lineName);
				}
			}
		}

		return true;
	}

	MesenVector Mesen::Vector() {
		if (!_inited) {
			firstVector();
			_inited = true;
			PrintStr("[ " + to_string(_timeFrame) + "Mesen ]Init!!");
		}
		else {
			judge(1);
		}
		resisapo();

		if (_showHline) {
			drawHline();
		}
		return _currentMesen;
	}

	double Mesen::TipPrice() {
		return _tipPrice;
	}

	double Mesen::MesenPrice() {
		return _mesenPrice;
	}

	double Mesen::ResisapoPrice() {
		return _resisapoPrice;
	}

	void Mesen::SetDrawLineType(MesenDrawLine drawline) {
		_drawline = drawline;
	}
}