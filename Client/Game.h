#pragma once

class CGameMgr;
class CGame : public IGame
{
private:
	CGameMgr *m_Mgr;
	int m_nGameID;		// Game id
	int m_nBankerID;	// The id of the banker
	int m_nBetMoney;	// How much money you bet in this game
	byte m_Poker[5];	// First 2 pokers are mine, next 3 are the open pokers
	CString m_strResultInfo;	// Content many information
	CString m_strPokerHide;
	CString m_strPokerOpen;
	CString m_strPokerAll;
	std::list<int> m_ltWinID;	// Winners id list

public:
	void SetID(int id){ m_nGameID = id; }
	void SetBankerID(int bankerID){ m_nBankerID = bankerID; }
	void SetPoker(byte *data);// { memcpy(m_Poker, data, 5); }
	void SetBetMoney(int money){ m_nBetMoney = money; }
	void AddWinID(int id){ m_ltWinID.push_back(id); }

	virtual int GetGameID(){ return m_nGameID; }
	virtual int GetBankerID(){ return m_nBankerID; }
	virtual int GetBetMoney(){ return m_nBetMoney; }
	virtual byte* GetPoker(int& count){ count = 5;  return m_Poker; }
	virtual CString GetPokersHide();
	virtual CString GetPokersOpen();
	virtual CString GetPokersAll();
	virtual CString GetResultInfo(){ return m_strResultInfo; }
	virtual std::list<int> GetWinners(){ return m_ltWinID; }

public:
	CGame(CGameMgr *mgr, int id);
	~CGame();

	int GetBetMoney(int nMaxBet, int nPrevBet, int nMyBet, int nTotal, std::map<int, int> mpBetMoney);
	void SetResultInfo(CString strInfo);
	CString GetInfoStr();
};

