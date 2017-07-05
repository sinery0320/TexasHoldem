#pragma once

namespace Game{
	namespace Texas{
		class CTexasPokerResult
		{
		private:
			CTexasPokerResult();
			~CTexasPokerResult();

		public:
			static CTexasPokerResult& GetMe(){ static CTexasPokerResult one; return one; }
			static CString GetPokerString(byte* p, int count = 5);
			static int Compare(byte* pL, byte* pR);	// 1 pL is bigger, 0 equal, -1 pR is bigger
		};
	}
}
