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
	, m_nBetMoney(0)
	, m_nTotalMoney(30)//10000)
	, m_nCurrentMoney(m_nTotalMoney)
	, m_bGiveUp(false)
	, m_nLoserNumber(-1)
{
	ChangeState(IGameMgr::REQ_IDLE);
}

CString CTexasPokerClient::GetInfoStr()
{
	CString strInfo;
	strInfo.Format(_T("curmoney:%d, bet:%d, giveup:%d, state:%d"),
		m_nCurrentMoney,
		m_nBetMoney,
		m_bGiveUp ? 1 : 0,
		m_nGameState);
	strInfo = strInfo + _T(", poker:") + CTexasPokerResult::GetPokerString(m_poker);
	if (m_nLoserNumber != -1)
	{
		CString str;
		str.Format(_T(", loseid:%d"), m_nLoserNumber);
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
	return m_nLoserNumber != -1 ||
		m_nCurrentMoney < 0 || //SET_MIN_MONEY ||
		m_nGameState == IGameMgr::REQ_GAMEOVER;
}

void CTexasPokerClient::CheckGameOver()
{
	if (m_nCurrentMoney < SET_MIN_MONEY)
	{
		CString str;
		str.Format(_T("The current money %d is limit"), m_nCurrentMoney);
		ChangeState(IGameMgr::REQ_GAMEOVER, str);
	}
}

void CTexasPokerClient::OnTimeOver()
{
	if (m_nGameState == IGameMgr::REQ_WAIT)
	{
		ChangeState(IGameMgr::REQ_OVERTIME);
	}
}

void CTexasPokerClient::InitOneGame()
{
	ChangeState(IGameMgr::REQ_IDLE);
}

void CTexasPokerClient::WinTheGame(int money)
{
	m_nCurrentMoney += money;
}

void CTexasPokerClient::ChangeState(int state, CString strReason)
{
	if (m_nGameState == state)	return;
	switch (state)
	{
	case IGameMgr::REQ_IDLE:
		m_bGiveUp = false;
		m_nBetMoney = 0;		// Bet money in one game
		memset(m_poker, 0, 5);
		m_nCurrentCmd = -1;
		break;

	case IGameMgr::REQ_WAIT:
		break;

	case IGameMgr::REQ_DONE:
		break;

	case IGameMgr::REQ_OVERTIME:
		ChangeState(IGameMgr::REQ_GAMEOVER, _T("Over time"));
		break;

	case IGameMgr::REQ_GAMEOVER:
		m_Mgr->AddLoser(GetID());
		m_strReason = strReason;
		break;
	}
	m_nGameState = state;
}


void CTexasPokerClient::OnCmdRespond(byte *pData, int count)
{
	if (count < 8)								return;
	if (m_nCurrentCmd != *(short *)&pData[2])	return;
	if (m_nGameState != IGameMgr::REQ_WAIT)		return;

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
		if (OnBet(&pData[8], count - 8))
			ChangeState(IGameMgr::REQ_DONE);
		break;

	case CMD_GIVERESULT:
		ChangeState(IGameMgr::REQ_DONE);
		break;
	}
}

bool CTexasPokerClient::OnBet(byte *data, int len)
{
	if (len != 4)	return false;
	int nMoney = *(int *)data;
	if (nMoney == -1)
	{
		auto game = m_Mgr->GetCurrentGame();
		if (game)
		{
			CString strInfo;
			strInfo.Format(_T("bet:"), nMoney);
			game->AddClientInfo(GetID(), strInfo);
		}

		SetGiveUp(true);
	}
	else
	{
		auto game = m_Mgr->GetCurrentGame();
		CString strReason;
		if (!game->AddBet(GetID(), nMoney, strReason))	// return false mean illegal value, this client game over
		{
			auto game = m_Mgr->GetCurrentGame();
			if (game)
			{
				CString strInfo;
				strInfo.Format(_T("bet:"), nMoney);
				game->AddClientInfo(GetID(), strInfo);
			}
			ChangeState(IGameMgr::REQ_GAMEOVER, strReason);
		}
	}
	return true;
}

int CTexasPokerClient::SendInitRequest()
{
	if (m_nGameState == IGameMgr::REQ_IDLE)
	{
		int byLen = 8 + 16;
		byte *pData = new byte[byLen];
		ZeroMemory(pData, byLen);

		m_nCurrentCmd = CMD_INIT;
		InitHead(pData, m_nCurrentCmd, 16);

		*(int*)&pData[8 + 0] = GetID();							// 
		*(int*)&pData[8 + 4] = m_nTotalMoney;					// 
		*(int*)&pData[8 + 8] = (int)m_Mgr->m_ltClient.size();	// 
		*(int*)&pData[8 + 12] = SET_UNIT_MONEY;					// 

		ChangeState(IGameMgr::REQ_WAIT);
		SendData(pData, byLen);
		delete[] pData;
	}
	return m_nGameState;
}

void CTexasPokerClient::SendPokerRequest(std::vector<byte> pokers, int banker, int gameid)
{
	if (pokers.size() != 5)	return;
	int byLen = 8 + 20;
	byte *pData = new byte[byLen];
	ZeroMemory(pData, byLen);

	m_nCurrentCmd = CMD_SENDPOKER;
	InitHead(pData, m_nCurrentCmd, 20);

	pData[8 + 0] = pokers[0];					// 
	pData[8 + 1] = pokers[1];					// 
	pData[8 + 2] = pokers[2];					// 
	pData[8 + 3] = pokers[3];					// 
	pData[8 + 4] = pokers[4];					// 
	memcpy(m_poker, &pData[8 + 0], 5);			// Init pokers
	*(int*)&pData[8 + 8] = banker;				// 
	*(int*)&pData[8 + 12] = SET_DEFAULT_MONEY;	// 
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
	InitHead(pData, m_nCurrentCmd, (short)(16 + nSize + 1));

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
	int byLen = 8 + 8 + nSize + 1;
	byte *pData = new byte[byLen];
	ZeroMemory(pData, byLen);

	m_nCurrentCmd = CMD_GIVERESULT;
	InitHead(pData, m_nCurrentCmd, (short)(8 + nSize + 1));

	*(int*)&pData[8 + 0] = bWin ? 1 : 0;				// 
	*(int*)&pData[8 + 4] = nTal;						// 
	memcpy(&pData[8 + 8], betInfo.GetBuffer(), nSize);	//

	ChangeState(IGameMgr::REQ_WAIT);
	SendData(pData, byLen);
	delete[] pData;
}

void CTexasPokerClient::SendGameOver(CString strReason)
{
	int nSize = strReason.GetLength() * sizeof(TCHAR);
	int byLen = 8 + 4 + nSize + 1;
	byte *pData = new byte[byLen];
	ZeroMemory(pData, byLen);

	m_nCurrentCmd = CMD_GAMEOVER;
	InitHead(pData, m_nCurrentCmd, (short)(8 + nSize + 1));

	*(int*)&pData[8 + 0] = m_nCurrentMoney;				// 
	memcpy(&pData[8 + 4], strReason.GetBuffer(), nSize);//

	ChangeState(IGameMgr::REQ_WAIT);
	SendData(pData, byLen);
	delete[] pData;
}
