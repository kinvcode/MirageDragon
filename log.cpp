#include "pch.h"
#include "log.h"

LogSystem::LogSystem() {
	if (!log_file.Open(L"log", CFile::modeCreate | CFile::modeWrite))
	{
		return;
	}
}

LogSystem::~LogSystem() {
	log_file.Close();
}


void LogSystem::info(CString text)
{
	CTime local_time = CTime::GetCurrentTime();
	CString date = local_time.Format(L"%H:%M:%S:") + text + "\n";
	//date = date + text + "\n";
	log_file.Write(date.GetBuffer(), date.GetLength() * 2);
}

LogSystem Log;