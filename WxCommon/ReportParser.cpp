//ReportParser and base class of MetarParser and SynopParser

#include "stdafx.h"
#include "ReportParser.h"
#include "TextUtils.h"

#include <string>

using namespace std;

void CReportParser::SetReport(string& Report)
{
    m_Report=Report;
    m_Tail=Report;
    SplitHeadTail();
}

void CReportParser::SplitHeadTail()
{
    //perform simple lexical analysis and split current tail into a
    //new head and tail with head containing the next token
    m_Tail=trim(m_Tail); //trim whitespace

    char Separator[] = {' ','\n','\t','='};
	int Pos=m_Tail.find_first_of(Separator);

    if (Pos>=0) {
        m_Head=m_Tail.substr(0,Pos);
        m_Tail=m_Tail.substr(Pos+1);
        m_Tail=trim(m_Tail);
    }
    else {
        m_Head=m_Tail;
        m_Tail="";
    }
}

//String ^CReportParser::PeekToken()
//{
//}

string CReportParser::GetNextToken()
{
    string result=m_Head;
    SplitHeadTail();
    return result;
}

int CReportParser::ReadInteger(string& Group,const int StartIndex, const int Length, const int MissingDataValue)
{
    //Read an integer from the startIndex to endIndex characters of string,
    //or a string of / characters indicating missing data,
    //or a number followed by / characters.

    //Check properly, making sure that all characters are /
    //String ^slash="/////";
	string temp=Group.substr(StartIndex,Length);
    //slash=slash->Substring(0,temp->Length);
    //if (slash->Equals(temp)) return missingDataValue;
	if (temp.find_first_of('/')!=string::npos) return MissingDataValue; //just check for containing one or more /
    else return atoi(temp.c_str());
}