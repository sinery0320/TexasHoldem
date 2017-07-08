#include "stdafx.h"
#include "GameServer.h"
#include "GameInfoDlg.h"
#include "Gbl.h"

#include "TexasGame.h"
#include "TexasPokerMgr.h"
#include "TexasPokerClient.h"
#include "TexasPokerResult.h"
#include "Poker.h"

using namespace Game;
using namespace Texas;

CTexasGame::CTexasGame(CTexasPokerMgr *mgr, int game_id)
	: m_Mgr(mgr)
	, m_nGameID(game_id)
	, m_nGameState(PS_IDLE)
	, m_nBankerID(0)			// The banker id
	, m_nCurClientID(1)			// Current client id
	, m_nTotalMoney(0)
{
}


CTexasGame::~CTexasGame()
{
}

CString CTexasGame::GetInfoStr()
{
	CString strInfo;
	strInfo.Format(_T("bankerid:%d, state:%d, allmoney:%d"),
		m_nBankerID,
		m_nGameState,
		m_nTotalMoney);
	if (!m_ltWinID.empty())
	{
		strInfo += _T(", winid:");
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

// Init game, make sure who is the banker in this game
bool CTexasGame::InitGame()
{
	for (std::shared_ptr<IClient> client : m_Mgr->m_ltClient)
	{
		client->InitOneGame();
	}

	CheckAllClient();

	if (CheckGameOver())
		return false;

	m_vtClinetInfo.resize(m_Mgr->m_vtClient.size());

	m_nBankerID = m_nGameID % m_Mgr->m_ltClient.size();

	std::shared_ptr<IClient> pClient = m_Mgr->m_vtClient[m_nBankerID];
	if (pClient->IsClientGameOver())	// if it's game over, find next id
		m_nBankerID = GetNextID(m_nBankerID);

	ASSERT(m_nBankerID >= 0);


	
	m_nGameState = PS_SEND_POKER;
	return true;
}

void CTexasGame::PlayGame()
{
	switch (m_nGameState)
	{
	case PS_SEND_POKER:
		if (CheckGameOver())
		{
			m_nGameState = PS_GIVE_RESULT_HIDE;
		}
		else
		{
			SendPoker();
		}
		break;

	case PS_SEND_POKER_WAIT:
		if (m_Mgr->CheckAllClientReturned())
		{
			m_nGameState = PS_SEND_BET;
			m_nCurClientID = GetNextID(m_nBankerID);
		}
		break;

	case PS_SEND_BET:
		if (CheckGameOver())
		{
			m_nGameState = PS_GIVE_RESULT_HIDE;
		}
		SendBet();
		break;

	case PS_SEND_BET_WAIT:
		if (m_Mgr->CheckOneClientReturned(m_nCurClientID))
		{
			// check if game over
			int nCheck = CheckBetOver();
			switch (nCheck)
			{
			// Murphy added
			case -1:m_nCurClientID = m_nBankerID; m_nGameState = PS_GIVE_RESULT_HIDE; break;
			case 0:	m_nCurClientID = GetNextID(m_nCurClientID); m_nGameState = PS_SEND_BET; break;
			case 1: m_nGameState = PS_GIVE_RESULT_HIDE; break;
			case 2: m_nGameState = PS_GIVE_RESULT_SHOW; break;
			}
		}
		break;

	case PS_GIVE_RESULT_HIDE:
		SendResult(false);
		break;

	case PS_GIVE_RESULT_SHOW:
		SendResult(true);
		break;

	case PS_GIVE_RESULT_WAIT:
		if (m_Mgr->CheckAllClientReturned())
		{
			m_nGameState = PS_DONE;
			for (int i = 0; i < (int)m_Mgr->m_vtClient.size(); i++)
			{
				auto pClient = m_Mgr->m_vtClient[i];
				auto client = (CTexasPokerClient *)pClient.get();
				if (!client->IsClientGameOver() && client->IsGiveUp())
					client->CheckGameOver();
			}
		}
		break;
	}
}

void CTexasGame::SendPoker()
{
	byte poker[52] = { 0 };
	CGbl *gbl = &CGbl::GetMe();

	std::function<byte()> GetRandPoker = [&]() -> byte { 
		byte rd = (byte)gbl->GetRand(52);
		while (poker[rd] != 0)
		{
			rd = (byte)gbl->GetRand(52);
		}
		return rd;
	};

	// init open hand hide
	for (int i = 0; i < 3; i++)
	{
		byte rd = GetRandPoker();
		poker[rd] = 1;
		m_vtOpenPoker[i] = rd;
	}

	// init and send poker to every client
	std::vector<byte> vtpk;
	vtpk.resize(5);
	vtpk[2] = m_vtOpenPoker[0];
	vtpk[3] = m_vtOpenPoker[1];
	vtpk[4] = m_vtOpenPoker[2];
	for (int i = 0; i < (int)m_Mgr->m_vtClient.size(); i++)
	{
		std::shared_ptr<IClient> pClient = m_Mgr->m_vtClient[i];
		CTexasPokerClient *client = (CTexasPokerClient *)pClient.get();
		if (client->IsClientGameOver())		continue;	// will not send pokers to the one who is game over

		if (client->GetID() == m_nBankerID)
		{
			client->AddBetMoeny(CTexasPokerMgr::GAME_DEFAULT_MONEY);
			m_nTotalMoney += CTexasPokerMgr::GAME_DEFAULT_MONEY;
			AddClientInfo(client->GetID(), _T("banker:1"));
		}
		else
		{
			AddClientInfo(client->GetID(), _T("banker:0"));
		}

		vtpk[0] = GetRandPoker();
		poker[vtpk[0]] = 1; // poker array not set
		vtpk[1] = GetRandPoker();
		poker[vtpk[1]] = 1; // poker array not set
		client->SendPokerRequest(vtpk, m_nBankerID, GetGameID());
		AddClientInfo(i, _T("opk:") + CTexasPokerResult::GetPokerString(m_vtOpenPoker, 3));
		AddClientInfo(i, _T("pk:") + client->GetPokerAll());
	}

	CString strClientInfo;
	strClientInfo.Format(_T("bet:%d"), CTexasPokerMgr::GAME_DEFAULT_MONEY);
	AddClientInfo(m_nBankerID, strClientInfo);

	m_nGameState = PS_SEND_POKER_WAIT;
}

// Send bet request
void CTexasGame::SendBet()
{
	// Murphy Added
	if (m_nCurClientID < 0)
	{
		m_nGameState = PS_GIVE_RESULT_WAIT;
		return;
	}

	ASSERT(m_nCurClientID >= 0);
	auto pClient = m_Mgr->m_vtClient[m_nCurClientID];
	auto client = (CTexasPokerClient *)pClient.get();
	int nPrevID = GetPrevID(m_nCurClientID);
	auto prevClient = (CTexasPokerClient *)m_Mgr->m_vtClient[nPrevID].get();

	int nMax = GetMaxBetMoney();
	int nTal = m_nTotalMoney;
	int nPrevBet = prevClient->GetBetMoney();
	int nYourBet = client->GetBetMoney();
	//CString strAllBet = GetAllBet();

	// init bet string
	CString strAllBet = _T(""), str;
	for (std::shared_ptr<IClient> pClient : m_Mgr->m_vtClient)
	{
		if (pClient->IsClientGameOver())	continue;	// will not send bet request to the one who is game over
		CTexasPokerClient *client = (CTexasPokerClient *)pClient.get();
		if (client->IsGiveUp())				continue;	// will not send bet request to the one who is give up in this game
		str.Format(_T("id:%d,bet:%d\r\n"), client->GetID(), client->GetBetMoney());
		strAllBet += str;
	}

	m_nGameState = PS_SEND_BET_WAIT;
	client->SendBetRequest(nMax, nPrevBet, nYourBet, nTal, strAllBet);
}

// Send result
void CTexasGame::SendResult(bool bShow)
{
	// find winners
	m_ltWinID = GetWinners();
	int nWinMoney = m_nTotalMoney / (int)m_ltWinID.size();
	std::map<int, std::shared_ptr<IClient>> mpWinner;
	for (int i : m_ltWinID)
	{
		std::shared_ptr<IClient> pWinner = m_Mgr->m_vtClient[i];
		pWinner->WinTheGame(nWinMoney);
		mpWinner[i] = pWinner;
	}

	// init result
	CString strAllBet = _T(""), str;
	for (std::shared_ptr<IClient> pClient : m_Mgr->m_vtClient)
	{
		CTexasPokerClient *client = (CTexasPokerClient *)pClient.get();
		str.Format(_T("id:%d,bet:%d,gg:%d,win:%d"),
			client->GetID(),
			client->GetBetMoney(),
			client->IsGiveUp() ? 1 : 0,
			mpWinner.find(client->GetID()) != mpWinner.end() ? 1 : 0);

		if (bShow && !client->IsClientGameOver())
		{
			//CString s = _T(",pk:%") + client->GetPokerHide();
			//s.Format(_T(",pk:%s"), CTexasPokerResult::GetPokerString(client->GetPokers(), 2));
			str = str + _T(",pk:%") + client->GetPokerHide();
		}
		strAllBet = strAllBet + str + _T("\r\n");
	}

	// send result
	for (int i = 0; i < (int)m_Mgr->m_vtClient.size(); i++)
	{
		std::shared_ptr<IClient> pClient = m_Mgr->m_vtClient[i];
		CTexasPokerClient *client = (CTexasPokerClient *)pClient.get();

		if (client->GetClientGameState() != IGameMgr::REQ_OVERTIME)
			client->SendResultRequest(mpWinner.find(i) != mpWinner.end(), nWinMoney, strAllBet);
	}
	m_nGameState = PS_GIVE_RESULT_WAIT;
}

bool CTexasGame::AddBet(int clientID, int nMoney, CString& strReason)
{
	if (clientID != m_nCurClientID)	{ ASSERT(FALSE);  return false; }
	auto client = (CTexasPokerClient *)m_Mgr->m_vtClient[clientID].get();

	if ((nMoney % CTexasPokerMgr::GAME_UNIT_MONEY) > 0)
	{
		strReason.Format(_T("Game id %d, the bet money %d must be integer multiple of %d"), GetGameID(), nMoney, CTexasPokerMgr::GAME_UNIT_MONEY);
		return false;
	}

	if (nMoney > GetMaxBetMoney())		// illegal value, bet request has told client the how much the max money is.
	{
		strReason.Format(_T("Game id %d, the bet money %d must not > min left %d"), GetGameID(), nMoney, GetMaxBetMoney());
		return false;
	}

	if (client->GetCurrentMoney() < nMoney)	// illegal value
	{
		strReason.Format(_T("Game id %d, current money %d < bet money %d"), GetGameID(), client->GetCurrentMoney(), nMoney);
		return false;
	}
	client->AddBetMoeny(nMoney);

	auto clientPrev = (CTexasPokerClient *)m_Mgr->m_vtClient[GetPrevID(clientID)].get();
	if (client->GetBetMoney() < clientPrev->GetBetMoney())	// illegal value, your bet money < prev client bet money
	{
		client->AddBetMoeny(0 - nMoney);
		strReason.Format(_T("Game id %d, all your bet money %d < previous bet money %d"), GetGameID(), client->GetBetMoney(), clientPrev->GetBetMoney());
		return false;
	}
	m_nTotalMoney += nMoney;
	return true;
}

void CTexasGame::CheckAllClient()
{
	for (int i = 0; i < (int)m_Mgr->m_vtClient.size(); i++)
	{
		auto pClient = m_Mgr->m_vtClient[i];
		auto client = (CTexasPokerClient *)pClient.get();
		if (!client->IsClientGameOver() && !client->IsGiveUp())
			client->CheckGameOver();
	}
}

bool CTexasGame::CheckGameOver()
{
	int count = 0;
	for (int i = 0; i < (int)m_Mgr->m_vtClient.size(); i++)
	{
		auto pClient = m_Mgr->m_vtClient[i];
		auto client = (CTexasPokerClient *)pClient.get();
		if (!client->IsClientGameOver() && !client->IsGiveUp())
			count++;
	}
	if (count < 2)	// count is 0 or 1, game over
	{
		m_nGameState = PS_DONE;
		return true;
	}
	return false;	
}

// -1-over but all player died, 0-not over, 1-over and only one player alive, 2-over and multi player alive
int CTexasGame::CheckBetOver()
{
	int nMoney = -1;
	if (CheckGameOver())	return 1;

	for (auto pClient : m_Mgr->m_vtClient)
	{
		auto client = (CTexasPokerClient *)pClient.get();
		if (!client->IsClientGameOver() && !client->IsGiveUp())
		{
			if (nMoney == -1)
			{
				nMoney = client->GetBetMoney();
			}
			else if (nMoney != client->GetBetMoney())
			{
				return 0;
			}
			}
		}

	// Murphy Added
	if (nMoney == -1)
	{
		return -1;
	}

	return 2;
}

int CTexasGame::GetPrevID(int nID)
{
	int nPrevID = nID - 1;
	while (true)
	{
		if (nPrevID < 0)
			nPrevID = m_Mgr->m_ltClient.size() - 1;

		auto pClient = m_Mgr->m_vtClient[nPrevID];
		auto client = (CTexasPokerClient *)pClient.get();
		if (!client->IsClientGameOver() && !client->IsGiveUp())
			return nPrevID;


		if (nPrevID == nID)
			return -1;

		nPrevID--;
	}
	return -1;
}

int CTexasGame::GetNextID(int nID)
{
	int nNextID = nID + 1;
	while (true)
	{
		if (nNextID >= (int)m_Mgr->m_ltClient.size())
			nNextID = 0;

		auto pClient = m_Mgr->m_vtClient[nNextID];
		auto client = (CTexasPokerClient *)pClient.get();
		if (!client->IsClientGameOver() && !client->IsGiveUp())
			return nNextID;

		if (nNextID == nID)
			return -1;
		
		// Murphy modified.
		nNextID++;
	}
	return -1;
}

// Get the max money that every player can bet
int CTexasGame::GetMaxBetMoney()
{
	int nMax = 2147483647;	// 2^31 - 1

	//for (int i = m_nCurClientID; i != m_nBankerID;)
	int i = m_nCurClientID;
	do
	{
		CTexasPokerClient *client = (CTexasPokerClient *)m_Mgr->m_vtClient[i].get();
		if (!client->IsClientGameOver())
		{
			if (nMax > ((client->GetCurrentMoney() + client->GetBetMoney())))
				nMax = (client->GetCurrentMoney() + client->GetBetMoney());
		}
		i = GetNextID(i);
	} while (i != m_nCurClientID);
	nMax -= (nMax % CTexasPokerMgr::GAME_UNIT_MONEY);
	return nMax;
}

std::list<int> CTexasGame::GetWinners()
{
	std::shared_ptr<IClient> pMax;
	std::list<int> ltMax;

	for (int i = 0; i < (int)m_Mgr->m_vtClient.size(); i++)
	{
		auto pClient = m_Mgr->m_vtClient[i];
		auto client = (CTexasPokerClient *)pClient.get();

		if (client->IsClientGameOver() || client->IsGiveUp())
			continue;

		if (!pMax)
		{
			pMax = pClient;
			ltMax.push_back(i);
		}
		else
		{
			int nResult = ComparePoker(pClient->GetID(), pMax->GetID());
			if (nResult == 1)		// client > max
			{
				pMax = pClient;
				ltMax.clear();
				ltMax.push_back(i);
			}
			else if (nResult == 0)	// client == max
			{
				ltMax.push_back(i);
			}
		}
	}
	return ltMax;
}

CString CTexasGame::GetAllBet()
{
	CString strAllBet = _T(""), str;
	for (std::shared_ptr<IClient> pClient : m_Mgr->m_vtClient)
	{
		if (pClient->IsClientGameOver())	continue;
		CTexasPokerClient *client = (CTexasPokerClient *)pClient.get();
		str.Format(_T("id:%d,bet:%d\r\n"), client->GetID(), client->GetBetMoney());
		strAllBet += str;
	}
	return strAllBet;
}

CString CTexasGame::GetAllBetAndPoker()
{
	CString strAllBet = _T(""), str;
	for (std::shared_ptr<IClient> pClient : m_Mgr->m_vtClient)
	{
		if (pClient->IsClientGameOver())	continue;
		CTexasPokerClient *client = (CTexasPokerClient *)pClient.get();

		str.Format(_T("id:%d,bet:%d\r\n,pk:"), client->GetID(), client->GetBetMoney());
		strAllBet = strAllBet + str + client->GetPokerAll();
	}
	return strAllBet;
}

// 1: l > r, 0: l == r, -1: l < r
int CTexasGame::ComparePoker(int lid, int rid)
{
	CTexasPokerClient *pL = (CTexasPokerClient *)m_Mgr->m_vtClient[lid].get();
	CTexasPokerClient *pR = (CTexasPokerClient *)m_Mgr->m_vtClient[rid].get();
	int nResult = CTexasPokerResult::Compare(pL->GetPokers(), pR->GetPokers());
	return nResult;
}

void CTexasGame::AddClientInfo(int id, CString strClientInfo)
{
	if (id >= 0 && id < (int)m_vtClinetInfo.size())
		m_vtClinetInfo[id] = m_vtClinetInfo[id] + strClientInfo + _T(", ");
}

void CTexasGame::ShowInfo()
{
	CGameInfoDlg dlg(this);
	dlg.DoModal();
}

void CTexasGame::ShowAllClientInfo(CListCtrl& listCtrl)
{
	CString str;
	for (int i = 0; i < (int)m_vtClinetInfo.size();i++)
	{
		str.Format(_T("%d"), i);
		listCtrl.InsertItem(i, str);
		listCtrl.SetItemText(i, 1, m_vtClinetInfo[i]);
	}
}

void CTexasGame::GetProcessInfo(CString& strProcess)
{
	CString str;
	str.Format(_T("Game:%d,\r\n"), GetGameID());
	strProcess += str;

	for (int i = 0; i < (int)m_vtClinetInfo.size(); i++)
	{
		str.Format(_T("Client:%d, "), i);
		strProcess += str;
		strProcess += m_vtClinetInfo[i];
		strProcess += _T("\r\n");
	}
}
