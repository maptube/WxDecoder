//MetarParser.h
#pragma once
#include "ReportParser.h"
#include "StationList.h"
#include "MetarData.h"

#include <string>
#include <ctime>

using namespace std;

class CMetarParser : public CReportParser {
private:
	CStationList* StationList;
    CMetarData* m_MetarData;
public:
    CMetarParser(CStationList* StationList);
	~CMetarParser();
    string GetErrorText();
    bool Parse(time_t BulletinDate,string& ORCA,string& Metar);
    CMetarData* GetMetarData() { return m_MetarData; };
    string GetWholeReport() { return m_MetarData->WholeReport; };
private:
    bool IsCompassPoint(string& str);
    bool DecodeICAO();
    bool DecodeDateTime();
    bool DecodeWind();
    bool DecodeVariability();
    bool DecodeVisibility();
    bool DecodeRVR();
    bool DecodePresentWeather();
    bool DecodeCloud(int Number);
    bool DecodeTemperature();
    bool DecodeQNH();
    bool DecodeRecentWeather();
};