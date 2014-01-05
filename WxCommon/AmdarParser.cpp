//Amdar Parser
#include "stdafx.h"
#include "AmdarParser.h"

#include <regex>

using namespace std;

CAmdarParser::CAmdarParser()
{
    this->m_AmdarData=new CAmdarData();
}

CAmdarParser::~CAmdarParser()
{
	delete m_AmdarData;
}

bool CAmdarParser::DecodeYYGG()
{
	const std::regex rx("(\\d{4})");
	if (regex_match(m_Head,rx)) {
        //read the date
		m_AmdarData->Day=atoi(m_Head.substr(0,2).c_str());
		m_AmdarData->Hour=atoi(m_Head.substr(2,2).c_str());
//        m_AmdarData->Minute=Convert::ToInt32(m_Head->Substring(4,2));
        //remember it's UTC
        //System.out.println("Date="+day+" "+hour+":"+minute+"Z");
        GetNextToken();
        return true;
    }
    return false;
}

bool CAmdarParser::DecodePhaseOfFlight()
{
    //ipipip
    //LVR=Level, LVW=Highest Wind in Level Flight, ASC=Ascending, DES=Descending, UNS=Unsteady phase
    if (m_Head=="LVL"||m_Head=="LVW"||m_Head=="ASC"||m_Head=="DES"||m_Head=="UNS")
    {
        m_AmdarData->PhaseOfFlight=m_Head;
        GetNextToken();
        return true;
    }
    return false;
}

bool CAmdarParser::DecodeAircraftIndicator()
{
    //IA...IA
    m_AmdarData->AircraftIndicator=m_Head;
    GetNextToken();
    return true;
}

bool CAmdarParser::DecodeLatitude()
{
    //LaLaLaLaA
    //Latitude in degrees and minutes, A=N or S
	if (m_Head.length()!=5) return false;
    int deg=ReadInteger(m_Head,0,2,CAmdarData::InvalidInt);
    int min=ReadInteger(m_Head,2,2,CAmdarData::InvalidInt);
    wchar_t ns=m_Head[4];
    if ((deg==CAmdarData::InvalidInt)||(min==CAmdarData::InvalidInt)) return false;
    if ((ns!='N')&&(ns!='S')) return false;
    float Lat=(float)deg+((float)min)/60.0f;
    if (ns=='S') Lat=-Lat;
    m_AmdarData->Lat=Lat;
    GetNextToken();
    return true;
}

bool CAmdarParser::DecodeLongitude()
{
    //LoLoLoLoLoB
    //Longitude in degrees and minutes, B=E or W
    if (m_Head.length()!=6) return false;
    int deg=ReadInteger(m_Head,0,3,CAmdarData::InvalidInt);
    int min=ReadInteger(m_Head,3,2,CAmdarData::InvalidInt);
    wchar_t ns=m_Head[5];
    if ((deg==CAmdarData::InvalidInt)||(min==CAmdarData::InvalidInt)) return false;
    if ((ns!='E')&&(ns!='W')) return false;
    float Lon=(float)deg+((float)min)/60.0f;
    if (ns=='E') Lon=-Lon;
    m_AmdarData->Lon=Lon;
    GetNextToken();
    return true;
}

bool CAmdarParser::DecodeYYGGgg()
{
	const std::regex rx("(\\d{6})");
    if (regex_match(m_Head,rx)) {
        //read the date
		m_AmdarData->Day=atoi(m_Head.substr(0,2).c_str());
		m_AmdarData->Hour=atoi(m_Head.substr(2,2).c_str());
		m_AmdarData->Minute=atoi(m_Head.substr(4,2).c_str());
        //remember it's UTC
        //System.out.println("Date="+day+" "+hour+":"+minute+"Z");
        GetNextToken();
        return true;
    }
    return false;
}

bool CAmdarParser::DecodePressureAltitude()
{
    //ShhIhIhI
    //Sh=F Pressure altitude zero or +ve (aircraft above 1013.2hPa)
    //Sh=A Pressure altitude -ve (aircraft below 1013.2hPa)
    //hIhIhI Pressure altitude in hundreds of feet relative to 1013.2hPa plane
    GetNextToken();
    return true;
}

bool CAmdarParser::DecodeAirTemperature()
{
    //SSTATATA
    //SS=PS or MS for positive or negative
    //TATATA=Air temp in 0.1 degrees at height hIhIhI
    if (m_Head.length()!=5) return false;
	string sign=m_Head.substr(0,2);
    if ((sign!="PS")&&(sign!="MS")) return false;
    int temp=ReadInteger(m_Head,2,3,CAmdarData::InvalidInt);
    if (temp==CAmdarData::InvalidInt) {
        m_AmdarData->AirTemperature=CAmdarData::InvalidFloat;
        GetNextToken();
        return true;
    }
    if (sign=="MS") temp=-temp;
    m_AmdarData->AirTemperature=((float)temp)/10.0f;
    GetNextToken();
    return true;
}

bool CAmdarParser::DecodeDewPoint()
{
    //SSTdTdTd
    //SS=PS or MS for positive or negative
    //TdTdTd=Dew point temp in 0.1 degrees
    if (m_Head.length()!=5) return false;
    string sign=m_Head.substr(0,2);
    if ((sign!="PS")&&(sign!="MS")) return false;
    int temp=ReadInteger(m_Head,2,3,CAmdarData::InvalidInt);
    if (temp==CAmdarData::InvalidInt) {
        m_AmdarData->DewPoint=CAmdarData::InvalidFloat;
        GetNextToken();
        return true;
    }
    if (sign=="MS") temp=-temp;
    m_AmdarData->DewPoint=((float)temp)/10.0f;
    GetNextToken();
    return true;
}

bool CAmdarParser::DecodeRelativeHumidity()
{
    //UUU
    //Relative humidity in percent
    if (m_Head.length()!=3) return false;
    GetNextToken();
    return true;
}

bool CAmdarParser::DecodeWind()
{
    //ddd/fff
    //ddd is degrees true to nearest 10
    //fff is in knots, measured at height level hIhIhI
    if (m_Head.length()!=7) return false;
    if (m_Head[3]!='/') return false;
    int winddir=ReadInteger(m_Head,0,3,CAmdarData::InvalidInt);
    int windspeed=ReadInteger(m_Head,4,3,CAmdarData::InvalidInt);
    if (winddir!=CAmdarData::InvalidInt) m_AmdarData->WindDirection=(float)winddir;
    if (windspeed!=CAmdarData::InvalidInt) m_AmdarData->WindSpeed=(float)windspeed;
    GetNextToken();
    return true;    
}

bool CAmdarParser::DecodeTurbulence()
{
    //TBBA
    //BA is turbulence code, table 0302: can be 0,1,2 or 3
	if (m_Head.length()!=3) return false;
    if (m_Head.substr(0,2)!="TB") return false;
    m_AmdarData->TurbulenceCode=ReadInteger(m_Head,2,1,CAmdarData::InvalidInt);
    GetNextToken();
    return true;
}

bool CAmdarParser::DecodeSystemData()
{
    //Ss1s2s3
    //s1=Navigation System, code 3866: 0=Inertial, 1=Omega
    //s2=Type of system (ACAR/ASDAR), code 3867
    //s3=Temperature Precision, 0=Low (2C), 1=High (1C)
    if (m_Head.length()!=4) return false;
    if (m_Head[0]!='S') return false;
    GetNextToken();
    return true;
}

bool CAmdarParser::DecodeFlightLevel()
{
    //Fhdhdhd
    //hdhdhd = flight level in hundreds of feet
    if (m_Head.length()!=4) return false;
    if (m_Head[0]!='F') return false;
    m_AmdarData->FlightLevel=ReadInteger(m_Head,1,3,CAmdarData::InvalidInt);
    GetNextToken();
    return true;
}

bool CAmdarParser::DecodeVerticalGust()
{
    //VGfgfgfg
    //fgfgfg = Maximum derived equivalent vertical gust in tenths of a metre per second
    if (m_Head.length()!=5) return false;
    if (m_Head.substr(0,2)=="VG") return false;
    GetNextToken();
    return true;
}

string CAmdarParser::GetErrorText() {
    return "Amdar Error: "+m_AmdarData->WholeReport+" error at "+m_Head+" "+m_Tail;
}

bool CAmdarParser::Parse(time_t BulletinDate,string ORCA,string Amdar) {
    try {
        m_AmdarData->Invalidate();
        SetReport(Amdar);
        m_AmdarData->ReportTime=BulletinDate;
        m_AmdarData->SetORCA(ORCA);
        m_AmdarData->WholeReport = Amdar; //do you need the header on this as well?

//AMDAR YYGG
//ipipip IA...IA LaLaLaLaA LoLoLoLoLoB YYGGgg ShhIhIhI
//SSTATATA SSTdTdTd|UUU ddd/fff TBBA Ss1s2s3
//333 Fhdhdhd VGfgfgfg

        if (m_Head=="AMDAR") {
            GetNextToken();
            DecodeYYGG();
        }
        DecodePhaseOfFlight();
        DecodeAircraftIndicator();
        DecodeLatitude();
        DecodeLongitude();
        DecodeYYGGgg();
        DecodePressureAltitude();
        DecodeAirTemperature();
        if (!DecodeDewPoint()) DecodeRelativeHumidity();
        DecodeWind();
        DecodeTurbulence();
        DecodeSystemData();
        
        if (m_Head!="333") return false;
        GetNextToken();
        DecodeFlightLevel();
        DecodeVerticalGust();
        return true;
    }
    catch (exception& e) {
        //String^ exceptionType = ex->GetType()->ToString();
        //Console::WriteLine(exceptionType);
		cout<<e.what()<<endl;
    }
    return false;
}
