#pragma once

class CGameSevDlg;
namespace Test{
	class CTest
	{
	private:
		CGameSevDlg *m_Dlg;

	public:
		CTest();
		~CTest();

		void DoTest(CGameSevDlg *dlg);
	};
}
