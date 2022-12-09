#pragma once

#include <mutex>
using namespace std;

class LogSystem
{
public:
	mutex log_lock;
	CFile log_file;
	void info(CString text,bool console = false);
	//void info(string text);
};

extern LogSystem Log;
