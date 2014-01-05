//WxData.cpp
#include "stdafx.h"
#include "WxData.h"

const float CWxData::InvalidFloat = -FLT_MAX;

CWxData::CWxData()
{
}

void CWxData::Invalidate()
{
	Lat=InvalidFloat;
	Lon=InvalidFloat;
	WholeReport="";
}

void CWxData::SetORCA(string ORCA)
{
	if (ORCA.length()<3) {
		//cover no CCC string e.g. original report, or a corrupt CCC string
		m_ORCA=Original;
		m_ORCASeqLetter='\0';
		return;
	}
	switch (ORCA[0]) {
		case 'R': m_ORCA=Retard; break;
		case 'C': m_ORCA=Correction; break;
		case 'A': m_ORCA=Amendment; break;
		default: m_ORCA=Original; break; //just in case CCC is corrupt
	}
	m_ORCASeqLetter=ORCA[2];
}

string CWxData::GetORCA()
{
	//get back the 3 letter CCC string e.g. to put into DB
	switch (m_ORCA) {
		case Retard: return "RR"+m_ORCASeqLetter; break;
		case Correction: return "CC"+m_ORCASeqLetter; break;
		case Amendment: return "AA"+m_ORCASeqLetter; break;
	}
	return "";
}

string CWxData::ToCSV()
{
	//write out members in CSV format
	//override this
	return "";
}

bool CWxData::IsLaterThan(CWxData* Existing)
{
	//return true if the date time of this bulletin is greater than Existing, or
	//the date times are the same and this supercedes existing due to the ORCA field.
	if (difftime(Existing->ReportTime,ReportTime)>0) return true;
	if (Existing->m_ORCA==m_ORCA) return m_ORCASeqLetter>=Existing->m_ORCASeqLetter;
	switch (Existing->m_ORCA) {
		case Original: return true;
		case Retard: return (m_ORCA==Original);
		case Correction: return (m_ORCA==Amendment);
		case Amendment: return false;
	}
	return true; //just in case there is a problem
}

bool CWxData::IsLaterThan(string ExistingORCA) {
	//return true if the ORCA string of this report supercedes the existing ORCA code
	//passed as a string. Used in database query to avoid having to make a full report
	//object by loading all the fields from the DB. Implicitly assumes report times are equal.
	//In this case, ExistingORCA is a string e.g. RRA, RRB etc
	if (ExistingORCA.empty()) return true;
	CReportORCA ExistingORCAType=Original;
	wchar_t ExistingORCASeqLetter='\0';
	if (ExistingORCA.length()==3) {
		switch (ExistingORCA[0]) {
			case 'R': ExistingORCAType=Retard; break;
			case 'C': ExistingORCAType=Correction; break;
			case 'A': ExistingORCAType=Amendment; break;
			default: ExistingORCAType=Original; break; //just in case CCC is corrupt
		}
		ExistingORCASeqLetter=ExistingORCA[2];
	}
	
	if (ExistingORCAType==m_ORCA) return m_ORCASeqLetter>=ExistingORCASeqLetter;
	switch (ExistingORCAType) {
		case Original: return true;
		case Retard: return (m_ORCA==Original);
		case Correction: return (m_ORCA==Amendment);
		case Amendment: return false;
	}
	return true; //just in case there is a problem
}

//CWxData^ CWxData::Clone() { return (CWxData ^)MemberwiseClone(); }
//CWxData* CWxData::Clone() { return new CWxData(*this); }
