#pragma once

#define PERIOD_MO1 43200

#pragma region Global Prameters
double OpenTime;
double OpenTimeM1;
double OpenTimeW1;
double OpenTimeD1;
double OpenTimeH4;
double OpenTimeH1;

void M1Setting();
void W1Setting();
void D1Setting();
void H4Setting();
void H1Setting();

bool CanLongEntry(double et, double tp);
bool CanShortEntry(double et, double tp);

void ShowStatus();

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
bool MesenD1ShowHline;
bool MesenH4ShowHline;
bool MesenH1ShowHline;

bool ResiSapoM1Filter;
bool ResiSapoW1Filter;
bool ResiSapoD1Filter;

int IkeIkeFilterType;
#pragma endregion

#pragma region Cafeaulait
int candlePeriod = 8;
bool isExplodedLong = false;
int candleCountLong = 0;
bool isExplodedShort = false;
int candleCountShort = 0;
#pragma endregion