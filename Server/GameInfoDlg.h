#pragma once
#include "afxcmn.h"


// CGameInfoDlg 对话框

class CGameInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CGameInfoDlg)

public:
	CGameInfoDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGameInfoDlg();

// 对话框数据
	enum { IDD = IDD_GAME_INFO_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void ShowInfo();

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ListCtrl;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
