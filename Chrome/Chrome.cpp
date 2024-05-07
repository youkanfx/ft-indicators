#include "pch.h"
#include <windows.h>
#include <malloc.h>
#include <string>
#include <vector>
#include <tchar.h>

#include "StrategyInterfaceUnit.h"
#include "TechnicalFunctions.h"
#include "Chrome.h"

using namespace std;
using namespace Technical;

void PrintStr(string str) {
	char* cstr = new char[str.size() + 1];
	char_traits<char>::copy(cstr, str.c_str(), str.size() + 1);
	Print(cstr);
	delete[] cstr;
}

EXPORT void __stdcall InitStrategy()
{
  StrategyShortName("Chrome");
  StrategyDescription("Chrome Rule EA");

  AddSeparator("Trigger");

  RegOption("Timeframe", ot_TimeFrame, &Timeframe);
  Timeframe = PERIOD_M15;

  RegOption("Entry", ot_EnumType, &op_EntryType);
  AddOptionValue("Entry", "Auto");
  AddOptionValue("Entry", "Manual");
  op_EntryType = et_Auto;

  RegOption("Reward Ratio", ot_Double, &RewardRatio);
  RewardRatio = 1.0;
  SetOptionRange("Reward Ratio", 0.5, 5.0);

  AddSeparator("M1 Filter");

  RegOption("M1 ResiSapo Filter", ot_Boolean, &ResiSapoM1Filter);
  ResiSapoM1Filter = true;

  RegOption("M1 Mesen DrawLine Type", ot_EnumType, &MesenM1DrawLine);
  AddOptionValue("M1 Mesen DrawLine Type", "None");
  AddOptionValue("M1 Mesen DrawLine Type", "All");
  AddOptionValue("M1 Mesen DrawLine Type", "Mesen Only");
  AddOptionValue("M1 Mesen DrawLine Type", "Mesen And Resisapo");
  AddOptionValue("M1 Mesen DrawLine Type", "Mesen And Tip");
  MesenM1DrawLine = dl_MesenAndResisapo;

  AddSeparator("W1 Filter");

  RegOption("M1 ResiSapo Filter", ot_Boolean, &ResiSapoW1Filter);
  ResiSapoW1Filter = true;

  RegOption("W1 Mesen DrawLine Type", ot_EnumType, &MesenW1DrawLine);
  AddOptionValue("W1 Mesen DrawLine Type", "None");
  AddOptionValue("W1 Mesen DrawLine Type", "All");
  AddOptionValue("W1 Mesen DrawLine Type", "Mesen Only");
  AddOptionValue("W1 Mesen DrawLine Type", "Mesen And Resisapo");
  AddOptionValue("W1 Mesen DrawLine Type", "Mesen And Tip");
  MesenW1DrawLine = dl_MesenAndResisapo;

  RegOption("W1 PivotPoints RR Filter", ot_Boolean, &PPW1RRFilter);
  PPW1RRFilter = true;

  AddSeparator("D1 Filter");

  RegOption("D1 STF Filter", ot_Boolean, &StfD1Filter);
  StfD1Filter = true;

  RegOption("D1 IkeIke Filter", ot_EnumType, &IkeIkeFilterType);
  AddOptionValue("D1 IkeIke Filter", "None");
  AddOptionValue("D1 IkeIke Filter", "IkeIke Only");
  AddOptionValue("D1 IkeIke Filter", "Not IkeIke Only");
  IkeIkeFilterType = BBFilterType::None;

  RegOption("D1 STF Orbit Filter", ot_Boolean, &StfOrbitD1Filter);
  StfOrbitD1Filter = true;

  RegOption("D1 ResiSapo Filter", ot_Boolean, &ResiSapoD1Filter);
  ResiSapoD1Filter = true;

  RegOption("D1 Mesen Filter", ot_Boolean, &MesenD1Filter);
  MesenD1Filter = true;

  RegOption("D1 Mesen DrawLine Type", ot_EnumType, &MesenD1DrawLine);
  AddOptionValue("D1 Mesen DrawLine Type", "None");
  AddOptionValue("D1 Mesen DrawLine Type", "All");
  AddOptionValue("D1 Mesen DrawLine Type", "Mesen Only");
  AddOptionValue("D1 Mesen DrawLine Type", "Mesen And Resisapo");
  AddOptionValue("D1 Mesen DrawLine Type", "Mesen And Tip");
  MesenD1DrawLine = dl_MesenAndResisapo;

  RegOption("D1 PivotPoints RR Filter", ot_Boolean, &PPD1RRFilter);
  PPD1RRFilter = true;

  AddSeparator("H4 Filter");

  RegOption("H4 STF Filter", ot_Boolean, &StfH4Filter);
  StfH4Filter = true;

  RegOption("H4 STF Orbit Filter", ot_Boolean, &StfOrbitH4Filter);
  StfOrbitH4Filter = false;

  RegOption("H4 Mesen Filter", ot_Boolean, &MesenH4Filter);
  MesenH4Filter = false;

  RegOption("H4 Mesen DrawLine Type", ot_EnumType, &MesenH4DrawLine);
  AddOptionValue("H4 Mesen DrawLine Type", "None");
  AddOptionValue("H4 Mesen DrawLine Type", "All");
  AddOptionValue("H4 Mesen DrawLine Type", "Mesen Only");
  AddOptionValue("H4 Mesen DrawLine Type", "Mesen And Resisapo");
  AddOptionValue("H4 Mesen DrawLine Type", "Mesen And Tip");
  MesenH4DrawLine = dl_MesenOnly;

  AddSeparator("H1 Filter");

  RegOption("H1 STF Filter", ot_Boolean, &StfH1Filter);
  StfH1Filter = true;

  RegOption("H1 STF Orbit Filter", ot_Boolean, &StfOrbitH1Filter);
  StfOrbitH1Filter = false;

  RegOption("H1 Mesen Filter", ot_Boolean, &MesenH1Filter);
  MesenH1Filter = false;

  RegOption("H1 Mesen DrawLine Type", ot_EnumType, &MesenH1DrawLine);
  AddOptionValue("H1 Mesen DrawLine Type", "None");
  AddOptionValue("H1 Mesen DrawLine Type", "All");
  AddOptionValue("H1 Mesen DrawLine Type", "Mesen Only");
  AddOptionValue("H1 Mesen DrawLine Type", "Mesen And Resisapo");
  AddOptionValue("H1 Mesen DrawLine Type", "Mesen And Tip");
  MesenH1DrawLine = dl_MesenOnly;

  AddSeparator("Options");

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
  meM1 = new Mesen(PERIOD_MO1, limit, RGB(0xFF, 0xFF, 0xFF), psSolid, dl_MesenAndResisapo);
  meW1 = new Mesen(PERIOD_W1, limit, RGB(0xFF, 0xFF, 0xFF), psDot, dl_MesenAndResisapo);
  meD1 = new Mesen(PERIOD_D1, limit, RGB(0xFF, 0x00, 0x00), psSolid, dl_MesenAndResisapo);
  meH4 = new Mesen(PERIOD_H4, limit, RGB(0xFF, 0x00, 0x00), psDot, dl_MesenOnly);
  meH1 = new Mesen(PERIOD_H1, limit, RGB(0x00, 0x00, 0xFF), psSolid, dl_MesenOnly);
}

EXPORT void __stdcall DoneStrategy()
{
	delete bb15m;
	delete stfD1;
	delete stfH4;
	delete stfH1;
	delete ppW1;
	delete ppD1;
	delete meM1;
	delete meW1;
	delete meD1;
	delete meH4;
	delete meH1;
	if (ObjectExists("chrome_status")) {
		ObjectDelete("chrome_status");
	}
}

EXPORT void __stdcall  ResetStrategy()
{
	meM1->SetDrawLineType(MesenM1DrawLine);
	meW1->SetDrawLineType(MesenW1DrawLine);
	meD1->SetDrawLineType(MesenD1DrawLine);
	meH4->SetDrawLineType(MesenH4DrawLine);
	meH1->SetDrawLineType(MesenH1DrawLine);
}

EXPORT void __stdcall GetSingleTick()
{
  SetCurrencyAndTimeframe(Symbol(), Timeframe);
  // M1 Setting
  M1Setting();
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
					if (op_EntryType == et_Auto) {
						if (!SendInstantOrder(Symbol(), op_Buy, 0.01, root, tp, "", 0, oh)) {
							Print("long entry error!!");
						}
					}
					else {
						Pause("Cafe!!");
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
					if (op_EntryType == et_Auto) {
						if (!SendInstantOrder(Symbol(), op_Sell, 0.01, root, tp, "", 0, oh)) {
							Print("short entry error!!");
						}
					}
					else {
						Pause("Cafe!!");
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

void M1Setting() {
	double time = iTime(Symbol(), PERIOD_MO1, 0);
	if (OpenTimeM1 == NULL) {
		OpenTimeM1 = time;
	}

	if (OpenTimeM1 != time) {
		OpenTimeM1 = time;

		meM1->Vector();
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
		meW1->Vector();
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
	return false;
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

bool ResiSapoFilterLong(bool enableFilter, double resisapoPrice, double tp) {
	if (!enableFilter) {
		return true;
	}

	if (resisapoPrice <= 0) {
		return true;
	}

	if (resisapoPrice < tp) {
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
	bool m1ResiSapoFilter = ResiSapoFilterLong(ResiSapoM1Filter, meM1->ResisapoPrice(), tp);
	bool w1ResiSapoFilter = ResiSapoFilterLong(ResiSapoW1Filter, meW1->ResisapoPrice(), tp);
	bool d1ResiSapoFilter = ResiSapoFilterLong(ResiSapoD1Filter, meD1->ResisapoPrice(), tp);

	string msg = "[ CanLongEntry ]STF Filter:" + boolToStr(stfFilter) +
		" / IkeIke Filter:" + boolToStr(ikeikeFilter) +
		" / W1 PP Filter:" + boolToStr(w1PPRRFilter) +
		" / D1 PP Filter:" + boolToStr(d1PPRRFilter) +
		" / D1 Orbit Filter:" + boolToStr(d1OrbitFilter) +
		" / H4 Orbit Filter:" + boolToStr(h4OrbitFilter) +
		" / H1 Orbit Filter:" + boolToStr(h1OrbitFilter) +
		" / D1 Mesen Filter:" + boolToStr(d1MesenFilter) +
		" / H4 Mesen Filter:" + boolToStr(h4MesenFilter) +
		" / H1 Mesen Filter:" + boolToStr(h1MesenFilter) +
		" / M1 ResiSapo Filter:" + boolToStr(m1ResiSapoFilter) +
		" / W1 ResiSapo Filter:" + boolToStr(w1ResiSapoFilter) +
		" / D1 ResiSapo Filter:" + boolToStr(d1ResiSapoFilter)
		;
	PrintStr(msg);

	bool canEntry = (stfFilter && ikeikeFilter && w1PPRRFilter && d1PPRRFilter &&
					d1OrbitFilter && h4OrbitFilter && h1OrbitFilter &&
					d1MesenFilter && h4MesenFilter && h1MesenFilter &&
					m1ResiSapoFilter && w1ResiSapoFilter && d1ResiSapoFilter);
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
	return false;
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

bool ResiSapoFilterShort(bool enableFilter, double resisapoPrice, double tp) {
	if (!enableFilter) {
		return true;
	}

	if (resisapoPrice <= 0) {
		return true;
	}

	if (resisapoPrice > tp) {
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
	bool m1ResiSapoFilter = ResiSapoFilterShort(ResiSapoM1Filter, meM1->ResisapoPrice(), tp);
	bool w1ResiSapoFilter = ResiSapoFilterShort(ResiSapoW1Filter, meW1->ResisapoPrice(), tp);
	bool d1ResiSapoFilter = ResiSapoFilterShort(ResiSapoD1Filter, meD1->ResisapoPrice(), tp);

	string msg = "[ CanShortEntry ]STF Filter:" + boolToStr(stfFilter) +
		" / IkeIke Filter:" + boolToStr(ikeikeFilter) +
		" / W1 PP Filter:" + boolToStr(w1PPRRFilter) +
		" / D1 PP Filter:" + boolToStr(d1PPRRFilter) +
		" / D1 Orbit Filter:" + boolToStr(d1OrbitFilter) +
		" / H4 Orbit Filter:" + boolToStr(h4OrbitFilter) +
		" / H1 Orbit Filter:" + boolToStr(h1OrbitFilter) +
		" / D1 Mesen Filter:" + boolToStr(d1MesenFilter) +
		" / H4 Mesen Filter:" + boolToStr(h4MesenFilter) +
		" / H1 Mesen Filter:" + boolToStr(h1MesenFilter) +
		" / M1 ResiSapo Filter:" + boolToStr(m1ResiSapoFilter) +
		" / W1 ResiSapo Filter:" + boolToStr(w1ResiSapoFilter) +
		" / D1 ResiSapo Filter:" + boolToStr(d1ResiSapoFilter)
		;
	PrintStr(msg);

	bool canEntry = (stfFilter && ikeikeFilter && w1PPRRFilter && d1PPRRFilter &&
		d1OrbitFilter && h4OrbitFilter && h1OrbitFilter &&
		d1MesenFilter && h4MesenFilter && h1MesenFilter &&
		m1ResiSapoFilter && w1ResiSapoFilter && d1ResiSapoFilter);
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