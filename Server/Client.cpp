#include "stdafx.h"
#include "Client.h"
#include "ClientTcp.h"
#include "ServerTcp.h"

using namespace Game;

IClient::~IClient()
{
	if (m_pTcp)
		delete m_pTcp;
	m_pTcp = nullptr;
}

void IClient::SetTcp(TCP::CClientTcp *pTcp)
{
	m_pTcp = pTcp;
	if (pTcp)
		pTcp->InitResponseFun(this);
}

void IClient::InitHead(byte* pData, short cmdid, short datalen)
{
	*(short*)&pData[0] = TCP::CServerTcp::DATA_SEND;	// 1:send, 2:return
	*(short*)&pData[2] = cmdid;		// command id
	*(short*)&pData[4] = 0;			// error type, 0-no error
	*(short*)&pData[6] = datalen;	// data len
}

void IClient::ResponseFun(byte *pData, int count)
{
	if (count < 8)											return;
	if (*(short*)&pData[0] != TCP::CServerTcp::DATA_BACK)	return;
	if (*(short*)&pData[4] != 0)							return;	// It's error
	OnCmdRespond(pData, count);
}

void IClient::SendData(byte* pData, int count)
{
	m_pTcp->SendData(pData, count);
}
