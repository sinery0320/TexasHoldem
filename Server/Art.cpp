#include "stdafx.h"
#include "Art.h"

using namespace Game;
using namespace Auction;
CArt::CArt(int id, int value)
	: m_nID(id)
	, m_Value(value)
	, m_StartPrice((int)(value * 0.5))
	, m_CurrentPrice(m_StartPrice)
	, m_bFailed(false)
{
}


CArt::~CArt()
{
}
