#include "stdafx.h"
#include "TexasPokerClient.h"
#include "TexasPokerMgr.h"
#include "ClientTcp.h"
#include "ServerTcp.h"
#include "TexasGame.h"
#include "TexasPokerResult.h"

using namespace Game;
using namespace Texas;

const short CMD_INIT = 1;
const short CMD_SENDPOKER = 2;
const short CMD_BET = 3;
const short CMD_GIVERESULT = 4;
const short CMD_GAMEOVER = 5;

CTexasPokerClient::CTexasPokerClient(CTexasPokerMgr* mgr)
	: m_Mgr(mgr)
{
	InitData();
}

CString CTexasPokerClient::GetInfoStr()
{
	CString strInfo;
	strInfo.Format(_T("curmoney:%d, state:%d"),
		m_nCurrentMoney,
		m_nClientGameState);
	if (GetNumber() != -1)
	{
		CString str;
		str.Format(_T(", number:%d"), GetNumber());
		strInfo += str;
	}
	if (!m_strReason.IsEmpty())
	{
		strInfo = strInfo + _T(", reason:") + m_strReason;
	}
	return strInfo;
}

bool CTexasPokerClient::IsClientGameOver()
{
	return GetNumber() != -1 ||
		m_nCurrentMoney < 0 ||
		GetClientGameState() == IGameMgr::REQ_GAMEOVER ||
		GetClientGameState() == IGameMgr::REQ_OVERTIME;
}

void CTexasPokerClient::CheckGameOver()
{
	if (m_nCurrentMoney < CTexasPokerMgr::GAME_MIN_MONEY)
	{
		CString strErr;
		int id = m_Mgr->GetCurrentGame() ? m_Mgr->GetCurrentGame()->GetGameID() : -1;
		strErr.Format(_T("Game id %d, The current money %d is limit."), id, m_nCurrentMoney);
		ChangeState(IGameMgr::REQ_GAMEOVER, strErr);
	}
}

void CTexasPokerClient::OnCheckOverTime()
{
	if (m_nClientGameState == IGameMgr::REQ_WAIT)
	{
		SYSTEMTIME stNow;
		::GetLocalTime(&stNow);

		COleDateTime dTimeB(m_CmdTime);
		COleDateTime dTimeE(stNow);
		COleDateTimeSpan dTimeSpan = dTimeE - dTimeB;
		double nSecondSpan = dTimeSpan.GetTotalSeconds();

		if (nSecondSpan >= CTexasPokerMgr::GAME_OVERTIME_SEC)
			ChangeState(IGameMgr::REQ_OVERTIME);
	}
}

void CTexasPokerClient::InitData()
{
	IClient::InitData();
	m_nBetMoney = 0;
	m_nClientTotalMoney = CTexasPokerMgr::GAME_CLIENT_MONEY_DEF;
	m_nCurrentMoney = m_nClientTotalMoney;
	m_bGiveUp = false;
	SetNumber(-1);
	m_strReason.Empty();
	ChangeState(IGameMgr::REQ_IDLE);
}

void CTexasPokerClient::InitOneGame()
{
	if (GetClientGameState() != IGameMgr::REQ_GAMEOVER &&
		GetClientGameState() != IGameMgr::REQ_OVERTIME)
		ChangeState(IGameMgr::REQ_IDLE);
}

void CTexasPokerClient::WinTheGame(int money)
{
	m_nCurrentMoney += money;
}

void CTexasPokerClient::ChangeState(int state, CString strReason)
{
	if (GetClientGameState() == state)	return;
	m_nClientGameState = state;
	switch (state)
	{
	case IGameMgr::REQ_IDLE:
		m_bGiveUp = false;
		m_nBetMoney = 0;		// Bet money in one game
		memset(m_poker, 0, 5);
		m_nCurrentCmd = -1;
		break;

	case IGameMgr::REQ_WAIT:
		::GetLocalTime(&m_CmdTime);	// for check over time
		break;

	case IGameMgr::REQ_DONE:
		m_nCurrentCmd = -1;
		break;

	case IGameMgr::REQ_OVERTIME:
		//ChangeState(IGameMgr::REQ_GAMEOVER, _T("Over time"));
		m_strReason = _T("Over time");
		break;

	case IGameMgr::REQ_GAMEOVER:
		m_Mgr->AddLoser(GetID());
		m_strReason = strReason;
		break;
	}
}


void CTexasPokerClient::OnCmdRespond(byte *pData, int count)
{
	if (count < 8)									return;
	if (m_nCurrentCmd != *(short *)&pData[2])		return;
	if (GetClientGameState() != IGameMgr::REQ_WAIT)	return;

	switch (m_nCurrentCmd)
	{
	case CMD_INIT:
		if (count > 8)
		{
			SetName(CString((TCHAR *)&pData[8]));
		}
		ChangeState(IGameMgr::REQ_DONE);
		break;

	case CMD_SENDPOKER:
		ChangeState(IGameMgr::REQ_DONE);
		break;

	case CMD_BET:
		ChangeState(IGameMgr::REQ_DONE);
		OnBet(&pData[8], count - 8);
		break;

	case CMD_GIVERESULT:
		ChangeState(IGameMgr::REQ_DONE);
		break;

	case CMD_GAMEOVER:
		ChangeState(IGameMgr::REQ_DONE);
		break;

	default:
		ASSERT(FALSE);
		break;
	}
}

void CTexasPokerClient::OnBet(byte *data, int len)
{
	if (len != 4)
	{
		CString strErr;
		int id = m_Mgr->GetCurrentGame() ? m_Mgr->GetCurrentGame()->GetGameID() : -1;
		strErr.Format(_T("Game id %d, the bytes returned is wrong."), id);
		ChangeState(IGameMgr::REQ_GAMEOVER, strErr);
		return;
	}
	int nMoney = *(int *)data;
	if (nMoney == -1)
	{
		auto game = m_Mgr->GetCurrentGame();
		if (game)
		{
			CString strInfo;
			strInfo.Format(_T("bet:%d"), nMoney);
			game->AddClientInfo(GetID(), strInfo);
		}

		SetGiveUp(true);
	}
	else
	{
		CString strReason;
		auto game = m_Mgr->GetCurrentGame();
		if (!game->AddBet(GetID(), nMoney, strReason))	// return false mean illegal value, this client game over
		{
			ChangeState(IGameMgr::REQ_GAMEOVER, strReason);
			return;
		}
		else
		{
			CString strInfo;
			strInfo.Format(_T("bet:%d"), nMoney);
			game->AddClientInfo(GetID(), strInfo);
		}
	}
	return;
}

void CTexasPokerClient::SendInitRequest()
{
	int byLen = 8 + 16;
	byte *pData = new byte[byLen];
	ZeroMemory(pData, byLen);

	m_nCurrentCmd = CMD_INIT;
	InitHead(pData, m_nCurrentCmd, byLen - 8);

	*(int*)&pData[8 + 0] = GetID();							// 
	*(int*)&pData[8 + 4] = GetTotalMoney();					// 
	*(int*)&pData[8 + 8] = (int)m_Mgr->m_ltClient.size();	// 
	*(int*)&pData[8 + 12] = CTexasPokerMgr::GAME_UNIT_MONEY;	// 

	ChangeState(IGameMgr::REQ_WAIT);
	SendData(pData, byLen);
	delete[] pData;
}

void CTexasPokerClient::SendPokerRequest(std::vector<byte> pokers, int banker, int gameid)
{
	if (pokers.size() != 5)	return;
	int byLen = 8 + 20;
	byte *pData = new byte[byLen];
	ZeroMemory(pData, byLen);

	m_nCurrentCmd = CMD_SENDPOKER;
	InitHead(pData, m_nCurrentCmd, byLen - 8);

	pData[8 + 0] = pokers[0];					// 
	pData[8 + 1] = pokers[1];					// 
	pData[8 + 2] = pokers[2];					// 
	pData[8 + 3] = pokers[3];					// 
	pData[8 + 4] = pokers[4];					// 
	memcpy(m_poker, &pData[8 + 0], 5);			// Init pokers
	m_strPokerHide = CTexasPokerResult::GetPokerString(m_poker, 2);
	m_strPokerAll = CTexasPokerResult::GetPokerString(m_poker, 5);
	*(int*)&pData[8 + 8] = banker;				// 
	*(int*)&pData[8 + 12] = CTexasPokerMgr::GAME_DEFAULT_MONEY;	// 
	*(int*)&pData[8 + 16] = gameid;				// 

	ChangeState(IGameMgr::REQ_WAIT);
	SendData(pData, byLen);
	delete[] pData;
}

void CTexasPokerClient::SendBetRequest(int nMax, int nPrevBet, int nYourBet, int nTal, CString strAllBet)
{
	int nSize = strAllBet.GetLength() * sizeof(TCHAR);
	int byLen = 8 + 16 + nSize + 1;
	byte *pData = new byte[byLen];
	ZeroMemory(pData, byLen);

	m_nCurrentCmd = CMD_BET;
	InitHead(pData, m_nCurrentCmd, byLen - 8);

	*(int*)&pData[8 + 0] = nMax;				// 
	*(int*)&pData[8 + 4] = nPrevBet;			// 
	*(int*)&pData[8 + 8] = nYourBet;			// 
	*(int*)&pData[8 + 12] = nTal;				// 
	memcpy(&pData[8 + 16], strAllBet.GetBuffer(), nSize);	//

	ChangeState(IGameMgr::REQ_WAIT);
	SendData(pData, byLen);
	delete[] pData;
}

void CTexasPokerClient::SendResultRequest(bool bWin, int nTal, CString betInfo)
{
	int nSize = betInfo.GetLength() * sizeof(TCHAR);
	int byLen = 8 + 8 + nSize + 2;
	byte *pData = new byte[byLen];
	ZeroMemory(pData, byLen);

	m_nCurrentCmd = CMD_GIVERESULT;
	InitHead(pData, m_nCurrentCmd, byLen - 8);

	*(int*)&pData[8 + 0] = bWin ? 1 : 0;				// 
	*(int*)&pData[8 + 4] = nTal;						// 
	memcpy(&pData[8 + 8], betInfo.GetBuffer(), nSize);	//

	ChangeState(IGameMgr::REQ_WAIT);
	SendData(pData, byLen);
	delete[] pData;
}

void CTexasPokerClient::SendGameOver(CString strProcess)
{
	int nSize = strProcess.GetLength() * sizeof(TCHAR);
	int byLen = 8 + 8 + nSize + 2;
	byte *pData = new byte[byLen];
	ZeroMemory(pData, byLen);

	m_nCurrentCmd = CMD_GAMEOVER;
	InitHead(pData, m_nCurrentCmd, byLen - 8);

	*(int*)&pData[8 + 0] = GetNumber();						// 
	*(int*)&pData[8 + 4] = GetCurrentMoney();				// 
	memcpy(&pData[8 + 8], strProcess.GetBuffer(), nSize);	//

	ChangeState(IGameMgr::REQ_WAIT);
	SendData(pData, byLen);
	delete[] pData;
}


