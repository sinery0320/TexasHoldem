#pragma once


// CPackageDlg dialog

class CPackageDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CPackageDlg)

public:
	CPackageDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPackageDlg();

// Dialog Data
	enum { IDD = IDD_PACKAGE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strPackage;
	afx_msg void OnBnClickedOk();
};
