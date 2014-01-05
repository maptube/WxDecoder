//AmdarData.h
#pragma once
#include "WxData.h"

#include <string>
#include <iostream>
#include <sstream>

using namespace std;

class CAmdarData : public CWxData {
public:
    int Day;
    int Hour;
    int Minute;
    string PhaseOfFlight;
    string AircraftIndicator;
    float AirTemperature;
    float DewPoint;
    float WindDirection;
    float WindSpeed;
    int TurbulenceCode;
    int FlightLevel;
public:
	//CAmdarData::CAmdarData() : CWxData() {};
    void CAmdarData::Invalidate()
    {
        //invlidate all the data to prevent fields not present persisting from previous decode
        CWxData::Invalidate();
        Day=InvalidInt;
        Hour=InvalidInt;
        Minute=InvalidInt;
        PhaseOfFlight="";
        AircraftIndicator="";
        AirTemperature=InvalidFloat;
        DewPoint=InvalidFloat;
        WindDirection=InvalidFloat;
        WindSpeed=InvalidFloat;
        TurbulenceCode=InvalidInt;
        FlightLevel=InvalidInt;
    };

    string ToCSV() {
        //write out members in CSV format
        //string result="";
		stringstream result (stringstream::in | stringstream::out);
        //day hour minute?
        result << ReportTime << ", ";
        result << PhaseOfFlight << ", ";
        result << AircraftIndicator << ", ";
		if (Lat!=CAmdarData::InvalidFloat) result << Lat << ", ";
		else result << "n/a, ";
        if (Lon!=CAmdarData::InvalidFloat) result << Lon << ", ";
		else result << "n/a, ";
		if (AirTemperature!=CAmdarData::InvalidFloat) result << AirTemperature << ", ";
		else result << "n/a, ";
        if (DewPoint!=CAmdarData::InvalidFloat) result << DewPoint << ", ";
		else result << "n/a, ";
        if (WindDirection!=CAmdarData::InvalidFloat) result << WindDirection << ", ";
		else result << "n/a, ";
        if (WindSpeed!=CAmdarData::InvalidFloat) result << WindSpeed << ", ";
		else result << "n/a, ";
        if (TurbulenceCode!=CAmdarData::InvalidInt) result << TurbulenceCode << ", ";
		else result << "n/a, ";
        if (FlightLevel!=CAmdarData::InvalidInt) result << FlightLevel << ", ";
		else result << "n/a, ";
        result << WholeReport;
        
        return result.str();
    }
};