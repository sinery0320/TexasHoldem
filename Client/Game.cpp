#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"
#include "Gbl.h"
#include "Game.h"
#include "GameMgr.h"
#include "PokerResult.h"

CGame::CGame(CGameMgr *mgr, int id)
	: m_Mgr(mgr)
	, m_nGameID(id)
	, m_nBankerID(-1)
	, m_nBetMoney(0)
{
}

CGame::~CGame()
{
}

CString CGame::GetInfoStr()
{
	CString strInfo;
	strInfo.Format(_T("bid:%d, betmoney:%d, hp:%s, sp:%s, ap:%s"),
		m_nBankerID,
		m_nBetMoney,
		m_strPokerHide,//CPokerResult::GetPokerString(m_Poker, 2),
		m_strPokerOpen,//CPokerResult::GetPokerString(&m_Poker[2], 3),
		m_strPokerAll); //CPokerResult::GetPokerString(m_Poker, 5));
	if (!m_ltWinID.empty())
	{
		strInfo += _T(", wid:");
		CString str;
		for (int i : m_ltWinID)
		{
			str.Format(_T("%d-"), i);
			strInfo += str;
		}
		strInfo = strInfo.Left(strInfo.GetLength() - 1);
	}
	return strInfo;
}

void CGame::SetPoker(byte *data)
{
	memcpy(m_Poker, data, 5);
	m_strPokerHide = CPokerResult::GetPokerString(data, 2);
	m_strPokerOpen = CPokerResult::GetPokerString(&data[2], 3);
	m_strPokerAll = CPokerResult::GetPokerString(data, 5);
}

int CGame::GetBetMoney(int nMaxBet, int nPrevBet, int nMyBet, int nTotal, std::map<int, int> mpBetMoney)
{
	//if (m_Mgr->IsGiveUp())	return -1;	// give up

	int nRet = nPrevBet - nMyBet;
	ASSERT(nRet >= 0);
	if (nRet < 0)	nRet = -1;
	m_nBetMoney = nPrevBet;
	ASSERT(m_nBetMoney > nMaxBet);
	return nRet;
}

void CGame::SetResultInfo(CString strInfo)
{
	m_ltWinID.clear();
	std::vector<CString> vtLine;
	CGbl::SpliteBy(strInfo, _T("\r\n"), vtLine);
	for (int i = 0; i < (int)vtLine.size(); i++)
	{
		CString strLine = vtLine[i].Trim();
		if (strLine.IsEmpty())	continue;
		std::vector<CString> vtInfo;
		CGbl::SpliteBy(strLine, _T(","), vtInfo);

		int id = -1;
		for (int j = 0; j < (int)vtInfo.size(); j++)
		{
			CString strKV = vtInfo[j].Trim();
			if (strKV.IsEmpty())	continue;

			std::vector<CString> vtKV;
			CGbl::SpliteBy(strKV, _T(":"), vtKV);
			if (vtKV.size() != 2)	continue;

			CString strK = vtKV[0].Trim();
			CString strV = vtKV[1].Trim();

			if (strK == _T("id"))
			{
				id = _ttoi(strV);
			}
			else if (strK == _T("win"))
			{
				if (id != -1 && strV == _T("1"))
				{
					AddWinID(id);
				}
			}
		}
	}

	m_strResultInfo = strInfo;
}

CString CGame::GetPokersHide()
{
	return m_strPokerHide; // CPokerResult::GetPokerString(m_Poker, 2);
}

CString CGame::GetPokersOpen()
{
	return m_strPokerOpen; // CPokerResult::GetPokerString(&m_Poker[2], 3);
}

CString CGame::GetPokersAll()
{
	return m_strPokerAll; // CPokerResult::GetPokerString(m_Poker, 5);
}

bool CGame::IfPlayerWin(int id)	// To check if this player win this one game
{
	for (auto iter = m_ltWinID.begin(); iter != m_ltWinID.end(); iter++)
	{
		if (id == *iter)
			return true;
	}
	return false;
}
