#pragma once
#include "afxcmn.h"


// CGameInfoDlg
namespace Game{
	namespace Texas{ class CTexasGame; }
}

class CGameInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CGameInfoDlg)
	Game::Texas::CTexasGame *m_pGame;
public:
	CGameInfoDlg(Game::Texas::CTexasGame *game, CWnd* pParent = NULL);
	virtual ~CGameInfoDlg();

	enum { IDD = IDD_GAME_INFO_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ListCtrl;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
