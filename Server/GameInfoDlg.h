#pragma once
#include "afxcmn.h"


// CGameInfoDlg �Ի���

class CGameInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CGameInfoDlg)

public:
	CGameInfoDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CGameInfoDlg();

// �Ի�������
	enum { IDD = IDD_GAME_INFO_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	void ShowInfo();

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ListCtrl;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
