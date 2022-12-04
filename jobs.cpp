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
#include "http.h"

UINT updateDataThread(LPVOID pParam)
{
	CMirageDragonDlg* MainDlg = (CMirageDragonDlg*)pParam;

	{
		InstanceLock lock(MainDlg);
		MainDlg->Log(L"数据更新线程已启动");
	}

	while (true)
	{
		// 游戏运行状态
		GLOBAL.is_running = readInt(0x140000000) == 0x905A4D;
		if (!GLOBAL.is_running) {
			GLOBAL.PID = getProcessPID(L"DNF.exe");
			{
				InstanceLock lock(MainDlg);
				MainDlg->Log(L"丢失读写权限！");
				if (GLOBAL.PID == 0)
				{
					MainDlg->Log(L"游戏进程已丢失！");
					//reloadProcess();
				}
			}
			programDelay(3000, 0);
			continue;
		}

		// 获取DNF窗口状态
		GLOBAL.window_top = windowIsTop();

		// 读取游戏状态
		GLOBAL.game_status = readInt(ADDR.x64("C_GAME_STATUS"));

		programDelay(1000, 0);
	}
	return 0;
}

UINT playGameThead(LPVOID pParam)
{
	CMirageDragonDlg* MainDlg = (CMirageDragonDlg*)pParam;

	{
		InstanceLock lock(MainDlg);
		MainDlg->Log(L"刷图线程已启动");
	}


	while (true) {
		// 判断运行状态
		if (!GLOBAL.is_running) {
			// 应该停止线程
			goto threadEnd;
		}

		// 游戏不同状态的处理
		switch (GLOBAL.game_status)
		{
		case 0:
			// 选择角色界面
			selectRoleLogic();
			break;
		case 1:
			townLogic();
			break;
		case 2:
			// 选则副本界面处理（注意加载副本界面也是这个状态）
			selectDungeonLogic();
			break;
		case 3:
			// 副本中的逻辑处理
			dungeonLogic();
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

// 选择角色逻辑
void selectRoleLogic()
{

}

// 城镇逻辑
void townLogic()
{
	//// 城镇中。关闭图内功能
	//first_room = true;
	//is_clearance = false;
	//allow_next_map = false;
	//first_room_functions = false;

	//if (GLOBAL.is_auto_play)
	//{
	//	pass_room_numbers = 0;
	//	room_history.clear();

	//	// 剧情处理
	//	if (GLOBAL.auto_play_type == 2)
	//	{
	//		// 对话处理
	//		dialogue();
	//	}

	//	// 自动进图
	//	autoEntryDungeon();
	//}

}

// 选择副本逻辑
void selectDungeonLogic()
{
	// 如果选图时间过长，则退出城镇
}

// 副本中逻辑
void dungeonLogic()
{
	// 刷图线程开启，关闭选角线程、城镇线程、选图线程

	// 如果刚进入地图
	if (!GLOBAL.dungeon_info.entered) {
		// 初始化副本信息
		initDungeonInfo();

		// 开启副本功能
		firstRoomFunctions();

		// 初始化后更改进入状态
		GLOBAL.dungeon_info.entered = true;
	}

	// 更新人物坐标
	updateUserCoor();

	// 遍历地图（人物、物品、怪物...）
	getDungeonAllObj();

	// 判断当前是否是boos房间
	bool is_boss = judgeIsBossRoom();

	// 判断是否开门
	bool is_open_door = judgeDoorOpen();

	// 未开门时处理逻辑
	if (is_open_door == false)
	{
		// 打怪逻辑 
		attackMonsterLogic();
	}
	// 开门后的逻辑处理
	else {
		// 捡物兜底
		finalGatherItems();

		if (is_boss)
		{
			// 判断是否通关
			bool is_clearance = judgeClearance();
			if (judgeClearance())
			{
				// 通关处理
				clearanceLogic();
			}
		}
		else {
			passRoomLogic();
		}
	}
}

void attackMonsterLogic() 
{
	//if (GLOBAL.is_auto_play) {
	//	if (GLOBAL.dungeon_info.current_room.monster_list.size() > 0)
	//	{
	//		// 判断技能冷却列表并释放随机技能
	//		int key = getCoolDownKey();
	//		MSDK_keyPress(key, 1);
	//	}
	//}
}

void finalGatherItems() 
{
	// 循环策略

	// 如果存在物品
	if (GLOBAL.dungeon_info.current_room.item_list.size() > 0) {
		// 开门捡物逻辑 gatherAfterDoor
		//if (GLOBAL.is_auto_play)
		//{
		//	{
		//		InstanceLock lock(MainDlg);
		//		MainDlg->Log(L"存在物品，关闭穿透");
		//	}
		//	penetrate(false);
		//}
	}
}

void passRoomLogic() 
{
	//if (GLOBAL.is_auto_play)
	//{
	// // 如果当前没有开启穿透，则启用穿透
	//	{
	//		InstanceLock lock(MainDlg);
	//		MainDlg->Log(L"没有物品，开启穿透");
	//	}
	//	penetrate(true);
	//}

	// 进入下个房间逻辑
	//if (GLOBAL.is_auto_play) {
	//	{
	//		InstanceLock lock(MainDlg);
	//		MainDlg->Log(L"进入下个房间");
	//	}
	//	// 自动跑图
	//	// 获取当前房间位置
	//	ROOMCOOR current_room;
	//	current_room.x = readLong(readLong(readLong(readLong(ADDR.x64("C_ROOM_NUMBER")) + ADDR.x64("C_TIME_ADDRESS")) + ADDR.x64("C_DOOR_TYPE_OFFSET")) + ADDR.x64("C_CURRENT_ROOM_X"));
	//	current_room.y = readLong(readLong(readLong(readLong(ADDR.x64("C_ROOM_NUMBER")) + ADDR.x64("C_TIME_ADDRESS")) + ADDR.x64("C_DOOR_TYPE_OFFSET")) + ADDR.x64("C_CURRENT_ROOM_Y"));
	//	room_history.push_back(current_room);
	//	autoNextRoom();
	//	pass_room_numbers++;
	//}
}

// 通关逻辑
void clearanceLogic() 
{
	// 关闭图内功能
	closeDungeonFunctions();

	//if (GLOBAL.is_auto_play) {
	//	// 关闭自动功能
	//	GLOBAL.use_pass_room_call = false;
	//	if (GLOBAL.dungeon_info.current_room.item_list.size() < 1) {
	//		// 翻牌
	//		MSDK_keyPress(1, 1);
	//		programDelay(1000, 0);
	//		// 分解装备
	//		// ESC
	//		MSDK_keyPress(Keyboard_ESCAPE, 1);
	//		programDelay(600, 0);
	//		// 疲劳为空返回城镇
	//		if (allow_next_map) {
	//			if (getUserFatigue() < 1)
	//			{
	//				// 返回城镇
	//				MSDK_keyPress(Keyboard_F12, 1);
	//			}
	//			if (GLOBAL.auto_play_type == 1) {
	//				// 再次挑战
	//				MSDK_keyPress(Keyboard_F10, 1);
	//			}
	//		}
	//	}
	//}
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