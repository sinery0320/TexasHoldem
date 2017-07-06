#include "stdafx.h"
#include "GameSevDlg.h"
#include "GameServer.h"
#include "AuctionClient.h"
#include "ClientTcp.h"
#include "ServerTcp.h"
#include "Art.h"

// CClientInfo
using namespace Game;
using namespace Auction;
int CAuctionClient::GetAllValue()
{
	int n = 0;
	for (std::shared_ptr<CArt> art : m_ltArt)
	{
		n += art->GetCurrentPrice();
	}
	return n;
}

void CAuctionClient::OnTimeOver()
{

}

// if name is not empty, means this command is done
void CAuctionClient::SendInitRequest(int artCount, int clientCount)
{
	if (!GetName().IsEmpty())	return;
	byte pData[16] = { 0 };
	*(int *)pData = GetID();
	*(int *)&pData[4] = m_nTotalMoney;
	*(int *)&pData[8] = artCount;
	*(int *)&pData[12] = clientCount;
}

//void CAuctionClient::OnInitClientInfoReturn(byte *pData, int count)
//{
//	UNREFERENCED_PARAMETER(count);
//	// set name
//	SetName(CString((TCHAR*)pData));
//}

// 
int CAuctionClient::RequestForArt(std::shared_ptr<CArt> art, int auctionTimes)
{
	if (m_CurrentArt != art)
	{
		m_CurrentArt = art;
		m_nState = CL_IDLE;
	}
	byte pData[16] = { 0 };
	switch (m_nState)
	{
	case CL_IDLE:
		m_nState = CL_ASKPRICE;
		*(int*)pData = art->GetID();
		*(int*)&pData[4] = art->GetValue();
		*(int*)&pData[8] = art->GetCurrentPrice();
		*(int*)&pData[12] = auctionTimes;
		break;

	case CL_ASKPRICE:
	case CL_DONE:
	case CL_OVERTIME:
		break;
	}
	return -1;
}

//void CAuctionClient::OnRequestForArt(byte *pData, int count)
//{
//	int nPrice = 0;
//	memcpy(&nPrice, pData, min(count, 4));
//	if (nPrice <= 0)	return;
//	if (nPrice > m_CurrentArt->GetCurrentPrice())
//	{
//		m_CurrentArt->SetCurrentPrice(nPrice, GetID());
//	}
//	else if (nPrice == m_CurrentArt->GetCurrentPrice())
//	{
//		m_CurrentArt->AddCurrentPrice(GetID());
//	}
//}

void CAuctionClient::OnCmdRespond(byte *pData, int count)
{
	UNREFERENCED_PARAMETER(pData);
	UNREFERENCED_PARAMETER(count);
}
