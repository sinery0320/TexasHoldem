#include "stdafx.h"
#include "GameSevDlg.h"
#include "AuctionMgr.h"
#include "Art.h"
#include "Client.h"
#include "ClientTcp.h"
#include "AuctionClient.h"

using namespace Game;
using namespace Auction;

CAuctionMgr::CAuctionMgr(CGameSevDlg *dlg)
	: Game::IGameMgr(dlg)
	, m_nGameState(GM_IDEL)
	, m_nArtCount(100)
	, m_nAccountInit(100000)
	, m_nAuctionTimes(0)
{
}


CAuctionMgr::~CAuctionMgr()
{
}

void CAuctionMgr::CreateClient(TCP::CClientTcp* tcp)
{
	std::shared_ptr<Game::IClient> pClient = std::shared_ptr<Game::IClient>(new CAuctionClient(this));
	pClient->SetID(m_ltClient.size());
	pClient->SetTcp(tcp);
	m_ltClient.push_back(pClient);
}

bool CAuctionMgr::StartGame()
{
	if (!Game::IGameMgr::StartGame())	return false;
	if (m_nArtCount <= 0)		return false;
	if (m_nAccountInit <= 0)	return false;

	m_ltArt.clear();
	m_ltArtSold.clear();
	m_ltArtFailed.clear();
	for (int i = 0; i < m_nArtCount; ++i)
	{
		std::shared_ptr<CArt> pArt = std::shared_ptr<CArt>(new CArt(m_ltArt.size(), 100 + 10 * i));
		m_ltArt.push_back(pArt);
	}
	m_nGameState = GM_IDEL;
	return false;
}

void CAuctionMgr::StopGame()
{
	Game::IGameMgr::StopGame();
	m_nGameState = GM_IDEL;
}

void CAuctionMgr::OnTimer100MillSec()
{
	bool bOK = true;
	switch (m_nGameState)
	{
	case GM_IDEL:
		m_nGameState = GM_INIT;
		break;

	case GM_INIT:
		for (std::shared_ptr<Game::IClient> pClient : m_ltClient)
		{
			if (!((Auction::CAuctionClient *)pClient.get())->SendInitRequest(m_ltArt.size(), m_ltClient.size()))
				bOK = false;
		}
		if (bOK)  m_nGameState = GM_GAME;
		break;

	case GM_GAME:
		if (SendArtToClient())
			m_nGameState = GM_END;
		break;
	}
}

// if arts empty, auction game is done, return true 
bool CAuctionMgr::SendArtToClient()
{
	//if (m_ltArt.empty())	return true;
	//int nReqType = 0;
	//for (std::shared_ptr<Game::IClient> pClient : m_ltClient)
	//{
	//	int nRet = ((Auction::CAuctionClient *)pClient.get())->RequestForArt(m_ltArt.front(), m_nAuctionTimes);
	//	if (nReqType < nRet)
	//	nReqType = nRet;
	//}
	//switch (nReqType)
	//{
	//	std::shared_ptr<CArt> art = m_ltArt.front();
	//	m_ltArt.pop_back();
	//	if (art->IsFailed())
	//		m_ltArtFailed.push_back(art);
	//	else
	//		m_ltArtSold.push_back(art);
	//}
	return false;
}