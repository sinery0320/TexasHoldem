#include "stdafx.h"
#include "Gbl.h"
#include "GameSevDlg.h"
#include "TexasPokerMgr.h"

#include "Poker.h"
#include "Client.h"
#include "ClientTcp.h"
#include "TexasPokerClient.h"
#include "TexasGame.h"
#include "TexasPokerResult.h"

using namespace Game;
using namespace Texas;

CTexasPokerMgr::CTexasPokerMgr(CGameSevDlg *dlg)
	: Game::IGameMgr(dlg)
	, m_nGameState(GM_IDEL)
{
	CTexasPokerResult::GetMe();
}

CTexasPokerMgr::~CTexasPokerMgr()
{
}

void CTexasPokerMgr::FillGrid(CListCtrl& lcClient, CListCtrl& lcGame)
{
	int row = 0;
	CString str;
	//lcClient.DeleteAllItems();
	for (std::shared_ptr<IClient> pClient : m_vtClient)
	{
		if (lcClient.GetItemCount() <= row)
		{
			str.Format(_T("%d"), pClient->GetID());
			lcClient.InsertItem(row, str);
		}
		lcClient.SetItemText(row, CGameSevDlg::COL_CL_NAME, pClient->GetName());
		lcClient.SetItemText(row, CGameSevDlg::COL_CL_INFO, pClient->GetInfoStr());
		row++;
	}

	row = 0;
	//lcGame.DeleteAllItems();
	for (std::shared_ptr<CTexasGame> game : m_vtGame)
	{
		if (lcGame.GetItemCount() <= row)
		{
			str.Format(_T("%d"), game->GetGameID());
			lcGame.InsertItem(row, str);
		}
		lcGame.SetItemText(row, CGameSevDlg::COL_GM_INFO, game->GetInfoStr());
		row++;
	}
}

void CTexasPokerMgr::ShowGameInfo(int gameID)
{
	if (gameID < 0 || gameID >= (int)m_vtGame.size())	return;
	auto game = m_vtGame[gameID];
	game->ShowInfo();
}

void CTexasPokerMgr::CreateClient(TCP::CClientTcp* tcp)
{
	std::shared_ptr<IClient> pClient = std::shared_ptr<IClient>(new CTexasPokerClient(this));
	pClient->SetTcp(tcp);
	m_ltClient.push_back(pClient);
}

void CTexasPokerMgr::OnClientDisconnect(TCP::CClientTcp* tcp)
{
	if (m_nGameState == GM_INIT || m_nGameState == GM_GAME)
	{
		for (auto pClient : m_ltClient)
		{
			if (pClient->GetTcp() == tcp)
			{
				auto client = (CTexasPokerClient *)pClient.get();
				client->ChangeState(IGameMgr::REQ_GAMEOVER, _T("This connect disconnected"));
				return;
			}
		}
	}
}

void CTexasPokerMgr::ReSortClient()
{
	// reset client id
	//int count = m_ltClient.size();
	//m_vtClient.resize(count);
	//for (std::shared_ptr<IClient> client : m_ltClient)
	//{
	//	int id = CGbl::GetMe().GetRand(count - 1);
	//	while (m_vtClient[id])
	//	{
	//		id = CGbl::GetMe().GetRand(count - 1);
	//	}
	//	client->SetID(id);
	//	m_vtClient[id] = client;
	//}
	m_vtClient.clear();
	for (std::shared_ptr<IClient> client : m_ltClient)
	{
		client->SetID((int)m_vtClient.size());
		m_vtClient.push_back(client);
	}
	m_nCurID = 0;
}

bool CTexasPokerMgr::StartGame()
{
	if (!Game::IGameMgr::StartGame())	return false;

	ReSortClient();
	m_nCurID = 0;		// current client id
	m_vtGame.clear();
	m_ltLoserID.clear();
	m_nGameState = GM_INIT;
	return true;
}

void CTexasPokerMgr::StopGame()
{
	Game::IGameMgr::StopGame();
	m_nGameState = GM_IDEL;
}

void CTexasPokerMgr::AddLoser(int id)
{
	if (id < 0 || id >= (int)m_vtClient.size())	return;
	auto pClient = m_vtClient[id];
	auto client = (CTexasPokerClient *)pClient.get();
	client->SetLoserNumber(m_ltLoserID.size());
	m_ltLoserID.push_back(id);
}

bool CTexasPokerMgr::CheckGameOver()
{
	int nCount = 0;
	for (std::shared_ptr<IClient> client : m_ltClient)
	{
		if (!client->IsClientGameOver())
		{
			nCount++;
		}
	}
	return nCount < 2;
}

void CTexasPokerMgr::OnTimer100MillSec()
{
	bool bOK = true;
	std::shared_ptr<CTexasGame> game;
	switch (m_nGameState)
	{
	case GM_IDEL:
		break;

	case GM_INIT:
		for (std::shared_ptr<Game::IClient> pClient : m_ltClient)
		{
			int nState = pClient->SendInitRequest();
			if (nState != Game::IGameMgr::REQ_DONE && nState != Game::IGameMgr::REQ_OVERTIME)
				bOK = false;
		}
		if (bOK)  m_nGameState = GM_GAME;
		break;

	case GM_GAME:
		if (!m_vtGame.empty())	game = m_vtGame.back();
		if (!game || game->GetGameState() == CTexasGame::PS_DONE)
		{
			if (CheckGameOver())
			{
				m_nGameState = GM_END;
				break;
			}
			else if (m_vtGame.size() >= GAME_MAX)
			{
				m_nGameState = GM_END;
				break;
			}
			game = std::shared_ptr<CTexasGame>(new CTexasGame(this, m_vtGame.size()));
			if (!game->InitGame())
			{
				m_nGameState = GM_END;
				break;
			}
			m_vtGame.push_back(game);
		}
		game->PlayGame();
		break;

	case GM_END:
		m_nGameState = GM_IDEL;
		OnGameOver();
		ShowResult();
		break;
	}
}

std::shared_ptr<CTexasGame> CTexasPokerMgr::GetCurrentGame()
{
	std::shared_ptr<CTexasGame> p;
	if (m_vtGame.empty())
		return p;
	return m_vtGame.back();
}

void CTexasPokerMgr::ShowResult()
{
	std::list<std::shared_ptr<IClient>> ltClient;

	// get winners
	for (auto c : m_ltClient)
	{
		if (!c->IsClientGameOver())
			ltClient.push_back(c);
	}
	if (ltClient.size() > 1)
	{
		ltClient.sort([this](std::shared_ptr<IClient> lth, std::shared_ptr<IClient> rth)->bool
		{
			return ((CTexasPokerClient *)lth.get())->GetCurrentMoney() > ((CTexasPokerClient *)rth.get())->GetCurrentMoney();
		});
	}

	// get losers
	for (auto iter = m_ltLoserID.rbegin(); iter != m_ltLoserID.rend(); iter++)
	{
		ltClient.push_back(m_vtClient[*iter]);
	}

	// get result string
	CString strResult, str;
	strResult = _T("The game is over, below is the result: \r\n\r\n");
	int i = 0;
	for (auto pClient : ltClient)
	{
		auto client = (CTexasPokerClient *)pClient.get();
		str.Format(_T("No. %d, ID is %d, Current money %d"), ++i, client->GetID(), client->GetCurrentMoney());
		strResult = strResult + str + _T("\r\n");
	}

	HWND h = (::AfxGetMainWnd() != NULL) ? ::AfxGetMainWnd()->GetSafeHwnd() : NULL;
	MessageBox(h, strResult, _T("Result"), MB_OK | MB_ICONINFORMATION);
}

