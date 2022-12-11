#include "pch.h"
#include "manual.h"
#include "GameData.h"
#include "dnfUser.h"
#include "scripts.h"
#include "dnfData.h"
#include "MirageDragon.h"
#include "MirageDragonDlg.h"
#include "dnfCALL.h"

// 选择角色
void ManualLogic::selectRole()
{
	time_t now_time = time(nullptr);
	if (now_time - GLOBAL.last_update_roles > 60)
	{
		// 更新角色列表
		getRoleList();
	}
}

// 在城镇中
void ManualLogic::inTown()
{
	// 如果刚进入城镇
	if (!GLOBAL.town_info.entered) {

		// 关闭其他状态的信息
		GLOBAL.dungeonInfoClean();

		// 关闭图内的功能
		closeDungeonFunctions();

		// 初始化后更改进入状态
		GLOBAL.town_info.entered = true;
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
	if (!GLOBAL.dungeon_info.entered) {
		// 初始化副本信息
		initDungeonInfo();

		// 关闭其他状态的信息
		GLOBAL.townInfoClean();

		// 开启副本功能
		firstRoom();

		// 初始化后更改进入状态
		GLOBAL.dungeon_info.entered = true;
	}

	// 更新人物坐标
	updateUserCoor();

	// 判断当前是否是boos房间(并且更新当前房间坐标)
	bool is_boss = judgeIsBossRoom();

	// 判断是否开门
	bool is_open_door = judgeDoorOpen();

	// 遍历地图（人物、物品、怪物...）
	getDungeonAllObj();

	// 通关处理
	if (is_open_door && is_boss)
	{
		bool is_clearance = judgeClearance();
		if (is_clearance)
		{
			// 关闭图内功能
			closeDungeonFunctions();
			GLOBAL.dungeonInfoClean();
			while (judgeIsBossRoom() && GLOBAL.game_status == 3)
			{
				// 等待离开BOSS房间
				programDelay(1000, 1);
			}
		}
	}
}

void ManualLogic::firstRoom()
{
	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;
	mainWindow->Log(L"开启首图功能");

	if (GLOBAL.function_switch.score)
	{
		superScore();
	}

	if (GLOBAL.function_switch.cool_down)
	{
		CString num;
		mainWindow->page2._cool_down.GetWindowText(num);
		float number = (float)_ttof(num);
		skillCoolDown(number);
	}

	if (GLOBAL.function_switch.hook_damage)
	{
		//hookDamage(true);
	}

	if (GLOBAL.function_switch.three_speed)
	{
		CString attack_speed, move_speed, casting_speed;
		mainWindow->page2._attack_speed.GetWindowText(attack_speed);
		mainWindow->page2._move_speed.GetWindowText(move_speed);
		mainWindow->page2._casting_speed.GetWindowText(casting_speed);
		threeSpeed(_ttoi(attack_speed), _ttoi(casting_speed), _ttoi(move_speed));
	}

	if (GLOBAL.function_switch.hidden_user)
	{
		hiddenUser();
	}
}

void ManualLogic::closeFunctions()
{
	if (GLOBAL.function_switch.cool_down)
	{
		skillCoolDown(0);
	}

	if (GLOBAL.function_switch.hook_damage)
	{
		//hookDamage(false);
	}

	if (GLOBAL.function_switch.three_speed)
	{
		threeSpeed(0, 0, 0);
	}
}