#include "pch.h"
#include <windows.h>
#include <malloc.h>
#include "StrategyInterfaceUnit.h"
#include "TechnicalFunctions.h"
#include <string>
#include <vector>

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
	BollingerBands(char* symbol, int timeFrame);
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

BollingerBands::BollingerBands(char* symbol, int timeFrame)
{
	_symbol = symbol;
	_timeFrame = timeFrame;
	_bbPeriod = 20;
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
	CafeaulaitState _state;
public:
	Cafeaulait(double rootPrice);
	void ModifyState(CafeaulaitState state);
	CafeaulaitState GetState();
	double GetRootPrice();
};
Cafeaulait::Cafeaulait(double rootPrice)
{
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
#pragma endregion
#pragma region Stf
enum StfVector {
	stfVecUp,
	stfVecLow,
	stfVecNone
};

class Stf {
private:
	char* _symbol;
	int _timeFrame;
	BollingerBands* _bb;
public:
	Stf(char* symbol, int timeFrame);
	~Stf();
	StfVector GetVector();
};

Stf::Stf(char* symbol, int timeFrame){
	_symbol = symbol;
	_timeFrame = timeFrame;
	_bb = new BollingerBands(symbol, timeFrame);
}

Stf::~Stf() {
	delete _bb;
}

StfVector Stf::GetVector() {
	for (int i = 1; i < iBars(Symbol(), _timeFrame); i++) {
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
#pragma endregion

double OpenTime;
double OpenTimeD1;
double OpenTimeH4;
double OpenTimeH1;
bool IsUpperExplosion = false;
bool IsLowerExplosion = false;
bool IsUpperMiddleTouch = false;
bool IsLowerMiddleTouch = false;
bool IsLongEntried = false;
bool IsShortEntried = false;
int LongOrderHandle = -1;
int ShortOrderHandle = -1;
double UpperExplosionRoot;
double LowerExplosionRoot;
StfVector StfD1Vector = stfVecNone;
StfVector StfH4Vector = stfVecNone;
StfVector StfH1Vector = stfVecNone;

void UpperCafeaulait(BBPrices bb);
void LowerCafeaulait(BBPrices bb);

void SetStfD1Filter();
void SetStfH4Filter();
void SetStfH1Filter();

bool CanLongEntry();
bool CanShortEntry();

BollingerBands *bb15m;
Stf* stfD1;
Stf* stfH4;
Stf* stfH1;
vector<Cafeaulait*> upCafes;
vector<Cafeaulait*> lowCafes;

bool StfD1Filter;
bool StfH4Filter;
bool StfH1Filter;
int Timeframe;

EXPORT void __stdcall InitStrategy()
{
  StrategyShortName("Chrome");
  StrategyDescription("Cafe au lait");

  RegOption("D1 STF Filter", ot_Boolean, &StfD1Filter);
  StfD1Filter = true;

  RegOption("H4 STF Filter", ot_Boolean, &StfH4Filter);
  StfH4Filter = true;

  RegOption("H1 STF Filter", ot_Boolean, &StfH1Filter);
  StfH1Filter = true;

  RegOption("Timeframe", ot_TimeFrame, &Timeframe);
  Timeframe = PERIOD_M15;

  bb15m = new BollingerBands(Symbol(), PERIOD_M15);
  stfD1 = new Stf(Symbol(), PERIOD_D1);
  stfH4 = new Stf(Symbol(), PERIOD_H4);
  stfH1 = new Stf(Symbol(), PERIOD_H1);
}

EXPORT void __stdcall DoneStrategy()
{
	delete bb15m;
	delete stfD1;
	delete stfH4;
	delete stfH1;
}

EXPORT void __stdcall  ResetStrategy()
{}

EXPORT void __stdcall GetSingleTick()
{
  SetCurrencyAndTimeframe(Symbol(), Timeframe);
  // D1STF
  SetStfD1Filter();
  // H4STF
  SetStfH4Filter();
  // H1STF
  SetStfH1Filter();

  // 15Mカフェオレ
  if (OpenTime == NULL) {
	  OpenTime = iTime(Symbol(), PERIOD_M15, 0);
  }
  if (OpenTime != iTime(Symbol(), PERIOD_M15, 0)) {
	  OpenTime = iTime(Symbol(), PERIOD_M15, 0);
	  BBPrices bb = bb15m->GetPrices(1);
	  if (CanLongEntry()) {
		// 上カフェオレ
		UpperCafeaulait(bb);
	  }

	  if (CanShortEntry()) {
		// 下カフェオレ
		LowerCafeaulait(bb);
	  }
  }
}

void UpperCafeaulait(BBPrices bbPrices) {
	if (iHigh(Symbol(), PERIOD_M15, 1) >= bbPrices.upper2) {
		// 根本探索
		bool existsRoot = false;
		double root;
		for (int i = 2; i < iBars(Symbol(), PERIOD_M15); i++) {
			BBPrices uBb = bb15m->GetPrices(i);
			if (iClose(Symbol(), PERIOD_M15, i) < uBb.lower1) {
				root = iLow(Symbol(), PERIOD_M15, i);
				existsRoot = true;
				break;
			}
		}

		if (!existsRoot) {
			return;
		}

		Cafeaulait* cafe = new Cafeaulait(root);
		upCafes.push_back(cafe);
	}

	if (bbPrices.middle >= iLow(Symbol(), PERIOD_M15, 1)) {
		for (Cafeaulait* c : upCafes) {
			if (c->GetState() == Exploded) {
				c->ModifyState(MiddleTouch);
			}
		}
	}

	if (iClose(Symbol(), PERIOD_M15, 1) > iOpen(Symbol(), PERIOD_M15, 1)) {
		for (Cafeaulait* c : upCafes) {
			if (c->GetState() == MiddleTouch) {
				// エントリー
				double root = c->GetRootPrice();
				double diff = Ask() - root;
				double tp = Ask() + diff;
				int oh;
				if (!SendInstantOrder(Symbol(), op_Buy, 0.01, root, tp, "", 0, oh)) {
					Print("long entry error!!");
				}
				else {
					auto end = remove(upCafes.begin(), upCafes.end(), c);
					upCafes.erase(end, upCafes.end());
				}
			}
		}
	}
}

void LowerCafeaulait(BBPrices bbPrices) {
	if (iLow(Symbol(), PERIOD_M15, 1) <= bbPrices.lower2) {
		// 根本探索
		bool existsRoot = false;
		double root;
		for (int i = 2; i < iBars(Symbol(), PERIOD_M15); i++) {
			BBPrices uBb = bb15m->GetPrices(i);
			if (iClose(Symbol(), PERIOD_M15, i) > uBb.upper1) {
				root = iHigh(Symbol(), PERIOD_M15, i);
				existsRoot = true;
				break;
			}
		}

		if (!existsRoot) {
			return;
		}

		Cafeaulait* cafe = new Cafeaulait(root);
		lowCafes.push_back(cafe);
	}

	if (bbPrices.middle <= iHigh(Symbol(), PERIOD_M15, 1)) {
		for (Cafeaulait* c : lowCafes) {
			if (c->GetState() == Exploded) {
				c->ModifyState(MiddleTouch);
			}
		}
	}

	if (iClose(Symbol(), PERIOD_M15, 1) < iOpen(Symbol(), PERIOD_M15, 1)) {
		for (Cafeaulait* c : lowCafes) {
			if (c->GetState() == MiddleTouch) {
				// エントリー
				double root = c->GetRootPrice();
				double diff = root - Bid();
				double tp = Bid() - diff;
				int oh;
				if (!SendInstantOrder(Symbol(), op_Sell, 0.01, root, tp, "", 0, oh)) {
					Print("short entry error!!");
				}
				else {
					auto end = remove(lowCafes.begin(), lowCafes.end(), c);
					lowCafes.erase(end, lowCafes.end());
				}
			}
		}
	}
}

void SetStfD1Filter() {
	if (OpenTimeD1 == NULL) {
		OpenTimeD1 = iTime(Symbol(), PERIOD_D1, 0);
	}

	if (OpenTimeD1 != iTime(Symbol(), PERIOD_D1, 0)) {
		OpenTimeD1 = iTime(Symbol(), PERIOD_D1, 0);
		StfD1Vector = stfD1->GetVector();
		string vec = "None";
		if (StfD1Vector == stfVecUp) {
			vec = "Up";
		}
		else if (StfD1Vector == stfVecLow) {
			vec = "Low";
		}
		PrintStr("STF D1 Vector: " + vec);
	}
}

void SetStfH4Filter() {
	if (OpenTimeH4 == NULL) {
		OpenTimeH4 = iTime(Symbol(), PERIOD_H4, 0);
	}

	if (OpenTimeH4 != iTime(Symbol(), PERIOD_H4, 0)) {
		OpenTimeH4 = iTime(Symbol(), PERIOD_H4, 0);
		StfH4Vector = stfH4->GetVector();
		string vec = "None";
		if (StfH4Vector == stfVecUp) {
			vec = "Up";
		}
		else if (StfH4Vector == stfVecLow) {
			vec = "Low";
		}
		PrintStr("STF H4 Vector: " + vec);
	}
}

void SetStfH1Filter() {
	if (OpenTimeH1 == NULL) {
		OpenTimeH1 = iTime(Symbol(), PERIOD_H1, 0);
	}

	if (OpenTimeH1 != iTime(Symbol(), PERIOD_H1, 0)) {
		OpenTimeH1 = iTime(Symbol(), PERIOD_H1, 0);
		StfH1Vector = stfH1->GetVector();
		string vec = "None";
		if (StfH1Vector == stfVecUp) {
			vec = "Up";
		}
		else if (StfH1Vector == stfVecLow) {
			vec = "Low";
		}
		PrintStr("STF H1 Vector: " + vec);
	}
}

bool CanLongEntry() {
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

bool CanShortEntry() {
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