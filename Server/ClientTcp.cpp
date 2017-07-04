// ClientTcp.cpp : implementation file
//

#include "stdafx.h"
#include "GameServer.h"
#include "ClientTcp.h"
#include "ServerTcp.h"
#include "Client.h"

///////////////////////////////////////////////////////////
using namespace TCP;
// CClientTcp
CClientTcp::CClientTcp(CServerTcp *server)
    : m_Server(server)
	, m_nLen(0)
{
	m_pData = new byte[CServerTcp::DATA_BUF_SIZE];
	memset(m_pData, 0, CServerTcp::DATA_BUF_SIZE);
}

CClientTcp::~CClientTcp()
{
	if (m_pData)
	{
		delete[] m_pData;
		m_pData = nullptr;
	}
}

void CClientTcp::ResponseError(int nErrorID)
{
	byte pData[CServerTcp::DATA_MIN_SIZE] = { 0 };

	// read / write
	memcpy(pData, m_pData, 2);

	// return type
	*(short *)&pData[2] = CServerTcp::DATA_ERROR;

	// error id
	*(int *)&pData[8] = nErrorID;

	// send data
	int nRet = Send(pData, CServerTcp::DATA_MIN_SIZE);
	UNREFERENCED_PARAMETER(nRet);
	ASSERT(nRet != SOCKET_ERROR);
}

BOOL CClientTcp::InitInfo()
{
	// Get ip and port
	CString strAddr;
	UINT nPort = 0;
	if (!GetPeerName(strAddr, nPort))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	// Build information
	SYSTEMTIME st;
	::GetLocalTime(&st);
	m_strClientInfo.Format(_T("%.4d/%.2d/%.2d %.2d:%.2d:%.2d %.3d  ->  "),
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	m_strClientInfo.Format(m_strClientInfo + strAddr + _T(":%d connected."), nPort);
	return TRUE;
}

void CClientTcp::OnReceive(int nErrorCode)
{
	UNREFERENCED_PARAMETER(nErrorCode);
	m_nLen = Receive(m_pData, CServerTcp::DATA_BUF_SIZE);
	if (m_nLen == SOCKET_ERROR)
		return;

	if (m_nLen < CServerTcp::DATA_MIN_SIZE ||
		m_nLen > CServerTcp::DATA_MAX_SIZE)
	{
		ResponseError(CServerTcp::DATA_ERR_SIZE);
		return;
	}

	if (!m_ResponseFun._Empty())
		m_ResponseFun(m_pData, m_nLen);

	CSocket::OnReceive(nErrorCode);
}

void CClientTcp::OnClose(int nErrorCode)
{
	UNREFERENCED_PARAMETER(nErrorCode);
	m_bConnect = false;
	if (m_pData)
	{
		delete[] m_pData;
		m_pData = nullptr;
	}
	CSocket::OnClose(nErrorCode);
}

void CClientTcp::InitResponseFun(Game::IClient *pClient)
{
	m_ResponseFun = std::bind(&Game::IClient::ResponseFun, pClient, std::placeholders::_1, std::placeholders::_2);
}
