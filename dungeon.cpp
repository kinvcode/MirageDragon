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

queue<DUNGEONJOB> DungeonLogic::dg_list = {};

void DungeonLogic::selectRole()
{
	if (!GAME.role_panel.entered) {
		//time_t now_time = time(nullptr);
		//if (now_time - PDATA.last_update_roles > 60)
		//{
		//}

		GAME.role_panel.entered = true;

		// 更新角色列表
		getRoleList();
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

	//// 进入区域
	areaCall(code);
	// 组包选图
	selectMap();
	// 组包进图
	entryDungeon(code, 0, 0, 0);
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

		// 初始化副本信息
		initDungeonInfo();

		// 关闭其他状态的信息
		GAME.townInfoClean();

		// 开启功能
		firstRoom();

		// 初始化后更改进入状态
		GAME.dungeon_info.entered = true;
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
			//finalGatherItems();
			Log.info(L"准备过图");

			// 过图逻辑（自动进入下个房间）
			autoNextRoom();
		}
	}
}

void DungeonLogic::firstRoom()
{
	Log.info(L"开启首图功能");
	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;

	// BUFF逻辑
	{
		// 上上空格
		MSDK_keyPress(Keyboard_UpArrow, 1);
		MSDK_keyPress(Keyboard_UpArrow, 1);
		MSDK_keyPress(Keyboard_KongGe, 1);
		programDelay(350);

		// 右右空格
		MSDK_keyPress(Keyboard_RightArrow, 1);
		MSDK_keyPress(Keyboard_RightArrow, 1);
		MSDK_keyPress(Keyboard_KongGe, 1);
		programDelay(350);
	}

	if (GAME.function_switch.score)
	{
		superScore();
	}

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

	if (GAME.function_switch.hidden_user)
	{
		hiddenUser();
	}

	// 呼出面板，三速生效

	while (!hasPanel())
	{
		MSDK_keyPress(Keyboard_m, 1);
		Sleep(500);
	}
	while (hasPanel())
	{
		MSDK_keyPress(Keyboard_m, 1);
		Sleep(500);
	}

	penetrate(true);
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
			//int key = getCoolDownKey();
			MSDK_keyPress(Keyboard_x, 1);
		}
	}
}

void DungeonLogic::clearanceLogic()
{
	// 关闭图内功能
	closeFunctions();

	DUNGEONJOB* job = &dg_list.front();

	// 副本次数更新
	if (job->times != -1 && job->times != 0) {
		job->times--;
	}

	bool dungeon_finished = false;

	if (job->times == 0)
	{
		dg_list.pop();
		dungeon_finished = true;
	}

	// 更新任务信息
	updateData();

	while (judgeIsBossRoom() && GAME.game_status == 3) {

		Log.info(L"BOSS房间捡物");
		//finalGatherItems();

		// 如果没翻牌
		while (!judgeAwarded())
		{
			// 进行ESC翻牌
			MSDK_keyPress(Keyboard_ESCAPE, 1);
			programDelay(500);
		}

		// 获取商店类型
		int shop_type = getClearanceShop();
		if (shop_type == 1003)
		{
			// 关闭加百利商店
			MSDK_keyPress(Keyboard_ESCAPE, 1);
			programDelay(1000);
		}

		// 分解装备
		__int64 fatigue = getUserFatigue();

		if (fatigue == 0 && job->times == -1)
		{
			dungeon_finished = true;
		}

		GAME.dungeonInfoClean();
		// 判断任务是否完成
		if (dungeon_finished)
		{
			Log.info(L"当前副本任务已完成", true);
			// 返回城镇
			while (GAME.game_status == 3)
			{
				MSDK_keyPress(Keyboard_F12, 1);
			}
		}
		else {
			// 疲劳为空返回城镇
			if (fatigue < 1)
			{
				Log.info(L"当前副本任务失败：疲劳不足", true);
				// 任务失败，返回城镇
				while (GAME.game_status == 3)
				{
					MSDK_keyPress(Keyboard_F12, 1);
				};
			}
			// 再次挑战
			while (GAME.game_status == 3)
			{
				MSDK_keyPress(Keyboard_F10, 1);
			}
		}
	}
}

void DungeonLogic::finalGatherItems()
{
	// 循环策略
	bool has_item = true;
	// 关闭穿透
	penetrateMap(false);
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
	penetrateMap(true);
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