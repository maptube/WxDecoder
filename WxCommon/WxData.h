//WxData.h
#pragma once

#include <limits>
#include <string>
#include <ctime>

using namespace std;

enum CReportORCA { Original, Retard, Correction, Amendment };

class CWxData {
private:
    //static const char* insertSQLString; //=""; //overridden to make insert statement for each data type
public:
    //static const int InvalidInt = numeric_limits<int>::min(); //flags invalid data
    //static const float InvalidFloat = numeric_limits<float>::min(); //flags invalid data
	static const int InvalidInt = LONG_MIN; //flags invalid data
    static const float InvalidFloat; // = -FLT_MAX; //flags invalid data

    time_t ReportTime;
    float Lat;
	float Lon;
    CReportORCA m_ORCA;
    wchar_t m_ORCASeqLetter;
    string WholeReport;
public:
	CWxData();
    void Invalidate();
    void SetORCA(string ORCA);
	string GetORCA();
    virtual string ToCSV();
    bool IsLaterThan(CWxData* Existing);
	bool IsLaterThan(string ExistingORCA);
    //CWxData^ CWxData::Clone() { return (CWxData ^)MemberwiseClone(); }
	//CWxData* Clone();
	virtual CWxData* Clone() const
        { return new CWxData( *this ); }
};

