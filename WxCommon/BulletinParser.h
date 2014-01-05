//BulletinParser.h
#pragma once
#include "StationList.h"
#include "SynopParser.h"
#include "MetarParser.h"
#include "AmdarParser.h"
#include "WxData.h"

#include <regex>
#include <string>
#include <ctime>
#include <iostream>
#include <fstream>
#include <hash_map>
#include <map>
#include <regex>

//using namespace System;
//using namespace System::IO;
//using namespace System::Data::Odbc;
//using namespace System::Text::RegularExpressions;

using namespace std;

class CBulletinParser {
private:
    static const char LineSeparator='\n';
    //StreamWriter^ m_ErrorFile;
	ofstream* m_ErrorFile;
	map<regex*,string> m_RoutingTable;
	const regex* HeaderExp;
	CSynopParser* SynopParser;
    CMetarParser* MetarParser;
    CAmdarParser* AmdarParser;
public:
    int m_Month,m_Year;
    string m_SeqNumber,m_TTAAii,m_Centre;
    int m_Day,m_Hour,m_Minute;
    string m_Orca; //original, retard, correction, amendment
    string m_CurrentPath,m_NoExtFilename;
	time_t m_BulletinDT;

private:
    //string RemoveWhitespace(string Report);
	string TTAAiiMatch(string& TTAAii);
    bool IsSequenceNumber(string& Line);
    void WriteError(string& Bulletin,string& ErrorText,string& WholeReport);
public:
	CBulletinParser(CStationList* StationList);
	~CBulletinParser();
    void ParseFile(string& Filename,list<CWxData*>& result,string& OutputDir);
    void WriteBulletin(string& Reports);
    void ParseBulletin(string& Bulletin,time_t FileCreationDT,list<CWxData*>& result);
	time_t ProbableDateTime(int Day,time_t FileCreationDT);
    string ParseGTSHeader(string& GTS,time_t FileCreationDT);
    void ParseReports(string& Bulletin,time_t FileCreationDT,list<CWxData*>& result);
	void WriteToDB(CWxData* report);
};
