#include "pch.h"
#include <malloc.h>
#include "StrategyInterfaceUnit.h"

EXPORT TInterfaceProcRec IntrfProcsRec;

BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
    {
        memset(&IntrfProcsRec, 0, sizeof(TInterfaceProcRec));
        IntrfProcsRec.dwSize = sizeof(TInterfaceProcRec);
        break;
    }

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        break;
    }

    return TRUE;
}

#ifndef rec
    #define rec IntrfProcsRec
#endif


//-----Replace PChar string with new one--------------------------------------
EXPORT void __stdcall ReplaceStr(PChar& dest, PChar source)
{
    free((void*)dest);
    dest = (PChar)malloc(strlen((char*)source) + 1);
    strcpy_s(dest, sizeof(dest), source);
    //strcpy(dest, source);

}

//-----Sell-------------------------------------------------------------------
int Sell(double LotSize, double StopLoss, double TakeProfit)
{
    double tp, sl;

    if (TakeProfit == 0) tp = 0; else tp = Bid() - TakeProfit * Point();
    if (StopLoss == 0) sl = 0; else sl = Bid() + StopLoss * Point();

    int result;
    SendInstantOrder(Symbol(), op_Sell, LotSize, sl, tp, "", 0, result);
    return result;
}

//-----Buy-------------------------------------------------------------------
int Buy(double LotSize, double StopLoss, double TakeProfit)
{
    double tp, sl;

    if (TakeProfit == 0) tp = 0; else tp = Ask() + TakeProfit * Point();
    if (StopLoss == 0) sl = 0; else sl = Ask() - StopLoss * Point();

    int result;
    SendInstantOrder(Symbol(), op_Buy, LotSize, sl, tp, "", 0, result);
    return result;
}

/*
//-----Convert time to string-------------------------------------------------
function StrTime(DateTime: TDateTime): string;
{
  DateTimeToString(result, 'yyyy.mm.dd hh:nn', DateTime);
}
*/

//-----Get currency information-----------------------------------------------
bool GetCurrencyInfo(PChar Symbol, PCurrencyInfo& info)
{
    return rec.GetCurrencyInfo(rec.pGetCurrencyInfo, Symbol, info);
}

//-----Register option--------------------------------------------------------
void RegOption(PChar OptionName, TOptionType OptionType, void* option)
{
    rec.RegOption(rec.pRegOption, OptionName, OptionType, option);
}

//-----Add separator----------------------------------------------------------
void AddSeparator(PChar text)
{
    rec.RegOption(rec.pRegOption, text, ot_Separator, NULL);
}

//-----Add option value (for drop down box options)---------------------------
void AddOptionValue(PChar OptionName, PChar value)
{
    rec.AddOptionValue(rec.pAddOptionValue, OptionName, value);
}

//-----Set option range (for integer, longword and double types)--------------
void SetOptionRange(PChar OptionName, double LowValue, double HighValue)
{
    rec.SetOptionRange(rec.pSetOptionRange, OptionName, LowValue, HighValue);
}

//-----Set option digits------------------------------------------------------
void SetOptionDigits(PChar OptionName, Word digits)
{
    rec.SetOptionDigits(rec.pSetOptionDigits, OptionName, digits);
}

//-----Print text in "Journal" window in ForexTester--------------------------
void Print(PChar s)
{
    rec.Print(rec.pPrint, s);
}

//-----Set strategy short name------------------------------------------------
void StrategyShortName(PChar name)
{
    rec.StrategyShortName(rec.pStrategyShortName, name);
}

//-----Set strategy description-----------------------------------------------
void StrategyDescription(PChar desc)
{
    rec.StrategyDescription(rec.pStrategyDescription, desc);
}

//-----Get Bid price----------------------------------------------------------
double Bid()
{
    return rec.Bid(rec.pBid);
}

//-----Get Ask price----------------------------------------------------------
double Ask()
{
    return rec.Ask(rec.pAsk);
}

//-----Currency name----------------------------------------------------------
PChar Symbol()
{
    return rec.Symbol(rec.pSymbol);
}

//-----Currency digits after point--------------------------------------------
int Digits()
{
    return rec.Digits(rec.pDigits);
}

//-----Minimal currency point-------------------------------------------------
double Point()
{
    return rec.Point(rec.pPoint);
}

//-----Get open value---------------------------------------------------------
double iOpen(PChar Symbol, int TimeFrame, int index)
{
    return rec.iOpen(rec.piOpen, Symbol, TimeFrame, index);
}

//-----Get close value--------------------------------------------------------
double iClose(PChar Symbol, int TimeFrame, int index)
{
    return rec.iClose(rec.piClose, Symbol, TimeFrame, index);
}

//-----Get high value---------------------------------------------------------
double iHigh(PChar Symbol, int TimeFrame, int index)
{
    return rec.iHigh(rec.piHigh, Symbol, TimeFrame, index);
}

//-----Get low value----------------------------------------------------------
double iLow(PChar Symbol, int TimeFrame, int index)
{
    return rec.iLow(rec.piLow, Symbol, TimeFrame, index);
}

//-----Get volume-------------------------------------------------------------
double iVolume(PChar Symbol, int TimeFrame, int index)
{
    return rec.iVolume(rec.piVolume, Symbol, TimeFrame, index);
}


//-----Get time of bar--------------------------------------------------------
TDateTime iTime(PChar Symbol, int TimeFrame, int index)
{
    return rec.iTime(rec.piTime, Symbol, TimeFrame, index);
}

//-----Get number of bars-----------------------------------------------------
int iBars(PChar Symbol, int TimeFrame)
{
    return rec.iBars(rec.piBars, Symbol, TimeFrame);
}

//-----Get open value in default currency and timeframe-----------------------
double Open(int index)
{
    return rec.Open(rec.pOpen, index);
}

//-----Get close value in default currency and timeframe----------------------
double Close(int index)
{
    return rec.Close(rec.pClose, index);
}

//-----Get high value in default currency and timeframe-----------------------
double High(int index)
{
    return rec.High(rec.pHigh, index);
}

//-----Get low value in default currency and timeframe------------------------
double Low(int index)
{
    return rec.Low(rec.pLow, index);
}

//-----Get volume in default currency and timeframe---------------------------
double Volume(int index)
{
    return rec.Volume(rec.pVolume, index);
}

//-----Get time of bar in default currency and timeframe----------------------
TDateTime Time(int index)
{
    return rec.Time(rec.pTime, index);
}

//-----Get number of bars in default currency and timeframe-------------------
int Bars()
{
    return rec.Bars(rec.pBars);
}

//-----Set currency and timeframe---------------------------------------------
bool SetCurrencyAndTimeframe(PChar Symbol, int TimeFrame)
{
    return rec.SetCurrencyAndTimeframe(rec.pSetCurrencyAndTimeframe, Symbol, TimeFrame);
}

//-----Get market information-------------------------------------------------
double MarketInfo(PChar Symbol, TMarketInfo _type)
{
    return rec.MarketInfo(rec.pMarketInfo, Symbol, _type);
}

//-----Send instant order-----------------------------------------------------
bool SendInstantOrder(PChar Symbol, TInstantOrderType OperationType, double LotSize,
    double StopLoss, double TakeProfit, PChar Comment, int MagicNumber, int& OrderHandle)
{
    return rec.SendInstantOrder2(rec.pSendInstantOrder2, Symbol, OperationType,
        LotSize, StopLoss, TakeProfit, Comment, MagicNumber, OrderHandle);
}

//-----Send pending order-----------------------------------------------------
bool SendPendingOrder(PChar Symbol, TPendingOrderType OperationType, double LotSize,
    double StopLoss, double TakeProfit, double ExecutionPrice,
    PChar Comment, int MagicNumber, int& OrderHandle)
{
    return rec.SendPendingOrder2(rec.pSendPendingOrder2, Symbol, OperationType,
        LotSize, StopLoss, TakeProfit, ExecutionPrice, Comment, MagicNumber, OrderHandle);
}

//-----Modify order-----------------------------------------------------------
bool ModifyOrder(int OrderHandle, double NewPrice, double StopLoss, double TakeProfit)
{
    return rec.ModifyOrder(rec.pModifyOrder, OrderHandle, NewPrice, StopLoss, TakeProfit);
}

//-----Delete order-----------------------------------------------------------
bool DeleteOrder(int OrderHandle)
{
    return rec.DeleteOrder(rec.pDeleteOrder, OrderHandle);
}

//-----Close order------------------------------------------------------------
bool CloseOrder(int OrderHandle)
{
    return rec.CloseOrder(rec.pCloseOrder, OrderHandle);
}

//-----Get order information--------------------------------------------------
bool GetOrderInfo(int OrderHandle, TTradePosition& info)
{
    return rec.GetOrderInfo(rec.pGetOrderInfo, OrderHandle, info);
}

//-----Select order-----------------------------------------------------------
bool OrderSelect(int index, TOrderSelectMode flags, TSearchMode pool)
{
    return rec.OrderSelect(rec.pOrderSelect, index, flags, pool);
}

//-----Get profit in dollars of selected order--------------------------------
double OrderProfit()
{
    return rec.OrderProfit(rec.pOrderProfit);
}

//-----Get profit in pips of selected order-----------------------------------
double OrderProfitPips()
{
    return rec.OrderProfitPips(rec.pOrderProfitPips);
}

//-----Check if order was closed----------------------------------------------
bool OrderClosed(int OrderHandle)
{
    return rec.OrderClosed(rec.pOrderClosed, OrderHandle);
}

//-----Number of closed positions---------------------------------------------
int HistoryTotal()
{
    return rec.HistoryTotal(rec.pHistoryTotal);
}

//-----Number of opened positions---------------------------------------------
int OrdersTotal()
{
    return rec.OrdersTotal(rec.pOrdersTotal);
}

//-----Open time of selected order--------------------------------------------
TDateTime OrderOpenTime()
{
    return rec.OrderOpenTime(rec.pOrderOpenTime);
}

//-----Close time of selected order-------------------------------------------
TDateTime OrderCloseTime()
{
    return rec.OrderCloseTime(rec.pOrderCloseTime);
}

//-----Lot size of selected order---------------------------------------------
double OrderLots()
{
    return rec.OrderLots(rec.pOrderLots);
}

//-----Handle of selected order-----------------------------------------------
int OrderTicket()
{
    return rec.OrderTicket(rec.pOrderTicket);
}

//-----Type of the selected order---------------------------------------------
TTradePositionType OrderType()
{
    return TTradePositionType(rec.OrderType(rec.pOrderType));
}

//-----Stop loss of selected order--------------------------------------------
double OrderStopLoss()
{
    return rec.OrderStopLoss(rec.pOrderStopLoss);
}

//-----Take profit of selected order------------------------------------------
double OrderTakeProfit()
{
    return rec.OrderTakeProfit(rec.pOrderTakeProfit);
}

//-----Open price of selected order-------------------------------------------
double OrderOpenPrice()
{
    return rec.OrderOpenPrice(rec.pOrderOpenPrice);
}

//-----Close price of selected order------------------------------------------
double OrderClosePrice()
{
    return rec.OrderClosePrice(rec.pOrderClosePrice);
}

//-----Currency of selected order---------------------------------------------
PChar OrderSymbol()
{
    return rec.OrderSymbol(rec.pOrderSymbol);
}

//-----Get stop loss in points------------------------------------------------
int GetStopLossPoints(int OrderHandle)
{
    int result = 0;

    if (OrderSelect(OrderHandle, SELECT_BY_TICKET, MODE_TRADES))
    {
        if (OrderStopLoss() == 0) return result;

        PCurrencyInfo pInfo;
        if (!GetCurrencyInfo(OrderSymbol(), pInfo)) return result;

        result = int((OrderOpenPrice() - OrderStopLoss()) / pInfo->Point);
        if (OrderType() == tp_Buy) result = -result;
    }

    return result;
}

//-----Get take profit points-------------------------------------------------
int GetTakeProfitPoints(int OrderHandle)
{
    int result = 0;

    if (OrderSelect(OrderHandle, SELECT_BY_TICKET, MODE_TRADES))
    {
        if (OrderTakeProfit() == 0) return result;

        PCurrencyInfo pInfo;
        if (!GetCurrencyInfo(OrderSymbol(), pInfo)) return result;

        result = int((OrderOpenPrice() - OrderTakeProfit()) / pInfo->Point);
        if (OrderType() == tp_Sell) result = -result;
    }

    return result;
}


//-----Set stop loss in points------------------------------------------------
void SetStopLossPoints(int OrderHandle, int NewStopLoss)
{
    if (OrderSelect(OrderHandle, SELECT_BY_TICKET, MODE_TRADES))
    {
        PCurrencyInfo pInfo;
        if (!GetCurrencyInfo(OrderSymbol(), pInfo)) return;

        switch (OrderType())
        {
        case tp_Buy:
            ModifyOrder(OrderHandle, OrderOpenPrice(),
                OrderOpenPrice() - NewStopLoss * pInfo->Point, OrderTakeProfit());
            break;

        case tp_Sell:
            ModifyOrder(OrderHandle, OrderOpenPrice(),
                OrderOpenPrice() + NewStopLoss * pInfo->Point, OrderTakeProfit());
            break;
        }
    }
}

//-----Set take profit in points----------------------------------------------
void SetTakeProfitPoints(int OrderHandle, int NewTakeProfit)
{
    if (OrderSelect(OrderHandle, SELECT_BY_TICKET, MODE_TRADES))
    {
        PCurrencyInfo pInfo;
        if (!GetCurrencyInfo(OrderSymbol(), pInfo)) return;

        switch (OrderType())
        {
        case tp_Buy:
            ModifyOrder(OrderHandle, OrderOpenPrice(),
                OrderOpenPrice() + NewTakeProfit * pInfo->Point, OrderTakeProfit());
            break;

        case tp_Sell:
            ModifyOrder(OrderHandle, OrderOpenPrice(),
                OrderOpenPrice() - NewTakeProfit * pInfo->Point, OrderTakeProfit());
            break;
        }
    }
}

//-----Get account balance----------------------------------------------------
double AccountBalance()
{
    if (rec.pAccountBalance == NULL) return 0;
    return rec.AccountBalance(rec.pAccountBalance);
}

//-----Get account equity-----------------------------------------------------
double AccountEquity()
{
    if (rec.pAccountEquity == NULL) return 0;
    return rec.AccountEquity(rec.pAccountEquity);
}

//-----Get account margin-----------------------------------------------------
double AccountMargin()
{
    if (rec.pAccountMargin == NULL) return 0;
    return rec.AccountMargin(rec.pAccountMargin);
}

//-----Get account free margin------------------------------------------------
double AccountFreeMargin()
{
    if (rec.pAccountFreeMargin == NULL) return 0;
    return rec.AccountFreeMargin(rec.pAccountFreeMargin);
}

//-----Get account leverage---------------------------------------------------
int AccountLeverage()
{
    if (rec.pAccountLeverage == NULL) return 0;
    return rec.AccountLeverage(rec.pAccountLeverage);
}

//-----Get account profit-----------------------------------------------------
double AccountProfit()
{
    if (rec.pAccountProfit == NULL) return 0;
    return rec.AccountProfit(rec.pAccountProfit);
}

//-----Breakpoint-------------------------------------------------------------
void Breakpoint(int number, PChar text)
{
    if (rec.pBreakpoint == NULL) return;
    rec.Breakpoint(rec.pBreakpoint, number, text);
}


//-----Create indicator and obtaind id----------------------------------------
int CreateIndicator(PChar Symbol, int TimeFrame, PChar IndicatorName, PChar parameters)
{
    if (IntrfProcsRec.pCreateIndicator == NULL) return 0;

    PChar IndicatorNameEx = (PChar)malloc(strlen(IndicatorName) + 4 + 1);
    strcpy_s(IndicatorNameEx, sizeof(IndicatorNameEx), IndicatorName);
    strcat_s(IndicatorNameEx, sizeof(IndicatorNameEx), ".dll");
    //strcpy(IndicatorNameEx, IndicatorName);
    //strcat(IndicatorNameEx, ".dll");

    return rec.CreateIndicator(rec.pCreateIndicator, Symbol, TimeFrame,
        IndicatorNameEx, parameters);

    free(IndicatorNameEx);
}

//-----Get indicator value----------------------------------------------------
double GetIndicatorValue(int IndicatorHandle, int index, int BufferIndex)
{
    if (rec.pGetIndicatorValue == NULL) return 0;
    return rec.GetIndicatorValue(rec.pGetIndicatorValue, IndicatorHandle, index, BufferIndex);
}

//-----Get order MagicNumber--------------------------------------------------
int OrderMagicNumber()
{
    if (rec.pOrderMagicNumber == NULL) return 0;
    return rec.OrderMagicNumber(rec.pOrderMagicNumber);
}

//-----Get order comment------------------------------------------------------
PChar OrderComment()
{
    if (rec.pOrderComment == NULL) return "";
    return rec.OrderComment(rec.pOrderComment);
}

//-----Order swap-------------------------------------------------------------
double OrderSwap()
{
    if (rec.pOrderSwap == NULL) return 0;
    return rec.OrderSwap(rec.pOrderSwap);
}

//-----Pause------------------------------------------------------------------
void Pause(PChar text)
{
    if (rec.pPause == NULL) return;
    rec.Pause(rec.pPause, text);
}

//-----Resume-----------------------------------------------------------------
void Resume()
{
    if (rec.pResume) return;
    rec.Resume(rec.pResume);
}


//-----Send instant order with selected price---------------------------------
bool _SendInstantOrder(PChar Symbol, TInstantOrderType OperationType, double price,
    double LotSize, double StopLoss, double TakeProfit, PChar Comment, int MagicNumber, int& OrderHandle)
{
    if (rec.p_SendInstantOrder == NULL)
    {
        OrderHandle = -1;
        return false;
    }

    return rec._SendInstantOrder(rec.p_SendInstantOrder, Symbol, OperationType,
        price, LotSize, StopLoss, TakeProfit, Comment, MagicNumber, OrderHandle);
}

//-----Close order with requested price---------------------------------------
bool _CloseOrder(int OrderHandle, double price)
{
    if (rec.p_CloseOrder == NULL) return false;
    return rec._CloseOrder(rec.p_CloseOrder, OrderHandle, price);
}

//-----Create object----------------------------------------------------------
bool ObjectCreate(PChar name, TObjectType ObjType, int window,
    TDateTime time1, double price1, TDateTime time2, double price2,
    TDateTime time3, double price3)
{
    if (rec.pObjectCreate == NULL) return false;
    return rec.ObjectCreate(rec.pObjectCreate, name, ObjType,
        window, time1, price1, time2, price2, time3, price3);
}

//-----Delete object by name--------------------------------------------------
void ObjectDelete(PChar name)
{
    if (rec.pObjectDelete == NULL) return;
    rec.ObjectDelete(rec.pObjectDelete, name);
}

//-----Check if object already exists-----------------------------------------
bool ObjectExists(PChar name)
{
    if (rec.pObjectExists == NULL) return false;
    return rec.ObjectExists(rec.pObjectExists, name);
}

//-----Get object type--------------------------------------------------------
TObjectType ObjectType(PChar name)
{
    if (rec.pObjectType == NULL) return obj_Text;
    return TObjectType(rec.ObjectType(rec.pObjectType, name));
}

//-----Set object property----------------------------------------------------
bool ObjectSet(PChar name, int index, double value)
{
    if (rec.pObjectSet == NULL) return false;
    return rec.ObjectSet(rec.pObjectSet, name, index, value);
}

//-----Get object property----------------------------------------------------
double ObjectGet(PChar name, int index)
{
    if (rec.pObjectGet == NULL) return 0;
    return rec.ObjectGet(rec.pObjectGet, name, index);
}

//-----Delete all objects-----------------------------------------------------
void ObjectsDeleteAll(int window, TObjectType ObjType)
{
    if (rec.pObjectsDeleteAll == NULL) return;
    rec.ObjectsDeleteAll(rec.pObjectsDeleteAll, window, ObjType);
}

//-----Get interface version--------------------------------------------------
void GetInterfaceVersion(int& MajorValue, int& MinorValue)
{
    if (rec.pGetInterfaceVersion == NULL)
    {
        MajorValue = 1;
        MinorValue = 4;
        return;
    }

    rec.GetInterfaceVersion(rec.pGetInterfaceVersion, MajorValue, MinorValue);
}

//-----Set text---------------------------------------------------------------
bool ObjectSetText(PChar name, PChar text, int FontSize, PChar FontName, TColor Color)
{
    if (rec.pObjectSetText == NULL) return false;
    return rec.ObjectSetText(rec.pObjectSetText, name, text, FontSize, FontName, Color);
}

//-----Get text---------------------------------------------------------------
PChar ObjectGetText(PChar name)
{
    if (rec.pObjectGetText == NULL) return NULL;
    return rec.ObjectGetText(rec.pObjectGetText, name);
}

//-----Get current time-------------------------------------------------------
TDateTime  TimeCurrent()
{
    if (rec.pTimeCurrent == NULL) return false;
    return rec.TimeCurrent(rec.pTimeCurrent);
}

//-----Set indicator's buffer style-------------------------------------------
void SetIndicatorBuffStyle(int IndicatorHandle, int BuffIndex, TPenStyle _style,
    int width, TColor color)
{
    if (rec.pSetIndicatorBuffStyle == NULL) return;
    rec.SetIndicatorBuffStyle(rec.pSetIndicatorBuffStyle, IndicatorHandle, BuffIndex, _style,
        width, color);
}

//-----Get bar shift by its time----------------------------------------------
int iBarShift(PChar Symbol, int TimeFrame, TDateTime time, bool Exact)
{
    if (rec.iBarShift == NULL) return -1;
    return rec.iBarShift(rec.piBarShift, Symbol, TimeFrame, time, Exact);
}

//-----Get highest value in array---------------------------------------------
int iHighest(PChar Symbol, int TimeFrame, int _type, int count, int index)
{
    if (rec.iHighest == NULL) return -1;
    return rec.iHighest(rec.piHighest, Symbol, TimeFrame, _type, count, index);
}

//-----Get lowest value in array----------------------------------------------
int iLowest(PChar Symbol, int TimeFrame, int _type, int count, int index)
{
    if (rec.iLowest == NULL) return -1;
    return rec.iLowest(rec.piLowest, Symbol, TimeFrame, _type, count, index);
}

//-----Close part of position-------------------------------------------------
bool CloseOrderPartial(int OrderHandle, double LotSize)
{
    if (rec.CloseOrderPartial == NULL) return false;
    return rec.CloseOrderPartial(rec.pCloseOrderPartial, OrderHandle, LotSize);
}


#undef rec
