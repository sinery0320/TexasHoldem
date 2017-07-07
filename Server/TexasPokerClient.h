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
			int m_nClientGameState;
			int m_nBetMoney;		// Bet money in one game
			bool m_bGiveUp;			// If give up this one game
			byte m_poker[5];		// first 2 are hide, behind 3 are show
			CString m_strPokerHide;
			CString m_strPokerAll;
			short m_nCurrentCmd;	// The current cmd id
			CString m_strReason;	// Why you game over
			//int m_nLoserNumber;		// Winner is -1
			int m_nNumber;			// Winner is -1

			SYSTEMTIME m_CmdTime;	// for check over time

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
			int GetClientGameState(){ return m_nClientGameState; }
			int GetBetMoney(){ return m_nBetMoney; }
			int GetNumber(){ return m_nNumber; }
			bool IsGiveUp(){ return m_bGiveUp; }
			byte* GetPokers(){ return m_poker; }
			CString GetPokerHide(){ return m_strPokerHide; }
			CString GetPokerAll(){ return m_strPokerAll; }

		public:
			virtual void OnCheckOverTime();
			void CheckGameOver();
			virtual CString GetInfoStr();
			virtual void InitData();
			virtual void InitOneGame();
			virtual void WinTheGame(int money);
			virtual bool IsClientGameOver();
			virtual void SendInitRequest();
			virtual void OnCmdRespond(byte *pData, int count);
			void SendPokerRequest(std::vector<byte> pokers, int banker, int gameid);
			void SendBetRequest(int nMax, int nPrevBet, int nYourBet, int nTal, CString strAllBet);
			void SendResultRequest(bool bWin, int nTal, CString betInfo);
			void SendGameOver(CString strProcess);
			void ChangeState(int state, CString strReason = _T(""));

		private:
			void OnBet(byte *data, int len);
		};
	}
}
