#pragma once
#include "GameMgr.h"

class CGameSevDlg;
namespace TCP{ class CClientTcp; }
namespace Game{
	namespace Auction{
		class CArt;
		class CAuctionMgr : public Game::IGameMgr
		{
		public:
			enum GAMESTATE{
				GM_IDEL,
				GM_INIT,
				GM_GAME,
				GM_END,
			};

		private:
			int m_nGameState;
			int m_nArtCount;
			int m_nAccountInit;
			int m_nAuctionTimes;

		public:
			std::list<std::shared_ptr<CArt>> m_ltArt;
			std::list<std::shared_ptr<CArt>> m_ltArtSold;
			std::list<std::shared_ptr<CArt>> m_ltArtFailed;

		public:
			int GetGameState(){ return m_nGameState; }
			void SetArtCount(int nArtCount){ m_nArtCount = nArtCount; }
			void SetAccountInit(int nAccountInit){ m_nAccountInit = nAccountInit; }

		public:
			CAuctionMgr(CGameSevDlg *dlg);
			~CAuctionMgr();

			virtual bool StartGame();
			virtual void StopGame();
			virtual void CreateClient(TCP::CClientTcp* tcp);
			virtual void OnClientDisconnect(TCP::CClientTcp* tcp){ UNREFERENCED_PARAMETER(tcp); }
			virtual void OnTimer100MillSec();

		private:
			virtual bool SendArtToClient();
		};
	}
}