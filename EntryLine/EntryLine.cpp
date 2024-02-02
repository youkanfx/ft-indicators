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
    EntryType type;
    string openTime;
    double openPrice;
    double sl;
    double tp;
    string closeTime;
    double closePrice;
    double maxEquity;
    double minEquity;
    string objName;
};

PChar filePath = NULL;
int profitLineColor = 0xFF0000;
int lossLineColor = 0x0000FF;
bool isObjectCreated = false;
int drawType;
bool isOutputCsv = false;
enum DrawType {
    VerticalLine = 0,
    HorizontalLine = 1
};

vector<Trade> trades;
double minAvarageAbs;
vector<string> objNames;

double datestring_to_double(string str);
bool load_csv(char* filePath);
string create_objname(string prefix, int index);
string create_vline(int objIndex, Trade trade);
string create_hline(int objIndex, Trade trade);
bool is_summertime(string date);

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

    RegOption("Profit Line Color", ot_Color, &profitLineColor);

    RegOption("Loss Line Color", ot_Color, &lossLineColor);

    RegOption("Entry DrawType", ot_EnumType, &drawType);
    AddOptionValue("Entry DrawType", "Vertical Line");
    AddOptionValue("Entry DrawType", "Horizontal Line");
    drawType = DrawType::VerticalLine;

    RegOption("Output CsvFile", ot_Boolean, &isOutputCsv);

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
            // Vertical Line
            /*double se = datestring_to_double(trade.openTime);
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
            }*/

            // Horizontal Line
            double se = datestring_to_double(trade.openTime);
            string objName = create_objname("EntryHline", objNameIndex);
            char* s = const_cast<char*>(objName.c_str());
            // 5時間分の長さ
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

            string slObjName = create_objname("SLHline", objNameIndex);
            char* s2 = const_cast<char*>(slObjName.c_str());
            ObjectCreate(s2, obj_TrendLine, 0, se, trade.sl, lineLength, trade.sl);
            ObjectSet(s2, OBJPROP_COLOR, 0x0000AA);
            ObjectSet(s2, OBJPROP_STYLE, TPenStyle::psDashDotDot);
            objNames.push_back(slObjName);

            string tpObjName = create_objname("TPHline", objNameIndex);
            char* s3 = const_cast<char*>(tpObjName.c_str());
            double diff = abs(trade.openPrice - trade.sl);
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
            }

            /*string objName;
            if (drawType == DrawType::VerticalLine) {
                objName = create_vline(objNameIndex, trade);
            }
            else if (drawType == DrawType::HorizontalLine) {
                objName = create_hline(objNameIndex, trade);
            }*/

            Print("EntryLine: create object -> " + objName + " / size: " + to_string(objName.size()));
            objNameIndex++;
        }

        isObjectCreated = true;
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

    // シリアル値小数部分の計算
    double hour_seconds = hour * 3600;
    double minutes_seconds = minutes * 60;
    double sm = (hour_seconds + minutes_seconds + seconds) / 86400;
    // シリアル値正数部分の計算
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
    // 5時間分の長さ
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
}
