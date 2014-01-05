//Metar Parser
#include "stdafx.h"
#include "MetarParser.h"

#include <iostream>
#include <string>
#include <ctime>
#include <regex>

using namespace std;

CMetarParser::CMetarParser(CStationList* StationList)
{
	this->StationList=StationList;
    this->m_MetarData=new CMetarData();
}

CMetarParser::~CMetarParser()
{
	delete m_MetarData;
}

bool CMetarParser::IsCompassPoint(string& str)
{
    //return true if str is one of N,S,E,W,NE,SE,SW,NW
    const string point4("NSEW"), point8("NESESWNW");
	if (str.length()==1) return (point4.find_first_of(str)>=0);
    if (str.length()==2) {
		size_t pos=point8.find_first_of(str);
        return ((pos>=0)&&(pos%2==0));
    }
    return false;
}

bool CMetarParser::DecodeICAO()
{
    if (m_Head.length()==4) {
        m_MetarData->ICAO = m_Head;
        CStationDetails* Details=StationList->GetStation(m_MetarData->ICAO);
        if (Details) {
            m_MetarData->StationName=Details->StationName;
            m_MetarData->Lat=Details->Lat;
            m_MetarData->Lon=Details->Lon;
        }
        GetNextToken();
        return true;
    }
    return false;
}

bool CMetarParser::DecodeDateTime()
{
    //was token.matches("\\d\\d\\d\\d\\d\\dZ")
	const std::regex rx("(\\d{6})Z");
	if (regex_match(m_Head,rx)) {
        //read the date
		m_MetarData->Day=atoi(m_Head.substr(0,2).c_str());
		m_MetarData->Hour=atoi(m_Head.substr(2,2).c_str());
		m_MetarData->Minute=atoi(m_Head.substr(4,2).c_str());
        //remember it's UTC
        //System.out.println("Date="+day+" "+hour+":"+minute+"Z");
        GetNextToken();
        return true;
    }
    return false;
}

bool CMetarParser::DecodeWind()
{
	const std::regex rx("(\\d\\d\\d)(\\d\\d)KT");
    if (regex_match(m_Head,rx)) {
		m_MetarData->WindDirection=(float)atof(m_Head.substr(0,3).c_str());
        m_MetarData->WindSpeed=(float)atof(m_Head.substr(3,2).c_str());
        m_MetarData->WindDirVRB=false;
        GetNextToken();
        return true;
    }
    else {
		const std::regex rx("(\\d\\d\\d)(\\d\\d)G(\\d\\d)KT");
        if (regex_match(m_Head,rx)) {
            m_MetarData->WindDirection=(float)atof(m_Head.substr(0,3).c_str());
            m_MetarData->WindSpeed=(float)atof(m_Head.substr(3,2).c_str());
            m_MetarData->WindDirVRB=false;
//            m_MetarData->Gust=Convert::ToSingle(m_Head->Substring(6,2));
            GetNextToken();
            return true;
        }
        else {
			const std::regex rx("VRB(\\d\\d)KT");
            if (regex_match(m_Head,rx)) {
                m_MetarData->WindSpeed=(float)atof(m_Head.substr(3,2).c_str());
                m_MetarData->WindDirVRB=false;
                GetNextToken();
                return true;
            }
        }
    }
    return false;

/*    if (token.matches("\\d\\d\\d\\d\\dKT")
        ||token.matches("\\d\\d\\d\\d\\dG\\d\\dKT")
        ||token.matches("VRB\\d\\dKT")) {
            if (token.startsWith("VRB")) {
                windDirVRB=true;
            }
            else {
                windDirection=Integer.parseInt(token.substring(0,3));
                validWindDirection=true;
                windDirVRB=false;
            }
            windSpeed=Integer.parseInt(token.substring(3,5));
            validWindSpeed=true;
            //should probably decode the gust here, but currently it is ignored
            
            if (!windDirVRB) System.out.println("WindDirection="+windDirection);
            else System.out.println("WindDirection=VRB");
            System.out.println("Windpeed="+windSpeed);
            readSoFar+=" "+token;
            return true;
    }
    return false;
*/
}

bool CMetarParser::DecodeVariability()
{
	const std::regex rx("\\d\\d\\dV\\d\\d\\d");
    if (regex_match(m_Head,rx)) {
        //read wind variability here
        //System.out.println("Wind Variability="+token);
        GetNextToken();
        return true;
    }
    return false;
}

bool CMetarParser::DecodeVisibility()
{
    if (m_Head=="CAVOK") {
        //set CAVOK flag here
        //System.out.println("Visibility="+token);
        GetNextToken();
        return true;
    }

	const std::regex rx("\\d\\dSM");
	const std::regex rx4d("\\d\\d\\d\\d");
    string dist="", dir="";
    if (regex_match(m_Head,rx)) {
        //ignore American/Canadian statute miles visibility
        //This is incomplete - should process 1/2SM 1/8SM etc
        GetNextToken();
        return true;
    }
    else if ((m_Head.length()==4)&&(regex_match(m_Head,rx4d))) {
        dist=m_Head.substr(0,4);
    }
    else if (m_Head.length()>4) {
        dist=m_Head.substr(0,4);
        dir=m_Head.substr(4);
    }
    if (regex_match(dist,rx4d)&&((dir.length()==0)||IsCompassPoint(dir)))
    {
        //read the horizontal visibility here
        //System.out.println("Visibility="+token);
        GetNextToken();
        return true;
    }
    return false;
}

bool CMetarParser::DecodeRVR()
{
	const std::regex rx("R\\d\\d[LR]?/\\d\\d\\d\\d[UDN]?");
    //CAN ALSO HAVE P1500 ?
    if (regex_match(m_Head,rx))
    {
        //read rvr information here
        //System.out.println("RunwayVisualRange="+token);
        GetNextToken();
        return true;
    }
    return false;
}

bool CMetarParser::DecodePresentWeather()
{
    //+,-,VC or empty
    //MI,PR,BC,DR,BL,SH,TS,FZ
    //DZ,RA,SN,SG,IC,PL,GR,GS,UP
    //BR,FG,FU,VA,DU,SA,HZ,PY
    //PO,SQ,FC,SS,DS
    const string wx="VCMIPRBCDRBLSHTSFZDZRASNSGICPLGRGSUPBRFGFUVADUSAHZPYPOSQFCSSDS";
    if (m_Head.length()==0) return false;
	string temp=m_Head;
    if ( (temp[0]=='+') || (temp[0]=='-') ) temp=temp.substr(1);
    while (temp.length()>=2) {
		int pos=wx.find_first_of(temp.substr(0,2));
        if ((pos==string::npos)||(pos%2!=0)) return false;
        temp=temp.substr(2);
    }
    //all valid present weather two digit codes, so this must be the present weather group
    
    m_MetarData->PresentWeather=m_Head;
    GetNextToken();
    return true;
}

bool CMetarParser::DecodeCloud(int Number)
{
    if (m_Head.length()<6) return false;
    
    string group=m_Head.substr(0,6);
    //THIS CAN HAVE TCU or CB AFTER IT!
    if ((regex_match(group,std::regex("FEW\\d\\d\\d") ))
        ||(regex_match(group,std::regex("SCT\\d\\d\\d") ))
        ||(regex_match(group,std::regex("BKN\\d\\d\\d") ))
        ||(regex_match(group,std::regex("OVC\\d\\d\\d") )))
    {
        m_MetarData->Cloud[Number]=m_Head;
        GetNextToken();
        return true;
    }
    return false;
}

bool CMetarParser::DecodeTemperature()
{
	const std::regex rx("M?\\d\\d/M?\\d\\d");
    if (regex_match(m_Head,rx)) {
		int pos=m_Head.find_first_of("/");
        string strAirTemp,strDewPoint;
        strAirTemp=m_Head.substr(0,pos);
        strDewPoint=m_Head.substr(pos+1);
        
		if (strAirTemp[0]=='M') m_MetarData->AirTemperature=-(float)atof(strAirTemp.substr(1,2).c_str());
		else m_MetarData->AirTemperature=(float)atof(strAirTemp.substr(0,2).c_str());
		if (strDewPoint[0]=='M') m_MetarData->DewPoint=-(float)atof(strDewPoint.substr(1,2).c_str());
		else m_MetarData->DewPoint=(float)atof(strDewPoint.substr(0,2).c_str());
        GetNextToken();
        return true;
    }
    return false;
}

bool CMetarParser::DecodeQNH()
{
	const std::regex rxQ("Q\\d\\d\\d\\d");
	const std::regex rxA("A\\d\\d\\d\\d");
    if (regex_match(m_Head,rxQ)) {
		m_MetarData->SurfacePressure=(float)atof(m_Head.substr(1,4).c_str());
        GetNextToken();
        return true;
    }
    else if (regex_match(m_Head,rxA)) {
        //and handle American reports in inches of mercury A#### ?
		float inches=(float)atof(m_Head.substr(1,4).c_str())/100.0f;
        //At 32 Fahrenheit: (inHg32) * 33.8639 = (mb)
        //At 60 Fahrenheit: (inHg60) * 33.7685 = (mb)
        m_MetarData->SurfacePressure=inches*33.8639f; //this is at 32F
        GetNextToken();
        return true;
    }
    return false;
}

bool CMetarParser::DecodeRecentWeather()
{
    if (m_Head.substr(0,2)=="RE") {
        //read the recent weather here
        GetNextToken();
        return true;
    }
    return false;
}

string CMetarParser::GetErrorText() {
    return "Metar Error: "+m_MetarData->WholeReport+" error at "+m_Head+" "+m_Tail;
}

bool CMetarParser::Parse(time_t BulletinDate,string& ORCA,string& Metar) {
    try {
        m_MetarData->Invalidate();
        SetReport(Metar);
        m_MetarData->ReportTime=BulletinDate;
        m_MetarData->SetORCA(ORCA);
        m_MetarData->WholeReport = Metar; //do you need the header on this as well?
        
        if (m_Head=="METAR") GetNextToken();
        else if (m_Head=="SPECI") {
            m_MetarData->IsSpeci=true;
            GetNextToken();
        }
        DecodeICAO();
        DecodeDateTime();
        if (m_Head=="NIL") return true; //terminate processing of nil report

        if (m_Head=="AUTO") {
            m_MetarData->IsAuto=true;
            GetNextToken();
        }
        DecodeWind();
        DecodeVariability();
        DecodeVisibility();
        while (DecodeRVR()) {};
        DecodePresentWeather();
        for (int i=0; i<4; i++) if (!DecodeCloud(i)) break;
        DecodeTemperature();
        DecodeQNH();
        DecodeRecentWeather();
        //ignore RMK section
        return true;
    }
    catch (exception ex) {
        //String^ exceptionType = ex->GetType()->ToString();
        //Console::WriteLine(exceptionType);
		cout<<ex.what()<<endl;
    }
    return false;
}
