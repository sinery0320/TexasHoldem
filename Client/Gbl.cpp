#include "stdafx.h"
#include "Gbl.h"


CGbl::CGbl()
{
}


CGbl::~CGbl()
{
}

void CGbl::SpliteBy(CString& text, CString rule, std::vector<CString>& vtStr)
{
	int pos = 0;
	vtStr.clear();
	CString value = text.Tokenize(rule, pos);
	while (value != ""){
		vtStr.push_back(value);
		value = text.Tokenize(rule, pos);
	}
}	

CString CGbl::GetCurrentTimeStr(bool bMill /* = true */)
{
	SYSTEMTIME t;
	GetLocalTime(&t);
	CString str;
	if (bMill)
		str.Format(_T("%.4d-%.2d-%.2d %.2d:%.2d:%.2d (%.3d)"), t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wDay, t.wMilliseconds);
	else
		str.Format(_T("%.4d-%.2d-%.2d %.2d:%.2d:%.2d"), t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wDay);
	return str;
}
