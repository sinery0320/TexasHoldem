#pragma once

namespace Game{
	namespace Texas{
		class CPoker
		{
		public:
			enum{
				PT_CLUB,
				PT_SQUARE,
				PT_SPADE,
				PT_HERT
			};
		private:
			byte m_value;	// 1~13
			byte m_type;	// Hert, Spade, Square, Club
			bool m_bOut;	// Have been sent

		public:
			byte Value(){ return m_value; }
			byte Type(){ return m_type; }
			bool IsOut(){ return m_bOut; }

			void SetOut(bool b){ m_bOut = b; }

		public:
			CPoker(byte val, byte type);
			~CPoker();
		};
	}
}
