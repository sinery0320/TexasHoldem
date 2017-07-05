#include "stdafx.h"
#include "Gbl.h"

CGbl::CGbl()
{
	int n = ::GetTickCount();
	n %= RAND_MAX;
	srand(n);
}

CGbl::~CGbl()
{
}

CString g_strExePath;

// Last c is '\'
CString CGbl::GetAppPath()
{
    if (g_strExePath.IsEmpty())
    {
        TCHAR exePath[MAX_PATH];
        ::ZeroMemory(exePath, sizeof(TCHAR)*MAX_PATH);
        ::GetModuleFileName(NULL, exePath, MAX_PATH);
        g_strExePath = exePath;
        g_strExePath = g_strExePath.Left(g_strExePath.ReverseFind('\\') + 1);
    }
    return g_strExePath;
}

void CGbl::SpliteBy(CString text, CString rule, std::vector<CString>& vtStr)
{
    int pos = 0;
    vtStr.clear();
    CString value = text.Tokenize(rule, pos);
    while (value != ""){
        vtStr.push_back(value);
        value = text.Tokenize(rule, pos);
    }
}

// return random number in [0 ~ nEnd]
int CGbl::GetRand(int nEnd /* = 100 */)
{
	if (nEnd < 1)	return 0;
	int nRd = rand() % (nEnd + 1);
	return nRd;
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
