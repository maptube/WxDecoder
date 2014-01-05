//MetarData.h
#pragma once
#include "WxData.h"

#include <string>
#include <ctime>
#include <sstream>

using namespace std;

class CMetarData : public CWxData {
public:
    string ICAO;
    string StationName;
    int Day;
    int Hour;
    int Minute;
    bool IsSpeci;
    bool IsAuto;
    float WindSpeed;
    float WindDirection;
    bool WindDirVRB;
    //static array<String^>^ Cloud={"","","",""}; //need gcnew for this
    string Cloud[4];
	float AirTemperature;
    float DewPoint;
    float SurfacePressure;
    string PresentWeather;
public:
    void CMetarData::Invalidate()
    {
        //invlidate all the data to prevent fields not present persisting from previous decode
        //don't reset iw, or time as they must persist from section 0 header
        CWxData::Invalidate();
        ICAO="";
        StationName="";
        Day=InvalidInt;
        Hour=InvalidInt;
        Minute=InvalidInt;
        IsSpeci=false;
        IsAuto=false;
        WindSpeed=InvalidFloat;
        WindDirection=InvalidFloat;
        bool WindDirVRB=false;
        //Cloud=Array::CreateInstance(String::GetType(),4);
        for (int i=0; i<4; i++) Cloud[i]="";
        AirTemperature=InvalidFloat;
        DewPoint=InvalidFloat;
        SurfacePressure=InvalidFloat;
        PresentWeather="";
    };

    string ToCSV() {
        //write out members in CSV format
        //string result="";
		stringstream result (stringstream::in | stringstream::out);
        if (IsSpeci) result << "SPECI, ";
		else result << "METAR, ";
        //day hour minute?
        result << ReportTime << ", ";
        result << ICAO << ", ";
        result << "\""+StationName+"\", ";
        if (Lat!=CMetarData::InvalidFloat) result << Lat << ", ";
		else result << "n/a, ";
        if (Lon!=CMetarData::InvalidFloat) result << Lon << ", ";
		else result << "n/a, ";
        if (IsAuto) result << "AUTO, ";
		else result << "MANUAL, ";
        if (WindSpeed!=CMetarData::InvalidFloat) result << WindSpeed << ", ";
		else result << "n/a, ";
        if (WindDirection!=CMetarData::InvalidFloat) result << WindDirection << ", ";
		else result << "n/a, ";
        if (WindDirVRB) result << "true, ";
		else result << "false, ";
//        result+=Cloud[0]+", "+Cloud[1]+", "+Cloud[2]+", "+Cloud[3]+", ";
        if (AirTemperature!=CMetarData::InvalidFloat) result << AirTemperature << ", ";
		else result << "n/a, ";
        if (DewPoint!=CMetarData::InvalidFloat) result << DewPoint << ", ";
		else result << "n/a, ";
        if (SurfacePressure!=CMetarData::InvalidFloat) result << SurfacePressure << ", ";
		else result << "n/a, ";
        result << PresentWeather << ", ";
        result << WholeReport;
        
        return result.str();
    }
};