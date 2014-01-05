//AmdarParser.h
#pragma once
#include "ReportParser.h"
#include "StationList.h"
#include "AmdarData.h"

#include <string>
#include <ctime>

using namespace std;

class CAmdarParser : public CReportParser {
private:
    CAmdarData* m_AmdarData;
public:
    CAmdarParser();
	~CAmdarParser();
    string GetErrorText();
    bool Parse(time_t BulletinDate,string ORCA,string Metar);
    CAmdarData* GetAmdarData() { return m_AmdarData; };
    string GetWholeReport() { return m_AmdarData->WholeReport; };
private:
    bool DecodeYYGG();
    bool DecodePhaseOfFlight();
    bool DecodeAircraftIndicator();
    bool DecodeLatitude();
    bool DecodeLongitude();
    bool DecodeYYGGgg();
    bool DecodePressureAltitude();
    bool DecodeAirTemperature();
    bool DecodeDewPoint();
    bool DecodeRelativeHumidity();
    bool DecodeWind();
    bool DecodeTurbulence();
    bool DecodeSystemData();
    bool DecodeFlightLevel();
    bool DecodeVerticalGust();
};