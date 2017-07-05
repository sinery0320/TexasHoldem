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
		CPokerResult::GetPokerString(m_Poker, 2),
		CPokerResult::GetPokerString(&m_Poker[2], 3),
		CPokerResult::GetPokerString(m_Poker, 5));
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

int CGame::GetBetMoney(int nMaxBet, int nPrevBet, int nMyBet, int nTotal, std::map<int, int> mpBetMoney)
{
	if (m_Mgr->IsGiveUp())	return -1;	// give up

	int nRet = nPrevBet - nMyBet;
	ASSERT(nRet >= 0);
	if (nRet < 0)	nRet = -1;
	m_nBetMoney = nPrevBet;
	ASSERT(m_nBetMoney > nMaxBet);
	return nRet;
}

void CGame::SetResultInfo(CString strInfo)
{
	m_strResultInfo = strInfo;
}

CString CGame::GetPokersHide()
{
	return CPokerResult::GetPokerString(m_Poker, 2);
}

CString CGame::GetPokersOpen()
{
	return CPokerResult::GetPokerString(&m_Poker[2], 3);
}

CString CGame::GetPokersAll()
{
	return CPokerResult::GetPokerString(m_Poker, 5);
}
