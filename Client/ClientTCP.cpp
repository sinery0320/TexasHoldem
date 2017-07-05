#include "stdafx.h"
#include "Gbl.h"
#include "ClientTcp.h"
#include "GameMgr.h"
#include "Game.h"

const short CMD_INIT = 1;
const short CMD_SENDPOKER = 2;
const short CMD_BET = 3;
const short CMD_GIVERESULT = 4;
const short CMD_GAMEOVER = 5;

CClientTcp::CClientTcp(CGameMgr *mgr)
	: m_Mgr(mgr)
	, m_bConnect(FALSE)
{
	BOOL b = ((CSocket *)this)->Create(NULL, SOCK_STREAM);
	ASSERT(b);
}


CClientTcp::~CClientTcp()
{
}

void InitHead(byte* pData, short cmdid, short datalen)
{
	*(short*)&pData[0] = 2;			// 1:send, 2:return
	*(short*)&pData[2] = cmdid;		// command id
	*(short*)&pData[4] = 0;			// error type, 0-no error
	*(short*)&pData[6] = datalen;	// data len
}

BOOL CClientTcp::ConnectTo(CString strIP, UINT uiPort)
{
	if (m_bConnect)
	{
		Close();
	}
	m_bConnect = Connect(strIP, uiPort);
	return m_bConnect;
}

BOOL CClientTcp::ReadData()
{
	byte data[DATA_BUF_SIZE] = { 0 };
	int nRet = Send(data, 10);
	ASSERT(nRet != SOCKET_ERROR);
	return nRet != SOCKET_ERROR;
}

void CClientTcp::OnReceive(int nErrorCode)
{
	byte data[DATA_BUF_SIZE] = { 0 };
	int len = Receive(data, DATA_BUF_SIZE);
	ASSERT(len >= DATA_MIN_SIZE && len <= DATA_MAX_SIZE);
	if (len < 0)		return;

	// Check head
	if (data[0] != 1)	return;	// 1:send, 2:return
	if ((len - 8) != *(short *)&data[6])
		return;					// data len

	switch (*(short *)&data[2])	// command id
	{
	case CMD_INIT:
		OnClientInit(&data[8], len - 8);
		break;

	case CMD_SENDPOKER:
		OnReceivePoker(&data[8], len - 8);
		break;

	case CMD_BET:
		OnReceiveBetRequest(&data[8], len - 8);
		break;

	case CMD_GIVERESULT:
		OnReceiveResult(&data[8], len - 8);
		break;

	case CMD_GAMEOVER:
		OnGameOver(&data[8], len - 8);
		break;
	}

	CSocket::OnReceive(nErrorCode);
}

void CClientTcp::OnClose(int nErrorCode)
{
	CSocket::OnClose(nErrorCode);
}

void CClientTcp::OnClientInit(byte *data, int len)
{
	m_Mgr->Init();
	m_Mgr->SetID(*(int *)&data[0]);
	m_Mgr->SetTotalMoney(*(int *)&data[4]);
	m_Mgr->SetPlayerCount(*(int *)&data[8]);
	m_Mgr->SetBetUnit(*(int *)&data[12]);

	CString name = m_Mgr->GetName();
	int nSize = name.GetLength() * sizeof(TCHAR);
	byte *pRet = new byte[8 + nSize + 1];
	memset(pRet, 0, 8 + nSize + 1);
	InitHead(pRet, CMD_INIT, nSize + 1);
	memcpy(&pRet[8], name.GetBuffer(), nSize);

	Send(pRet, 8 + nSize + 1);
	delete[] pRet;
}

void CClientTcp::OnReceivePoker(byte *data, int len)
{
	auto game = m_Mgr->CreateNewGame();
	game->SetPoker(data);
	game->SetBankerID(*(int *)&data[8]);
	if (game->GetBankerID() == m_Mgr->GetID())
	{
		m_Mgr->AddBetMoney(*(int *)&data[12]);
	}
	game->SetID(*(int *)&data[16]);

	byte pRet[8] = { 0 };
	InitHead(pRet, CMD_SENDPOKER, 0);
	Send(pRet, 8);
}

void CClientTcp::OnReceiveBetRequest(byte *data, int len)
{
	auto game = m_Mgr->CurrentGame();
	int nMax = *(int *)&data[0];
	int nPrevBet = *(int *)&data[4];
	int nMyBet = *(int *)&data[8];
	int nTotal = *(int *)&data[12];
	CString strAllBet((TCHAR*)&data[16]);

	std::vector<CString> vtClient;
	std::map<int, int> mpBetMoney;	// first is id, second is bet money
	CGbl::SpliteBy(strAllBet, _T("\r\n"), vtClient);
	for (int i = 0; i < (int)vtClient.size(); i++)
	{
		std::vector<CString> vtInfGroup;
		CGbl::SpliteBy(vtClient[i], _T(","), vtInfGroup);

		int id = -1, money = 0;
		for (int j = 0; j < (int)vtInfGroup.size(); j++)
		{
			std::vector<CString> vtPair;
			CGbl::SpliteBy(vtInfGroup[j], _T(":"), vtPair);

			if (vtPair.size() != 2)	continue;
			if (vtPair[0] == _T("id"))			id = _ttoi(vtPair[1]);
			else if (vtPair[0] == _T("bet"))	money = _ttoi(vtPair[1]);
		}
		if (id != -1) mpBetMoney[id] = money;
	}

	int nBetMoney = game->GetBetMoney(nMax, nPrevBet, nMyBet, nTotal, mpBetMoney);

	byte pRet[8 + 4] = { 0 };
	InitHead(pRet, CMD_BET, 0);
	*(int *)&pRet[8] = nBetMoney;
	Send(pRet, 8 + 4);
}

void CClientTcp::OnReceiveResult(byte *data, int len)
{
	auto game = m_Mgr->CurrentGame();

	bool bIfWin = *(int *)&data[0] == 1 ? true : false;
	int nWinMoney = *(int *)&data[4];

	if (bIfWin)	m_Mgr->AddWinMoney(nWinMoney);

	CString strAllBet((TCHAR*)&data[8]);
	game->SetResultInfo(strAllBet);

	byte pRet[8] = { 0 };
	InitHead(pRet, CMD_GIVERESULT, 0);
	Send(pRet, 8);
}

void CClientTcp::OnGameOver(byte *data, int len)
{
	auto game = m_Mgr->CurrentGame();

	CString strReason((TCHAR*)&data[4]);
	MessageBox(NULL, strReason, _T("Game over"), MB_OK | MB_ICONINFORMATION);
}
