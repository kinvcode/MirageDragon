#include "pch.h"
#include "jobs.h"
#include "MirageDragonDlg.h"

UINT userPointerThread(LPVOID pParam)
{
	CMirageDragonDlg* MainDlg = (CMirageDragonDlg*)pParam;

	bool statusChange = false;
	__int64 emptyAddress;

	while (true)
	{
		// 如果切换到后台
		//if (!dnf->windowIsTop()) {
		//	//切换到后台，停止跑图;
		//	M_ReleaseAllKey(MainDlg->msdk_handle);
		//}

		if (readInt(0x140000000) != 0x905A4D) {
			// 游戏结束
			statusChange = false;
			MainDlg->Log(L"游戏已结束");
			break;

		}
		else {
			MainDlg->Log(L"更新人物指针");
		}

		//if (judgeGameStatus() >= 1 && statusChange == false)
		//{
		//	emptyAddress = C_EMPTY_ADDRESS + 4000;
		//	C_USER_POINTER = getUserPointer(emptyAddress);
		//	C_USER = emptyAddress;
		//	if (C_USER_POINTER == 0)
		//	{
		//		// 重新读取人物指针
		//		continue;
		//	}
		//	statusChange = true;
		//	// 人物指针已改变
		//	MainDlg->Log(L"人物指针已改变");
		//}
		//else if (judgeGameStatus() == 0)
		//{
		//	// 选择角色
		//	statusChange = false;
		//}
		//dnf->programDelay(300);
		Sleep(300);
	}
	return 0;
}