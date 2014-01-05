//StationList.h
#pragma once

#include <string>
#include <vector>
#include <hash_map>

using namespace std;

class CStationDetails {
public:
    int WMONumber;
    string StationName;
    string ICAO;
    float Lat,Lon;
    float Elevation;
};

class CStationList {
private:
	hash_map<int,CStationDetails> WMONumTable; //was Hashtable^ Table;
	hash_map<string,CStationDetails> ICAOTable;
    //static String^ MasterStationListFilename="MasterStationList.txt";
	string m_MasterStationListFilename;
    float ConvertLatitude(string& lat);
    float ConvertLongitude(string& lon);
public:
    CStationList(string& masterStationListFilename);
	~CStationList();
    CStationDetails* GetStation(const int WMONumber);
    CStationDetails* GetStation(string& ICAO);
    void WriteToDB();
};