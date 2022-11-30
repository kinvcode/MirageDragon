#include "pch.h"
#include "jobs.h"
#include "MirageDragonDlg.h"
#include "dnfData.h"
#include "Lock.h"
#include "scripts.h"
#include "keyboardDriver.h"
#include "dnfUser.h"
#include "dnfTask.h"
#include "baseAddress.h"
#include "GameData.h"

UINT updateDataThread(LPVOID pParam)
{
	CMirageDragonDlg* MainDlg = (CMirageDragonDlg*)pParam;

	bool statusChange = false; // 人物指针已改变

	{
		InstanceLock lock(MainDlg);
		MainDlg->Log(L"数据更新线程已启动");
	}

	while (true)
	{
		// 获取DNF窗口状态
		GLOBAL.window_top = windowIsTop();

		// 游戏运行状态
		GLOBAL.is_running = readInt(0x140000000) == 0x905A4D;
		if (!GLOBAL.is_running) {
			statusChange = false;
			GLOBAL.PID = getProcessPID(L"DNF.exe");
			{
				InstanceLock lock(MainDlg);
				MainDlg->Log(L"丢失读写权限！！！");
				if (GLOBAL.PID == 0)
				{
					MainDlg->Log(L"即将重启应用");
					//reloadProcess();
				}
			}
			programDelay(3000, 0);
			continue;
		}

		// 读取游戏状态
		GLOBAL.game_status = readInt(ADDR.x64("C_GAME_STATUS"));

		// 游戏不同状态的处理
		//switch (game_status)
		//{
		//case 0:
		//	// 选择角色界面
		//	statusChange = false;
		//	break;
		//case 1:
		//	// 城镇中。关闭图内功能
		//	break;
		//case 2:
		//	// 选择副本界面
		//	break;
		//case 3:
		//	// 图内中
		//	break;
		//default:
		//	break;
		//}

		programDelay(1000, 0);
	}
	return 0;
}

UINT playGameThead(LPVOID pParam)
{
	bool is_boss = false; // 当前为BOOS房间
	bool first_room = true; // 当前为第一个房间
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
		// 判断运行状态
		if (!GLOBAL.is_running) {
			goto threadEnd;
		}

		// 游戏不同状态的处理
		switch (GLOBAL.game_status)
		{
		case 0:
			// 选择角色界面
			break;
		case 1:
			// 城镇中。关闭图内功能
			first_room = true;
			is_clearance = false;
			allow_next_map = false;
			first_room_functions = false;

			if (GLOBAL.is_auto_play)
			{
				pass_room_numbers = 0;
				room_history.clear();

				// 剧情处理
				if (GLOBAL.auto_play_type == 2)
				{
					// 对话处理
					dialogue();
				}

				// 自动进图
				autoEntryDungeon();
			}

			break;
		case 2:
			// 选图界面处理

			// 如果选图时间过长，则退出城镇

			break;
		case 3:
			// 刷图线程开启，关闭选角线程、城镇线程、选图线程

		roomBegin:
			// 遍历物品和怪物信息
			getMonsterAndItems();

			// 聚集怪物
			convergeMonsterAndItems();

			// 判断当前是否是boos房间
			is_boss = judgeIsBossRoom();

			// 判断是否通关
			is_clearance = judgeClearance();

			// 判断当前是否是第一次进图
			if (first_room == true && is_clearance == false)
			{
				{
					InstanceLock lock(MainDlg);
					MainDlg->Log(L"开启首图功能");
				}
				first_room = false;
				firstRoomFunctions();
				first_room_functions = true;

				if (GLOBAL.is_auto_play)
				{
					MSDK_keyPress(Keyboard_m, 1);
					programDelay(400, 0);
					MSDK_keyPress(Keyboard_m, 1);
				}

			}

			// 判断是否开门
			is_open_door = judgeDoorOpen();

			// 未开门时处理逻辑
			if (is_open_door == false)
			{
				if (GLOBAL.is_auto_play) {

					if (GLOBAL.dungeon_info.current_room.monster_list.size() > 0)
					{
						// 判断技能冷却列表并释放随机技能
						int key = getCoolDownKey();
						MSDK_keyPress(key, 1);
					}
				}

			}
			else {
				// 开门后的逻辑处理
				programDelay(1000, 0);
				getMonsterAndItems();
				convergeMonsterAndItems();
				if (GLOBAL.dungeon_info.current_room.item_list.size() > 0 || GLOBAL.dungeon_info.current_room.monster_list.size() > 0) {
					if (GLOBAL.is_auto_play)
					{
						{
							InstanceLock lock(MainDlg);
							MainDlg->Log(L"存在物品，关闭穿透");
						}
						penetrate(false);
					}
					if (!is_boss) {
						// 给个延迟，防止没有捡到物品
						goto roomBegin;
					}
				}
				else {
					if (GLOBAL.is_auto_play)
					{
						{
							InstanceLock lock(MainDlg);
							MainDlg->Log(L"没有物品，开启穿透");
						}
						penetrate(true);
					}
					// 普通房间进行跑图
					if (!is_boss) {
						if (GLOBAL.is_auto_play) {
							{
								InstanceLock lock(MainDlg);
								MainDlg->Log(L"进入下个房间");
							}
							// 自动跑图
							// 获取当前房间位置
							ROOMCOOR current_room;
							current_room.x = readLong(readLong(readLong(readLong(ADDR.x64("C_ROOM_NUMBER")) + ADDR.x64("C_TIME_ADDRESS")) + ADDR.x64("C_DOOR_TYPE_OFFSET")) + ADDR.x64("C_CURRENT_ROOM_X"));
							current_room.y = readLong(readLong(readLong(readLong(ADDR.x64("C_ROOM_NUMBER")) + ADDR.x64("C_TIME_ADDRESS")) + ADDR.x64("C_DOOR_TYPE_OFFSET")) + ADDR.x64("C_CURRENT_ROOM_Y"));
							room_history.push_back(current_room);
							autoNextRoom();
							pass_room_numbers++;
						}
					}
				}

				if (is_boss)
				{
					if (is_clearance)
					{
						first_room = true;
						if (first_room_functions == true)
						{
							{
								InstanceLock lock(MainDlg);
								MainDlg->Log(L"关闭图内功能");
							}
							closeDungeonFunctions();
							first_room_functions = false;
						}

						if (GLOBAL.is_auto_play) {

							// 关闭自动功能
							GLOBAL.use_pass_room_call = false;
							pass_room_numbers = 0;
							room_history.clear();

							if (GLOBAL.dungeon_info.current_room.item_list.size() < 1) {
								// 翻牌
								MSDK_keyPress(1, 1);
								programDelay(1000, 0);
								// 分解装备

								allow_next_map = true;

								// ESC
								MSDK_keyPress(Keyboard_ESCAPE, 1);
								programDelay(600, 0);

								// 疲劳为空返回城镇

								if (allow_next_map) {
									if (getUserFatigue() < 1)
									{
										// 返回城镇
										MSDK_keyPress(Keyboard_F12, 1);
									}

									if (GLOBAL.auto_play_type == 1) {
										// 再次挑战
										MSDK_keyPress(Keyboard_F10, 1);
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
		if (GLOBAL.is_auto_play)
		{

		}

	threadEnd:
		Sleep(50);
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