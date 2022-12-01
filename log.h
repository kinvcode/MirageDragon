#pragma once
using namespace std;

class LogSystem
{
public:
	CFile log_file;
	LogSystem();
	~LogSystem();
	void info(CString text);
};

extern LogSystem Log;
