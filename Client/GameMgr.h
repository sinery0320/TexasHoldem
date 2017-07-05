#pragma once

class CClientDlg;
class CClientTcp;
class CGame;
class CGameMgr
{
private:
	CClientDlg *m_pDlg;
	std::shared_ptr<CPlayer> m_Player;
	int m_nID;				// My id, from 0
	int m_nPlayerCount;		// How many players
	int m_nTotalMoney;		// How much money do you have at first time
	int m_nCurrentMoney;	// How much money you have right now
	int m_nBetMoney;		// How much money you bet this game
	int m_nBetUnitMoney;	// Your bet money must be n * m_nBetUnitMoney
	std::shared_ptr<CClientTcp> m_pClientTcp;
	std::vector<std::shared_ptr<CGame>> m_vtGame;

public:
	void SetID(int id){ m_nID = id; }
	void SetPlayerCount(int count){ m_nPlayerCount = count; }
	void SetTotalMoney(int money){ m_nTotalMoney = money; m_nCurrentMoney = money; }
	void SetBetUnit(int money){ m_nBetUnitMoney = money; }

	std::shared_ptr<CGame> CurrentGame(){ std::shared_ptr<CGame> e; if (m_vtGame.empty()) return e; return m_vtGame.back(); }
	int GetID(){ return m_nID; }
	int GetPlayCount(){ return m_nPlayerCount; }		// How many players
	int GetBetMoney(){ return m_nBetMoney; }			// How much money you bet this game
	int GetTotalMoney(){ return m_nTotalMoney; }		// How much money do you have at first time
	int GetCurrentMoney(){ return m_nCurrentMoney; }	// How much money you have right now

public:
	CGameMgr(CClientDlg *dlg);
	~CGameMgr();

	void Init();
	BOOL StartGame(int nIP, int nPort);
	BOOL StopGame();
	BOOL IsGiveUp();
	std::shared_ptr<CGame> CreateNewGame();
	void AddBetMoney(int money);
	void AddWinMoney(int money);
	void FillGrid(CListCtrl& lcGame);
	CString GetName();
};

