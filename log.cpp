#include "pch.h"
#include "log.h"
#include "MirageDragonDlg.h"
#include "MirageDragon.h"

void LogSystem::info(CString text)
{
	log_lock.lock();
	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;

	if (!log_file.Open(L"log.txt", CFile::modeWrite | CFile::modeNoTruncate | CFile::modeCreate))
	{
		log_lock.unlock();
		return;
	}
	log_file.SeekToEnd();
	CTime local_time = CTime::GetCurrentTime();
	CString date = local_time.Format(L"%H:%M:%S:") + text + "\r\n";
	mainWindow->Log(date);
	log_file.Write(date.GetBuffer(), date.GetLength() * 2);
	log_file.Close();
	log_lock.unlock();
}

//void LogSystem::info(string text)
//{
//	log_lock.lock();
//	if (!log_file.Open(L"log.txt", CFile::modeWrite | CFile::modeNoTruncate | CFile::modeCreate))
//	{
//		log_lock.unlock();
//		return;
//	}
//	log_file.SeekToEnd();
//
//	text += "\r\n";
//	log_file.Write(text.c_str(), (UINT)text.size());
//	log_file.Close();
//	log_lock.unlock();
//}

LogSystem Log;