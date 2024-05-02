#pragma once
#include <string>
using namespace std;

namespace Technical {
	enum MesenMode {
		Simple,
		YoriDow,
	};
	enum MesenVector {
		mesenUe,
		mesenShita,
		mesenWakaranai,
		mesenUeYorinoWakaranai,
		mesenShitaYorinoWakaranai,
	};
	enum MesenDrawLine {
		dl_None,
		dl_All,
		dl_MesenOnly,
		dl_MesenAndResisapo,
		dl_MesenAndTip,
	};
	class Mesen {
		MesenMode _mode;
		MesenDrawLine _drawline;
		int _timeFrame;
		int _limit;
		int _lineColor;
		int _lineStyle;
		double _resisapoPrice;
		double _mesenPrice;
		double _tipPrice;
		double _mesenCandidatePrice;
		bool _inited;
		MesenVector _currentMesen;
		void judge(int index);
		void firstVector();
		bool drawHline();
		string periodName();
		bool _showHline;
		void resisapo();
	public:
		Mesen(int timeFrame, int limit, int linecolor, int linestyle, MesenDrawLine drawLine);
		MesenVector Vector();
		double TipPrice();
		double MesenPrice();
		double ResisapoPrice();
		void SetDrawLineType(MesenDrawLine drawline);
	};
}