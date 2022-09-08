#include "pch.h"
#include "dnfUser.h"
#include "MirageDragon.h"
#include "MirageDragonDlg.h"
#include "Lock.h"

void penetrate(bool on)
{
	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;
	InstanceLock wind(mainWindow);
	if (on) {
		writeInt(readLong(C_USER) + C_PENETRATE_MAP, -255);
		writeInt(readLong(C_USER) + C_PENETRATE_BUILDING, -255);
		penetrate_status = 1;
		mainWindow->Log(L"�ѿ�����͸");
	}
	else {
		writeInt(readLong(C_USER) + C_PENETRATE_MAP, 10);
		writeInt(readLong(C_USER) + C_PENETRATE_BUILDING, 40);
		penetrate_status = 0;
		mainWindow->Log(L"�ѹرմ�͸");
	}
}