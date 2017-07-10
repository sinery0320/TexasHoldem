#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "Resource.h"

// CGameSevDlg dialog
namespace Game{ class IGameMgr; }
class CGameSevDlg : public CDialog
{
public:
	enum{
		COL_CL_ID = 0,
		COL_CL_NAME = 1,
		COL_CL_INFO = 2,

		COL_GM_ID = 0,
		COL_GM_INFO = 1,
	};

	DECLARE_DYNAMIC(CGameSevDlg)
private:
	std::shared_ptr<Game::IGameMgr> m_pGameMgr;
	int m_nListenPort;
	CListCtrl m_ListCtrlClient;
	CListCtrl m_ListCtrlGame;
	BOOL m_ckIsOverTime;

public:
	std::shared_ptr<Game::IGameMgr> GetGameMgr(){ return m_pGameMgr; }
	BOOL IsCheckOverTime(){ return m_ckIsOverTime; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	enum { IDD = IDD_GAMESEV_DLG };
	CGameSevDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGameSevDlg();
	void OnStateChanged();
	afx_msg void OnBnClickedBtListen();
	virtual BOOL OnInitDialog();
	afx_msg void OnNMDblclkListClient(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListGame(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtStartgame();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtRestartgame();
	afx_msg void OnBnClickedBtExport();
	afx_msg void OnBnClickedCkOvertime();
};
