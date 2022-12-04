#include "pch.h"
#include "log.h"

void LogSystem::info(CString text)
{
	if (!log_file.Open(L"log", CFile::modeWrite | CFile::modeNoTruncate | CFile::modeCreate))
	{
		return;
	}
	log_file.SeekToEnd();
	CTime local_time = CTime::GetCurrentTime();
	CString date = local_time.Format(L"%H:%M:%S:") + text + "\n";
	log_file.Write(date.GetBuffer(), date.GetLength() * 2);
	log_file.Close();
}

void LogSystem::info(string text)
{
	if (!log_file.Open(L"log", CFile::modeWrite | CFile::modeNoTruncate | CFile::modeCreate))
	{
		return;
	}
	log_file.SeekToEnd();
	//CTime local_time = CTime::GetCurrentTime();
	//CString date = local_time.Format(L"%H:%M:%S:") + text + "\n";
	log_file.Write(text.c_str(), text.size());
	log_file.Close();
}

LogSystem Log;