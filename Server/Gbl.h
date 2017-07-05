#pragma once
class CGbl
{
private:
	CGbl();
	~CGbl();

public:
	static CGbl& GetMe(){ static CGbl one; return one; }
	static CString GetAppPath();    // Last c is '\'
	static void SpliteBy(CString text, CString rule, std::vector<CString>& vtStr);

	int GetRand(int nEnd = 100);	// return random number in [0 ~ nEnd]
	static CString GetCurrentTimeStr(bool bMill = true);
};

