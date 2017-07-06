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

void IClient::InitHead(byte* pData, short cmdid, int datalen)
{
	*(short*)&pData[0] = TCP::CServerTcp::DATA_SEND;	// 1:send, 2:return
	*(short*)&pData[2] = cmdid;							// command id
	*(int*)&pData[4] = datalen;							// byte count
}

void IClient::ResponseFun(byte *pData, int count)
{
	if (count < 8)											return;
	if (*(short*)&pData[0] != TCP::CServerTcp::DATA_BACK)	return;
	if (*(int*)&pData[4] > (count - 8))						return;	// It's error
	ASSERT(*(int*)&pData[4] <= (count - 8));
	OnCmdRespond(pData, count);
}

void IClient::SendData(byte* pData, int count)
{
	m_pTcp->SendData(pData, count);
}
