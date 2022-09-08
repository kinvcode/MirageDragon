#include "pch.h"
#include "jobs.h"
#include "MirageDragonDlg.h"
#include "dnfData.h"
#include "Lock.h"

int game_status = 0;
bool window_top = false;
bool is_running = false;
__int64 C_USER = 0;
__int64 C_USER_POINTER = 0;

UINT updateDataThread(LPVOID pParam)
{
	CMirageDragonDlg* MainDlg = (CMirageDragonDlg*)pParam;

	bool statusChange = false;
	__int64 emptyAddress;

	{
		InstanceLock lock(MainDlg);
		MainDlg->Log(L"数据更新线程已启动");
	}

	while (true)
	{
		// 获取DNF窗口状态
		window_top = windowIsTop();

		// 游戏运行状态
		is_running = readInt(0x140000000) == 0x905A4D;
		if (!is_running) {
			// 游戏结束
			statusChange = false;
			MainDlg->Log(L"游戏已结束");
			// 停止线程
			return 0;
		}

		// 读取游戏状态
		game_status = readInt(C_GAME_STATUS);

		if (game_status >= 1 && statusChange == false) {
			emptyAddress = C_EMPTY_ADDRESS + 4000;
			C_USER_POINTER = getUserPointer(emptyAddress);
			C_USER = emptyAddress;
			if (C_USER_POINTER != 0)
			{
				statusChange = true;
				// 人物指针已改变
				MainDlg->Log(L"人物指针已改变");
			}
			// 重新读取人物指针

		}

		// 重新读取人物指针
		if (game_status == 0) {
			statusChange = false;
		}


		programDelay(300, 0);
	}
	return 0;
}

UINT playGameThead(LPVOID pParam)
{
	CMirageDragonDlg* MainDlg = (CMirageDragonDlg*)pParam;
	
	{
		InstanceLock lock(MainDlg);
		MainDlg->Log(L"刷图线程已启动");

		MainDlg->Log(L"刷图线程已结束");
	}

	return 0;
}