//CStationList.cpp

#include "stdafx.h"
#include "StationList.h"
#include "TextUtils.h"

#include <limits>
#include <iostream>
#include <fstream>
#include <string>
#include <regex>

using namespace std;

CStationList::CStationList(string& masterStationListFilename)
{
    //Build two tables, one indexed by WMO number and the other indexed by ICAO

    //Load the station details from a file into a memory map.
    //The format is (fixed positions on line):
    //NUMBER CALL   NAME + COUNTRY/STATE   LAT   LON    ELEV (METERS)
    //010010 ENJA   JAN MAYEN(NOR-NAVY) NO 7056N 00840W 0009
    //NOTE: if the elevation was negative, the above would be 00840W-0009

    m_MasterStationListFilename=masterStationListFilename;

	const std::regex Exp("^(\\d{6}|//////) (.{4})   (.{19}) (\\w{2}) (\\d{4}\\w{1}) (\\d{5}\\w{1})(.{5})?( )*$");

	char buf[2048]; //maximum line size
	ifstream in(m_MasterStationListFilename.c_str(),ifstream::in);
	if (in.is_open()) {
		while (in.getline(buf,sizeof(buf))) {
			string Line(buf);
			match_results<string::const_iterator> sm;
			if (regex_match(Line,sm,Exp,regex_constants::match_default)) {
				//Limit the table to only the 5 digit WMO codes e.g. convert 6 to 5 by dropping last 0
				string WMOString=sm[1].str();
				string ICAO=trim(sm[2].str());
				
				CStationDetails* Details=new CStationDetails();
				if (WMOString=="//////") Details->WMONumber=-1;
				else
					Details->WMONumber=atoi(sm[1].str().substr(0,5).c_str());
				Details->ICAO=ICAO;
				Details->StationName=trim(sm[3].str());
				//Group[4] is the country/state code e.g. NO=Norway
				Details->Lat=ConvertLatitude(sm[5].str());
				Details->Lon=ConvertLongitude(sm[6].str());
				string temp=trim(sm[7].str());
				if (temp.length()>0) Details->Elevation=(float)atof(temp.c_str());
				else Details->Elevation=-FLT_MIN;
					
				if ((Details->WMONumber!=-1)/*&&(*WMOString.rbegin()=='0')*/) //was this a UK hack?
					WMONumTable.insert(pair<int,CStationDetails>(Details->WMONumber,*Details));
				if (ICAO.length()>0)
					ICAOTable.insert(pair<string,CStationDetails>(Details->ICAO,*Details));
			}
		}
		in.close();
	}
	else
	{
		cout<<"Error: Station file "<<masterStationListFilename<<" not found"<<endl;
	}
}

CStationList::~CStationList()
{
	//TODO: need to delete CStation details in WMONumTable and ICAOTable
}

float CStationList::ConvertLatitude(string& lat)
{
    //convert 7056N into decimal 70 degrees 56 minutes north
    float deg=(float)atof(lat.substr(0,2).c_str());
    float min=(float)atof(lat.substr(2,2).c_str());
    float result=deg+min/60.0f;
	if (*lat.rbegin()=='S') result=-result;
    return result;
}

float CStationList::ConvertLongitude(string& lon)
{
    //convert 00840W into decimal 8 degrees and 40 minutes west
    float deg=(float)atof(lon.substr(0,3).c_str());
    float min=(float)atof(lon.substr(3,2).c_str());
    float result=deg+min/60.0f;
    if (*lon.rbegin()=='W') result=-result;
    return result;
}

CStationDetails* CStationList::GetStation(const int WMONumber)
{
	//if (Table->ContainsKey(WMONumber))
	//	return (CStationDetails^)Table[WMONumber];
	//string strWMONumber = std::to_string((long long)WMONumber);
	//return &WMONumTable[strWMONumber];
	
	return &WMONumTable[WMONumber];
}

CStationDetails* CStationList::GetStation(string& ICAO)
{
    return &ICAOTable[ICAO];
}

void CStationList::WriteToDB()
{
	/*
    //Write the information to the stations table of the oracle database
    //DSN-less
    //Driver={Microsoft ODBC for Oracle};Server=OracleServer.world;Uid=myUsername;Pwd=myPassword"
    String^ connectionString = "DSN=OracleXE;Uid=met;Pwd=met";
    String^ queryString = 
        "INSERT INTO STATIONS (WMO_NUMBER, ICAO, LATITUDE, LONGITUDE, STATION_NAME, ELEVATION)"
        +" VALUES(:wmo,:icao,:lat,:lon,:name,:ele)";
    OdbcCommand command(queryString);

    OdbcConnection^ connection;
    try {
        connection=gcnew OdbcConnection(connectionString);
        command.Connection = connection;
        connection->Open();
        command.Prepare();
        System::Collections::IDictionaryEnumerator^ en=Table->GetEnumerator();
        while (en->MoveNext())
        {
            CStationDetails^ stn=(CStationDetails^)en->Value;
            command.Parameters->Clear();
            command.Parameters->AddWithValue("wmo",stn->WMONumber);
            command.Parameters->AddWithValue("icao",stn->ICAO);
            command.Parameters->AddWithValue("lat",stn->Lat);
            command.Parameters->AddWithValue("lon",stn->Lon);
            command.Parameters->AddWithValue("name",stn->StationName);
            command.Parameters->AddWithValue("ele",stn->Elevation);
            command.ExecuteNonQuery();
        }
    }
    finally {
        connection->Close();
    }
	*/
}