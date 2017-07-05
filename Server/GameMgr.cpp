#include "stdafx.h"
#include "GameSevDlg.h"
#include "ServerTcp.h"
#include "GameMgr.h"
#include "Client.h"
#include "ClientTcp.h"

using namespace Game;
IGameMgr::IGameMgr(CGameSevDlg *dlg)
	: m_Dlg(dlg)
	, m_nServerState(SEV_IDEL)
	, m_bInTimer(false)
{
}


IGameMgr::~IGameMgr()
{
}

bool IGameMgr::StartListen(unsigned short wPort)
{
	if (m_nServerState != SEV_IDEL)	return false;
	m_pServerTcp = std::shared_ptr<TCP::CServerTcp>(new TCP::CServerTcp(m_Dlg));
	if (0 == m_pServerTcp->CreateTcp(wPort))
	{
		m_nServerState = SEV_LISTEN;
		return true;
	}
	return false;
}

bool IGameMgr::StopListen()
{
	if (m_nServerState != SEV_LISTEN)	return false;
	m_pServerTcp->Close();
	m_pServerTcp.reset();
	m_nServerState = SEV_IDEL;
	return true;
}

bool IGameMgr::StartGame()
{
	if (m_nServerState != SEV_LISTEN)	return false;
	if (m_ltClient.size() <= 0)			return false;
	for (auto pClient : m_ltClient)
	{
		pClient->InitData();
	}

	m_nServerState = SEV_START;
	return true;
}

void IGameMgr::StopGame()
{
	m_nServerState = SEV_IDEL;
	StopListen();
	m_ltClient.clear();
}

void IGameMgr::OnGameOver()
{
	m_nServerState = SEV_LISTEN;
	m_Dlg->OnStateChanged();
}

void IGameMgr::OnTimer100MSec()
{
	if (m_bInTimer)						return;
	if (m_nServerState != SEV_START)	return;
	if (m_ltClient.empty())				return;
	m_bInTimer = true;
	OnTimer100MillSec();
	m_bInTimer = false;
}

