#include "pch.h"
#include "dungeon.h"
#include "procData.h"
#include "dnfUser.h"
#include "scripts.h"
#include "GameData.h"
#include "log.h"
#include "dnfData.h"
#include "keyboardDriver.h"
#include "usbhidkeycode.h"
#include "dnfCALL.h"
#include "MirageDragonDlg.h"
#include "MirageDragon.h"
#include "dnfPacket.h"
#include "dataStruct.h"
#include "http.h"
#include "dnfBase.h"
#include "baseAddress.h"

queue<DUNGEONJOB> DungeonLogic::dg_list = {};

bool DungeonLogic::com_hook = false; // 普通房间HOOK伤害设置

bool DungeonLogic::boss_hook = false; // 普通房间HOOK伤害设置

void DungeonLogic::selectRole()
{
	if (!GAME.role_panel.entered) {
		//time_t now_time = time(nullptr);
		//if (now_time - PDATA.last_update_roles > 60)
		//{
		//}

		// 关闭其他状态的信息
		GAME.dungeonInfoClean();
		GAME.townInfoClean();

		GAME.role_panel.entered = true;

		// 更新角色列表
		//getRoleList();
	}

}

void DungeonLogic::inTown()
{
	// 如果刚进入城镇
	if (!GAME.town_info.entered) {
		// 初始化城镇信息

		// 关闭其他状态的信息
		GAME.rolePanelClean();
		GAME.dungeonInfoClean();

		// 关闭图内的功能(防止直接出图)
		closeFunctions();

		// 初始化后更改进入状态
		GAME.town_info.entered = true;

		// 获取角色ID
		//getRoleID();

		// 检查疲劳状态
		if (getUserFatigue() == 0) {
			return;
		}
	}

	// 没有任务则不处理
	if (dg_list.size() < 1) {
		return;
	}

	// 读取当前要处理的副本ID
	int code = dg_list.front().dungeon_code;

	// 读取人物等级
	int level = getUserLevel();
	// 读取人物名望
	int prestige = getUserPrestige();
	// 地图难度 0普通 1冒险 2勇士 3王者 4噩梦
	int difficulty = 0;
	if (level >= 110)
	{
		if (prestige >= 25837)
		{
			// 英豪
			if (prestige >= 29369) {
				difficulty = 1;
			}
			if (prestige >= 30946) {
				difficulty = 2;
			}
			if (prestige >= 32523) {
				difficulty = 3;
			}
			if (prestige >= 33989) {
				difficulty = 4;
			}
		}
		else {
			// 普通
			if (prestige >= 8602) {
				difficulty = 1;
			}
			if (prestige >= 13195) {
				difficulty = 2;
			}
			if (prestige >= 21675) {
				difficulty = 3;
			}
			if (prestige >= 23259) {
				difficulty = 4;
			}
		}
	}

	//// 进入区域
	areaCall(code);
	Sleep(500);
	// 组包选图
	selectMap();
	Sleep(500);
	// 组包进图
	entryDungeon(code, difficulty, 0, 0);
	Sleep(500);
}

void DungeonLogic::selectDungeon()
{
	// 如果选图时间过长，则退出城镇
}
void DungeonLogic::inDungeon()
{
	// 刷图线程开启，关闭选角线程、城镇线程、选图线程

	// 如果刚进入地图
	if (!GAME.dungeon_info.entered) {
		Log.info(L"准备初始化副本", true);
		// 单独的BOSS房间判断(防止再次挑战问题)
		if (!dungeonRoomIsBoss())
		{
			Log.info(L"初始化副本", true);
			// 初始化副本信息
			initDungeonInfo();

			// 关闭其他状态的信息
			GAME.townInfoClean();

			// 开启功能
			firstRoom();

			// 初始化后更改进入状态
			GAME.dungeon_info.entered = true;
		}
	}

	// 更新人物坐标
	updateUserCoor();

	// 判断当前是否是boos房间(并且更新当前房间坐标)
	bool is_boss = judgeIsBossRoom();

	// 判断是否开门
	bool is_open_door = judgeDoorOpen();

	// 遍历地图（人物、物品、怪物...）
	Log.info(L"遍历怪物与物品");
	getDungeonAllObj();

	// 未开门时处理逻辑
	if (is_open_door == false)
	{
		Log.info(L"当前未开门，开始清怪");

		if (is_boss) {
			// HOOK伤害设置
			setBossHook();
		}
		else {
			// HOOK伤害设置
			setGeneralHook();
		}

		// 打怪逻辑 
		attackMonsterLogic();
	}
	// 开门后的逻辑处理
	else {
		if (is_boss)
		{
			Log.info(L"BOSS房间怪物清理完毕");
			// 判断是否通关（篝火判断）
			bool is_clearance = judgeClearance();
			if (is_clearance)
			{
				Log.info(L"开始通关处理");
				// 通关处理
				clearanceLogic();
			}
		}
		else {
			// 捡物兜底
			Log.info(L"已开门，准备捡物");
			finalGatherItems();
			Log.info(L"准备过图");

			// 如果存在时空旋涡
			if (GAME.dungeon_info.current_room.room_has_urgent)
			{
				// 取出时空旋涡位置
				ROOMCOOR urgent_coor = getUrgentCoor();
				// 跑到时空旋涡处
				runToMonter(urgent_coor.x, urgent_coor.y);
				return;
			}
			else {
				// 过图逻辑（自动进入下个房间）
				autoNextRoom();
			}
		}
	}
}

void DungeonLogic::firstRoom()
{
	Log.info(L"开启首图功能");
	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;

	if (GAME.function_switch.hidden_user)
	{
		hiddenUser();
	}

	if (GAME.function_switch.score)
	{
		superScore();
	}

	// 锁定耐久
	lockDurable();

	if (GAME.function_switch.cool_down)
	{
		CString num;
		mainWindow->page2._cool_down.GetWindowText(num);
		float number = (float)_ttof(num);
		skillCoolDown(number);
	}

	if (GAME.function_switch.three_speed)
	{
		CString attack_speed, move_speed, casting_speed;
		mainWindow->page2._attack_speed.GetWindowText(attack_speed);
		mainWindow->page2._move_speed.GetWindowText(move_speed);
		mainWindow->page2._casting_speed.GetWindowText(casting_speed);
		threeSpeed(_ttoi(attack_speed), _ttoi(casting_speed), _ttoi(move_speed));
	}

	// 呼出面板，三速生效
	panelCall(0);
	Sleep(200);
	panelCall(0);

	penetrate(true);

	// BUFF逻辑
	{
		// 上上空格
		MSDK_keyPress(Keyboard_UpArrow, 1);
		Sleep(100);
		MSDK_keyPress(Keyboard_UpArrow, 1);
		Sleep(100);
		MSDK_keyPress(Keyboard_KongGe, 1);
		Sleep(800);

		// 右右空格
		MSDK_keyPress(Keyboard_RightArrow, 1);
		Sleep(100);
		MSDK_keyPress(Keyboard_RightArrow, 1);
		Sleep(100);
		MSDK_keyPress(Keyboard_KongGe, 1);
		Sleep(500);
	}
}

void DungeonLogic::closeFunctions()
{
	if (GAME.function_switch.cool_down)
	{
		skillCoolDown(0);
	}

	if (GAME.function_switch.three_speed)
	{
		threeSpeed(0, 0, 0);
	}

	penetrate(false);
}

void DungeonLogic::attackMonsterLogic()
{
	while (true)
	{
		// 刷新怪物
		getDungeonAllObj();

		// 获取当前怪物数量
		int monster_num = (int)GAME.dungeon_info.current_room.monster_list.size();

		if (monster_num < 1)
		{
			break;
		}

		// 跟随怪物
		DUNGEONOBJ cur = GAME.dungeon_info.current_room.monster_list.front();
		bool res = runToMonter(cur.coor.x, cur.coor.y);
		if (res) {
			// 攻击怪物
			int key = getCoolDownKey();
			MSDK_keyPress(key, 1);
		}
	}
}

void DungeonLogic::clearanceLogic()
{
	// 关闭图内功能
	closeFunctions();

	DUNGEONJOB* job = &dg_list.front();

	// 副本次数更新
	if (job->times > 0) {
		job->times--;
	}

	bool dungeon_finished = false;

	if (job->times == 0)
	{
		if (getUserFatigue() > 0)
		{
			dg_list.pop();
			// 更新任务信息
			updateData();
		}
		dungeon_finished = true;
	}

	if (judgeIsBossRoom() && GAME.game_status == 3) {

		// 如果没翻牌
		while (!judgeAwarded())
		{
			// 进行ESC翻牌
			Log.info(L"进行翻牌", true);
			MSDK_keyPress(Keyboard_ESCAPE, 1);
			Sleep(500);
		}

		// 获取商店类型
		int shop_type = getClearanceShop();
		if (shop_type == 1003)
		{
			Log.info(L"关闭商店", true);
			// 关闭加百利商店
			MSDK_keyPress(Keyboard_ESCAPE, 1);
			Sleep(1000);
		}

		// 分解装备
		if (getBackpackLoad() > 50) {
			Log.info(L"分解装备", true);
			getPackageOfEq();
		}

		Log.info(L"BOSS房间聚物", true);
		finalGatherItems();

		// 重置伤害
		com_hook = false;
		boss_hook = false;

		// 疲劳判断
		int fatigue = getUserFatigue();
		if (fatigue == 0 && job->times == -1)
		{
			dungeon_finished = true;
		}

		// 判断任务是否完成
		if (dungeon_finished)
		{
			Log.info(L"当前副本任务已完成", true);
			// 返回城镇
			MSDK_keyPress(Keyboard_F12, 1);
			Sleep(333);
		}
		else {
			// 疲劳为空返回城镇
			if (fatigue < 1)
			{
				Log.info(L"当前副本任务失败：疲劳不足", true);
				// 任务失败，返回城镇
				MSDK_keyPress(Keyboard_F12, 1);
				Sleep(333);
			}
			else {
				Log.info(L"再次挑战", true);
				MSDK_keyPress(Keyboard_F10, 1);
				Sleep(5000);
			}
		}
	}
	GAME.dungeonInfoClean();
}

void DungeonLogic::finalGatherItems()
{
	// 循环策略
	bool has_item = true;
	// 关闭穿透
	penetrate(false);
	while (has_item)
	{
		Log.info(L"刷新怪物与物品");
		getDungeonAllObj();
		has_item = (bool)GAME.dungeon_info.current_room.item_list.size();
		// 如果存在物品
		if (has_item) {
			Log.info(L"存在物品");
			updateUserCoor();
			for (auto item : GAME.dungeon_info.current_room.item_list) {
				gatherAtUser(GAME.dungeon_info.user_coordinate, item);
			}

			// 判断脚下是否有物品
			while (itemUnderFooter())
			{
				Log.info(L"进行捡物");
				// x捡物
				MSDK_keyPress(Keyboard_x, 1);
				Sleep(300);
			}
		}
	}
	Log.info(L"捡物完毕");
	penetrate(true);
}

void DungeonLogic::initDG()
{
	JOB dungeon_array = PDATA.jobs_list.front().job.front();
	json json_data = dungeon_array.data;

	for (json::iterator it = json_data.begin(); it != json_data.end(); ++it) {
		dg_list.push(it->get<DUNGEONJOB>());
	}
}

void DungeonLogic::updateData()
{
	// 提交任务完成类型
	http.updateJob(0);
}

void DungeonLogic::setGeneralHook()
{
	if (!com_hook) {
		int hook_value = GAME.dungeon_info.monster_max_blood;
		if (hook_value != 0) {
			hook_value = hook_value / 4;
			com_hook = true;
			CString msg;
			msg.Format(L"已设置普通房间HOOK伤害为：%d", hook_value);
			Log.info(msg, true);
			updateHookValue(hook_value);
		}

	}
}

void DungeonLogic::setBossHook()
{
	if (!boss_hook)
	{
		int hook_value = GAME.dungeon_info.monster_max_blood;
		if (hook_value != 0) {
			hook_value = hook_value / 8;
			com_hook = true;
			CString msg;
			msg.Format(L"已设置BOSS房间HOOK伤害为：%d", hook_value);
			Log.info(msg, true);
			updateHookValue(hook_value);
		}
	}
}

bool DungeonLogic::dungeonRoomIsBoss()
{
	// 更新时间指针和门型指针（每次进图会变化/副本发生变化时）
	__int64 time_pointer = readLong(readLong(ADDR.x64("C_ROOM_NUMBER")) + ADDR.x64("C_TIME_ADDRESS"));
	__int64 door_pointer = readLong(time_pointer + ADDR.x64("C_DOOR_TYPE_OFFSET"));
	// 获取当前房间位置
	ROOMCOOR current_coor;
	current_coor.x = readInt(time_pointer + ADDR.x64("C_BEGIN_ROOM_X"));
	current_coor.y = readInt(time_pointer + ADDR.x64("C_BEGIN_ROOM_Y"));

	// 获取BOSS房间位置
	ROOMCOOR boss_coor;
	boss_coor.x = (int)decrypt(door_pointer + ADDR.x64("C_BOSS_ROOM_X"));
	boss_coor.y = (int)decrypt(door_pointer + ADDR.x64("C_BOSS_ROOM_Y"));
	if (current_coor.x == boss_coor.x && current_coor.y == boss_coor.y)
	{
		return true;
	}

	return false;
}