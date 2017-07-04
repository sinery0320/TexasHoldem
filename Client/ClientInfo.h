#pragma once
class CClientInfo
{
private:
	int m_nID;
	int m_nTotalMoney;

public:
	void SetTotalMoney(int money){ m_nTotalMoney = money; }

public:
	CClientInfo(int id);
	~CClientInfo();
};

