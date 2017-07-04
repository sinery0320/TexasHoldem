#include "stdafx.h"
#include "Poker.h"

using namespace Game;
using namespace Texas;

CPoker::CPoker(byte val, byte type)
	: m_value(val)
	, m_type(type)
	, m_bOut(false)
{
}


CPoker::~CPoker()
{
}
