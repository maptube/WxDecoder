//ReportParser.h
#pragma once

#include <string>

using namespace std;


class CReportParser {
protected:
    string m_Report;
    string m_Head;
    string m_Tail;
    void SplitHeadTail();
    void SetReport(string& Report);
    int ReadInteger(string& group,const int startIndex, const int endIndex, const int missingDataValue);
public:
    string PeekNextToken() { return m_Head; };
    string GetNextToken();
};