#include "pch.h"
#include "jobs.h"
#include "MirageDragonDlg.h"
#include "dnfData.h"
#include "Lock.h"
#include "scripts.h"
#include "keyboardDriver.h"
#include "dnfUser.h"

int game_status = 0;
bool window_top = false;
bool is_running = false;
__int64 C_USER = 0;
__int64 C_USER_POINTER = 0;

UINT updateDataThread(LPVOID pParam)
{
	CMirageDragonDlg* MainDlg = (CMirageDragonDlg*)pParam;

	__int64 emptyAddress;
	bool statusChange = false; // 人物指针已改变

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
			{
				InstanceLock lock(MainDlg);
				MainDlg->Log(L"游戏已结束");
			}

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
				{
					InstanceLock lock(MainDlg);
					MainDlg->Log(L"人物指针已改变");
				}
			}
			// 重新读取人物指针
		}

		// 游戏不同状态的处理
		switch (game_status)
		{
		case 0:
			// 选择角色界面
			statusChange = false;
			break;
		case 1:
			// 城镇中。关闭图内功能
			break;
		case 2:
			// 选择副本界面
			break;
		case 3:
			// 图内中
			break;
		default:
			break;
		}

		programDelay(300, 0);
	}
	return 0;
}

UINT playGameThead(LPVOID pParam)
{
	bool is_boss = false; // 当前为BOOS房间
	bool first_room = false; // 当前为第一个房间
	bool is_open_door = false; // 是否可以进入下个房间
	bool is_clearance = false; // 判断是否已通关

	CMirageDragonDlg* MainDlg = (CMirageDragonDlg*)pParam;

	{
		InstanceLock lock(MainDlg);
		MainDlg->Log(L"刷图线程已启动");
	}


	while (true) {
		// 游戏不同状态的处理
		switch (game_status)
		{
		case 0:
			// 选择角色界面
			break;
		case 1:
			// 城镇中。关闭图内功能
			first_room = false;
			is_clearance = false;

			break;
		case 3:
			// 遍历物品信息
			getMonsterAndItems();

			// 聚集怪物
			convergeMonsterAndItems();

			// 判断当前是否是boos房间
			is_boss = judgeIsBossRoom();

			// 判断当前是否是第一次进图
			if (first_room == false && is_clearance == false)
			{
				{
					InstanceLock lock(MainDlg);
					MainDlg->Log(L"开启首图功能");
				}
				first_room = true;
				firstRoomFunctions();
			}

			// 判断是否开门
			is_open_door = judgeDoorOpen();

			// 未开门时处理逻辑
			if (is_open_door == false)
			{
				if (is_auto_play) {
					// 判断技能冷却列表并释放随机技能
					int key = getCoolDownKey();
					MSDK_keyPress(key, 1);
				}

			}
			else {
				// 开门后的逻辑处理

				if (item_list.size() > 0) {
					//gather_item_times++;
					if(is_auto_play)
					{
						{
							InstanceLock lock(MainDlg);
							MainDlg->Log(L"存在物品，关闭穿透");
						}
						penetrate(false);
					}
				}
				else {
					if(is_auto_play)
					{
						{
							InstanceLock lock(MainDlg);
							MainDlg->Log(L"没有物品，开启穿透");
						}
						penetrate(true);
					}
					// 普通房间进行跑图
					if (!is_boss) {
						if (is_auto_play) {
							{
								InstanceLock lock(MainDlg);
								MainDlg->Log(L"进入下个房间");
							}
							autoNextRoom();
						}
					}
				}

				if (is_boss)
				{
					// 判断是否通关
					is_clearance = judgeClearance();
					if (is_clearance)
					{
						{
							InstanceLock lock(MainDlg);
							MainDlg->Log(L"关闭图内功能");
						}
						//closeDungeonFunctions();
					}
				}

			}

			// 判断是否已通关

			// 

			break;
		default:
			break;
		}


		// 自动开关
		if (is_auto_play)
		{

		}

		programDelay(300, 0);
	}

	{
		InstanceLock lock(MainDlg);
		MainDlg->Log(L"刷图线程已结束");
	}

	return 0;
}