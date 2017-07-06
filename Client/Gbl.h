#pragma once
class CGbl
{
private:
	CGbl();
	~CGbl();

public:
	static void SpliteBy(CString text, CString rule, std::vector<CString>& vtStr);
	static CString GetCurrentTimeStr(bool bMill = true);
};

