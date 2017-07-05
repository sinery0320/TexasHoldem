// ServerTcp.cpp : implementation file
//

#include "stdafx.h"
#include "GameServer.h"
#include "GameSevDlg.h"
#include "GameMgr.h"
#include "ServerTcp.h"
#include "ClientTcp.h"

//CServerTcpClient::CServerTcpClient(CServerTcp *server)
//    : m_Server(server)
//    , m_nLen(0)
//{
//    m_pData = new byte[CServerTcp::DATA_BUF_SIZE];
//    memset(m_pData, 0, CServerTcp::DATA_BUF_SIZE);
//}
//
//CServerTcpClient::~CServerTcpClient()
//{
//    delete[] m_pData;
//}
//
//void CServerTcpClient::ResponseError(int nErrorID)
//{
//    byte pData[CServerTcp::DATA_MIN_SIZE] = { 0 };
//
//    // read / write
//    memcpy(pData, m_pData, 2);
//
//    // return type
//    *(short *)&pData[2] = CServerTcp::DATA_ERROR;
//
//    // error id
//    *(int *)&pData[8] = nErrorID;
//
//    // send data
//    int nRet = Send(pData, CServerTcp::DATA_MIN_SIZE);
//    UNREFERENCED_PARAMETER(nRet);
//    ASSERT(nRet != SOCKET_ERROR);
//}
//
//void CServerTcpClient::ResponseRead()
//{
//    byte pData[CServerTcp::DATA_MAX_SIZE] = { 0 };
//
//    // read / write
//    memcpy(pData, m_pData, 2);
//
//    // return type
//    *(short *)&pData[2] = CServerTcp::DATA_BACK;
//
//    // show track
//    //CAuctionGameServerDlg *pDlg = (CAuctionGameServerDlg *)AfxGetApp()->GetMainWnd();
//    //if (!pDlg)
//    //{
//    //    ResponseError(CServerTcp::DATA_ERR_OTHERS);
//    //    return;
//    //}
//    //pData[4] = pDlg->m_bShowTrack ? 1 : 0;
//    //
//    //// value data
//    //if (pDlg)
//    //{
//    //    int nLen = 0;
//    //    byte *p = &pData[16];
//    //    if (!pDlg->m_auctionGameMgr.BuildData(p, nLen))
//    //    {
//    //        ResponseError(CServerTcp::DATA_ERR_SIZE);
//    //        return;
//    //    }
//    //    // set data len
//    //    memcpy(&pData[12], &nLen, 4);
//    //
//    //    // send data
//    //    nLen += 16;
//    //    int nRet = Send(pData, nLen);
//    //    UNREFERENCED_PARAMETER(nRet);
//    //    ASSERT(nRet != SOCKET_ERROR);
//    //}
//}
//
//void CServerTcpClient::OnReceive(int nErrorCode)
//{
//    UNREFERENCED_PARAMETER(nErrorCode);
//    m_nLen = Receive(m_pData, CServerTcp::DATA_BUF_SIZE);
//    if (m_nLen == SOCKET_ERROR)
//        return;
//
//    if (m_nLen < CServerTcp::DATA_MIN_SIZE ||
//        m_nLen > CServerTcp::DATA_MAX_SIZE)
//    {
//        ResponseError(CServerTcp::DATA_ERR_SIZE);
//        return;
//    }
//
//    //CAuctionGameServerDlg *pDlg = (CAuctionGameServerDlg *)AfxGetApp()->GetMainWnd();
//    //if (!pDlg || pDlg->m_RetType != 0)
//    //{
//    //    ResponseError(CServerTcp::DATA_ERR_OTHERS);
//    //    return;
//    //}
//	//
//    //// Do the analisis
//    //switch (m_pData[0])
//    //{
//    //case CServerTcp::DATA_READ:
//    //    ResponseRead();
//    //    break;
//	//
//    //case CServerTcp::DATA_WRITE:
//    //    ResponseError(CServerTcp::DATA_ERR_NOTSUPPORT);
//    //    break;
//	//
//    //default:
//    //    ResponseError(CServerTcp::DATE_ERR_REQUEST);
//    //    break;
//    //}
//    CSocket::OnReceive(nErrorCode);
//}
//
//void CServerTcpClient::OnClose(int nErrorCode)
//{
//    UNREFERENCED_PARAMETER(nErrorCode);
//    m_Server->OnCloseClient(this);
//    CSocket::OnClose(nErrorCode);
//}
//
//BOOL CServerTcpClient::InitInfo()
//{
//    // Get ip and port
//    CString strAddr;
//    UINT nPort = 0;
//    if (!GetPeerName(strAddr, nPort))
//    {
//        ASSERT(FALSE);
//        return FALSE;
//    }
//
//    // Build information
//    SYSTEMTIME st;
//    ::GetLocalTime(&st);
//    m_strClientInfo.Format(_T("%.4d/%.2d/%.2d %.2d:%.2d:%.2d %.3d  ->  "),
//        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
//    m_strClientInfo.Format(m_strClientInfo + strAddr + _T(":%d connected."), nPort);
//    return TRUE;
//}


/////////////////////////////////////////////
// CServerTcp
using namespace TCP;
CServerTcp::CServerTcp(CGameSevDlg *dlg)
	: m_Dlg(dlg)
{
}

CServerTcp::~CServerTcp()
{
}

int CServerTcp::CreateTcp(unsigned short wPort)
{
    BOOL b = Create(wPort, SOCK_STREAM);// , _T("10.108.178.31"));
    if (!b) return GetLastError();
    b = Listen();
    if (!b) return GetLastError();
    return 0;
}

// CServerTcp member functions
void CServerTcp::OnConnect(int nErrorCode)
{
    UNREFERENCED_PARAMETER(nErrorCode);
    CSocket::OnConnect(nErrorCode);
}

void CServerTcp::OnAccept(int nErrorCode)
{
    UNREFERENCED_PARAMETER(nErrorCode);
    if (nErrorCode == 0)
    {
		auto pMgr = m_Dlg->GetGameMgr();
		if ((int)pMgr->m_ltClient.size() >= CLIENT_MAX_COUNT)
			return;

		if (pMgr->GetServerState() == Game::IGameMgr::SEV_LISTEN)
		{
			CClientTcp *pSock = new CClientTcp(this);
			if (Accept(*pSock) && pSock->InitInfo())
			{
				pMgr->CreateClient(pSock);
			}
		}
    }
    CSocket::OnAccept(nErrorCode);
}

void CServerTcp::OnClientDisconnect(CClientTcp *pClientTcp)
{
	auto pMgr = m_Dlg->GetGameMgr();
	pMgr->OnClientDisconnect(pClientTcp);
}

