//Synop Parser
#include "stdafx.h"
#include "MetUtils.h"
#include "SynopParser.h"
#include "TextUtils.h"

#include <math.h>

CSynopParser::CSynopParser(CStationList* StationList)
{
	this->StationList=StationList;
    this->m_SynopData=new CSynopData();
}

CSynopParser::~CSynopParser()
{
	delete this->m_SynopData;
}

bool CSynopParser::DecodeYYGGiw() {
//check that this works correctly in the UTC zone
    //YY Day of Month
    //GG Hour of Day
    //iw Indicator for source and units of windspeed. Code table 1855
    if (m_Head.length()==0) return false; //nothing left to process
    if (m_Head.length()!=5) return false;
    int YY=ReadInteger(m_Head,0,2,CSynopData::InvalidInt);
    int GG=ReadInteger(m_Head,2,2,CSynopData::InvalidInt);
    m_SynopData->iw=ReadInteger(m_Head,4,1,CSynopData::InvalidInt);
    //if (YY>0) calendar.set(Calendar.DAY_OF_MONTH,YY);
    //if (GG>0) calendar.set(Calendar.HOUR_OF_DAY,GG);
    struct tm tmReport;
	gmtime_s(&tmReport,&m_SynopData->ReportTime);
	if ((YY>0)||(GG>0)) {
		int Year=tmReport.tm_year;
		int Month=tmReport.tm_mon;
		int Day=tmReport.tm_mday;
		int Hour=tmReport.tm_hour;
        if (YY>0) Day=YY;
        if (GG>0) Hour=GG;
        m_SynopData->ReportTime = mktime(&tmReport);
    }
    GetNextToken();
    return true;
}

bool CSynopParser::DecodeIIiii() {
    //make this raise a synopformatexception
    //this doesn't handle 6 digit ones
    if (m_Head.length()<=0) return false; //nothing left to process
    m_SynopData->WMONumber=atoi(m_Head.c_str());
    if (m_SynopData->WMONumber<1000) m_SynopData->WMONumber+=3000; //if only three digits then assume 03xxx
	CStationDetails* Details=StationList->GetStation(m_SynopData->WMONumber);
	if (Details) {
		//System::Console::WriteLine("Found one!");
        m_SynopData->StationName=Details->StationName;
        m_SynopData->Lat=Details->Lat;
		m_SynopData->Lon=Details->Lon;
	}
	else {
		//Console::WriteLine("Station "+m_Head+" not found in master station list");
		//throw new SynopFormatException();
	}
    GetNextToken();
    return true;
}

bool CSynopParser::DecodeDDDD() {
	//SHIP callsign - place in station name field
    if (m_Head.length()<=0) return false; //nothing left to process
	m_SynopData->StationName=m_Head;
    GetNextToken();
    return true;
}

bool CSynopParser::Decode99LaLaLa() {
	//Latitude in tenths of a degree
    if (m_Head.length()<=0) return false; //nothing left to process
    if (m_Head.length()!=5) return false;
    if (m_Head.substr(0,2)!="99") return false;
	int ILat = ReadInteger(m_Head,2,3,CSynopData::InvalidInt);
	if (ILat!=CSynopData::InvalidInt) {
		m_SynopData->Lat=((float)ILat)/10.0f;
	}
	else m_SynopData->Lat=CSynopData::InvalidFloat;

    GetNextToken();
    return true;
}

bool CSynopParser::DecodeQcLoLoLoLo() {
	//Quadrant of the Globe and Longitude in tenths of a degree
    if (m_Head.length()<=0) return false; //nothing left to process
    if (m_Head.length()!=5) return false;
	m_SynopData->Quadrant=ReadInteger(m_Head,0,1,CSynopData::InvalidInt);
	
	int ILon = ReadInteger(m_Head,1,4,CSynopData::InvalidInt);
	if (ILon!=CSynopData::InvalidInt) {
		m_SynopData->Lon=((float)ILon)/10.0f;
	}
	else m_SynopData->Lon=CSynopData::InvalidFloat;

	//Code table 3333 - NOTE 99LaLaLa always comes first, so Lat already read as positive.
	//Use paranoid processing and ABS existing Lat/Lon just in case.
	switch (m_SynopData->Quadrant) {
		case 1: //North, East
			if (m_SynopData->Lat!=CSynopData::InvalidFloat)
				m_SynopData->Lat=abs(m_SynopData->Lat);
			if (m_SynopData->Lon!=CSynopData::InvalidFloat)
				m_SynopData->Lon=abs(m_SynopData->Lon);
			break;
		case 3: //South, East
			if (m_SynopData->Lat!=CSynopData::InvalidFloat)
				m_SynopData->Lat=-abs(m_SynopData->Lat);
			if (m_SynopData->Lon!=CSynopData::InvalidFloat)
				m_SynopData->Lon=abs(m_SynopData->Lon);
			break;
		case 5: //South, West
			if (m_SynopData->Lat!=CSynopData::InvalidFloat)
				m_SynopData->Lat=-abs(m_SynopData->Lat);
			if (m_SynopData->Lon!=CSynopData::InvalidFloat)
				m_SynopData->Lon=-abs(m_SynopData->Lon);
			break;
		case 7: //North, West
			if (m_SynopData->Lat!=CSynopData::InvalidFloat)
				m_SynopData->Lat=abs(m_SynopData->Lat);
			if (m_SynopData->Lon!=CSynopData::InvalidFloat)
				m_SynopData->Lon=-abs(m_SynopData->Lon);
			break;
		default: break;
	}

    GetNextToken();
    return true;
}

bool CSynopParser::DecodeMMMULAULO() {
//TODO:
    if (m_Head.length()<=0) return false; //nothing left to process
    if (m_Head.length()!=5) return false;
    GetNextToken();
    return true;
}

bool CSynopParser::Decodeh0h0h0h0im() {
//TODO:
    if (m_Head.length()<=0) return false; //nothing left to process
    if (m_Head.length()!=5) return false;
    GetNextToken();
    return true;
}

bool CSynopParser::DecodeNddff() {
    //N total cloud
    //dd wind direction - 99 indicates extension into 00fff group?
    //ff wind speed
    //wind speed units are given in iw
    if (m_Head.length()<=0) return false; //nothing left to process
    if (m_Head.length()!=5) return false;
    m_SynopData->TotalCloud=ReadInteger(m_Head,0,1,CSynopData::InvalidInt);
    m_SynopData->WindDirection=ReadInteger(m_Head,1,2,CSynopData::InvalidInt);
    if (m_SynopData->WindDirection!=CSynopData::InvalidInt) m_SynopData->WindDirection*=10;
    int ff=ReadInteger(m_Head,3,2,CSynopData::InvalidInt);
    if (ff==99) m_SynopData->WindSpeed=CSynopData::InvalidFloat; //was Float.MIN_VALUE
    else if (ff!=CSynopData::InvalidFloat) {
        //ix: (0=metres per second estimated) (1=metres per second measured)
        //    (3=knots estimated) (4=knots measured)
        if (m_SynopData->iw<3) m_SynopData->WindSpeed=(float)ff;
        else m_SynopData->WindSpeed=KnotsToMetresPerSec((float)ff);
    }
    else m_SynopData->WindSpeed=CSynopData::InvalidFloat; //Float.MIN_VALUE;
    
    GetNextToken();
    return true;
}

bool CSynopParser::Decode00fff() {
    //extension wind speed group - should check that previous ff=99
    if (m_Head.length()<=0) return false; //nothing left to process
    if (m_Head.length()!=5) return false;
    if (m_Head.substr(0,2)!="00") return false;
    int fff=ReadInteger(m_Head,2,3,CSynopData::InvalidInt);
    if (fff>CSynopData::InvalidInt) {
        //ix: (0=metres per second estimated) (1=metres per second measured)
        //    (3=knots estimated) (4=knots measured)
        if (m_SynopData->iw<3) m_SynopData->WindSpeed=(float)fff;
        else m_SynopData->WindSpeed=KnotsToMetresPerSec((float)fff);
    }
    else m_SynopData->WindSpeed=CSynopData::InvalidFloat; //Float.MIN_VALUE;
    
    GetNextToken();
    return true;
}

bool CSynopParser::DecodeiRixhVV() {
    //iR PPN data inclusion code table 1819
    //ix station type 123=manned, 4567=aws code table 1860
    //h cloud base height code table 1600
    //VV horizontal visibility
    if (m_Head.length()<=0) return false; //nothing left to process
    if (m_Head.length()!=5) return false;
    m_SynopData->StationType=ReadInteger(m_Head,1,1,CSynopData::InvalidInt); 
    m_SynopData->CloudBaseHeight=ReadInteger(m_Head,2,1,CSynopData::InvalidInt);
    GetNextToken();
    return true;
}

bool CSynopParser::Decode1snTTT() {
    //snTTT signed air temperature
    //if sn=0 then TTT +ve, else if sn=1 then TTT -ve
    if (m_Head.length()<=0) return false; //nothing left to process
    if (m_Head.length()!=5) return false;
    if (m_Head[0]!='1') return false;
    int sn=ReadInteger(m_Head,1,1,CSynopData::InvalidInt);
    if (sn==CSynopData::InvalidInt) {
        m_SynopData->AirTemperature=CSynopData::InvalidFloat;
        GetNextToken();
        return true;
    }
    if (sn>1) return false;
    
    int TTT=ReadInteger(m_Head,2,3,CSynopData::InvalidInt);
    if (TTT!=CSynopData::InvalidInt) {
        m_SynopData->AirTemperature=(float)TTT/10.0f;
        if (sn==1) m_SynopData->AirTemperature=-m_SynopData->AirTemperature;
    }
    else m_SynopData->AirTemperature=CSynopData::InvalidFloat;
    GetNextToken();
    return true;
}

bool CSynopParser::DecodeUUU() {
//TO DO: This is not decoded
    //29UUU relative humidity used where dew point is not available
    if (m_Head.length()<=0) return false; //nothing left to process
    if (m_Head.length()!=5) return false;
    if (m_Head.substr(0,2)!="29") return false;
    GetNextToken();
    return true;
}

bool CSynopParser::Decode2snTdTdTd() {
    //snTdTdTd signed dew point temperature
    //if sn=0,5 then TTT +ve, else if sn=1,2,6,7 then TTT -ve
    if (m_Head.length()<=0) return false; //nothing left to process
    if (m_Head.length()!=5) return false;
    if (m_Head.substr(0,2)!="2") return false;
    int sn=ReadInteger(m_Head,1,1,CSynopData::InvalidInt);
    if (sn==CSynopData::InvalidInt) {
        m_SynopData->DewPointTemperature=CSynopData::InvalidFloat;
        GetNextToken();
        return true;
    }
    if ((sn==3)||(sn==4)||(sn>=8)) return false;
    
    int TTT=ReadInteger(m_Head,2,3,CSynopData::InvalidInt);
    if (TTT==CSynopData::InvalidInt) {
        m_SynopData->DewPointTemperature=CSynopData::InvalidFloat;
        GetNextToken();
        return true;
    }
    
    if ((sn==0)||(sn==5)) {
        //positive
        m_SynopData->DewPointTemperature=(float)TTT/10.0f;
        GetNextToken();
        return true;
    }
    else if ((sn==1)||(sn==2)||(sn==6)||(sn==7)) {
        //negative
        m_SynopData->DewPointTemperature=-(float)TTT/10.0f;
        GetNextToken();
        return true;
    }
    
    return false;
}

bool CSynopParser::Decode3P0P0P0P0() {
    //station pressure
    if (m_Head.length()<=0) return false; //nothing left to process
    if (m_Head.length()!=5) return false;
    if (m_Head[0]!='3') return false;
    int PPPP=ReadInteger(m_Head,1,4,CSynopData::InvalidInt);
    if (PPPP!=CSynopData::InvalidInt) {
        m_SynopData->StationPressure=(float)PPPP/10.0f;
        if (m_SynopData->StationPressure<400) m_SynopData->StationPressure+=1000;
    }
    else m_SynopData->StationPressure=CSynopData::InvalidFloat;
    
    GetNextToken();
    return true;
}

bool CSynopParser::Decode4PPPP4a3hhh() {
//TODO: This doesn't process a3hhh correctly - is this inches of mercury?
    //PPPP or <a3><hhh>
    //MSL Pressure
    if (m_Head.length()<=0) return false; //nothing left to process
    if (m_Head.length()!=5) return false;
    if (m_Head[0]!='4') return false;
    int PPPP=ReadInteger(m_Head,1,4,CSynopData::InvalidInt);
    if (PPPP!=CSynopData::InvalidInt) {
        m_SynopData->MSLPressure=(float)PPPP/10.0f;
        if (m_SynopData->MSLPressure<400) m_SynopData->MSLPressure+=1000;
    }
    else m_SynopData->MSLPressure=CSynopData::InvalidFloat;
    
    GetNextToken();
    return true;
}

bool CSynopParser::Decode5appp() {
//TO DO: This is not decoded
    //<a><ppp> pressure tendency group
    if (m_Head.length()<=0) return false; //nothing left to process
    if (m_Head.length()!=5) return false;
    if (m_Head[0]!='5') return false;
    GetNextToken();
    return true;
}

bool CSynopParser::Decode6RRRtR() {
//TO DO: this is not decoded
    //<RR><tR> PPN amount group
    if (m_Head.length()<=0) return false; //nothing left to process
    if (m_Head.length()!=5) return false;
    if (m_Head[0]!='6') return false;
    GetNextToken();
    return true;
}

bool CSynopParser::Decode7wwW1W2() {
    //wwW1W2 significant weather manned station
    //wawawa1wa2 significant weather automatic station
    if (m_Head.length()<=0) return false; //nothing left to process
    if (m_Head.length()!=5) return false;
    if (m_Head[0]!='7') return false;
    m_SynopData->PresentWeather=ReadInteger(m_Head,1,2,CSynopData::InvalidInt);
    m_SynopData->PastWeather=ReadInteger(m_Head,3,2,CSynopData::InvalidInt);
    GetNextToken();
    return true;
}

bool CSynopParser::Decode8NhCLCMCH() {
    //cloud group
    if (m_Head.length()<=0) return false; //nothing left to process
    if (m_Head.length()!=5) return false;
    if (m_Head[0]!='8') return false;
    m_SynopData->CloudAmount=ReadInteger(m_Head,1,1,CSynopData::InvalidInt);
    m_SynopData->LowCloudType=ReadInteger(m_Head,2,1,CSynopData::InvalidInt);
    m_SynopData->MedCloudType=ReadInteger(m_Head,3,1,CSynopData::InvalidInt);
    m_SynopData->HighCloudType=ReadInteger(m_Head,4,1,CSynopData::InvalidInt);
    GetNextToken();
    return true;
}

bool CSynopParser::Decode9GGgg() {
//TODO: this is ignored
    //GGgg time
    if (m_Head.length()<=0) return false; //nothing left to process
    if (m_Head.length()!=5) return false;
    if (m_Head[0]!='9') return false;
    GetNextToken();
    return true;
}

//Section 333 decoders
bool CSynopParser::Decode0DotDotDot() {
//TODO: this is ignored
	if (m_Head.length()<=0) return false; //nothing left to process
    if (m_Head.length()!=5) return false;
    if (m_Head[0]!='0') return false;
    GetNextToken();
    return true;
}

bool CSynopParser::Decode1snTxTxTx() {
//TODO: this is ignored
	if (m_Head.length()<=0) return false; //nothing left to process
    if (m_Head.length()!=5) return false;
    if (m_Head[0]!='1') return false;
    GetNextToken();
    return true;
}

bool CSynopParser::Decode2snTnTnTn() {
//TODO: this is ignored
	if (m_Head.length()<=0) return false; //nothing left to process
    if (m_Head.length()!=5) return false;
    if (m_Head[0]!='2') return false;
    GetNextToken();
    return true;
}

bool CSynopParser::Decode3Ejjj() {
//TODO: this is ignored
	if (m_Head.length()<=0) return false; //nothing left to process
    if (m_Head.length()!=5) return false;
    if (m_Head[0]!='3') return false;
    GetNextToken();
    return true;
}

bool CSynopParser::Decode4Esss() {
	//Section 3 snow depth - really 4E'sss
	if (m_Head.length()<=0) return false; //nothing left to process
    if (m_Head.length()!=5) return false;
    if (m_Head[0]!='4') return false;
	m_SynopData->SnowStateOfGround=ReadInteger(m_Head,1,1,CSynopData::InvalidInt);
    m_SynopData->SnowTotalDepth=ReadInteger(m_Head,2,3,CSynopData::InvalidInt);
	GetNextToken();
	return true;
}

string CSynopParser::GetErrorText() {
    return "Synop Error: "+m_SynopData->WholeReport+" error at "+m_Head+" "+m_Tail;
}

bool CSynopParser::Parse(time_t BulletinDate,string& Synop) {
    //only pass AAXX/BBXX/OOXX part onwards - no TTAAii
    //Bulletin date is the date from the bulletin header. The time fields from
    //inside the synop are added to this to get the full date.

    try {
        m_SynopData->Invalidate();
        SetReport(Synop);
        m_SynopData->ReportTime=BulletinDate;
        m_SynopData->WholeReport = Synop; //do you need the header on this as well?

//section 0 - should strictly only check once per report
        if (to_upper(m_Head)=="AAXX") {
            m_SynopData->ReportType = AAXX;
            GetNextToken();
            //read AAXX header
            //YYGG day hour
            //iw
            DecodeYYGGiw();
        }
        else if (to_upper(m_Head)=="BBXX") {
            m_SynopData->ReportType = BBXX;
            GetNextToken();
        }
        else if (to_upper(m_Head)=="OOXX") {
            m_SynopData->ReportType = OOXX;
            GetNextToken();
        }
        
        switch (m_SynopData->ReportType) {
        case AAXX: //<II><iii> or <iii> or extra digit one
            DecodeIIiii();
            break;
        case BBXX: //<D....D> <YY><GG><iw> 99<LaLaLa> <Qc><LoLoLoLo>
			DecodeDDDD();
			DecodeYYGGiw();
			Decode99LaLaLa();
			DecodeQcLoLoLoLo();
          return true;
        case OOXX:

          //<D....D> <YY><GG><MinZero><iw>
          //99<LaLaLa>
          //<Qc><LoLoLoLo>
          //<MMM><ULA><ULO> <h0h0h0h0><im>
          //FIX: OOXX is ignored!
          return true;
        }
        
        if (m_Head=="NIL") return true;

//section 1
        //<iR><ix><h><VV>
        if (!DecodeiRixhVV())return false;
        //<N><dd><ff>( 00{///|<fff>})
        if (!DecodeNddff()) {
            if (!Decode00fff())return false;
        }
        
        Decode1snTTT(); //air temp
        
        //29<UUU> relative humidity or 2<snTdTdTd> dewpoint
        if (m_Head.substr(0,2)=="29") DecodeUUU();
        else Decode2snTdTdTd();
        
        Decode3P0P0P0P0();
        Decode4PPPP4a3hhh();
        Decode5appp();
        Decode6RRRtR();
        Decode7wwW1W2();
        Decode8NhCLCMCH();
        Decode9GGgg();

		//find the start of the 333 group
		while ((m_Head.length()>0)&&(m_Head!="333")) {
			GetNextToken();
		}
		GetNextToken(); //skip 333 token (or null if nothing left to process

		//decoding 333 group contents
		Decode0DotDotDot();
		Decode1snTxTxTx();
		Decode2snTnTnTn();
		Decode3Ejjj();
		Decode4Esss(); //4E'sss

        return true;
    }
    catch (exception ex) {
        //String^ exceptionType = ex->GetType()->ToString();
		//Console::WriteLine(ex->Message);
		//Console::WriteLine(ex->StackTrace);
		cout << ex.what() << endl;
    }
    return false;
}