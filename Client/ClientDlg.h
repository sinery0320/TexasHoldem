
// ClientDlg.h : header file
//

#pragma once
#include "afxcmn.h"


// CClientDlg dialog
class CGameMgr;
class CClientDlg : public CDialogEx
{
// Construction
public:
	CClientDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CLIENT_DIALOG };
	enum { COL_ID, COL_INFO };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	std::shared_ptr<CGameMgr> m_pMgr;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	DWORD m_nIP;
	int m_nPort;
	int m_nMoneyTotal;
	int m_nMoneyCurrent;
	int m_nID;
	int m_nPlayerCount;
	CListCtrl m_ListCtrl;
	afx_msg void OnBnClickedOk();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedQuit();
	afx_msg void OnBnClickedCkGiveup();
	BOOL m_bGiveUp;
};
