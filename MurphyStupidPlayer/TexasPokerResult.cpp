#include "stdafx.h"
#include <vector>
#include <map>
#include "Gbl.h"
#include "TexasPokerResult.h"

using namespace Game;
using namespace Texas;

std::map<CString, int> g_mpResult;

CString UnEncrypt(CString strIn)
{
	std::map<TCHAR, TCHAR> mpCharMap;
	mpCharMap[_T('a')] = _T('0');
	mpCharMap[_T('b')] = _T('1');
	mpCharMap[_T('c')] = _T('2');
	mpCharMap[_T('d')] = _T('3');
	mpCharMap[_T('e')] = _T('4');
	mpCharMap[_T('f')] = _T('5');
	mpCharMap[_T('g')] = _T('6');
	mpCharMap[_T('h')] = _T('7');
	mpCharMap[_T('i')] = _T('8');
	mpCharMap[_T('j')] = _T('9');
	mpCharMap[_T('k')] = _T('A');
	mpCharMap[_T('l')] = _T('J');
	mpCharMap[_T('m')] = _T('Q');
	mpCharMap[_T('n')] = _T('K');
	mpCharMap[_T('o')] = _T('T');
	mpCharMap[_T('p')] = _T('o');
	mpCharMap[_T('q')] = _T('s');
	mpCharMap[_T('r')] = _T(',');
	mpCharMap[_T('s')] = _T('\r');
	mpCharMap[_T('t')] = _T('\n');
	mpCharMap[_T('\0')] = _T('\0');

	CString strOut;
	TCHAR *pChar = strIn.GetBuffer();
	int len = strIn.GetLength();
	for (int i = 0; i < len; i++)
	{
		if (mpCharMap.find(pChar[i]) != mpCharMap.end())
		{
			strOut += mpCharMap[pChar[i]];
		}
		else
		{
			CString str = _T("Can't find this char : '");
			str = str + pChar[i] + _T("'.");
			MessageBox(NULL, str, _T("Error"), MB_OK | MB_ICONERROR);
		}
	}
	return strOut;
}

void AddResult(CString key, CString val)
{
	key.Trim();
	val.Trim();
	g_mpResult[key] = ::_ttoi(val);
}

CTexasPokerResult::CTexasPokerResult()
{
	CString strPath = CGbl::GetMe().GetAppPath() + _T("TexasTable.eny");
	CFile file;
	if (!file.Open(strPath, CFile::modeRead))
	{
		AfxMessageBox(L"Failed to open the POWER file");
		return;
	}

	UINT count = (UINT)file.GetLength() / sizeof(TCHAR);
	TCHAR *pChar = new TCHAR[count + 1];
	::ZeroMemory(pChar, (count + 1)* sizeof(TCHAR));
	file.Read(pChar, (UINT)file.GetLength());

	CString strIn = CString(pChar);
	delete[] pChar;
	file.Close();
	CString strOut = UnEncrypt(strIn);

	g_mpResult.clear();
	std::vector<CString> vtLine;
	CGbl::SpliteBy(strOut, _T("\r\n"), vtLine);
	for (CString strLine : vtLine)
	{
		if (strLine.Find(_T(",")) == -1)	continue;
		
		std::vector<CString> vtKV;
		CGbl::SpliteBy(strLine, _T(","), vtKV);

		if (vtKV.size() != 2)				continue;
		AddResult(vtKV[0], vtKV[1]);
	}
}


CTexasPokerResult::~CTexasPokerResult()
{
}


CString CTexasPokerResult::GetPokerString(byte* p, int count /* = 5 */)
{
	CString strPoker;
	bool bSameColor = true;
	byte nColor = 255;

	// Sort value
	std::list<byte> ltVal;
	for (size_t i = 0; i < count; i++)
	{
		ltVal.push_back(p[i] % 13); // p[i] % 12 will never reach 12, is this correct ???
	}
	ltVal.sort();

	// Get values
	int i = 0;
	for (int n : ltVal)
	{
		switch (n)
		{
		case 0:		strPoker += _T('2');	break;
		case 1:		strPoker += _T('3');	break;
		case 2:		strPoker += _T('4');	break;
		case 3:		strPoker += _T('5');	break;
		case 4:		strPoker += _T('6');	break;
		case 5:		strPoker += _T('7');	break;
		case 6:		strPoker += _T('8');	break;
		case 7:		strPoker += _T('9');	break;
		case 8:		strPoker += _T('T');	break;
		case 9:		strPoker += _T('J');	break;
		case 10:	strPoker += _T('Q');	break;
		case 11:	strPoker += _T('K');	break;
		case 12:	strPoker += _T('A');	break;
		}

		if (bSameColor)
		{
			if (nColor == (byte)255)				nColor = (byte)(p[i] / 12);
			else if (nColor != (byte)(p[i] / 12))	bSameColor = false;
		}
		i++;
	}

	// Get color
	strPoker += bSameColor ? _T("s") : _T("o");
	return strPoker;
}

// 1 pL is bigger, 0 equal, -1 pR is bigger
int CTexasPokerResult::Compare(byte* pL, byte* pR)
{
	CString strPL = GetPokerString(pL);	if (strPL == _T("ERROR"))	return -1;
	CString strPR = GetPokerString(pR);	if (strPR == _T("ERROR"))	return -1;
	if (g_mpResult.find(strPL) == g_mpResult.end())
	{
		ASSERT(FALSE);
		return -1;
	}
	if (g_mpResult.find(strPR) == g_mpResult.end())
	{
		ASSERT(FALSE);
		return -1;
	}
	int vL = g_mpResult[strPL];
	int vR = g_mpResult[strPR];
	if (vL < vR)		return 1;
	else if (vL > vR)	return -1;
	return 0;
}

int Game::Texas::CTexasPokerResult::GetRank(byte * pCards)
{
	CString str = GetPokerString(pCards);
	if (str == L"ERROR")
	{
		return -1;
	}

	if (g_mpResult.find(str) == g_mpResult.end())
	{
		ASSERT(FALSE);
		return -1;
	}

	int rank = g_mpResult[str];
	return rank;
}

double Game::Texas::CTexasPokerResult::GetWinRate(byte *pPokers)
{
	byte pokers[52] = { 0 };
	int myRank = GetRank(pPokers);
	if (myRank <= 0)
	{
		::MessageBox(NULL, L"illegal pokers", NULL, MB_OK);
		return 0.0;
	}

	int win = 0;
	int total = 0;

	for (int i = 0; i < 5;++i)
	{
		pokers[pPokers[i]] = 1;
	}

	for (byte i = 0; i < 52; ++i)
	{
		if (pokers[i] == 1)
			continue;
		for (byte j = i; j < 52; ++j)
		{
			if (pokers[j] == 1 || j ==i)
				continue;

			byte temp[5] = {0};
			temp[0] = i;
			temp[1] = j;
			memcpy_s(temp + 2, 3, pPokers + 2, 3);

			int rivalRank = GetRank(temp);
			if (rivalRank <= 0)
			{
				::MessageBox(NULL, L"illegal pokers", NULL, MB_OK);
				return 0.0;
			}
			if (myRank <= rivalRank)
			{
				win++;
			}
			total++;
		}
	}

	return double(win)/double(total);
}
