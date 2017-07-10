#include "stdafx.h"
#include "PokerResult.h"


CPokerResult::CPokerResult()
{
}


CPokerResult::~CPokerResult()
{
}

CString CPokerResult::GetPokerString(byte* p, int count)
{
	CString strPoker;
	bool bSameColor = true;
	byte nColor = 255;

	// Sort value
	std::list<byte> ltVal;
	for (int i = 0; i < count; i++)
	{
		ltVal.push_back(p[i] % 13);
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
			if (nColor == (byte)255)				nColor = (byte)(p[i] / 13);
			else if (nColor != (byte)(p[i] / 13))	bSameColor = false;
		}
		i++;
	}

	// Get color
	strPoker += bSameColor ? _T("s") : _T("o");
	return strPoker;
}
