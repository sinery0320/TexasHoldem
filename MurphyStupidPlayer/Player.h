// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PLAYER_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PLAYER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#pragma once

#ifdef PLAYER_EXPORTS
#define PLAYER_API __declspec(dllexport)
#else
#define PLAYER_API __declspec(dllimport)
#endif

class PLAYER_API IGame{
public:
	IGame(void){}
	virtual int GetGameID() = 0;			// Game id, from 0
	virtual int GetBankerID() = 0;			// Banker ID in one game
	virtual int GetBetMoney() = 0;			// All bet money in one game, currently
	virtual byte* GetPoker(int& count) = 0;	// Your pokers, count must be 5
	virtual CString GetPokersHide() = 0;	// Get poker string, for hide pokers
	virtual CString GetPokersOpen() = 0;	// Get poker string, for open pokers
	virtual CString GetPokersAll() = 0;		// Get poker string, for all your pokers
	virtual CString GetResultInfo() = 0;	// Content many information which need you analyze
	virtual std::list<int> GetWinners() = 0;// When this game over, you can know who (Player ID) win this game
};

class PLAYER_API IPlayerInfo{
public:
	IPlayerInfo(void){}
	virtual int GetID() = 0;				// My id (Player ID), from 0
	virtual int GetPlayCount() = 0;			// How many players
	virtual int GetTotalMoney() = 0;		// How much money you have right now
	virtual int GetCurrentMoney() = 0;		// How much money you bet this game
	virtual int GetBetUnitMoney() = 0;		// Your bet money must be integer multiple of m_nBetUnitMoney
	virtual int GetGameCount() = 0;			// Get how many games
	virtual IGame* GetCurrentGame() = 0;	// Get current game object, current game maybe not game over
	virtual IGame* GetGame(int id) = 0;		// Get the game object you want, if not exist, will return NULL
};


// This class is exported from the Player.dll
class PLAYER_API CPlayer {
private:
	IPlayerInfo *m_PlayerInfo;

public:
	CPlayer(IPlayerInfo *playerInfo):m_PlayerInfo(playerInfo){}
	CString GetName();						// Get your name
	void OnInitPlayer();					// It's a event at first time
	void OnOneGameBegin();					// When one game begin
	void OnReceivePokers();					// When you get pokers from server
	int RequireBetMoney(int nMax, int nPrevBet, int nMyBet, int nTotal, CString strAllBet);	// Need you give the bet money
	void OnOneGameOver(bool bIfWin, int nWinMoney, CString strResultInfo);					// When one game over
	void OnAllGameOver(int number);			// When all games over, you will get your number
};

