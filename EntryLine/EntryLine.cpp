//---------------------------------------------------------------------------
// Ishimoku indicator
//---------------------------------------------------------------------------
#include "pch.h"
#include <windows.h>
#include "IndicatorInterfaceUnit.h"
#include "TechnicalFunctions.h"
#include <string.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <numeric>
#include <time.h>

using namespace std;

enum CsvHeaderIndex {
    chi_Symbol = 1,
    chi_Type = 2,
    chi_OpenTime = 4,
    chi_OpenPrice = 5,
    chi_SL = 6,
    chi_TP = 7,
    chi_CloseTime = 8,
    chi_ClosePrice = 9,
    chi_MaxEquity = 12,
    chi_MinEquity = 13,
};

enum EntryType {
    None = -1,
    Buy = 0,
    Sell = 1,
    Deposit = 2
};

struct Trade {
    string symbol;
    EntryType type = EntryType::None;
    string openTime;
    double openPrice = 0.0;
    double sl = 0.0;
    double tp = 0.0;
    string closeTime;
    double closePrice = 0.0;
    double maxEquity = 0.0;
    double minEquity = 0.0;
    string objName;
};

PChar filePath = NULL;
PChar comment = NULL;
PChar objNamePrefix = NULL;
int profitLineColor = 0xFF0000;
int lossLineColor = 0x0000FF;
bool isObjectCreated = false;
int drawType;
bool isOutputCsv = false;
enum DrawType {
    VerticalLine = 0,
    HorizontalLine = 1,
    Circle = 2
};
bool showStopLossLine = true;
bool showTakeProfitLine = true;
double labelValignmentPrice = 0.0;
int circleColor = clYellow;
int tpColor = clLime;
int slColor = clRed;
int timeDifference = 0;
int labelFontSize;

enum SlDirection {
    RightToLeft = 0,
    LeftToRight = 1
};
int slDirection = SlDirection::RightToLeft;

vector<Trade> trades;
double minAvarageAbs;
vector<string> objNames;

double datestring_to_double(string str);
bool load_csv(char* filePath);
string create_objname(string prefix, int index);
string create_vline(int objIndex, Trade trade);
string create_hline(int objIndex, Trade trade);
bool is_summertime(string date);
int day_of_week(int year, int month, int day);
enum WeekDay {
    Sun = 0,
    Mon = 1,
    Tue = 2,
    Wed = 3,
    Thu = 4,
    Fri = 5,
    Sat = 6
};

//---------------------------------------------------------------------------
// Initialize indicator
//---------------------------------------------------------------------------
EXPORT void __stdcall Init()
{
	IndicatorShortName("EntryLine");
	SetOutputWindow(ow_ChartWindow);

	AddSeparator("Common");
	RegOption("File Path", ot_PChar, &filePath);
    ReplaceStr(filePath, "C:\\Users\\User\\Desktop\\data.csv");

    //RegOption("Entry DrawType", ot_EnumType, &drawType);
    // AddOptionValue("Entry DrawType", "Vertical Line");
    // AddOptionValue("Entry DrawType", "Horizontal Line");
    //AddOptionValue("Entry DrawType", "Circle");
    drawType = DrawType::Circle;

    AddSeparator("Entry");
    RegOption("Circle Color", ot_Color, &circleColor);
    RegOption("Show TP Line", ot_Boolean, &showTakeProfitLine);
    RegOption("TP Color", ot_Color, &tpColor);
    RegOption("Show SL Line", ot_Boolean, &showStopLossLine);
    RegOption("SL Color", ot_Color, &slColor);

    AddSeparator("Label");
    RegOption("Label Vertical Alignment Price", ot_Double, &labelValignmentPrice);
    RegOption("Label FontSize", ot_Integer, &labelFontSize);
    labelFontSize = 10;

    AddSeparator("Option");
    RegOption("Comment", ot_PChar, &comment);
    ReplaceStr(comment, "");
    RegOption("ObjectName Prefix", ot_PChar, &objNamePrefix);
    ReplaceStr(objNamePrefix, "");
    RegOption("Time Difference", ot_Integer, &timeDifference);

    /*RegOption("Profit Line Color", ot_Color, &profitLineColor);
    RegOption("Loss Line Color", ot_Color, &lossLineColor);
    RegOption("SL Direction", ot_EnumType, &slDirection);
    AddOptionValue("SL Direction", "Right To Left");
    AddOptionValue("SL Direction", "Left To Right");
    RegOption("Output CsvFile", ot_Boolean, &isOutputCsv);*/

    objNames = {};
}

EXPORT void __stdcall Done()
{
    for (const string objName : objNames) {
        Print("EntryLine: delete objectname -> " + objName);
        char* s = const_cast<char*>(objName.c_str());
        ObjectDelete(s);
    }

    trades.clear();
    minAvarageAbs = 0.0;
    objNames.clear();

    isObjectCreated = false;
    Print("EntryLine: completed delete object.");
}

EXPORT void __stdcall OnParamsChange()
{
    Done();
    load_csv(filePath);
}

//---------------------------------------------------------------------------
// Calculate requested bar
//---------------------------------------------------------------------------
EXPORT void __stdcall Calculate(int index)
{
    if (!isObjectCreated) {
        int objNameIndex = 1;
        string prefix = "VLine";
        for (Trade trade : trades) {
            if (drawType == DrawType::VerticalLine) {
                // Vertical Line
                double se = datestring_to_double(trade.openTime);
                string objName = create_objname(prefix, objNameIndex);
                objNames.push_back(objName);
                char* s = const_cast<char*>(objName.c_str());
                ObjectCreate(s, obj_VLine, 0, se, 0);
                Print("EntryLine: objectname char: " + string(s));
                Print("EntryLine: create object -> " + objName + " / size: " + to_string(objName.size()));
                if (trade.maxEquity >= minAvarageAbs) {
                    ObjectSet(s, OBJPROP_COLOR, profitLineColor);
                }
                else {
                    ObjectSet(s, OBJPROP_COLOR, lossLineColor);
                }
            }
            else if (drawType == DrawType::HorizontalLine) {
                // Horizontal Line
                double se = datestring_to_double(trade.openTime);
                string objName = create_objname("EntryHline", objNameIndex);
                char* s = const_cast<char*>(objName.c_str());
                // 5éûä‘ï™ÇÃí∑Ç≥
                double lineLength = se + ((5.0 * 3600) / 86400);
                Print("EntryLine: tline lengh -> from: " + to_string(se) + " / to: " + to_string(lineLength));
                ObjectCreate(s, obj_TrendLine, 0, se, trade.openPrice, lineLength, trade.openPrice);
                if (trade.type == EntryType::Buy) {
                    ObjectSet(s, OBJPROP_COLOR, profitLineColor);
                }
                else if (trade.type == EntryType::Sell) {
                    ObjectSet(s, OBJPROP_COLOR, lossLineColor);
                }
                ObjectSet(s, OBJPROP_STYLE, TPenStyle::psDash);
                objNames.push_back(objName);
                Print("EntryLine: create object -> " + objName + " / size: " + to_string(objName.size()));

                if (showStopLossLine) {
                    string slObjName = create_objname("SLHline", objNameIndex);
                    char* s2 = const_cast<char*>(slObjName.c_str());
                    ObjectCreate(s2, obj_TrendLine, 0, se, trade.sl, lineLength, trade.sl);
                    ObjectSet(s2, OBJPROP_COLOR, slColor);
                    ObjectSet(s2, OBJPROP_STYLE, TPenStyle::psDashDotDot);
                    objNames.push_back(slObjName);
                    Print("EntryLine: create object -> " + slObjName + " / size: " + to_string(slObjName.size()));
                }

                if (showTakeProfitLine) {
                    string tpObjName = create_objname("TPHline", objNameIndex);
                    char* s3 = const_cast<char*>(tpObjName.c_str());
                    double diff = abs(trade.openPrice - trade.tp);
                    double tpPrice = 0.0;
                    if (trade.type == EntryType::Buy) {
                        tpPrice = trade.openPrice + diff;
                    }
                    else if (trade.type == EntryType::Sell) {
                        tpPrice = trade.openPrice - diff;
                    }

                    if (tpPrice > 0) {
                        ObjectCreate(s3, obj_TrendLine, 0, se, tpPrice, lineLength, tpPrice);
                        ObjectSet(s3, OBJPROP_COLOR, 0x00AA00);
                        ObjectSet(s3, OBJPROP_STYLE, TPenStyle::psDashDotDot);
                        objNames.push_back(tpObjName);
                        Print("EntryLine: create object -> " + tpObjName + " / size: " + to_string(tpObjName.size()));
                    }
                }
            }
            else if (drawType == DrawType::Circle) {
                // Horizontal Line
                double se = datestring_to_double(trade.openTime);
                if (timeDifference > 0) {
                    double td = (double)timeDifference;
                    double add = td * 3600.0 / 86400.0;
                    se += add;
                }
                string objName = create_objname("EntryCircle", objNameIndex);
                char* s = const_cast<char*>(objName.c_str());
                ObjectCreate(s, obj_Text, 0, se, trade.openPrice, 0, 0);
                char c[] = { 0xe2, 0x97, 0x8b, 0x00 };
                ObjectSetText(s, c, 12, "Meiryo UI", circleColor);
                ObjectSet(s, OBJPROP_VALIGNMENT, tlCenter);
                ObjectSet(s, OBJPROP_HALIGNMENT, taCenter);
                objNames.push_back(objName);
                Print("EntryLine: create object -> " + objName + " / size: " + to_string(objName.size()));

                string labelObjName = create_objname("EntryCircleLabel", objNameIndex);
                char* s3 = const_cast<char*>(labelObjName.c_str());
                double labelX = se + ((15.0 * 60.0) / 86400.0);
                double labelY = trade.openPrice + labelValignmentPrice;
                ObjectCreate(s3, obj_Text, 0, labelX, labelY, 0, 0);
                char* labelText = trade.type == EntryType::Buy ? " buy" : " sell";
                if (strlen(comment) > 0) {
                    int size = strlen(labelText) + strlen(comment) + 1;
                    char t[100] = {};
                    if (size > 100) {
                        ObjectSetText(s3, labelText, labelFontSize, "Meiryo UI", circleColor);
                    }
                    else {
                        strcat_s(t, size, labelText);
                        strcat_s(t, size, comment);
                        ObjectSetText(s3, t, labelFontSize, "Meiryo UI", circleColor);
                    }
                }
                else {
                    ObjectSetText(s3, labelText, labelFontSize, "Meiryo UI", clYellow);
                }
                ObjectSet(s3, OBJPROP_VALIGNMENT, tlCenter);
                ObjectSet(s3, OBJPROP_HALIGNMENT, taLeftJustify);
                objNames.push_back(labelObjName);

                if (showStopLossLine) {
                    string slObjName = create_objname("SLHline", objNameIndex);
                    char* s2 = const_cast<char*>(slObjName.c_str());
                    double lineLength = 0.0;
                    if (slDirection == SlDirection::RightToLeft) {
                        // 5éûä‘ï™ÇÃí∑Ç≥
                        lineLength = se - ((5.0 * 3600) / 86400);
                    }
                    else if (slDirection == SlDirection::LeftToRight) {
                        lineLength = se + ((5.0 * 3600) / 86400);
                    }
                    ObjectCreate(s2, obj_TrendLine, 0, se, trade.sl, lineLength, trade.sl);
                    ObjectSet(s2, OBJPROP_COLOR, slColor);
                    ObjectSet(s2, OBJPROP_STYLE, TPenStyle::psDashDotDot);
                    objNames.push_back(slObjName);
                    Print("EntryLine: create object -> " + slObjName + " / size: " + to_string(slObjName.size()));
                }

                if (showTakeProfitLine) {
                    string tpObjName = create_objname("TPHline", objNameIndex);
                    char* s3 = const_cast<char*>(tpObjName.c_str());
                    double lineLength = 0.0;
                    if (slDirection == SlDirection::RightToLeft) {
                        // 5éûä‘ï™ÇÃí∑Ç≥
                        lineLength = se - ((5.0 * 3600) / 86400);
                    }
                    else if (slDirection == SlDirection::LeftToRight) {
                        lineLength = se + ((5.0 * 3600) / 86400);
                    }
                    ObjectCreate(s3, obj_TrendLine, 0, se, trade.tp, lineLength, trade.tp);
                    ObjectSet(s3, OBJPROP_COLOR, tpColor);
                    ObjectSet(s3, OBJPROP_STYLE, TPenStyle::psDashDotDot);
                    objNames.push_back(tpObjName);
                    Print("EntryLine: create object -> " + tpObjName + " / size: " + to_string(tpObjName.size()));
                }
            }
            objNameIndex++;

            isObjectCreated = true;
        }
    }
}

double datestring_to_double(string str) {
    int year = atoi(str.substr(0, 4).c_str());
    int month = atoi(str.substr(5, 2).c_str());
    int day = atoi(str.substr(8, 2).c_str());
    double hour = std::stod(str.substr(11, 2));
    double minutes = std::stod(str.substr(14, 2));
    double seconds = std::stod(str.substr(17, 2));

    int base_year = 1900;
    int base_month = 1;
    int base_day = 1;

    // ÉVÉäÉAÉãílè¨êîïîï™ÇÃåvéZ
    double hour_seconds = hour * 3600;
    double minutes_seconds = minutes * 60;
    double sm = (hour_seconds + minutes_seconds + seconds) / 86400;
    // ÉVÉäÉAÉãílê≥êîïîï™ÇÃåvéZ
    double sda = day - base_day + 1;

    double sdb = 1;
    int np;
    for (np = base_month; np < month; np++) {
        if (np == 2) {
            if (year % 400 == 0) {
                sdb = sdb + 29;
            }
            else if (year % 100 == 0) {
                sdb = sdb + 28;
            }
            else if (year % 4 == 0) {
                sdb = sdb + 29;
            }
            else {
                sdb = sdb + 28;
            }
        }
        else if (np == 4 || np == 6 || np == 9 || np == 11) {
            sdb = sdb + 30;
        }
        else {
            sdb = sdb + 31;
        }
    }

    int sdc = 0;
    int yp = 0;
    for (yp = base_year; yp < year; yp++) {
        if (yp % 400 == 0) {
            sdc = sdc + 366;
        }
        else if (yp % 100 == 0) {
            sdc = sdc + 365;
        }
        else if (yp % 4 == 0) {
            sdc = sdc + 366;
        }
        else {
            sdc = sdc + 365;
        }
    }

    double sd = sda + sdb + sdc;

    double se = sm + sd;

    /*if (is_summertime(str)) {
        double ajust = 1.0 * 3600.0 / 86400.0;
        se -= ajust;
        Print("EntryLine: Today is SummerTime: " + to_string(se));
    }*/

    return se;
}

bool load_csv(char* path) {
    Print("EntryLine: load csvfilepath -> " + string(path)); 
    ifstream ifs(path, ios::in);
    if (!ifs) {
        Print("EntryLine: failure load csvfile.");
        return false;
    }

    string tmp;
    string str;

    int row = 0;
    vector<double> mins = {};
    int objNameIndex = 1;
    while (getline(ifs, tmp)) {
        if (row == 0) {
            row++;
            continue;
        }

        istringstream i_stream(tmp);
        int col = 0;
        Trade trade;
        while (getline(i_stream, str, ',')) {
            switch (col) {
                case chi_Symbol:
                    trade.symbol = str;
                case chi_Type:
                    if (str == "buy") {
                        trade.type = EntryType::Buy;
                    }
                    else if (str == "sell") {
                        trade.type = EntryType::Sell;
                    }
                    else if (str == "deposit") {
                        trade.type = EntryType::Deposit;
                    }
                    else {
                        trade.type = EntryType::None;
                    }
                    break;
                case chi_OpenTime:
                    Print("EntryLine: loaddata -> " + str);
                    trade.openTime = str;
                    break;
                case chi_OpenPrice:
                    trade.openPrice = stod(str);
                    break;
                case chi_SL:
                    trade.sl = stod(str);
                    break;
                case chi_TP:
                    trade.tp = stod(str);
                    break;
                case chi_CloseTime:
                    trade.closeTime = str;
                    break;
                case chi_ClosePrice:
                    trade.closePrice = stod(str);
                    break;
                case chi_MaxEquity:
                    trade.maxEquity = stod(str);
                    break;
                case chi_MinEquity:
                    double min = stod(str);
                    trade.minEquity = min;
                    mins.push_back(min);
                    break;
            }
            col++;
        }

        if (trade.type == EntryType::Buy || trade.type == EntryType::Sell) {
            trades.push_back(trade);
        }

        row++;
    }

    minAvarageAbs = (accumulate(mins.begin(), mins.end(), 0.0) / mins.size()) * -1;

    ifs.close();
    Print("EntryLine: completed load csvfile.");
}

string create_objname(string prefix, int index) {
    string prefixParam = string(objNamePrefix);
    if (!prefixParam.empty()) {
        prefix = prefixParam + prefix;
    }

    string objIndex = to_string(index);
    objIndex = string(max(0, 5 - objIndex.size()), '0') + objIndex;
    string objName = prefix + objIndex;
    return objName;
}

string create_vline(int objIndex, Trade trade) {
    double se = datestring_to_double(trade.openTime);
    string objName = create_objname("VLine", objIndex);
    char* s = const_cast<char*>(objName.c_str());
    ObjectCreate(s, obj_VLine, 0, se, 0);
    if (trade.maxEquity >= minAvarageAbs) {
        ObjectSet(s, OBJPROP_COLOR, profitLineColor);
    }
    else {
        ObjectSet(s, OBJPROP_COLOR, lossLineColor);
    }
}

string create_hline(int objIndex, Trade trade) {
    double se = datestring_to_double(trade.openTime);
    string objName = create_objname("EntryHline", objIndex);
    char* s = const_cast<char*>(objName.c_str());
    // 5éûä‘ï™ÇÃí∑Ç≥
    double lineLength = trade.openPrice + (5 * 3600);
    ObjectCreate(s, obj_TrendLine, 0, se, trade.openPrice, lineLength, trade.openPrice);
    if (trade.type == EntryType::Buy) {
        ObjectSet(s, OBJPROP_COLOR, profitLineColor);
    }
    else if (trade.type == EntryType::Sell){
        ObjectSet(s, OBJPROP_COLOR, lossLineColor);
    }
}

bool is_summertime(string date) {
    int month = atoi(date.substr(5, 2).c_str());
    if (month <= 2 || 12 <= month) {
        return false;
    }

    if (4 <= month && month <= 10) {
        return true;
    }

    int year = atoi(date.substr(0, 4).c_str());
    int day = atoi(date.substr(8, 2).c_str());
    double hour = std::stod(date.substr(11, 2));
    // USTÅF3åéëÊ2ì˙ójì˙AM2:00Å`11åéëÊ1ì˙ójì˙AM2:00
    if (month == 3) {
        int ustStartDay = 0;
        int sunCount = 0;
        for (int d = 1; d <= 31; d++) {
            int week = day_of_week(year, month, d);
            if (week == WeekDay::Sun) {
                sunCount++;
                if (sunCount == 2) {
                    ustStartDay = d;
                    break;
                }
            }
        }

        if (ustStartDay < day) {
            return true;
        }
        else if (ustStartDay == day && hour >= 2) {
            return true;
        }
        else {
            return false;
        }
    }
    else if (month == 11) {
        int ustEndDay = 0;
        int sunCount = 0;
        for (int d = 1; d <= 31; d++) {
            int week = day_of_week(year, month, d);
            if (week == WeekDay::Sun) {
                sunCount++;
                if (sunCount == 1) {
                    ustEndDay = d;
                    break;
                }
            }
        }

        if (ustEndDay > day) {
            return true;
        }
        else if (ustEndDay == day && hour <= 1) {
            return true;
        }
        else {
            return false;
        }
    }
    return false;
}

int day_of_week(int year, int month, int day) {
    if (month == 1 || month == 2) {
        year--;
        month += 12;
    }

    int week = (year + year / 4 - year / 100 + year / 400 +
        (13 * month + 8) / 5 + day) % 7;
    return week;
}
