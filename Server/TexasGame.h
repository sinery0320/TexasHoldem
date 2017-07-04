#pragma once

namespace Game{
	namespace Texas{
		class CPoker;
		class CTexasPokerMgr;
		class CTexasGame
		{
		public:
			enum{
				PS_IDLE,
				PS_SEND_POKER,
				PS_SEND_POKER_WAIT,
				PS_SEND_BET,
				PS_SEND_BET_WAIT,
				PS_GIVE_RESULT_HIDE,
				PS_GIVE_RESULT_SHOW,
				PS_GIVE_RESULT_WAIT,
				PS_DONE,
			};

		private:
			CTexasPokerMgr *m_Mgr;
			int m_nGameID;
			int m_nGameState;
			int m_nBankerID;			// The banker id
			int m_nCurClientID;			// Current client id
			int m_nTotalMoney;			// How much money the client have bet
			std::list<int> m_ltWinID;	// Winners id
			std::vector<byte> m_vtOpenPoker;
			std::vector<CString> m_vtClinetInfo;

		public:
			int GetGameID(){ return m_nGameID; }
			int GetGameState(){ return m_nGameState; }

		public:
			CTexasGame(CTexasPokerMgr *mgr, int game_id);
			~CTexasGame();
			void PlayGame();
			bool InitGame();				// Init game, make sure who is the banker in this game
			bool AddBet(int clientID, int nMoney, CString& strReason);
			CString GetInfoStr();
			void AddClientInfo(int id, CString strClientInfo);
			void ShowInfo();

		private:
			void SendPoker();				// Send pokers to every client (not quit one)
			void SendBet();
			void SendResult(bool bShow);	// Send result

			void CheckAllClient();
			bool CheckGameOver();
			bool CheckAllClientReturned();
			bool CheckOneClientReturned(int id);
			int CheckBetOver();		// 0-not over, 1-over and only one player alive, 2-over and multi player alive

			int GetPrevID(int id);
			int GetNextID(int id);
			int GetMaxBetMoney();
			int ComparePoker(int lid, int rid);		// 1: l > r, 0: l == r, -1: l < r
			std::list<int> GetWinners();
			CString GetAllBet();
			CString GetAllBetAndPoker();
		};
	}
}
