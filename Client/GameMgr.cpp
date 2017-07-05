#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"
#include "GameMgr.h"
#include "ClientTcp.h"
#include "Game.h"

CGameMgr::CGameMgr(CClientDlg *dlg)
	: m_pDlg(dlg)
	, m_nID(-1)
	, m_nPlayerCount(0)
	, m_nTotalMoney(0)
	, m_nCurrentMoney(0)
	, m_nBetUnitMoney(0)
	, m_nBetMoney(0)
{
	m_Player = std::shared_ptr<CPlayer>(new CPlayer());
	StopGame();
}

CGameMgr::~CGameMgr()
{
	StopGame();
}

void CGameMgr::Init()
{
	m_nID = -1;
	m_nPlayerCount = 0;
	m_nTotalMoney = 0;
	m_nCurrentMoney = 0;
	m_nBetUnitMoney = 0;
	m_nBetMoney = 0;
	m_vtGame.clear();
}

BOOL CGameMgr::StartGame(int nIP, int nPort)
{
	if (!m_pClientTcp)
	{
		m_pClientTcp = std::shared_ptr<CClientTcp>(new CClientTcp(this));
	}

	CString strIP;
	byte *byIP = (byte *)&nIP;
	strIP.Format(_T("%d.%d.%d.%d"), byIP[3], byIP[2], byIP[1], byIP[0]);
	BOOL b = m_pClientTcp->ConnectTo(strIP, nPort);
	return b;
}

BOOL CGameMgr::StopGame()
{
	if (m_pClientTcp)
		m_pClientTcp.reset();
	m_nID = -1;
	m_nPlayerCount = 0;
	m_nTotalMoney = 0;
	m_nCurrentMoney = 0;
	m_nBetUnitMoney = 0;
	m_nBetMoney = 0;
	m_vtGame.clear();
	return TRUE;
}

std::shared_ptr<CGame> CGameMgr::CreateNewGame()
{
	auto game = std::shared_ptr<CGame>(new CGame(this, m_vtGame.size()));
	m_vtGame.push_back(game);
	return game;
}

void CGameMgr::AddBetMoney(int money)
{
	m_nCurrentMoney -= money;
}

void CGameMgr::AddWinMoney(int money)
{
	m_nCurrentMoney += money;
}

CString CGameMgr::GetName()
{
	return _T("Atlas Liu");
}

BOOL CGameMgr::IsGiveUp()
{
	return m_pDlg->m_bGiveUp;
}

void CGameMgr::FillGrid(CListCtrl& lcGame)
{
	CString str;
	lcGame.DeleteAllItems();
	for (int i = 0; i < (int)m_vtGame.size(); i++)
	{
		std::shared_ptr<CGame> game = m_vtGame[i];
		str.Format(_T("%d"), game->GetGameID());
		lcGame.InsertItem(i, str);
		lcGame.SetItemText(i, CClientDlg::COL_INFO, game->GetInfoStr());
	}
}

