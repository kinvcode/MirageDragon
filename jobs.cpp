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
bool use_pass_room_call = false;

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
				MainDlg->Log(L"丢失读写权限！！！");
			}
			Sleep(3000);
			continue;
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
		Sleep(300);
		//programDelay(300, 0);
	}
	return 0;
}

UINT playGameThead(LPVOID pParam)
{
	bool is_boss = false; // 当前为BOOS房间
	bool first_room = false; // 当前为第一个房间
	bool first_room_functions = false; // 首图功能是否开启
	bool is_open_door = false; // 是否可以进入下个房间
	bool is_clearance = false; // 判断是否已通关
	bool allow_next_map = false; // 允许进入下个地图
	int pass_room_numbers = 0; // 进入下个房间次数
	vector<ROOMCOOR> room_history; // 路过的房间历史数据

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
			allow_next_map = false;
			use_pass_room_call = false;
			pass_room_numbers = 0;
			room_history.clear();
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
				first_room_functions = true;
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
					if (is_auto_play)
					{
						{
							InstanceLock lock(MainDlg);
							MainDlg->Log(L"存在物品，关闭穿透");
						}
						penetrate(false);
					}
				}
				else {
					if (is_auto_play)
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
							// 自动跑图
							// 获取当前房间位置
							ROOMCOOR current_room;
							current_room.x = readLong(readLong(readLong(readLong(C_ROOM_NUMBER) + C_TIME_ADDRESS) + C_DOOR_TYPE_OFFSET) + C_CURRENT_ROOM_X);
							current_room.y = readLong(readLong(readLong(readLong(C_ROOM_NUMBER) + C_TIME_ADDRESS) + C_DOOR_TYPE_OFFSET) + C_CURRENT_ROOM_Y);
							room_history.push_back(current_room);
							if (roomRepeats(room_history, current_room) > 2)
							{
								use_pass_room_call = true;
							}
							else {
								use_pass_room_call = false;
							}
							autoNextRoom();
							pass_room_numbers++;
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

						if (first_room_functions == true)
						{
							//closeDungeonFunctions();
							first_room_functions = false;
						}

						if (is_auto_play) {

							// 关闭自动功能
							use_pass_room_call = false;
							pass_room_numbers = 0;
							room_history.clear();

							if (item_list.size() < 1) {
								// 翻牌
								MSDK_keyPress(1, 1);
								MSDK_DelayRandom(500, 1000);
								// 分解装备

								allow_next_map = true;
								// ESC
								//MSDK_keyPress(Keyboard_ESCAPE, 1);

								if (allow_next_map) {
									if (auto_play_type == 1) {
										// 再次挑战
										MSDK_keyPress(Keyboard_F10, 1);
									}
									else {
										// 返回城镇
										MSDK_keyPress(Keyboard_F12, 1);
									}
								}
							}
						}
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
		Sleep(300);
		//programDelay(300, 0);
	}

	{
		InstanceLock lock(MainDlg);
		MainDlg->Log(L"刷图线程已结束");
	}

	return 0;
}

int roomRepeats(vector<ROOMCOOR> list, ROOMCOOR room)
{
	__int64 length = list.size();
	if (length <= 1)
	{
		return 1;
	}

	int numbers = 0;

	for (__int64 i = 0; i < length; i++)
	{
		if (list[i].x == room.x && list[i].y == room.y)
		{
			numbers++;
		}
	}
	return numbers;
}