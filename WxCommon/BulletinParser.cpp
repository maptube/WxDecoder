//BulletinParser.cpp
#include "stdafx.h"
#include "BulletinParser.h"
#include "TextUtils.h"

//#include "Windows.h"
#include <sys/types.h>
#include <sys/stat.h>
//#include "errno.h"
//#include "share.h"

#include <math.h>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <string>
#include <ctime>
#include <regex>
#include <iostream>
#include <fstream>
#include <sstream>
#include <hash_map>
#include <map>

using namespace std;

//TODO: Need a proper bulletin routing table
//TODO: The bulletin parser should parse a single report at a time from the files
//and return this back to whatever is calling it e.g. like an enumerator going through
//a bulletin one report at a time. This would give the programmer the flexibility
//to put the data into a database, or keep the data in memory as required. At the
//moment everything is read into memory and return which is very inefficient.

CBulletinParser::CBulletinParser(CStationList* StationList)
{
	//HeaderExp=gcnew Regex("(\\d{3}) (\\w{4}\\d{2}) (\\w{4}) (\\d{2})(\\d{2})(\\d{2})( \\w{3})? ");
	//HeaderExp = new regex("(\\d{3}) (\\w{4}\\d{2}) (\\w{4}) (\\d{2})(\\d{2})(\\d{2})( \\w{3})? ");
	HeaderExp = new regex("^(\\d{3}) (\\w{4}\\d{2}) (\\w{4}) (\\d{2})(\\d{2})(\\d{2})( \\w{3})? (.*)");
	SynopParser = new CSynopParser(StationList);
    MetarParser = new CMetarParser(StationList);
	AmdarParser = new CAmdarParser();

	//build default bulletin routing table
	//see: http://www.c-sharpcorner.com/uploadfile/b81385/efficient-string-matching-algorithm-with-use-of-wildcard-characters/
	//BUT I'm going to use a regex instead
	m_RoutingTable.insert(pair<regex*,string>(new regex("^(SM|SN|SI)\\w{2}\\d{2}$"),"SYNOP"));
	m_RoutingTable.insert(pair<regex*,string>(new regex("^SX.*$"),"")); //CMAN or MMXX??????
	m_RoutingTable.insert(pair<regex*,string>(new regex("^SS\\w{2}\\d{2}$"),"BUOY")); //BUOY
	m_RoutingTable.insert(pair<regex*,string>(new regex("^(UD|UA)\\w{2}\\d{2}$"),"AMDAR"));
}

CBulletinParser::~CBulletinParser()
{
	delete HeaderExp;
	delete SynopParser;
	delete MetarParser;
	delete AmdarParser;
	//delete bulletin routing table regex
	for (map<regex*,string>::iterator it = m_RoutingTable.begin(); it!=m_RoutingTable.end(); it++)
	{
		delete it->first;
	}
}

//string CBulletinParser::RemoveWhitespace(string Report)
//{
//    //remove any extra whitespace between elements of a report
//    //String^ result="";
//    //array<wchar_t>^ chars=Report->ToCharArray();
//
//    //bool lastWhite=true;
//    //for (int i=0; i<Report->Length; i++) {
//    //    wchar_t ch=chars[i];
//    //    bool thisWhite=(ch<=' ')||(ch>'Z');
//    //    if (lastWhite) {
//    //        if (!thisWhite) result=result+ch;
//    //    }
//    //    else {
//    //        if (thisWhite) result=result+" "; //convert all whitespace into space
//    //        else result=result+ch;
//    //    }
//    //    lastWhite=thisWhite;
//    //}
//    //return result;
//	return Report->Trim(); //this is about 1000 times faster than above!
//}

//Match a TTAAII against the bulletin routing table and return the string describing the correct parser, or an empty string to ignore
string CBulletinParser::TTAAiiMatch(string& TTAAii)
{
	for (map<regex*,string>::iterator it = m_RoutingTable.begin(); it!=m_RoutingTable.end(); it++)
	{
		if (regex_match(TTAAii,*(it->first))) return it->second;
	}
	return string("");
}

bool CBulletinParser::IsSequenceNumber(string& Line)
{
    //return true if Line contains a valid sequence number. That's 3 digits on its own line.
	if (Line.length()==3) {
		//it's not pretty, but it's fast
		wchar_t c0=Line[0];
		wchar_t c1=Line[1];
		wchar_t c2=Line[2];
		return (c0>='0')&&(c0<='9')&&(c1>='0')&&(c1<='9')&&(c2>='0')&&(c2<='9');
    }
    return false;
}

//convert string to wide string
//std::wstring s2ws (const std::string& s)
// {
//     std::wstring ws;
//     ws.assign (s.begin (), s.end ());
//     return ws;
// }

//convert wide string to string
//std::string ws2s (const std::wstring& ws)
// {
//     std::wstring s;
//     s.assign (ws.begin (), ws.end ());
//     return s;
// }
 

void CBulletinParser::ParseFile(string& Filename,list<CWxData*>& result,string& OutputDir)
{
    //Split a bulletin file into separate reports and parse them.
    //Filename is the input file.
    //result is a hashtable of the processed reports.
    //OutputDir is the location to write the data and error files to.
    //Requires the result hashtable to be already constructed.
    //Collections::ArrayList^ result=gcnew Collections::ArrayList();
    //Collections::Hashtable^ result=gcnew Collections::Hashtable();

	//Get creation time of file as we need it to make a best guess at
	//the valid time of the file
	//FILETIME ftCreate, ftAccess, ftWrite;
    //SYSTEMTIME stUTC, stLocal;
    //DWORD dwRet;

	//wstring wFilename = s2ws(Filename); //API use a wide string pointer, so need to convert
	//HANDLE hFile = hFile = CreateFile(wFilename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	//dwRet = GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite);
	//FileInfo^ fileInfo=gcnew FileInfo(Filename);
	//DateTime^ FileCreationDT = fileInfo->CreationTimeUtc;
	//CloseHandle(hFile);
	//if (!dwRet) return; //failed

	//use stat here as it should improve portability
	struct _stat statbuf;
	int erno = _stat( Filename.c_str(), &statbuf );
	//if (erno!=0) error
	//buf.st_ctime creation time
//TODO: probably need file time conversion here!!!!
	time_t FileCreationDT = statbuf.st_ctime;

    //strip the extension off the filename and add our own for the result files
	ifstream in(Filename.c_str(),ifstream::in|ifstream::binary); //MUST open in binary for non-alpha chars to be processed (comms errors)
	string LogFilename;
	int Pos=Filename.find_last_of("\\");
	if (Pos>=0) LogFilename=Filename.substr(Pos+1);
    else LogFilename=Filename;
    if (OutputDir[OutputDir.length()-1]!='\\') OutputDir.push_back('\\');

    try {
		m_ErrorFile = new ofstream(OutputDir+"error-"+LogFilename);

        //Define array of whitespace characters to be ignored
        //array<Char>^ WhiteSpace={0x01,0x03,0x0a,0x0d,0x20};
		//const char WhiteSpace[]={0x01,0x03,0x0a,0x0d,0x20}; //TODO: this isn't actually used
		stringstream Bulletin(stringstream::in | stringstream::out);

		const char startseq[] = { 0x01 }; //STX
		const char endseq[] = { 0x03 }; //ETX

		//char LineBuf[1024]; //maximum line size to read
		if (in.is_open()) {
			string Line;
			bool WSFlag = true;
			while (std::getline(in,Line))
			{
				//in.getline(LineBuf,sizeof(LineBuf))
				//string Line(LineBuf);
				//cout<<"Line: "<<Line<<endl;
				//Line=trim_all(Line);
				//string L2 = Line.substr(0,6);
				//if (Line.substr(0,3)=="631") {
				//	cout<<"UK BULLETIN"<<endl;
				//}
				//Line=trim_all(Line); //can't trim here as we lose the STX ETX
				//make sure bulletins stay in sync by always starting with seq number
				if (Line.length()>0) {
					//find start and end of transmission bytes in line (if present)
					//string::iterator itSTX = find_first_of(Line.begin(),Line.end(),startseq,startseq+sizeof(startseq));
					//string::iterator itETX = find_first_of(Line.begin(),Line.end(),endseq,endseq+sizeof(endseq));
					for (string::iterator it = Line.begin(); it<Line.end(); it++)
					{
						bool isStart = ((*it==startseq[0])&&(find_first_of(it,it+sizeof(startseq),startseq,startseq+sizeof(startseq))==it));
						bool isEnd = ((!isStart)&&(*it==endseq[0])&&(find_first_of(it,it+sizeof(endseq),endseq,endseq+sizeof(endseq))==it));
						if (isStart||isEnd)
						{
							//start or end sequence detected, but we do the same processing for both, so write any existing bulletin (if not empty),
							//then clear the current bulletin for the next one to be read in
							if (Bulletin.tellp()>0)
							{
								ParseBulletin(Bulletin.str(),FileCreationDT,result);
							}
							Bulletin.str(std::string()); //clear Bulletin stringstream
							WSFlag=true; //prevent leading WS being added
						}
						else
						{
							//accumulate regular characters, but watch for duplicate WS
							char ch = *it;
							if (ch=='\r'||ch=='n') ch=' ';
							bool isWS = (ch==' ');
							if ((!isWS)||(!WSFlag))
								Bulletin<<ch;
							WSFlag = isWS;
						}
					}

					//if (IsSequenceNumber(Line)) {
					//	if (Bulletin.tellp()>0) {
					//		//cout<<"ParseBulletin: "<<Bulletin.str()<<endl;
					//		ParseBulletin(Bulletin.str(),FileCreationDT,result);
					//	}
					//	Bulletin.str(std::string()); //clear Bulletin stringstream
					//	Bulletin << Line << " ";
					//}
					//else
					//{
					//	Bulletin << Line << " ";
					//}
				}
				if (!WSFlag)
				{
					Bulletin<<" "; //substitute endl for space in reports, but don't add leading WS
					WSFlag=true;
				}
				/*if (in.fail()) {
					cout<<"FAIL"<<endl;
				}*/
			}
			if (Bulletin.tellp()>0) ParseBulletin(Bulletin.str(),FileCreationDT,result); //parse last bulletin in file

			in.close();
		}
		else {
			cout<<"error: could not open input file "<<Filename<<endl;
		}
        m_ErrorFile->close();
    }
    catch (...) {}

	delete m_ErrorFile;

    cout << "Finished decoding bulletin in file " << Filename << endl;
}

void CBulletinParser::WriteBulletin(string& Reports)
{
/*
    //write out the current bulletin as a file with the filename the bulletin header
    try {
      BufferedWriter writer=new BufferedWriter(
        new FileWriter(currentPath+"\\b-"+noExtFilename+"-"+TTAAii+centre+DDHHMM+orca+"."+seqNumber));
      writer.write(seqNumber+"\n");
      writer.write(TTAAii+" "+centre+" "+DDHHMM+" "+orca+"\n");
      writer.write(reports+"\n");
      writer.close();
    }
    catch (java.io.IOException ie) {}
*/
}

void CBulletinParser::ParseBulletin(string& Bulletin,time_t FileCreationDT,list<CWxData*>& result)
{
	try {
		string reports = trim_all(ParseGTSHeader(trim_all(Bulletin),FileCreationDT));
		ParseReports(reports,FileCreationDT,result);
	}
    catch (exception ex) {
        *m_ErrorFile<<"****************BULLETIN ERROR*****************"<<endl;
        *m_ErrorFile<<"SequenceNumber="<<m_SeqNumber<<endl;
        *m_ErrorFile<<"TTAAii="<<m_TTAAii<<endl;
        *m_ErrorFile<<"Bulletin: "<<Bulletin<<endl;
		*m_ErrorFile<<"Exception: "<<ex.what()<<endl;
        *m_ErrorFile<<"***********************************************"<<endl;
    }
}

time_t CBulletinParser::ProbableDateTime(int Day, time_t FileCreationDT)
{
	//Return probably bulletin time using the file creation time as a base.
	//Bulletins only have the day and hour, so find the year and month closest
	//to the file creation time. It's either going to be the FileCreationDT month
	//or the previous month, whichever is closest.
	//NOTE: days and months start from 1 - zero causes an exception
	/*DateTime ThisMonth(FileCreationDT->Year, FileCreationDT->Month, 1, 0, 0, 0 );
	DateTime LastMonth(ThisMonth.AddMonths(-1));
	//now add the same number of days to each - careful about days in month
	ThisMonth = ThisMonth.AddDays(Day-1);
	LastMonth = LastMonth.AddDays(Day-1);
	//OK, we've safely constructed two probable times, so which is closer to FileCreationDT?
	int ts = TimeSpan::Compare(
		ThisMonth.Subtract(*FileCreationDT),
		FileCreationDT->Subtract(LastMonth));
	if (ts<0) return ThisMonth;
	return LastMonth;*/

	//We've got a file time and a day of the month, but we're not sure whether it refers to this month or last month
	//relative to the file time.
	//mktime, timegm, timelocal
	struct tm tmFile = {0};
	gmtime_s(&tmFile,&FileCreationDT);
	struct tm tmThisMonth = {0};
	tmThisMonth.tm_year = tmFile.tm_year;
	tmThisMonth.tm_mon = tmFile.tm_mon;
	tmThisMonth.tm_mday = Day;
	//HH:MM:SS=0
	//mktime(&tmThisMonth);

	struct tm tmLastMonth = {0};
	tmLastMonth.tm_year = tmFile.tm_year;
	tmLastMonth.tm_mon = tmFile.tm_mon-1;
	tmLastMonth.tm_mday = Day;
	//HH:MM:SS=0
	if (tmLastMonth.tm_mon<=0) {
		//if month has wound back past January, go to December of previous year
		tmLastMonth.tm_mon=12;
		--tmLastMonth.tm_year;
	}

	time_t ThisMonth = mktime(&tmThisMonth);
	time_t LastMonth = mktime(&tmLastMonth); //returns -1 if invalid i.e. last month doesn't have Day (i.e. 30th Feb or 30th March)
	if (abs(ThisMonth-FileCreationDT)<abs(LastMonth-FileCreationDT))
		return ThisMonth;
	return LastMonth;
}

string CBulletinParser::ParseGTSHeader(string& GTS,time_t FileCreationDT) {
    //Strip off the GTS bulletin header and return a string containing
    //just the reports in this bulletin.
	//FileCreationDT is the date used to determine the year and month of
	//this bulletin. It's the date time of the file we're reading.
    //Sequence number, TTAAii, centre name, day, hour and minute
    //e.g. 053 SIVD01 CWHX 302100
//TODO: what about RRA etc? syntax is handled, but precedence is not

	m_SeqNumber="";
	m_TTAAii="";
	m_Centre="";
	m_Day=0;
	m_Hour=0;
	m_Minute=0;
	m_BulletinDT=0;

	match_results<string::const_iterator> sm;
	if (regex_match(GTS,sm,*HeaderExp))
	{
        m_SeqNumber=sm[1];
        m_TTAAii=sm[2];
        m_Centre=sm[3];
		m_Day=atoi(sm[4].str().c_str());
        m_Hour=atoi(sm[5].str().c_str());
        m_Minute=atoi(sm[6].str().c_str());
        //m_BulletinDT=DateTime::UtcNow;
        //int Year=m_BulletinDT->Year;
        //int Month=m_BulletinDT->Month;
        //m_BulletinDT = DateTime(Year,Month,m_Day,m_Hour,m_Minute,0);
		m_BulletinDT = ProbableDateTime(m_Day,FileCreationDT); //return year month day with hour min sec zeroed
		struct tm tmBulletin = {0};
		gmtime_s(&tmBulletin,&m_BulletinDT);
		tmBulletin.tm_hour = m_Hour;
		tmBulletin.tm_min = m_Minute;
		m_BulletinDT = mktime(&tmBulletin);
		if (sm.length()>6) m_Orca=sm[7];
        else m_Orca="";
		//sm[8] is the remaining part of the report after the header
		//GTS=GTS.substr(GTS.length()-sm[8].length()); //old version
		GTS=sm[8];
    }
	else
	{
		WriteError(GTS,string("Unrecognised bulletin header"),string(""));
	}

    return GTS;
}

void CBulletinParser::WriteError(string& Bulletin,string& ErrorText,string& WholeReport)
{
    *m_ErrorFile << "*************ERROR***************" << endl;
    *m_ErrorFile << "SequenceNumber=" << m_SeqNumber << endl;
    *m_ErrorFile << "TTAAii=" << m_TTAAii << endl;
    *m_ErrorFile << "BULLETIN: " << Bulletin << endl;
    *m_ErrorFile << "ERRORTEXT: " << ErrorText << endl;
    *m_ErrorFile << "REPORT: " << WholeReport << endl;
    *m_ErrorFile << "****************END***************" << endl;
}

void CBulletinParser::ParseReports(string& Bulletin,time_t FileCreationDT,list<CWxData*>& result)
{
	//The GTS header has already been stripped off - Bulletin contains everything after the GTS header
	cout << m_TTAAii << " " << m_SeqNumber << " " << Bulletin.length() << endl;
	//if (m_TTAAii=="SMUK01") {
	//	cout<<"UK MAIN BULLETIN"<<endl;
	//}
	size_t StartPos=0;
	while ((StartPos<Bulletin.length())&&(StartPos!=string::npos)) {
		size_t Equals=Bulletin.find_first_of('=',StartPos);
		if (Equals==string::npos) Equals = Bulletin.length();
		string RawReport=Bulletin.substr(StartPos,Equals+1);
		StartPos=Equals+1;
        //String^ RawReport=Reports[i]->Trim(); //do you need to call Trim?
		try {
            if (RawReport.length()==0) continue;
			string BulletinType = TTAAiiMatch(m_TTAAii);
            if (BulletinType=="SYNOP")
            {
                if (SynopParser->Parse(m_BulletinDT,RawReport))
                {
                    CSynopData* synop=(CSynopData*)SynopParser->GetSynopData()->Clone();
					string testdata = synop->ToCSV();
					if (synop->ReportType==AAXX)
						//result.insert(pair<string,CWxData*>(std::to_string((long long)synop->WMONumber),synop)); //WMONumber
						result.push_back(synop);
					else if (synop->ReportType==BBXX)
						//result.insert(pair<string,CWxData*>(synop->StationName,synop)); //SHIP has no WMONumber
						result.push_back(synop);
					//else if (synop->ReportType==CSynopReportType::OOXX)
                }
                else
                {
                    WriteError(Bulletin,SynopParser->GetErrorText(),SynopParser->GetWholeReport());
                }
            }
			//else if (TT=="SX") {
			//	//ignore CMAN or MMXX surface observation
			//}
            //else if (TT=="SS") {
//          System.out.println("Ignore Buoy bulletin on SS header");
            //}
			else if (BulletinType=="BUOY") {
			}
            else if (BulletinType=="AMDAR") {
                if (AmdarParser->Parse(m_BulletinDT,m_Orca,RawReport))
                {
                    CAmdarData* amdar=(CAmdarData*)AmdarParser->GetAmdarData()->Clone();
                    //This code used the hashmap with a unique identifier to check the precedence for bulletins. Using a list
					//this isn't possible, so changed to just pushing new data. ORCA check will need to be done later.
					//use the aircraft id and the time as the unique identifier
                    //std::ostringstream ss;
					//ss << amdar->AircraftIndicator << (amdar->Day) << (amdar->Hour) << (amdar->Minute);
					//string id=ss.str();
                    //CAmdarData* existing=(CAmdarData*)&result[id];
                    //if (!existing) result.insert(pair<string,CWxData*>(id,amdar));
                    //else if (amdar->IsLaterThan(existing)) result.insert(pair<string,CWxData*>(id,amdar));
					result.push_back(amdar);
                }
                else
                {
                    WriteError(Bulletin,AmdarParser->GetErrorText(),AmdarParser->GetWholeReport());
                }
            }
            else if (BulletinType=="METAR") {
                if (MetarParser->Parse(m_BulletinDT,m_Orca,RawReport))
                {
                    CMetarData* metar=(CMetarData*)MetarParser->GetMetarData()->Clone();
                    //This does the ORCA check - not possible using list<CWxData*>
					//CMetarData* existing=(CMetarData*)&result[metar->ICAO];
                    //if (!existing) result.insert(pair<string,CWxData*>(metar->ICAO,metar));
                    //else if (metar->IsLaterThan(existing)) result.insert(pair<string,CWxData*>(metar->ICAO,metar));
					result.push_back(metar);
                }
                else
                {
                    WriteError(Bulletin,MetarParser->GetErrorText(),MetarParser->GetWholeReport());
                }
            }
			else {
				string Error = "Bulletin header "+m_TTAAii+" not recognised - ignored";
				WriteError(Bulletin,Error,string(""));
			}
        }
        catch (exception ex) {
            //put catch here to trap an exception while parsing a report, so that this report
            //write out its error and we can continue parsing the rest of the reports without
            //rejecting the whole bulletin just because of a single error in a report
            *m_ErrorFile << "****Exception in ParseReports*****************************" << endl;
            *m_ErrorFile << "Report=" << RawReport << endl;
			*m_ErrorFile << ex.what() << endl;
            *m_ErrorFile << "**********************************************************" << endl;
		}
    }
}

//void CBulletinParser::WriteToDB(CWxData^ report)
//{
//	//write a report to the database
//	if (report->GetType()->FullName=="CMetarData")
//    {
//		CMetarData^ metar=(CMetarData^)report;
//	}
//    else if (report->GetType()->FullName=="CSynopData")
//    {
//		CSynopData^ synop=(CSynopData^)report;
//		OdbcConnection^ conn=gcnew OdbcConnection("DSN=PostgreSQL35W");
//		conn->Open();
//		//TODO: it might be easier to do this through a stored procedure?
//		//Get the ORCA string for this report if one already exists, otherwise null.
//		//If ORCA of new report greater than existing then delete and insert new report.
//		//ReportTime and WMO Number are unique keys for a synop (AAXX)
//		OdbcCommand^ comm=gcnew OdbcCommand(synop->orcaSQLString,conn);
//		comm->Parameters->AddWithValue("reptime",synop->ReportTime);
//		comm->Parameters->AddWithValue("wmo",synop->WMONumber);
//		String^ ORCA=(String^)comm->ExecuteScalar();
//		if (synop->IsLaterThan(ORCA)) {
//			//TODO: this is tricky as ORCA==null doesn't mean there isn't an existing record.
//			//Dropping a non-existent record is harmless, so we'll do that at the expense of time.
//			//Could also try update or insert, but this syntax is DB implementation dependent.
//			//drop existing report and insert the new one
//			OdbcCommand^ delcomm=gcnew OdbcCommand(synop->deleteSQLString,conn);
//			delcomm->Parameters->AddWithValue("reptime",synop->ReportTime);
//			delcomm->Parameters->AddWithValue("wmo",synop->WMONumber);
//			int count=delcomm->ExecuteNonQuery();
//			//if count!=1 then error
//			OdbcCommand^ insertcomm=gcnew OdbcCommand(synop->insertSQLString,conn);
//			synop->FillDBParams(insertcomm->Parameters);
//			count=insertcomm->ExecuteNonQuery();
//			//if count!=1 then error
//		}
//		conn->Close();
//	}
//}