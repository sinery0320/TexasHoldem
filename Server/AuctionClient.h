#pragma once
#include "Client.h"

namespace Game{
	namespace Auction{
		class CArt;
		class CAuctionMgr;
		class CAuctionClient : public Game::IClient
		{
		public:
			enum{
				CL_IDLE,
				CL_ASKPRICE,
				CL_DONE,
				CL_OVERTIME
			};

		private:
			CAuctionMgr *m_Mgr;
			int m_nTotalMoney;
			int m_nCurrentMoney;
			int m_nState;
			std::list<std::shared_ptr<CArt>> m_ltArt;
			std::shared_ptr<CArt> m_CurrentArt;

		public:
			CAuctionClient(CAuctionMgr *mgr) : m_Mgr(mgr), m_nState(CL_IDLE){}
			~CAuctionClient(){ ; }

			void SetTotalMoney(int money){ m_nTotalMoney = money; }
			void SetCurrentMoney(int money){ m_nCurrentMoney = money; }

			int GetArtCount(){ return m_ltArt.size(); }
			int GetTotalMoney(){ return m_nTotalMoney; }
			int GetCurrentMoney(){ return m_nCurrentMoney; }
			int GetResidualMoney() { return m_nTotalMoney - m_nCurrentMoney; }

		public:
			int GetAllValue();
			void OnTimeOver();

			void SendInitRequest(int artCount, int clientCount);

			int RequestForArt(std::shared_ptr<CArt> art, int auctionTimes);
			//void OnRequestForArt(byte *pData, int count);

			virtual void OnCmdRespond(byte *pData, int count);
		};
	}
}