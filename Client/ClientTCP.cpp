#include "stdafx.h"
#include "Gbl.h"
#include "Client.h"
#include "ClientDlg.h"
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

void InitHead(byte* pData, short cmdid, int datalen)
{
	*(short*)&pData[0] = 2;			// 1:send, 2:return
	*(short*)&pData[2] = cmdid;		// command id
	*(int*)&pData[4] = datalen;		// data len
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

struct SPack{
	int len;
	byte *p;
	SPack(int unitLen){
		p = new byte[unitLen];
		ZeroMemory(p, unitLen);
	}
	~SPack(){ delete[] p; p = NULL; }
};

void CClientTcp::OnReceive(int nErrorCode)
{
	int unitLen = 1024, allLen = 0, index = 0;

	std::list<std::shared_ptr<SPack>> ltPack;
	while (true)
	{
		std::shared_ptr<SPack> sp = std::shared_ptr<SPack>(new SPack(unitLen));
		sp->len = Receive(sp->p, unitLen);
		if (sp->len <= 0)		break;
		allLen += sp->len;
		ltPack.push_back(sp);
		if (sp->len < unitLen)	break;	// finish reading
	}
	if (allLen <= 0)			return;

	byte *pData = new byte[allLen + 2];
	ZeroMemory(pData, allLen + 2);

	for (auto sp : ltPack)
	{
		memcpy(&pData[index], sp->p, sp->len);
		index += sp->len;
	}

	m_Mgr->GetDlg()->m_CountRcv++;
	m_Mgr->GetDlg()->UpdateData(FALSE);

	// Check head
	if (pData[0] != 1)	return;	// 1:send, 2:return
	ASSERT((allLen - 8) >= *(int *)&pData[4]);
	if ((allLen - 8) > *(int *)&pData[4])
	{
		int l = allLen - 8 - *(int *)&pData[4];
		byte *p = &pData[8 + *(int *)&pData[4]];
		CString str, s;
		for (int i = 0; i < l; i++)
		{
			s.Format(_T("%.2X "), p[i]);
			str += s;
		}
		MessageBox(NULL, str, _T("Error byte"), MB_OK);
	}
	int dataLen = *(int *)&pData[4];

	switch (*(short *)&pData[2])	// command id
	{
	case CMD_INIT:
		m_Mgr->ClearPackage();
		KeepPackage(false, pData, allLen);
		OnClientInit(&pData[8], dataLen);
		break;

	case CMD_SENDPOKER:
		KeepPackage(false, pData, allLen);
		OnReceivePoker(&pData[8], dataLen);
		break;

	case CMD_BET:
		KeepPackage(false, pData, allLen);
		OnReceiveBetRequest(&pData[8], dataLen);
		break;

	case CMD_GIVERESULT:
		KeepPackage(false, pData, allLen);
		OnReceiveResult(&pData[8], dataLen);
		break;

	case CMD_GAMEOVER:
		KeepPackage(false, pData, allLen);
		OnGameOver(&pData[8], dataLen);
		break;

	default:
		KeepPackage(false, pData, allLen);
		break;
	}

	delete[] pData;
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

	m_Mgr->OnInit();

	int nSize = name.GetLength() * sizeof(TCHAR);
	byte *pRet = new byte[8 + nSize + 1];
	memset(pRet, 0, 8 + nSize + 1);
	InitHead(pRet, CMD_INIT, nSize + 1);
	memcpy(&pRet[8], name.GetBuffer(), nSize);

	SendData(pRet, 8 + nSize + 1);
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

	m_Mgr->OnReceivePokers();

	byte pRet[8] = { 0 };
	InitHead(pRet, CMD_SENDPOKER, 0);
	SendData(pRet, 8);
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

	//int nBetMoney = game->GetBetMoney(nMax, nPrevBet, nMyBet, nTotal, mpBetMoney);
	int nBetMoney = m_Mgr->RequireBetMoney(nMax, nPrevBet, nMyBet, nTotal, strAllBet);

	byte pRet[8 + 4] = { 0 };
	InitHead(pRet, CMD_BET, 0);
	*(int *)&pRet[8] = nBetMoney;
	SendData(pRet, 8 + 4);
}

void CClientTcp::OnReceiveResult(byte *data, int len)
{
	auto game = m_Mgr->CurrentGame();

	bool bIfWin = *(int *)&data[0] == 1 ? true : false;
	int nWinMoney = *(int *)&data[4];

	if (bIfWin)	m_Mgr->AddWinMoney(nWinMoney);

	CString strResultInfo((TCHAR*)&data[8]);
	game->SetResultInfo(strResultInfo);

	m_Mgr->OnOneGameOver(bIfWin, nWinMoney, strResultInfo);

	byte pRet[8] = { 0 };
	InitHead(pRet, CMD_GIVERESULT, 0);
	SendData(pRet, 8);
}

void CClientTcp::OnGameOver(byte *data, int len)
{
	auto game = m_Mgr->CurrentGame();
	int nNumber = *(int *)data;
	int nMoney = *(int *)&data[4];
	CString strProcess((TCHAR*)&data[8]);
	m_Mgr->OnAllGameOver(nNumber, nMoney, strProcess);

	byte pRet[8] = { 0 };
	InitHead(pRet, CMD_GAMEOVER, 0);
	SendData(pRet, 8);
}

void CClientTcp::SendData(byte *data, int len)
{
	m_Mgr->GetDlg()->m_CountSend++;
	m_Mgr->GetDlg()->UpdateData(FALSE);
	KeepPackage(true, data, len);
	Send(data, len);
}

void CClientTcp::KeepPackage(bool bSend, byte *data, int len)
{
	CString strOut = CGbl::GetCurrentTimeStr();
	strOut += bSend ? _T(" -> ") : _T(" <- ");
	CString str;
	for (int i = 0; i < len; i++)
	{
		str.Format(_T("%.2X "), data[i]);
		strOut += str;
	}
	m_Mgr->AddPackage(strOut);
}
