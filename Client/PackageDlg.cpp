// PackageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "PackageDlg.h"
#include "afxdialogex.h"


// CPackageDlg dialog

IMPLEMENT_DYNAMIC(CPackageDlg, CDialogEx)

CPackageDlg::CPackageDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPackageDlg::IDD, pParent)
	, m_strPackage(_T(""))
{

}

CPackageDlg::~CPackageDlg()
{
}

void CPackageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PACKAGE, m_strPackage);
}


BEGIN_MESSAGE_MAP(CPackageDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CPackageDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CPackageDlg message handlers


void CPackageDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}
