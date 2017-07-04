#include "stdafx.h"
#include "Gbl.h"


CGbl::CGbl()
{
}


CGbl::~CGbl()
{
}

void CGbl::SpliteBy(CString text, CString rule, std::vector<CString>& vtStr)
{
	int pos = 0;
	vtStr.clear();
	CString value = text.Tokenize(rule, pos);
	while (value != ""){
		vtStr.push_back(value);
		value = text.Tokenize(rule, pos);
	}
}