#include "pch.h"
#include <windows.h>
#include <malloc.h>
#include "StrategyInterfaceUnit.h"
#include "TechnicalFunctions.h"


int Timeframe;
double OpenTime;

EXPORT void __stdcall InitStrategy()
{
  StrategyShortName("Chrome");
  StrategyDescription("Cafe au lait And Tanatana");

  RegOption("Timeframe", ot_TimeFrame, &Timeframe);
  Timeframe = PERIOD_M15;
}

EXPORT void __stdcall DoneStrategy()
{
}

EXPORT void __stdcall  ResetStrategy()
{}

EXPORT void __stdcall GetSingleTick()
{
  SetCurrencyAndTimeframe(Symbol(), Timeframe);
  if (OpenTime == NULL) {
	  OpenTime = Time(0);
  }
  if (OpenTime != Time(0)) {
	  Print("fixed");
	  OpenTime = Time(0);
  }
}
