#pragma once
#include "GameMgr.h"

class CGameSevDlg;
namespace TCP{ class CClientTcp; }
namespace Game{
	class IClient;
	namespace Texas{
		//class CPoker;
		class CTexasGame;
		class CTexasPokerMgr : public Game::IGameMgr
		{
		public:
			enum {
				GM_IDEL,
				GM_INIT,
				GM_GAME,
				GM_END,

				GAME_MAX = 10,
				GAME_MIN_MONEY = 10,
				GAME_DEFAULT_MONEY = 10,
				GAME_UNIT_MONEY = 5,
			};
		private:
			int m_nGameState;	// game state
			int m_nCurID;		// current client id

		public:
			std::vector<std::shared_ptr<IClient>> m_vtClient;
			std::vector<std::shared_ptr<CTexasGame>> m_vtGame;
			std::list<int> m_ltLoserID;

		public:
			int GetGameState(){ return m_nGameState; }

		public:
			CTexasPokerMgr(CGameSevDlg *dlg);
			~CTexasPokerMgr();

			virtual bool StartGame();
			virtual void StopGame();
			virtual void OnGameOver();
			virtual void CreateClient(TCP::CClientTcp* tcp);
			virtual void OnClientDisconnect(TCP::CClientTcp* tcp);
			virtual void OnTimer100MillSec();
			virtual void FillGrid(CListCtrl& lcClient, CListCtrl& lcGame);
			virtual void ShowGameInfo(int gameID);
			std::shared_ptr<CTexasGame> GetCurrentGame();
			void AddLoser(int id);

		private:
			void ReSortClient();
			bool CheckGameOver();
			void GetGameResult();
		};
	}
}
