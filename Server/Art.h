#pragma once

namespace Game{
	namespace Auction{
		class CArt
		{
		private:
			int m_nID;
			int m_Value;			// how much value it worth
			int m_StartPrice;		// how much price at first time

			int m_CurrentPrice;		// the highest price currently
			std::list<int> m_ltID;	// whos send the highest price, empty means no one
			bool m_bFailed;			// no one will buy it

		public:
			int GetID(){ return m_nID; }
			int GetValue(){ return m_Value; }
			int GetCurrentPrice(){ return m_CurrentPrice; }
			bool IsFailed(){ return m_bFailed; }

			void SetCurrentPrice(int price, int id){ m_CurrentPrice = price; m_ltID.clear(); m_ltID.push_back(id); }
			void AddCurrentPrice(int id){ m_ltID.push_back(id); }

		public:
			CArt(int id, int value);
			~CArt();
		};
	}
}