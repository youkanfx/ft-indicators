#include "pch.h"
#include <windows.h>
#include <malloc.h>
#include "StrategyInterfaceUnit.h"
#include "TechnicalFunctions.h"
#include <string>
#include <vector>
#include <tchar.h>

using namespace std;

void PrintStr(string str) {
	char* cstr = new char[str.size() + 1];
	char_traits<char>::copy(cstr, str.c_str(), str.size() + 1);
	Print(cstr);
	delete[] cstr;
}

#pragma region BollingerBands
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
		sum += pow(abs((iClose(Symbol(), _timeFrame, i) - middle)), 2);
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
#pragma endregion
#pragma region Cafeaulait
enum CafeaulaitState {
	Exploded,
	MiddleTouch,
	Entried,
};

class Cafeaulait {
private:
	double _rootPrice;
	double _topPrice;
	CafeaulaitState _state;
public:
	Cafeaulait(double rootPrice, double topPrice);
	void ModifyState(CafeaulaitState state);
	CafeaulaitState GetState();
	double GetRootPrice();
	double GetTopPrice();
};
Cafeaulait::Cafeaulait(double rootPrice, double topPrice)
{
	_topPrice = topPrice;
	_rootPrice = rootPrice;
	_state = Exploded;
}

void Cafeaulait::ModifyState(CafeaulaitState state)
{
	_state = state;
}

CafeaulaitState Cafeaulait::GetState()
{
	return _state;
}

double Cafeaulait::GetRootPrice() {
	return _rootPrice;
}

double Cafeaulait::GetTopPrice() {
	return _topPrice;
}
#pragma endregion
#pragma region Stf
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

Stf::Stf(char* symbol, int timeFrame, int period){
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
#pragma endregion
#pragma region Pivot Points
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
#pragma endregion
#pragma region Mesen
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
class Mesen {
	MesenMode _mode;
	int _timeFrame;
	int _limit;
	int _lineColor;
	double _mesenPrice;
	double _tipPrice;
	double _mesenCandidatePrice;
	MesenVector _currentMesen;
	void judge(int index);
	void firstVector();
public:
	Mesen(int timeFrame, int limit, int color);
	MesenVector Vector();
	double TipPrice();
	double MesenPrice();
};

Mesen::Mesen(int timeFrame, int limit, int color) {
	_mode = MesenMode::Simple;
	_timeFrame = timeFrame;
	_limit = limit;
	_mesenPrice = 0;
	_tipPrice = 0;
	_lineColor = color;
	_mesenCandidatePrice = 0;
	firstVector();
}

void Mesen::judge(int index) {
	// 現在、シンプル目線の実装のみ
	if (_tipPrice == 0 || _mesenPrice == 0) {
		return;
	}

	double open = iOpen(Symbol(), _timeFrame, index);
	double close = iClose(Symbol(), _timeFrame, index);
	if (_currentMesen == mesenUe) {
		// 高値更新
		if (_tipPrice < close) {
			// 高値ラインと目線ラインを更新
			_tipPrice = close;
			if (_mesenCandidatePrice > 0) {
				_mesenPrice = _mesenCandidatePrice;
				_mesenCandidatePrice = 0;
			}
		}
		// 押し安値割り
		else if (_mesenPrice > close) {
			_currentMesen = mesenShita;
			_mesenPrice = _tipPrice;
			_tipPrice = close;
			_mesenCandidatePrice = 0;
		}
		// 高値更新、目線ライン更新無し
		else if (_tipPrice >= close && _mesenPrice <= close) {
			// 陰線の場合は終値が目線ライン候補
			if (open > close) {
				_mesenCandidatePrice = close;
			}
		}
	}
	else if (_currentMesen == mesenShita) {
		// 安値更新
		if (_tipPrice > close) {
			// 安値ラインと目線ラインを更新
			_tipPrice = close;
			if (_mesenCandidatePrice > 0) {
				_mesenPrice = _mesenCandidatePrice;
				_mesenCandidatePrice = 0;
			}
		}
		// 戻り高値越え
		else if (_mesenPrice < close) {
			_currentMesen = mesenUe;
			_mesenPrice = _tipPrice;
			_tipPrice = close;
			_mesenCandidatePrice = 0;
		}
		// 安値更新、目線ライン更新無し
		else if (_tipPrice <= close && _mesenPrice >= close) {
			// 陽線の場合は終値が目線ライン候補
			if (open < close) {
				_mesenCandidatePrice = close;
			}
		}
	}
	else {
		// 今のところ未実装
	}
}

void Mesen::firstVector() {
	int barCount = iBars(Symbol(), _timeFrame);
	if (barCount > _limit) {
		barCount = _limit;
	}

	// 遡ったローソク足の陰陽で暫定目線を決定
	double firstOpen = iOpen(Symbol(), _timeFrame, barCount);
	double firstClose = iClose(Symbol(), _timeFrame, barCount);
	_currentMesen = firstOpen < firstClose ? mesenUe : mesenShita;
	_tipPrice = firstClose;
	_mesenPrice = firstOpen;
	_mesenCandidatePrice = 0;

	for (int i = (barCount - 1); i >= 0; i--) {
		judge(i);
	}
}

MesenVector Mesen::Vector() {
	judge(1);
	return _currentMesen;
}

double Mesen::TipPrice() {
	return _tipPrice;
}

double Mesen::MesenPrice() {
	return _mesenPrice;
}
#pragma endregion


#pragma region Global Parameters
double OpenTime;
double OpenTimeW1;
double OpenTimeD1;
double OpenTimeH4;
double OpenTimeH1;

StfVector StfD1Vector = stfVecNone;
StfVector StfH4Vector = stfVecNone;
StfVector StfH1Vector = stfVecNone;

StfOrbit StfD1Orbit = stfOrbitUnknown;
StfOrbit StfH4Orbit = stfOrbitUnknown;
StfOrbit StfH1Orbit = stfOrbitUnknown;

MesenVector MesenD1Vector = mesenWakaranai;
MesenVector MesenH4Vector = mesenWakaranai;
MesenVector MesenH1Vector = mesenWakaranai;

void UpperCafeaulait(BBPrices bb);
void LowerCafeaulait(BBPrices bb);

void W1Setting();
void D1Setting();
void H4Setting();
void H1Setting();

bool CanLongEntry(double et, double tp);
bool CanShortEntry(double et, double tp);

void ShowStatus();

PivotPoints* ppD1;
PivotPoints* ppW1;
BollingerBands *bb15m;
Stf* stfD1;
Stf* stfH4;
Stf* stfH1;
Cafeaulait* upCafe;
Cafeaulait* lowCafe;
Mesen* meD1;
Mesen* meH4;
Mesen* meH1;

bool StfD1Filter;
bool StfH4Filter;
bool StfH1Filter;
bool StfOrbitD1Filter;
bool StfOrbitH4Filter;
bool StfOrbitH1Filter;
int Timeframe;
double RewardRatio;
bool PPW1RRFilter;
bool PPD1RRFilter;
bool IsShowStatus;
bool ReverseFilter;
bool MesenD1Filter;
bool MesenH4Filter;
bool MesenH1Filter;

int IkeIkeFilterType;
enum BBFilterType {
	None = 0,
	IkeIkeOnly = 1,
	NotIkeIkeOnly = 2,
};
#pragma endregion

EXPORT void __stdcall InitStrategy()
{
  StrategyShortName("Chrome");
  StrategyDescription("Chrome Rule EA");

  RegOption("D1 STF Filter", ot_Boolean, &StfD1Filter);
  StfD1Filter = true;

  RegOption("D1 IkeIke Filter", ot_EnumType, &IkeIkeFilterType);
  AddOptionValue("D1 IkeIke Filter", "None");
  AddOptionValue("D1 IkeIke Filter", "IkeIke Only");
  AddOptionValue("D1 IkeIke Filter", "Not IkeIke Only");
  IkeIkeFilterType = BBFilterType::None;

  RegOption("D1 STF Orbit Filter", ot_Boolean, &StfOrbitD1Filter);
  StfOrbitD1Filter = true;

  RegOption("D1 Mesen Filter", ot_Boolean, &MesenD1Filter);
  MesenD1Filter = true;

  RegOption("H4 STF Filter", ot_Boolean, &StfH4Filter);
  StfH4Filter = true;

  RegOption("H4 STF Orbit Filter", ot_Boolean, &StfOrbitH4Filter);
  StfOrbitH4Filter = false;

  RegOption("H4 Mesen Filter", ot_Boolean, &MesenH4Filter);
  MesenH4Filter = true;

  RegOption("H1 STF Filter", ot_Boolean, &StfH1Filter);
  StfH1Filter = true;

  RegOption("H1 STF Orbit Filter", ot_Boolean, &StfOrbitH1Filter);
  StfOrbitH1Filter = false;

  RegOption("H1 Mesen Filter", ot_Boolean, &MesenH1Filter);
  MesenH1Filter = true;

  RegOption("Timeframe", ot_TimeFrame, &Timeframe);
  Timeframe = PERIOD_M15;

  RegOption("Reward Ratio", ot_Double, &RewardRatio);
  RewardRatio = 1.0;
  SetOptionRange("Reward Ratio", 0.5, 5.0);

  RegOption("W1 PivotPoints RR Filter", ot_Boolean, &PPW1RRFilter);
  PPW1RRFilter = true;

  RegOption("D1 PivotPoints RR Filter", ot_Boolean, &PPD1RRFilter);
  PPD1RRFilter = true;

  RegOption("Show Status", ot_Boolean, &IsShowStatus);
  IsShowStatus = true;

  RegOption("Reverse Filter", ot_Boolean, &ReverseFilter);
  ReverseFilter = false;

  int bbPeriod = 20;
  bb15m = new BollingerBands(Symbol(), Timeframe, bbPeriod);
  stfD1 = new Stf(Symbol(), PERIOD_D1, bbPeriod);
  stfH4 = new Stf(Symbol(), PERIOD_H4, bbPeriod);
  stfH1 = new Stf(Symbol(), PERIOD_H1, bbPeriod);
  ppW1 = new PivotPoints(Symbol(), PERIOD_W1);
  ppD1 = new PivotPoints(Symbol(), PERIOD_D1);
  int limit = 100;
  meD1 = new Mesen(PERIOD_D1, limit, RGB(0xFF, 0x00, 0x00));
  meH4 = new Mesen(PERIOD_H4, limit, RGB(0x00, 0xFF, 0x00));
  meH1 = new Mesen(PERIOD_H1, limit, RGB(0x00, 0x00, 0xFF));
}

EXPORT void __stdcall DoneStrategy()
{
	delete bb15m;
	delete stfD1;
	delete stfH4;
	delete stfH1;
	delete ppW1;
	delete ppD1;
	delete meD1;
	delete meH4;
	delete meH1;
	if (ObjectExists("chrome_status")) {
		ObjectDelete("chrome_status");
	}
}

EXPORT void __stdcall  ResetStrategy()
{}

EXPORT void __stdcall GetSingleTick()
{
  SetCurrencyAndTimeframe(Symbol(), Timeframe);
  // W1 Setting
  W1Setting();
  // D1 Setting
  D1Setting();
  // H4 Setting
  H4Setting();
  // H1 Setting
  H1Setting();

  if (IsShowStatus) {
	  ShowStatus();
  }

  // 15Mカフェオレ
  if (OpenTime == NULL) {
	  OpenTime = iTime(Symbol(), Timeframe, 0);
  }
  if (OpenTime != iTime(Symbol(), Timeframe, 0)) {
	  OpenTime = iTime(Symbol(), Timeframe, 0);
	  BBPrices bb = bb15m->GetPrices(1);
	  UpperCafeaulait(bb);
	  LowerCafeaulait(bb);
  }
}

int candlePeriod = 8;
bool isExplodedLong = false;
int candleCountLong = 0;
void UpperCafeaulait(BBPrices bbPrices) {
	double high = iHigh(Symbol(), Timeframe, 1);
	if (!isExplodedLong && high >= bbPrices.upper2) {
		// 根本探索
		bool existsRoot = false;
		double root;
		for (int i = 2; i < iBars(Symbol(), Timeframe); i++) {
			BBPrices uBb = bb15m->GetPrices(i);
			if (iClose(Symbol(), Timeframe, i) < uBb.lower1) {
				root = iLow(Symbol(), Timeframe, i);
				existsRoot = true;
				break;
			}
		}

		if (!existsRoot) {
			return;
		}

		PrintStr("[ Upper : Create Cafe Object ] Root Is " + to_string(root) + " / High Is " + to_string(high));
		upCafe = new Cafeaulait(root, high);
		isExplodedLong = true;
		candleCountLong = 0;
		return;
	}

	if (isExplodedLong && upCafe != NULL) {
		double close = iClose(Symbol(), Timeframe, 1);
		// 根本、頂点のどちらかを終値で更新した場合は破棄
		if (upCafe->GetTopPrice() < close || upCafe->GetRootPrice() > close) {
			delete upCafe;
			isExplodedLong = false;
			PrintStr("[ Upper : Delete Cafe Object ] Break Root or Top");
			return;
		}

		// 爆発後のミドルタッチ判定
		if (upCafe->GetState() == Exploded && bbPrices.middle >= iLow(Symbol(), Timeframe, 1)) {
			upCafe->ModifyState(MiddleTouch);
			PrintStr("[ Upper : Modify Cafe Object ] Root Is " + to_string(upCafe->GetRootPrice()));
			return;
		}

		if (upCafe->GetState() == MiddleTouch) {
			// ミドルタッチ後、ローソク足のカウントスタート
			candleCountLong += 1;
			// ローソク足が8本経過したらカフェオレオブジェクト破棄
			if (candleCountLong >= candlePeriod) {
				delete upCafe;
				isExplodedLong = false;
				PrintStr("[ Upper : Delete Cafe Object ] Over Period");
				return;
			}

			double open = iOpen(Symbol(), Timeframe, 1);
			// ミドルタッチ後、陽線確定でエントリー
			if (close > open) {
				// エントリー
				double et = Ask();
				double root = upCafe->GetRootPrice();
				double diff = et - root;
				double tp = et + (diff * RewardRatio);
				int oh;

				if (CanLongEntry(et, tp)) {
					if (!SendInstantOrder(Symbol(), op_Buy, 0.01, root, tp, "", 0, oh)) {
						Print("long entry error!!");
					}

					delete upCafe;
					isExplodedLong = false;
					PrintStr("[ Upper : Delete Cafe Object ] Long Entried");
					return;
				}
			}
		}
	}
}

bool isExplodedShort = false;
int candleCountShort = 0;
void LowerCafeaulait(BBPrices bbPrices) {
	double low = iLow(Symbol(), Timeframe, 1);
	if (!isExplodedShort && low <= bbPrices.lower2) {
		// 根本探索
		bool existsRoot = false;
		double root;
		for (int i = 2; i < iBars(Symbol(), Timeframe); i++) {
			BBPrices uBb = bb15m->GetPrices(i);
			if (iClose(Symbol(), Timeframe, i) > uBb.upper1) {
				root = iHigh(Symbol(), Timeframe, i);
				existsRoot = true;
				break;
			}
		}

		if (!existsRoot) {
			return;
		}

		PrintStr("[ Low : Create Cafe Object ] Root Is " + to_string(root) + " / Low Is " + to_string(low));
		lowCafe = new Cafeaulait(root, low);
		isExplodedShort = true;
		candleCountShort = 0;
		return;
	}

	if (isExplodedShort && lowCafe != NULL) {
		double close = iClose(Symbol(), Timeframe, 1);
		// 根本、頂点のどちらかを終値で更新した場合は破棄
		if (lowCafe->GetTopPrice() > close || lowCafe->GetRootPrice() < close) {
			delete lowCafe;
			isExplodedShort = false;
			PrintStr("[ Low : Delete Cafe Object ] Break Root or Top");
			return;
		}

		// 爆発後のミドルタッチ判定
		if (lowCafe->GetState() == Exploded && bbPrices.middle <= iHigh(Symbol(), Timeframe, 1)) {
			lowCafe->ModifyState(MiddleTouch);
			PrintStr("[ Low : Modify Cafe Object ] Root Is " + to_string(lowCafe->GetRootPrice()));
			return;
		}

		if (lowCafe->GetState() == MiddleTouch) {
			// ミドルタッチ後、ローソク足のカウントスタート
			candleCountShort += 1;
			// ローソク足が8本経過したらカフェオレオブジェクト破棄
			if (candleCountShort >= candlePeriod) {
				delete lowCafe;
				isExplodedShort = false;
				PrintStr("[ Low : Delete Cafe Object ] Over Period");
				return;
			}

			double open = iOpen(Symbol(), Timeframe, 1);
			// ミドルタッチ後、陰線確定でエントリー
			if (close < open) {
				// エントリー
				double et = Bid();
				double root = lowCafe->GetRootPrice();
				double diff = root - et;
				double tp = et - (diff * RewardRatio);
				int oh;

				if (CanShortEntry(et, tp)) {
					if (!SendInstantOrder(Symbol(), op_Sell, 0.01, root, tp, "", 0, oh)) {
						Print("short entry error!!");
					}

					delete lowCafe;
					isExplodedShort = false;
					PrintStr("[ Low : Delete Cafe Object ] Short Entried");
					return;
				}
			}
		}
	}
}

void W1Setting() {
	double time = iTime(Symbol(), PERIOD_W1, 0);
	if (OpenTimeW1 == NULL) {
		OpenTimeW1 = time;
	}

	if (OpenTimeW1 != time) {
		OpenTimeW1 = time;

		ppW1->Calculate();
		Print("Calculate W1 PP!!");
	}
}

void D1Setting() {
	double time = iTime(Symbol(), PERIOD_D1, 0);
	if (OpenTimeD1 == NULL) {
		OpenTimeD1 = time;
	}

	if (OpenTimeD1 != time) {
		OpenTimeD1 = time;

		StfD1Vector = stfD1->GetVector();
		StfD1Orbit = stfD1->GetOrbit();
		ppD1->Calculate();
		Print("Calculate D1 PP!!");
		MesenD1Vector = meD1->Vector();
	}
}

void H4Setting() {
	double time = iTime(Symbol(), PERIOD_H4, 0);
	if (OpenTimeH4 == NULL) {
		OpenTimeH4 = time;
	}

	if (OpenTimeH4 != time) {
		OpenTimeH4 = time;

		StfH4Vector = stfH4->GetVector();
		StfH4Orbit = stfH4->GetOrbit();
		MesenH4Vector = meH4->Vector();
	}
}

void H1Setting() {
	double time = iTime(Symbol(), PERIOD_H1, 0);
	if (OpenTimeH1 == NULL) {
		OpenTimeH1 = time;
	}

	if (OpenTimeH1 != time) {
		OpenTimeH1 = time;
		
		StfH1Vector = stfH1->GetVector();
		StfH1Orbit = stfH1->GetOrbit();
		MesenH1Vector = meH1->Vector();
	}
}

string boolToStr(bool b) {
	return b ? "OK" : "NG";
}

bool StfFilterLong() {
	if (!StfD1Filter) {
		return true;
	}

	if (StfD1Filter && StfD1Vector != stfVecUp) {
		return false;
	}

	if (!StfH4Filter && !StfH1Filter) {
		return true;
	}

	if (StfH4Filter && StfH4Vector == stfVecUp) {
		return true;
	}

	if (StfH1Filter && StfH1Vector == stfVecUp) {
		return true;
	}

	return false;
}

bool IkeIkeFilterLong() {
	if (IkeIkeFilterType == BBFilterType::None) {
		return true;
	}
	
	StfArea stfArea = stfD1->GetArea();
	if (IkeIkeFilterType == BBFilterType::IkeIkeOnly) {
		if (stfArea == StfArea::areaUpperIkeIke) {
			return true;
		}
		else {
			return false;
		}
	}
	else if (IkeIkeFilterType == BBFilterType::NotIkeIkeOnly) {
		if (stfArea == StfArea::areaUpperIkeIke) {
			return false;
		}
		else {
			return true;
		}
	}
}

bool W1PPRewardFilterLong(double et, double tp) {
	if (!PPW1RRFilter) {
		return true;
	}

	double r2 = ppW1->R2();
	double r3 = ppW1->R3();
	if (r2 <= 0 || r3 <= 0) {
		return true;
	}

	if (et < r2 && (tp > r2 || tp > r3)) {
		return false;
	}

	if (et >= r2 && tp > r3) {
		return false;
	}
	
	return true;
}

bool D1PPRewardFilterLong(double et, double tp) {
	if (!PPD1RRFilter) {
		return true;
	}

	double r2 = ppD1->R2();
	double r3 = ppD1->R3();
	if (r2 <= 0 || r3 <= 0) {
		return true;
	}

	if (et < r2 && (tp > r2 || tp > r3)) {
		return false;
	}

	if (et >= r2 && tp > r3) {
		return false;
	}

	return true;
}

bool StfOrbitFilterLong(bool enableFilter, StfOrbit orbit) {
	if (!enableFilter) {
		return true;
	}

	if (orbit != stfOrbitUp) {
		return false;
	}

	return true;
}

bool MesenFilterLong(bool enableFilter, MesenVector vector) {
	if (!enableFilter) {
		return true;
	}

	if (vector != mesenUe) {
		return false;
	}

	return true;
}

bool CanLongEntry(double et, double tp) {
	bool stfFilter = StfFilterLong();
	bool ikeikeFilter = IkeIkeFilterLong();
	bool w1PPRRFilter = W1PPRewardFilterLong(et, tp);
	bool d1PPRRFilter = D1PPRewardFilterLong(et, tp);
	bool d1OrbitFilter = StfOrbitFilterLong(StfOrbitD1Filter, StfD1Orbit);
	bool h4OrbitFilter = StfOrbitFilterLong(StfOrbitH4Filter, StfH4Orbit);
	bool h1OrbitFilter = StfOrbitFilterLong(StfOrbitH1Filter, StfH1Orbit);
	bool d1MesenFilter = MesenFilterLong(MesenD1Filter, MesenD1Vector);
	bool h4MesenFilter = MesenFilterLong(MesenH4Filter, MesenH4Vector);
	bool h1MesenFilter = MesenFilterLong(MesenH1Filter, MesenH1Vector);

	string msg = "[ CanLongEntry ]STF Filter:" + boolToStr(stfFilter) +
		" / IkeIke Filter:" + boolToStr(ikeikeFilter) +
		" / W1 PP Filter:" + boolToStr(w1PPRRFilter) +
		" / D1 PP Filter:" + boolToStr(d1PPRRFilter) +
		" / D1 Orbit Filter:" + boolToStr(d1OrbitFilter) +
		" / H4 Orbit Filter:" + boolToStr(h4OrbitFilter) +
		" / H1 Orbit Filter:" + boolToStr(h1OrbitFilter) +
		" / D1 Mesen Filter:" + boolToStr(d1MesenFilter) +
		" / H4 Mesen Filter:" + boolToStr(h4MesenFilter) +
		" / H1 Mesen Filter:" + boolToStr(h1MesenFilter)
		;
	PrintStr(msg);

	bool canEntry = (stfFilter && ikeikeFilter && w1PPRRFilter && d1PPRRFilter &&
					d1OrbitFilter && h4OrbitFilter && h1OrbitFilter &&
					d1MesenFilter && h4MesenFilter && h1MesenFilter);
	return ReverseFilter ? !canEntry : canEntry;
}

bool StfFilterShort() {
	if (!StfD1Filter) {
		return true;
	}

	if (StfD1Filter && StfD1Vector != stfVecLow) {
		return false;
	}

	if (!StfH4Filter && !StfH1Filter) {
		return true;
	}

	if (StfH4Filter && StfH4Vector == stfVecLow) {
		return true;
	}

	if (StfH1Filter && StfH1Vector == stfVecLow) {
		return true;
	}

	return false;
}

bool IkeIkeFilterShort() {
	if (IkeIkeFilterType == BBFilterType::None) {
		return true;
	}

	StfArea stfArea = stfD1->GetArea();
	if (IkeIkeFilterType == BBFilterType::IkeIkeOnly) {
		if (stfArea == StfArea::areaLowerIkeIke) {
			return true;
		}
		else {
			return false;
		}
	}
	else if (IkeIkeFilterType == BBFilterType::NotIkeIkeOnly) {
		if (stfArea == StfArea::areaLowerIkeIke) {
			return false;
		}
		else {
			return true;
		}
	}
}

bool W1PPRewardFilterShort(double et, double tp) {
	if (!PPW1RRFilter) {
		return true;
	}

	double s2 = ppW1->S2();
	double s3 = ppW1->S3();
	if (s2 <= 0 || s3 <= 0) {
		return true;
	}

	if (et > s2 && (tp < s2 || tp < s3)) {
		return false;
	}

	if (et <= s2 && tp < s3) {
		return false;
	}

	return true;
}

bool D1PPRewardFilterShort(double et, double tp) {
	if (!PPD1RRFilter) {
		return true;
	}

	double s2 = ppD1->S2();
	double s3 = ppD1->S3();
	if (s2 <= 0 || s3 <= 0) {
		return true;
	}

	if (et > s2 && (tp < s2 || tp < s3)) {
		return false;
	}

	if (et <= s2 && tp < s3) {
		return false;
	}

	return true;
}

bool StfOrbitFilterShort(bool enableFilter, StfOrbit orbit) {
	if (!enableFilter) {
		return true;
	}

	if (orbit != stfOrbitDown) {
		return false;
	}

	return true;
}

bool MesenFilterShort(bool enableFilter, MesenVector vector) {
	if (!enableFilter) {
		return true;
	}

	if (vector != mesenShita) {
		return false;
	}

	return true;
}

bool CanShortEntry(double et, double tp) {
	bool stfFilter = StfFilterShort();
	bool ikeikeFilter = IkeIkeFilterShort();
	bool w1PPRRFilter = W1PPRewardFilterShort(et, tp);
	bool d1PPRRFilter = D1PPRewardFilterShort(et, tp);
	bool d1OrbitFilter = StfOrbitFilterShort(StfOrbitD1Filter, StfD1Orbit);
	bool h4OrbitFilter = StfOrbitFilterShort(StfOrbitH4Filter, StfH4Orbit);
	bool h1OrbitFilter = StfOrbitFilterShort(StfOrbitH1Filter, StfH1Orbit);
	bool d1MesenFilter = MesenFilterShort(MesenD1Filter, MesenD1Vector);
	bool h4MesenFilter = MesenFilterShort(MesenH4Filter, MesenH4Vector);
	bool h1MesenFilter = MesenFilterShort(MesenH1Filter, MesenH1Vector);

	string msg = "[ CanShortEntry ]STF Filter:" + boolToStr(stfFilter) +
		" / IkeIke Filter:" + boolToStr(ikeikeFilter) +
		" / W1 PP Filter:" + boolToStr(w1PPRRFilter) +
		" / D1 PP Filter:" + boolToStr(d1PPRRFilter) +
		" / D1 Orbit Filter:" + boolToStr(d1OrbitFilter) +
		" / H4 Orbit Filter:" + boolToStr(h4OrbitFilter) +
		" / H1 Orbit Filter:" + boolToStr(h1OrbitFilter) +
		" / D1 Mesen Filter:" + boolToStr(d1MesenFilter) +
		" / H4 Mesen Filter:" + boolToStr(h4MesenFilter) +
		" / H1 Mesen Filter:" + boolToStr(h1MesenFilter)
		;
	PrintStr(msg);

	bool canEntry = (stfFilter && ikeikeFilter && w1PPRRFilter && d1PPRRFilter &&
		d1OrbitFilter && h4OrbitFilter && h1OrbitFilter &&
		d1MesenFilter && h4MesenFilter && h1MesenFilter);
	return ReverseFilter ? !canEntry : canEntry;
}

string stfVectorStr(StfVector vec) {
	switch (vec)
	{
	case stfVecUp:
		return "UP";
	case stfVecLow:
		return "Down";
	case stfVecNone:
		return "None";
	default:
		return "None";
	}
}

string stfOrbitStr(StfOrbit orbit) {
	switch (orbit)
	{
	case stfOrbitUp:
		return "UP";
	case stfOrbitDown:
		return "Down";
	case stfOrbitUnknown:
		return "None";
	default:
		return "None";
	}
}

string mesenStr(MesenVector mesen) {
	switch (mesen)
	{
	case mesenUe:
		return "UP";
	case mesenShita:
		return "Down";
	default:
		return "None";
	}
}

void ShowStatus() {
	char* objName = "chrome_status";
	if (!ObjectExists(objName)) {
		ObjectCreate(objName, obj_Text, 0, Time(0), Close(0));
		ObjectSet(objName, OBJPROP_SCREENCOORDS, 1);
		ObjectSet(objName, OBJPROP_SCRHALIGNMENT, 800);
		ObjectSet(objName, OBJPROP_SCRVALIGNMENT, 50);
	}
	// 右上にSTFのD,4H,1Hの向き、軌道
	string text = "D1: [STF(VEC)]" + stfVectorStr(StfD1Vector);
	text += " [STF(ORBIT)]" + stfOrbitStr(StfD1Orbit);
	text += " [MESEN]" + mesenStr(MesenD1Vector) + "\r\n";
	text += "H4: [STF(VEC)]" + stfVectorStr(StfH4Vector);
	text += " [STF(ORBIT)]" + stfOrbitStr(StfH4Orbit);
	text += " [MESEN]" + mesenStr(MesenH4Vector) + "\r\n";
	text += "H1: [STF(VEC)]" + stfVectorStr(StfH1Vector);
	text += " [STF(ORBIT)]" + stfOrbitStr(StfH1Orbit);
	text += " [MESEN]" + mesenStr(MesenH1Vector);
	
	char* cstr = new char[text.size() + 1];
	char_traits<char>::copy(cstr, text.c_str(), text.size() + 1);
	ObjectSetText(objName, cstr, 8, "Meiryo UI", RGB(0xff, 0xff, 0xff));
	delete[] cstr;
}