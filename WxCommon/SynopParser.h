//SynopParser.h
#pragma once
#include <string>
#include <ctime>
#include "ReportParser.h"
#include "StationList.h"
#include "SynopData.h"

class CSynopParser : public CReportParser {
private:
	CStationList* StationList;
    CSynopData* m_SynopData;
public:
    CSynopParser(CStationList* StationList);
	~CSynopParser();
    string GetErrorText();
    bool Parse(time_t BulletinDate,string& Synop);
    CSynopData* GetSynopData() { return m_SynopData; };
    string GetWholeReport() { return m_SynopData->WholeReport; };
private:
    bool DecodeYYGGiw();
    bool DecodeIIiii();
    bool DecodeDDDD();
    bool Decode99LaLaLa();
    bool DecodeQcLoLoLoLo();
    bool DecodeMMMULAULO();
    bool Decodeh0h0h0h0im();
    bool DecodeNddff();
    bool Decode00fff();
    bool DecodeiRixhVV();
    bool Decode1snTTT();
    bool DecodeUUU();
    bool Decode2snTdTdTd();
    bool Decode3P0P0P0P0();
    bool Decode4PPPP4a3hhh();
    bool Decode5appp();
    bool Decode6RRRtR();
    bool Decode7wwW1W2();
    bool Decode8NhCLCMCH();
    bool Decode9GGgg();
	//section 333
	bool Decode0DotDotDot();
	bool Decode1snTxTxTx();
	bool Decode2snTnTnTn();
	bool Decode3Ejjj();
	bool Decode4Esss();
};