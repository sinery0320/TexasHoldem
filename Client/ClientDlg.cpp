
// ClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"
#include "afxdialogex.h"

#include "GameMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CClientDlg dialog
CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CClientDlg::IDD, pParent)
	//, m_nIP(16777343)
	, m_nIP(2130706433)
	, m_nPort(23456)
	, m_nMoneyTotal(0)
	, m_nMoneyCurrent(0)
	, m_nID(0)
	, m_nPlayerCount(0)
	, m_bGiveUp(FALSE)
	, m_CountSend(0)
	, m_CountRcv(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pMgr = std::shared_ptr<CGameMgr>(new CGameMgr(this));
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_IPAddress(pDX, IDC_IP_CTRL, m_nIP);
	DDX_Text(pDX, IDC_EDIT_PORT, m_nPort);
	DDX_Text(pDX, IDC_EDIT_MONEY_TOTAL, m_nMoneyTotal);
	DDX_Text(pDX, IDC_EDIT_MONEY_CURRENT, m_nMoneyCurrent);
	DDX_Text(pDX, IDC_EDIT_ID, m_nID);
	DDX_Text(pDX, IDC_EDIT_COUNT, m_nPlayerCount);
	DDX_Control(pDX, IDC_LIST, m_ListCtrl);
	DDX_Check(pDX, IDC_CK_GIVEUP, m_bGiveUp);
	DDX_Text(pDX, IDC_EDIT_SEND, m_CountSend);
	DDX_Text(pDX, IDC_EDIT_RCV, m_CountRcv);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CClientDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDQUIT, &CClientDlg::OnBnClickedQuit)
	ON_BN_CLICKED(IDC_CK_GIVEUP, &CClientDlg::OnBnClickedCkGiveup)
END_MESSAGE_MAP()


// CClientDlg message handlers

BOOL CClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	int nStyle = m_ListCtrl.GetExtendedStyle();
	nStyle |= LVS_EX_FULLROWSELECT;
	m_ListCtrl.SetExtendedStyle(nStyle);

	m_ListCtrl.InsertColumn(COL_ID, _T("ID"), LVCFMT_IMAGE | LVCFMT_LEFT);
	m_ListCtrl.InsertColumn(COL_INFO, _T("Info"));

	m_ListCtrl.SetColumnWidth(COL_ID, 40);
	m_ListCtrl.SetColumnWidth(COL_INFO, 460);

	GetDlgItem(IDC_IP_CTRL)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_PORT)->EnableWindow(TRUE);
	GetDlgItem(IDOK)->EnableWindow(TRUE);
	GetDlgItem(IDQUIT)->EnableWindow(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CClientDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	if (m_pMgr->StartGame(m_nIP, m_nPort))
	{
		GetDlgItem(IDC_IP_CTRL)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_PORT)->EnableWindow(FALSE);
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		GetDlgItem(IDQUIT)->EnableWindow(TRUE);
		SetTimer(1, 1000, NULL);
	}
	else
	{
		CString strError;
		DWORD nErr = GetLastError();
		strError.Format(_T("Connect to server failed, please check the ip and port, error code - 0x%.8X (%d)"), nErr, nErr);
		MessageBox(strError, _T("Error"), MB_OK | MB_ICONERROR);
	}
}

void CClientDlg::OnBnClickedQuit()
{
	KillTimer(1);
	m_pMgr->StopGame();
	m_pMgr->FillGrid(m_ListCtrl);
	GetDlgItem(IDC_IP_CTRL)->EnableWindow(TRUE);
	GetDlgItem(IDC_EDIT_PORT)->EnableWindow(TRUE);
	GetDlgItem(IDOK)->EnableWindow(TRUE);
	GetDlgItem(IDQUIT)->EnableWindow(FALSE);
}

void CClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		m_pMgr->FillGrid(m_ListCtrl);
		m_nMoneyTotal = m_pMgr->GetTotalMoney();
		m_nMoneyCurrent = m_pMgr->GetCurrentMoney();
		m_nID = m_pMgr->GetID();
		m_nPlayerCount = m_pMgr->GetPlayCount();
		UpdateData(FALSE);
	}
}

void CClientDlg::OnBnClickedCkGiveup()
{
	UpdateData(TRUE);
}
