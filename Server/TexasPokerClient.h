#pragma once
#include "Client.h"
#include "GameMgr.h"

namespace Game{
	namespace Texas{
		class CTexasPokerMgr;
		class CTexasPokerClient : public Game::IClient
		{
		private:
			CTexasPokerMgr* m_Mgr;
			int m_nClientTotalMoney;
			int m_nCurrentMoney;	// Current game, the money this client have bet
			int m_nGameState;
			int m_nBetMoney;		// Bet money in one game
			bool m_bGiveUp;			// If give up this one game
			byte m_poker[5];		// first 2 are hide, behind 3 are show
			short m_nCurrentCmd;	// The current cmd id
			CString m_strReason;	// Why you game over
			//int m_nLoserNumber;		// Winner is -1
			int m_nNumber;			// Winner is -1

		public:
			CTexasPokerClient(CTexasPokerMgr* mgr);
			~CTexasPokerClient(){ ; }

			void AddBetMoeny(int money){ m_nBetMoney += money; m_nCurrentMoney -= money; }
			void SetTotalMoney(int money){ m_nClientTotalMoney = money; }
			void SetCurrentMoney(int money){ m_nCurrentMoney = money; }
			void SetGiveUp(bool b){ m_bGiveUp = b; }
			//void SetLoserNumber(int id){ m_nLoserNumber = id; }
			void SetNumber(int number){ m_nNumber = number; }

			int GetTotalMoney(){ return m_nClientTotalMoney; }
			int GetCurrentMoney(){ return m_nCurrentMoney; }
			int GetGameState(){ return m_nGameState; }
			int GetBetMoney(){ return m_nBetMoney; }
			bool IsGiveUp(){ return m_bGiveUp; }
			byte* GetPokers(){ return m_poker; }

		public:
			void OnTimeOver();
			void CheckGameOver();
			virtual CString GetInfoStr();
			virtual void InitData();
			virtual void InitOneGame();
			virtual void WinTheGame(int money);
			virtual bool IsClientGameOver();
			virtual int SendInitRequest();
			virtual void OnCmdRespond(byte *pData, int count);
			void SendPokerRequest(std::vector<byte> pokers, int banker, int gameid);
			void SendBetRequest(int nMax, int nPrevBet, int nYourBet, int nTal, CString strAllBet);
			void SendResultRequest(bool bWin, int nTal, CString betInfo);
			void SendGameOver(/*int nNumber, CString strReason*/);
			void ChangeState(int state, CString strReason = _T(""));

		private:
			bool OnBet(byte *data, int len);
		};
	}
}
