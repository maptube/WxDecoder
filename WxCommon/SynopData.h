//SynopData.h
#pragma once
#include "WxData.h"

#include <string>
#include <iostream>
#include <sstream>

using namespace std;

enum CSynopReportType { Unknown, AAXX, BBXX, OOXX };

class CSynopData : public CWxData {
public:
	//static string sqlTableName = "SYNOP";
	//static string orcaSQLString =
	//	"SELECT \"ORCA\" FROM \"SYNOP\" WHERE \"REPORT_TIME\"=? AND \"WMO_NUMBER\"=?";
	//static string deleteSQLString =
	//	"DELETE FROM \"SYNOP\" WHERE \"REPORT_TIME\"=? AND \"WMO_NUMBER\"=?";
    //static string insertSQLString = 
    //    "INSERT INTO \"SYNOP\" (\"ORCA\", \"REPORT_TIME\", \"LATITUDE\", \"LONGITUDE\","
	//	+" \"WMO_NUMBER\", \"STATION_NAME\","
    //    +" \"STATION_TYPE\", \"CLD_BASE_HT\", \"CLD_TTL_AMT_ID\","
    //    +" \"WIND_DIRECTION\", \"WIND_SPEED\", \"AIR_TEMPERATURE\", \"DEWPOINT\", \"STN_PRESSURE\","
    //    +" \"MSL_PRESSURE\", \"PRST_WX_ID\", \"PAST_WX_ID\", \"CLOUD_AMOUNT\", \"LOW_CLOUD_TYPE\", "
    //    +" \"MED_CLOUD_TYPE\", \"HIGH_CLOUD_TYPE\","
	//	+" \"SNOW_STATE_OF_GROUND\", \"SNOW_TOTAL_DEPTH\","
    //    +" \"WHOLE_REPORT\")"
    //    +" VALUES(?,?,?,?,"
	//	+" ?,?,"
    //    +" ?,?,?,"
    //    +" ?,?,?,?,?,"
    //    +" ?,?,?,?,?,"
	//	+" ?,?,"
	//	+" ?,?,"
    //    +" ?)";
public:
    CSynopReportType ReportType;
    int iw;
	int Quadrant; //Qc Quadrant of Globe Code Table 3333
    int WMONumber;
    string StationName;
    int StationType;
    int TotalCloud;
    int CloudBaseHeight;
    int WindDirection;
    float WindSpeed; //in metres per second
    float AirTemperature;
    float DewPointTemperature;
    float StationPressure;
    float MSLPressure;
    int PresentWeather;
    int PastWeather;
    int CloudAmount; //<Nh>
    int LowCloudType; //<CL>
    int MedCloudType; //<CM>
    int HighCloudType; //<CH>
	//section 333
	int SnowStateOfGround; //E' code table 0975
	int SnowTotalDepth; //sss code table 3889
public:
    void CSynopData::Invalidate()
    {
        //invlidate all the data to prevent fields not present persisting from previous decode
        //don't reset iw, or time as they must persist from section 0 header
        CWxData::Invalidate();
        WMONumber=InvalidInt;
        StationName="";
        StationType=InvalidInt;
        TotalCloud=InvalidInt;
        CloudBaseHeight=InvalidInt;
        WindDirection=InvalidInt;
        WindSpeed=InvalidFloat;
        AirTemperature=InvalidFloat;
        DewPointTemperature=InvalidFloat;
        StationPressure=InvalidFloat;
        MSLPressure=InvalidFloat;
        PresentWeather=InvalidInt;
        PastWeather=InvalidInt;
        CloudAmount=InvalidInt;
        LowCloudType=InvalidInt;
        MedCloudType=InvalidInt;
        HighCloudType=InvalidInt;
		SnowStateOfGround=InvalidInt;
		SnowTotalDepth=InvalidInt;
    };

    string ToCSV() {
        //write out members in CSV format
        //string result="";
		stringstream result (stringstream::in | stringstream::out);
        switch (ReportType) {
            case AAXX: result << "SYNOP, "; break;
            case BBXX: result << "SHIP, "; break;
            case OOXX: result << "SYNOP MOBILE, "; break;
            default: result << "UNKNOWN TYPE, "; break;
        }
        result << ReportTime << ", ";
        if (ReportType==AAXX) result << WMONumber << ", ";
		else result << "n/a, ";
        result << "\"" << StationName << "\", ";
        if (Lat!=CSynopData::InvalidFloat) result << Lat << ", ";
		else result << "n/a, ";
        if (Lon!=CSynopData::InvalidFloat) result << Lon << ", ";
		else result << "n/a, ";
        if (StationType!=CSynopData::InvalidInt) result << StationType << ", ";
		else result << "n/a, ";
        if (TotalCloud!=CSynopData::InvalidInt) result << TotalCloud << ", oktas, ";
		else result << "n/a, oktas, ";
        switch (CloudBaseHeight) {
            case 0: result<<CloudBaseHeight<<", 0 to 50m, "; break;
            case 1: result<<CloudBaseHeight<<", 50 to 100m, "; break;
            case 2: result<<CloudBaseHeight<<", 100 to 200m, "; break;
            case 3: result<<CloudBaseHeight<<", 200 to 300m, "; break;
            case 4: result<<CloudBaseHeight<<", 300 to 600m, "; break;
            case 5: result<<CloudBaseHeight<<", 600 to 1000m, "; break;
            case 6: result<<CloudBaseHeight<<", 1000 to 1500m, "; break;
            case 7: result<<CloudBaseHeight<<", 1500 to 2000m, "; break;
            case 8: result<<CloudBaseHeight<<", 2000 to 2500m, "; break;
            case 9: result<<CloudBaseHeight<<", above 2500m, "; break;
            default:
                if (CloudBaseHeight==CSynopData::InvalidInt) result<<"n/a, n/a, ";
                else result<<CloudBaseHeight<<", unknown, ";
                break;
        }
        if (WindDirection!=CSynopData::InvalidInt) result << WindDirection << ", deg, ";
		else result << "n/a, deg, ";
        if (WindSpeed!=CSynopData::InvalidFloat) result << WindSpeed << ", ms-1, ";
		else result << "n/a, ms-1, ";
		if (AirTemperature!=CSynopData::InvalidFloat) result << AirTemperature << ", C, ";
		else result << "n/a, C, ";
        if (DewPointTemperature!=CSynopData::InvalidFloat) result << DewPointTemperature << ", C, ";
		else result << "n/a, C, ";
        if (StationPressure!=CSynopData::InvalidFloat) result << StationPressure << ", hPa, ";
		else result << "n/a, hPa, ";
        if (MSLPressure!=CSynopData::InvalidFloat) result << MSLPressure << ", hPa, ";
		else result << "n/a, hPa, ";
        if (PresentWeather!=CSynopData::InvalidInt) result << PresentWeather << ", ";
		else result << "n/a, ";
        if (PastWeather!=CSynopData::InvalidInt) result << PastWeather << ", ";
		else result << "n/a, ";
        if (CloudAmount!=CSynopData::InvalidInt) result << CloudAmount << ", oktas, ";
		else result << "n/a, oktas, ";
        if (LowCloudType!=CSynopData::InvalidInt) result << LowCloudType << ", ";
		else result << "n/a, ";
        if (MedCloudType!=CSynopData::InvalidInt) result << MedCloudType << ", ";
		else result << "n/a, ";
        if (HighCloudType!=CSynopData::InvalidInt) result << HighCloudType << ", ";
		else result << "n/a, ";
		if (SnowStateOfGround!=CSynopData::InvalidInt) result << SnowStateOfGround << ", ";
		else result << "n/a, ";
		if (SnowTotalDepth!=CSynopData::InvalidInt) result << SnowTotalDepth << ", ";
		else result << "n/a, ";

        result << WholeReport;
        
        return result.str();
    }

	string ToXML() {
        //write out members in XML format
        string openTag="UNKNOWN";
		//string result="";
		stringstream result (stringstream::in | stringstream::out);
        switch (ReportType) {
            case AAXX: openTag="SYNOP"; break;
            case BBXX: openTag="SHIP"; break;
            case OOXX: openTag="SYNOPMOBILE"; break;
            default: openTag="UNKNOWN"; break;
        }
		result<< "<" << openTag << ">";
        result<< "<REPORTTIME>" << ReportTime << "</REPORTTIME>";
        if (ReportType==AAXX) result<< "<WMONUMBER>" << WMONumber << "</WMONUMBER>";
        result<< "<STATIONNAME>" << StationName << "</STATIONNAME>";
        if (Lat!=CSynopData::InvalidFloat) result << "<LAT>" << Lat << "</LAT>";
		else result << "<LAT></LAT>";
        if (Lon!=CSynopData::InvalidFloat) result << "<LON>" << Lon << "</LON>";
		else result << "<LON></LON>";
        if (StationType!=CSynopData::InvalidInt) result << "<STATIONTYPE>" << StationType << "</STATIONTYPE>";
		else result << "<STATIONTYPE></STATIONTYPE>";
        if (TotalCloud!=CSynopData::InvalidInt) result << "<TOTALCLOUD UNITS=\"oktas\">" << TotalCloud << "</TOTALCLOUD>";
		else result << "<TOTALCLOUD/>";
        /*switch (CloudBaseHeight) {
            case 0: result+=CloudBaseHeight+", 0 to 50m, "; break;
            case 1: result+=CloudBaseHeight+", 50 to 100m, "; break;
            case 2: result+=CloudBaseHeight+", 100 to 200m, "; break;
            case 3: result+=CloudBaseHeight+", 200 to 300m, "; break;
            case 4: result+=CloudBaseHeight+", 300 to 600m, "; break;
            case 5: result+=CloudBaseHeight+", 600 to 1000m, "; break;
            case 6: result+=CloudBaseHeight+", 1000 to 1500m, "; break;
            case 7: result+=CloudBaseHeight+", 1500 to 2000m, "; break;
            case 8: result+=CloudBaseHeight+", 2000 to 2500m, "; break;
            case 9: result+=CloudBaseHeight+", above 2500m, "; break;
            default:
                if (CloudBaseHeight==CSynopData::InvalidInt) result+="n/a, n/a, ";
                else result+=CloudBaseHeight+", unknown, ";
                break;
        }*/
        if (WindDirection!=CSynopData::InvalidInt) result << "<WINDDIR UNITS=\"deg\">" << WindDirection << "</WINDDIR>";
		else result << "<WINDDIR/>";
        if (WindSpeed!=CSynopData::InvalidFloat) result << "<WINDSPEED UNITS=\"ms-1\">" << WindSpeed << "</WINDSPEED>";
		else result << "<WINDSPEED/>";
        if (AirTemperature!=CSynopData::InvalidFloat) result << "<AIRTEMP UNITS=\"degC\">" << AirTemperature << "</AIRTEMP>";
		else result << "<AIRTEMP/>";
        if (DewPointTemperature!=CSynopData::InvalidFloat) result << "<DEWPOINT UNITS=\"degC\">" << DewPointTemperature << "</DEWPOINT>";
		else result << "<DEWPOINT/>";
        if (StationPressure!=CSynopData::InvalidFloat) result << "<STNPRESSURE UNITS=\"hPa\">" << StationPressure << "</STNPRESSURE>";
		else result << "<STNPRESSURE/>";
        if (MSLPressure!=CSynopData::InvalidFloat) result << "<MSLPRESSURE UNITS=\"hPa\">" << MSLPressure << "</MSLPRESSURE>";
		else result << "<MSLPRESSURE/>";
        if (PresentWeather!=CSynopData::InvalidInt) result << "<PRESENTWX>" << PresentWeather << "</PRESENTWX>";
		else result << "<PRESENTWX/>";
        if (PastWeather!=CSynopData::InvalidInt) result << "<PASTWX>" << PastWeather << "</PASTWX>";
		else result << "<PASTWX/>";
        if (CloudAmount!=CSynopData::InvalidInt) result << "<CLOUDAMOUNT UNITS=\"oktas\">" << CloudAmount << "</CLOUDAMOUNT>";
		else result << "<CLOUDAMOUNT/>";
        /*result+=(LowCloudType!=CSynopData::InvalidInt) ? LowCloudType+", " : "n/a, ";
        result+=(MedCloudType!=CSynopData::InvalidInt) ? MedCloudType+", " : "n/a, ";
        result+=(HighCloudType!=CSynopData::InvalidInt) ? HighCloudType+", " : "n/a, ";
		*/
		if (SnowStateOfGround!=CSynopData::InvalidInt) result << "<SNOWSTATEOFGROUND UNITS=\"code0975\">" << SnowStateOfGround << "</SNOWSTATEOFGROUND>";
		else result << "<SNOWSTATEOFGROUND/>";
		if (SnowTotalDepth!=CSynopData::InvalidInt) result << "<SNOWTOTALDEPTH UNITS=\"code3889\">" << SnowTotalDepth << "</SNOWTOTALDEPTH>";
		else result << "<SNOWTOTALDEPTH/>";
        result<< "<REPORT>" << WholeReport << "</REPORT>";
		result<< "</" << openTag << ">";
        
		return result.str();
    }

  //  void FillDBParams(OdbcParameterCollection^ params) {
  //      //put data into odbc parameters for writing to database (using insertSQLString as the query)
  //		//params->AddWithValue("ORCA",this->GetORCA());
  //      params->AddWithValue("reptime",this->ReportTime);
  //      params->AddWithValue("lat",this->Lat);
  //      params->AddWithValue("lon",this->Lon);
  //      params->AddWithValue("wmo",this->WMONumber);
  //      params->AddWithValue("name",this->StationName);
  //      //params->AddWithValue("ppninc",this->
  //      params->AddWithValue("stntype",this->StationType);
  //      params->AddWithValue("cldbaseht",this->CloudBaseHeight);
  //      //params->AddWithValue("vis",this->
  //      //params->AddWithValue(":viscode,
  //      params->AddWithValue("cldttlamt",this->TotalCloud);
  //      params->AddWithValue("winddir",this->WindDirection);
  //      params->AddWithValue("windspeed",this->WindSpeed);
  //      params->AddWithValue("airtemp",this->AirTemperature);
  //      params->AddWithValue("dewpoint",this->DewPointTemperature);
  //      //params->AddWithValue("relhum",this->
  //      params->AddWithValue("stnpres",this->StationPressure);
  //      params->AddWithValue("mslpres",this->MSLPressure);
  //      params->AddWithValue("prstwx",this->PresentWeather);
  //      params->AddWithValue("pastwx",this->PastWeather);
  //      params->AddWithValue("cldamt",this->CloudAmount);
  //      params->AddWithValue("lowcloud",this->LowCloudType);
  //      params->AddWithValue("medcloud",this->MedCloudType);
  //      params->AddWithValue("highcloud",this->HighCloudType);
  //		//params->AddWithValue("snowstateofground",this->SnowStateOfGround);
  //		//params->AddWithValue("snowtotaldepth",this->SnowTotalDepth);
  //      params->AddWithValue("wholereport",this->WholeReport);
  //  }
	virtual CSynopData* Clone() const
        { return new CSynopData( *this ); }

};