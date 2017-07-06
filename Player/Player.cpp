// Player.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Player.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString CPlayer::GetName()
{
	//! Enter your name
	return _T("AI0");
}

// It's a event at first time
void CPlayer::OnInitPlayer()
{
	/*	When this method called, the player can get below information:
		- player id
		- total money the player have at first time
		- how many players in the game
		- bet unit money (the bet money must be integer multiple of unit money every time)
	*/
}

// When one game begin
void CPlayer::OnOneGameBegin()
{
}

// When you get pokers from server
void CPlayer::OnReceivePokers()
{
	/*	Now you can see your pokers
		- IGame *game = m_PlayerInfo->GetCurrentGame(); // to get current game
		- byte *pokers = game->GetPoker();				// your pokers, first and second one is hide pokers, next three are open pokers
		- CString str = game->GetPokersHide()/GetPokersOpen()/GetPokersAll();	// switch your pokers to string, 2,3,4,5,6,7,8,9,T,J,Q,K,A (T->10), o(different color), s(same color)
		- int id = game->GetBankerID();					// you will know who it the banker, the banker will give bet money at first time
		- int id = game->GetGameID();					// current game id, from 0
	*/
}

// Need you give the delta bet money to server. The value you return is the delta bet money, this value + nMyBet will be all your bet in this game
int CPlayer::RequireBetMoney(int nMax, int nPrevBet, int nMyBet, int nTotal, CString strAllBet)
{
	/*	Parameter description
		- nMax: Your all bet money must not be bigger than this value, that means (the value you return + nMyBet) must <= (nMax), otherwise you will lost the game
		- nPrevBet: The all bet money previous player gave in this game, if your id is 5, the player whose id is 4 is your previous, if your id is 0, 7 will be previous (if there are 8 palyers)
					(The money you return + nMyBet) must >= (nPrevBet)
		- nMyBet: The money you have bet in this game
		- nTotal: How much money already in server for this game
		- strAllBet: You can get all bet money information from this string
			- id: client id, from 0
			- bet: all bet money
			- gg: if give up, 1-give up
			- win: if win, 1-win
			- pk: what is the pokers

		- return: Delta bet money you need bet, you can return -1, or >= 0
				  -1: Give up this game
				  >=0: nMax >= (this value + nMyBet) >= nPrevBet

		If all players bet the same money (except losers and give ups), server will open all pokers, the biggest will get all money.
		If all players give up or lose game, only one left, that guy will get all money, but you can't get his pokers.
	*/

	// AI0 code: only bet the same as previous
	return nPrevBet - nMyBet;

	// AI1 code: bet 30 at first time, so we can see two round of betting
	int nDeltaBet = nPrevBet - nMyBet;
	if (nDeltaBet == 10)	nDeltaBet = 30;
	return nDeltaBet;

	// AI2 code: give up every time
	return -1;
}

// When one game over
void CPlayer::OnOneGameOver(bool bIfWin, int nWinMoney, CString strResultInfo)
{
	/*	Parameter description
		- bIfWin: true means you will this game
		- nWinMoney: the money that every winner will get
		- strResultInfo: you can get many information from this string, if result is open, you can see every one's pokers who is not give up
	*/
}

// When all games over, you will get your number
void CPlayer::OnAllGameOver(int number)
{
	/*	Parameter description
		- number: Your number, from 1, 1 is the final winner
	*/
}

