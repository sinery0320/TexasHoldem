// GameSevDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GameServer.h"
#include "GameSevDlg.h"
#include "afxdialogex.h"
#include "Client.h"

#include "GameMgr.h"
#include "AuctionMgr.h"
#include "TexasPokerMgr.h"

#include "Test.h"


// CGameSevDlg dialog

IMPLEMENT_DYNAMIC(CGameSevDlg, CDialog)

CGameSevDlg::CGameSevDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGameSevDlg::IDD, pParent)
	, m_nListenPort(23456)
{
	//m_pGameMgr = std::shared_ptr<Game::IGameMgr>(new Game::Auction::CAuctionMgr(this));
	m_pGameMgr = std::shared_ptr<Game::IGameMgr>(new Game::Texas::CTexasPokerMgr(this));
}

CGameSevDlg::~CGameSevDlg()
{
}

void CGameSevDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_LISTEN_PORT, m_nListenPort);
	DDX_Control(pDX, IDC_LIST_CLIENT, m_ListCtrlClient);
	DDX_Control(pDX, IDC_LIST_GAME, m_ListCtrlGame);
}

BEGIN_MESSAGE_MAP(CGameSevDlg, CDialog)
	ON_BN_CLICKED(IDC_BT_LISTEN, &CGameSevDlg::OnBnClickedBtListen)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CLIENT, &CGameSevDlg::OnNMDblclkListClient)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_GAME, &CGameSevDlg::OnNMDblclkListGame)
	ON_BN_CLICKED(IDC_BT_STARTGAME, &CGameSevDlg::OnBnClickedBtStartgame)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CGameSevDlg message handlers

BOOL CGameSevDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	int nStyle = m_ListCtrlClient.GetExtendedStyle();
	nStyle |= LVS_EX_FULLROWSELECT;
	m_ListCtrlClient.SetExtendedStyle(nStyle);

	m_ListCtrlClient.InsertColumn(COL_CL_ID, _T("ID"), LVCFMT_IMAGE | LVCFMT_LEFT);
	m_ListCtrlClient.InsertColumn(COL_CL_NAME, _T("Name"));
	m_ListCtrlClient.InsertColumn(COL_CL_INFO, _T("Information"));

	m_ListCtrlClient.SetColumnWidth(COL_CL_ID, 30);
	m_ListCtrlClient.SetColumnWidth(COL_CL_NAME, 140);
	m_ListCtrlClient.SetColumnWidth(COL_CL_INFO, 450);

	m_ListCtrlGame.SetExtendedStyle(nStyle);

	m_ListCtrlGame.InsertColumn(COL_GM_ID, _T("ID"), LVCFMT_IMAGE | LVCFMT_LEFT);
	m_ListCtrlGame.InsertColumn(COL_GM_INFO, _T("Information"));

	m_ListCtrlGame.SetColumnWidth(COL_GM_ID, 30);
	m_ListCtrlGame.SetColumnWidth(COL_GM_INFO, 600);

	OnStateChanged();

	Test::CTest test;
	test.DoTest(this);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CGameSevDlg::OnStateChanged()
{
	switch (m_pGameMgr->GetServerState())
	{
	case Game::IGameMgr::SEV_IDEL:
		GetDlgItem(IDC_EDIT_LISTEN_PORT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BT_LISTEN)->EnableWindow(TRUE);
		GetDlgItem(IDC_BT_STARTGAME)->EnableWindow(FALSE);
		GetDlgItem(IDC_BT_STARTGAME)->SetWindowText(_T("Start Game"));
		break;

	case Game::IGameMgr::SEV_LISTEN:
		GetDlgItem(IDC_EDIT_LISTEN_PORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BT_LISTEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_BT_STARTGAME)->EnableWindow(TRUE);
		GetDlgItem(IDC_BT_STARTGAME)->SetWindowText(_T("Start Game"));
		break;

	case Game::IGameMgr::SEV_START:
		GetDlgItem(IDC_EDIT_LISTEN_PORT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BT_LISTEN)->EnableWindow(FALSE);
		GetDlgItem(IDC_BT_STARTGAME)->EnableWindow(TRUE);
		GetDlgItem(IDC_BT_STARTGAME)->SetWindowText(_T("Stop Game"));
		break;

	default:
		break;
	}
}

void CGameSevDlg::OnBnClickedBtListen()
{
	if (m_nListenPort < 1000 || m_nListenPort > 60000)
	{
		MessageBox(_T("Port rang error, must between 1000~60000"), _T("Error"), MB_OK | MB_ICONERROR);
		GotoDlgCtrl(GetDlgItem(IDC_EDIT_LISTEN_PORT));
	}
	else
	{
		if (!m_pGameMgr->StartListen((unsigned short)m_nListenPort))
		{
			CString strErr;
			strErr.Format(_T("Start TCP Server failed, error id is %d (0x%.8X)"), GetLastError(), GetLastError());
			MessageBox(strErr, _T("Error"), MB_OK | MB_ICONERROR);
			GotoDlgCtrl(GetDlgItem(IDC_EDIT_LISTEN_PORT));
		}
	}
	OnStateChanged();
}

void CGameSevDlg::OnNMDblclkListClient(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	UNREFERENCED_PARAMETER(pNMItemActivate);
	*pResult = 0;
	int row = pNMItemActivate->iItem;
	row = row;
}

void CGameSevDlg::OnNMDblclkListGame(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	UNREFERENCED_PARAMETER(pNMItemActivate);
	*pResult = 0;
	int row = pNMItemActivate->iItem;
	m_pGameMgr->ShowGameInfo(row);
}

void CGameSevDlg::OnBnClickedBtStartgame()
{
	if (m_pGameMgr->GetServerState() == Game::IGameMgr::SEV_START)
	{
		KillTimer(1);
		m_pGameMgr->StopGame();
		m_pGameMgr->FillGrid(m_ListCtrlClient, m_ListCtrlGame);
		UpdateData(FALSE);
	}
	else if (m_pGameMgr->GetServerState() == Game::IGameMgr::SEV_LISTEN)
	{
		if (m_pGameMgr->StartGame())
		{
			SetTimer(1, 1000, NULL);
			SetTimer(2, 500, NULL);
		}
		else
		{
			CString strErr = _T("Start game failed");
			MessageBox(strErr, _T("Error"), MB_OK | MB_ICONERROR);
		}
	}
	OnStateChanged();
}

void CGameSevDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		m_pGameMgr->FillGrid(m_ListCtrlClient, m_ListCtrlGame);
	}
	else if (nIDEvent == 2)
	{
		m_pGameMgr->OnTimer100MillSec();
	}
	CDialog::OnTimer(nIDEvent);
}
