#include "pch.h"
#include "manual.h"
#include "GameData.h"
#include "dnfUser.h"
#include "scripts.h"
#include "dnfData.h"
#include "MirageDragon.h"
#include "MirageDragonDlg.h"
#include "dnfCALL.h"
#include "procData.h"
#include "log.h"
#include "keyboardDriver.h"
#include "usbhidkeycode.h"

// 选择角色
void ManualLogic::selectRole()
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

// 在城镇中
void ManualLogic::inTown()
{
	// 如果刚进入城镇
	if (!GAME.town_info.entered) {

		// 关闭其他状态的信息
		GAME.rolePanelClean();
		GAME.dungeonInfoClean();

		// 关闭图内的功能
		closeFunctions();

		// 初始化后更改进入状态
		GAME.town_info.entered = true;

		// 获取角色ID
		//Log.info(L"更新角色ID", true);
		//getRoleID();
	}
}

// 选择副本
void ManualLogic::selectDungeon()
{

}

// 在副本中
void ManualLogic::inDungeon()
{
	// 刷图线程开启，关闭选角线程、城镇线程、选图线程

	// 如果刚进入地图
	if (!GAME.dungeon_info.entered) {
		// 初始化副本信息
		initDungeonInfo();

		// 关闭其他状态的信息
		GAME.townInfoClean();

		// 开启副本功能
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
	getDungeonAllObj();

	//// 通关处理
	//if (is_open_door && is_boss)
	//{
	//	bool is_clearance = judgeClearance();
	//	if (is_clearance)
	//	{
	//		// 关闭图内功能
	//		closeFunctions();
	//		GAME.dungeonInfoClean();
	//		while (judgeIsBossRoom() && GAME.game_status == 3)
	//		{
	//			// 等待离开BOSS房间
	//			Sleep(1000);
	//		}
	//	}
	//}

		// 未开门时处理逻辑
	if (is_open_door == false)
	{
		// 打怪逻辑 
		if (PDATA.play_model) 
		{
			attackMonsterLogic();
		}
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
				// 关闭图内功能
				closeFunctions();
				GAME.dungeonInfoClean();
			}
		}
		else {

			if (PDATA.play_model)
			{
				// 捡物兜底
				finalGatherItems();

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

}

void ManualLogic::firstRoom()
{
	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;
	mainWindow->Log(L"开启首图功能");

	if (GAME.function_switch.score)
	{
		superScore();
	}

	// 锁定耐久
	//lockDurable();

	if (GAME.function_switch.cool_down)
	{
		CString num;
		mainWindow->page2._cool_down.GetWindowText(num);
		float number = (float)_ttof(num);
		skillCoolDown(number);
	}

	if (GAME.function_switch.hook_damage)
	{
		//hookDamage(true);
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
}

void ManualLogic::closeFunctions()
{
	if (GAME.function_switch.cool_down)
	{
		skillCoolDown(0);
	}

	if (GAME.function_switch.hook_damage)
	{
		//hookDamage(false);
	}

	if (GAME.function_switch.three_speed)
	{
		threeSpeed(0, 0, 0);
	}
}


// 打怪逻辑
void ManualLogic::attackMonsterLogic() 
{
	while (true)
	{
		if (!PDATA.play_model) {
			break;
		}
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

void ManualLogic::finalGatherItems()
{
	// 循环策略
	bool has_item = true;
	while (has_item)
	{
		if (!PDATA.play_model) {
			break;
		}
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
}