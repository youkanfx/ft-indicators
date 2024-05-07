#pragma once
#include "BollingerBands.h"
#include "Cafeaulait.h"
#include "Stf.h"
#include "Mesen.h"
#include "PivotPoints.h"

using namespace Technical;

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

PivotPoints* ppD1;
PivotPoints* ppW1;
BollingerBands* bb15m;
Stf* stfD1;
Stf* stfH4;
Stf* stfH1;
Cafeaulait* upCafe;
Cafeaulait* lowCafe;
Mesen* meM1;
Mesen* meW1;
Mesen* meD1;
Mesen* meH4;
Mesen* meH1;

MesenDrawLine MesenM1DrawLine;
MesenDrawLine MesenW1DrawLine;
MesenDrawLine MesenD1DrawLine;
MesenDrawLine MesenH4DrawLine;
MesenDrawLine MesenH1DrawLine;

enum BBFilterType {
	None = 0,
	IkeIkeOnly = 1,
	NotIkeIkeOnly = 2,
};

enum EntryType {
	et_Auto,
	et_Manual
};

EntryType op_EntryType;
#pragma endregion

#pragma region Cafeaulait
int candlePeriod = 8;
bool isExplodedLong = false;
int candleCountLong = 0;
bool isExplodedShort = false;
int candleCountShort = 0;
#pragma endregion