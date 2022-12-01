#pragma once
using namespace std;

class LogSystem
{
public:
	CFile log_file;
	void info(CString text);
};

extern LogSystem Log;
