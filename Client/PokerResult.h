#pragma once
class CPokerResult
{
private:
	CPokerResult();
	~CPokerResult();

public:
	static CPokerResult& GetMe(){ static CPokerResult one; return one; }
	static CString GetPokerString(byte* p, int count);
};
