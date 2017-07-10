#pragma once

class CClientDlg;
class CClientTcp;
class CGame;
class CGameMgr : public IPlayerInfo
{
private:
	CClientDlg *m_pDlg;
	std::shared_ptr<CPlayer> m_Player;
	int m_nID;				// My id, from 0
	int m_nPlayerCount;		// How many players
	int m_nTotalMoney;		// How much money do you have at first time
	int m_nCurrentMoney;	// How much money you have right now
	//int m_nBetMoney;		// How much money you bet this game
	int m_nBetUnitMoney;	// Your bet money must be integer multiple of m_nBetUnitMoney
	CString m_strPackage;	// All package info
	std::shared_ptr<CClientTcp> m_pClientTcp;
	std::vector<std::shared_ptr<CGame>> m_vtGame;

public:
	void SetID(int id){ m_nID = id; }
	void SetPlayerCount(int count){ m_nPlayerCount = count; }
	void SetTotalMoney(int money){ m_nTotalMoney = money; m_nCurrentMoney = money; }
	void SetBetUnit(int money){ m_nBetUnitMoney = money; }
	void ClearPackage(){ m_strPackage.Empty(); }
	void AddPackage(CString strPack){ m_strPackage = m_strPackage + strPack + _T("\r\n"); }

	std::shared_ptr<CGame> CurrentGame(){ std::shared_ptr<CGame> e; if (m_vtGame.empty()) return e; return m_vtGame.back(); }
	CClientDlg *GetDlg(){ return m_pDlg; }
	CString GetPackage(){ return m_strPackage; }

	virtual int GetID(){ return m_nID; }
	virtual int GetPlayCount(){ return m_nPlayerCount; }		// How many players
	//virtual int GetBetMoney(){ return m_nBetMoney; }			// How much money you bet this game
	virtual int GetTotalMoney(){ return m_nTotalMoney; }		// How much money do you have at first time
	virtual int GetCurrentMoney(){ return m_nCurrentMoney; }	// How much money you have right now
	virtual int GetBetUnitMoney(){ return m_nBetUnitMoney; }	// Your bet money must be integer multiple of m_nBetUnitMoney
	virtual int GetGameCount() { return (int)m_vtGame.size(); }	// Get how many games
	virtual IGame* GetCurrentGame() { auto g = CurrentGame(); if (g) return (IGame *)g.get(); return NULL; }	// Get current game object
	virtual IGame* GetGame(int id) { if (id < 0 || id >= (int)m_vtGame.size()) return NULL; return (IGame *)m_vtGame[id].get(); }		// Get current game object


public:
	CGameMgr(CClientDlg *dlg);
	~CGameMgr();

	bool Init();
	BOOL StartGame(int nIP, int nPort);
	BOOL StopGame();
	std::shared_ptr<CGame> CreateNewGame();
	void AddBetMoney(int money);
	void AddWinMoney(int money);
	void FillGrid(CListCtrl& lcGame);

	CString GetName();
	void OnInit();
	void OnReceivePokers();
	int RequireBetMoney(int nMax, int nPrevBet, int nMyBet, int nTotal, CString strAllBet);
	void OnOneGameOver(bool bIfWin, int nWinMoney, CString strResultInfo);
	void OnAllGameOver(int nNumber, int nCurMoney, CString strProcess);

private:
	std::shared_ptr<CPlayer> Player();
};

