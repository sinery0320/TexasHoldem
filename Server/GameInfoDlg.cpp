// GameInfoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "GameServer.h"
#include "GameInfoDlg.h"
#include "afxdialogex.h"


// CGameInfoDlg �Ի���
const int COL_ID = 0;
const int COL_INFO = 1;
IMPLEMENT_DYNAMIC(CGameInfoDlg, CDialogEx)

CGameInfoDlg::CGameInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGameInfoDlg::IDD, pParent)
{

}

CGameInfoDlg::~CGameInfoDlg()
{
}

void CGameInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CLIENT, m_ListCtrl);
}


BEGIN_MESSAGE_MAP(CGameInfoDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CGameInfoDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CGameInfoDlg ��Ϣ�������
void CGameInfoDlg::OnBnClickedOk()
{
	CDialogEx::OnOK();
}


BOOL CGameInfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_ListCtrl.InsertColumn(COL_ID, _T("ID"), LVCFMT_IMAGE | LVCFMT_LEFT);
	m_ListCtrl.InsertColumn(COL_INFO, _T("Information"));

	CRect rect;
	m_ListCtrl.GetClientRect(rect);
	m_ListCtrl.SetColumnWidth(COL_ID, 30);
	m_ListCtrl.SetColumnWidth(COL_INFO, rect.Width() - 30);

	return TRUE;
}

void CGameInfoDlg::ShowInfo()
{

}
