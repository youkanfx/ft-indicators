//---------------------------------------------------------------------------
// Ishimoku indicator
//---------------------------------------------------------------------------

#include "pch.h"
#include <windows.h>
#include "IndicatorInterfaceUnit.h"
#include "TechnicalFunctions.h"


// External variables
int period;

// Buffers
TIndexBuffer middle, sigma1high, sigma1low, sigma2high, sigma2low;

//---------------------------------------------------------------------------
// Initialize indicator
//---------------------------------------------------------------------------
EXPORT void __stdcall Init()
{
  // define properties
  IndicatorShortName("Bollinger Bands 12");
  SetOutputWindow(ow_ChartWindow);

  // register options
  period = 20;

  // create buffers
  middle = CreateIndexBuffer();
  sigma1high = CreateIndexBuffer();
  sigma1low = CreateIndexBuffer();
  sigma2high = CreateIndexBuffer();
  sigma2low = CreateIndexBuffer();

  IndicatorBuffers(5);
  SetIndexBuffer(0, middle);
  SetIndexBuffer(1, sigma1high);
  SetIndexBuffer(2, sigma1low);
  SetIndexBuffer(3, sigma2high);
  SetIndexBuffer(4, sigma2low);

  SetIndexStyle(0, ds_Line, psSolid, 1, clDimGray);
  SetIndexLabel(0, "Middle");
  SetIndexStyle(1, ds_Line, psSolid, 1, clDimGray);
  SetIndexLabel(1, "+1");
  SetIndexStyle(2, ds_Line, psSolid, 1, clDimGray);
  SetIndexLabel(2, "-1");
  SetIndexStyle(3, ds_Line, psSolid, 1, clDimGray);
  SetIndexLabel(3, "+2");
  SetIndexStyle(4, ds_Line, psSolid, 1, clDimGray);
  SetIndexLabel(4, "-2");
}

//---------------------------------------------------------------------------
// Calculate requested bar
//---------------------------------------------------------------------------
EXPORT void __stdcall Calculate(int index)
{
  if (index + period >= Bars()) {
      return;
  }

  double middleValue = GetMA(index, 0, period, ma_SMA, pt_Close, middle[index + 1]);
  middle[index] = middleValue;
  
  // calculate bands
  double sum = 0;
  for (int i = index; i < (index + period); i++) {
      sum += pow(abs((Close(i) - middleValue)), 2);
  }
  double squareRoot = sqrt(sum / period);
  double sd1 = squareRoot * 1;
  double sd2 = squareRoot * 2;

  sigma1high[index] = middleValue + sd1;
  sigma1low[index] = middleValue - sd1;
  sigma2high[index] = middleValue + sd2;
  sigma2low[index] = middleValue - sd2;
}
