#include "pch.h"
#include "dailyJob.h"
#include "opencv.h"
#include "keyboardDriver.h"
#include "usbhidkeycode.h"
#include "GameData.h"

int DailyJobLogic::dungeon_times = 0;

bool DailyJobLogic::daily_complete = false;

bool DailyJobLogic::com_hook = false; // 普通房间HOOK伤害设置

bool DailyJobLogic::boss_hook = false; // 普通房间HOOK伤害设置

bool DailyJobLogic::job_faild = false; // 任务失败

void DailyJobLogic::initData()
{
	dungeon_times = 0;
	job_faild = false;
	daily_complete = false;
}

// 选择角色
void DailyJobLogic::selectRole()
{

}

// 在城镇中
void DailyJobLogic::inTown()
{
	// 判断任务是否完成
	if (dungeon_times >= 4)
	{
		Log.info(L"检测是否有面板", true);
		if (hasPanel()) {
			while (hasPanel())
			{
				MSDK_keyPress(Keyboard_ESCAPE, 1);
				Sleep(300);
			}
		}

		// 打开每日
		Log.info(L"打开每日面板", true);
		ROOMCOOR coor = OpenCV::getDailyIcon1();
		if (coor.x != 0 && !hasPanel()) {
			MSDK_MoveTo(coor.x, coor.y);
			Sleep(500);
			MSDK_LeftClick();
			Sleep(1000);
			
			bool submit = true;
			while (submit && hasPanel())
			{
				Log.info(L"点击完成按钮", true);
				ROOMCOOR coor2 = OpenCV::getDailySubmit();
				if (coor2.x != 0) {
					MSDK_MoveTo(coor2.x, coor2.y);
					Sleep(500);
					MSDK_LeftClick();
					Sleep(1000);
				}
				else {
					Log.info(L"完成按钮已点击完毕", true);
					submit = false;
					Log.info(L"退出鼠标事件", true);
					daily_complete = true;
					break;
				}
				Sleep(2000);
			}
		}
		return;
	}
	else {
		// 判断疲劳
		if (getUserFatigue() > 0) {
			Log.info(L"疲劳满足，开始寻找传送位置", true);

			Log.info(L"检测是否有面板", true);
			if (hasPanel()) {
				while (hasPanel())
				{
					MSDK_keyPress(Keyboard_ESCAPE, 1);
					Sleep(300);
				}
			}

			// 打开每日
			Log.info(L"打开每日", true);
			ROOMCOOR coor = OpenCV::getDailyIcon1();
			if (coor.x != 0 && !hasPanel()) {
				MSDK_MoveTo(coor.x, coor.y);
				Sleep(500);
				MSDK_LeftClick();
				Sleep(1000);

				// 点击传送按钮
				Log.info(L"点击传送", true);
				ROOMCOOR coor2 = OpenCV::getDelivery();
				if (coor2.x != 0 && hasPanel()) {
					MSDK_MoveTo(coor2.x, coor2.y);
					Sleep(500);
					MSDK_LeftClick();
					Sleep(1000);
					// 获取确认弹窗
					Log.info(L"点击确认", true);
					ROOMCOOR coor3 = OpenCV::getDailySpace();
					if (coor3.x != 0 && hasPanel())
					{
						while (GAME.game_status == 1)
						{
							MSDK_keyPress(Keyboard_KongGe, 1);
							Sleep(200);
						}
					}
				}
				else {
					// 没有传送按钮，任务失败
					Log.info(L"找不到传送按钮，任务失败", true);
					job_faild = true;
					return;
				}
			}
		}
		else {
			return;
		}
	}
}

// 选择副本
void DailyJobLogic::selectDungeon()
{
	while (GAME.game_status == 2)
	{
		MSDK_keyPress(Keyboard_KongGe, 1);
		Sleep(200);
	}
}

// 在副本中
void DailyJobLogic::inDungeon()
{
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

			// 过图逻辑（自动进入下个房间）
			
			if (GAME.penetrate_status != 1) {
				penetrate(true);
			}
			int map_code = getMapNumber();
			if (map_code == 100002705) {
				if (GAME.dungeon_info.current_room.coordinate.x == 2 || GAME.dungeon_info.current_room.coordinate.x == 3) 
				{
					GAME.use_pass_room_call = true;
				}
			}
			autoNextRoom();
		}
	}
}

// 首图功能
void DailyJobLogic::firstRoom()
{
	Log.info(L"开启首图功能");
	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;

	// BUFF逻辑
	{
		// 上上空格
		MSDK_keyPress(Keyboard_UpArrow, 1);
		MSDK_keyPress(Keyboard_UpArrow, 1);
		MSDK_keyPress(Keyboard_KongGe, 1);
		Sleep(350);

		// 右右空格
		MSDK_keyPress(Keyboard_RightArrow, 1);
		MSDK_keyPress(Keyboard_RightArrow, 1);
		MSDK_keyPress(Keyboard_KongGe, 1);
		Sleep(350);
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
	panelCall(0);
	Sleep(200);
	panelCall(0);

	penetrate(true);
}

// 关闭图内功能
void DailyJobLogic::closeFunctions()
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

// 打怪逻辑
void DailyJobLogic::attackMonsterLogic()
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
			if (key == Keyboard_x) {
				for (int i = 0; i < 4; i++)
				{
					MSDK_keyPress(key, 1);
					Sleep(300);
				}
			}
			else {
				MSDK_keyPress(key, 1);
			}
		}
	}
}

// 通关处理
void DailyJobLogic::clearanceLogic()
{
	// 关闭图内功能
	closeFunctions();

	// 副本次数更新
	dungeon_times++;

	// 每日是否完成
	bool dungeon_finished = false;
	if (dungeon_times >= 4)
	{
		dungeon_finished = true;
	}

	while (judgeIsBossRoom() && GAME.game_status == 3) {

		finalGatherItems();

		// 如果没翻牌
		while (!judgeAwarded())
		{
			// 进行ESC翻牌
			MSDK_keyPress(Keyboard_ESCAPE, 1);
			Sleep(500);
		}

		// 获取商店类型
		int shop_type = getClearanceShop();
		if (shop_type == 1003)
		{
			// 关闭加百利商店
			MSDK_keyPress(Keyboard_ESCAPE, 1);
			Sleep(1000);
		}

		// 分解装备
		if (getBackpackLoad() > 50) {
			getPackageOfEq();
		}

		finalGatherItems();

		// 重置伤害
		com_hook = false;
		boss_hook = false;

		// 疲劳判断
		int fatigue = getUserFatigue();
		if (fatigue == 0)
		{
			// 返回城镇进行下一轮逻辑处理
			while (GAME.game_status == 3)
			{
				MSDK_keyPress(Keyboard_F12, 1);
			}
		}

		GAME.dungeonInfoClean();

		// 判断任务是否完成
		if (dungeon_finished)
		{
			// 返回城镇
 			while (GAME.game_status == 3)
			{
				MSDK_keyPress(Keyboard_F12, 1);
			}
		}
		else {
			// 继续挑战下一个副本
			while (GAME.game_status == 3)
			{
				MSDK_keyPress(Keyboard_KongGe, 1);
			}
		}
	}
}

// 捡物兜底
void DailyJobLogic::finalGatherItems()
{
	// 循环策略
	bool has_item = true;
	// 关闭穿透
	penetrate(false);
	while (has_item)
	{
		getDungeonAllObj();
		has_item = (bool)GAME.dungeon_info.current_room.item_list.size();
		// 如果存在物品
		if (has_item) {
			updateUserCoor();
			for (auto item : GAME.dungeon_info.current_room.item_list) {
				gatherAtUser(GAME.dungeon_info.user_coordinate, item);
			}

			// 判断脚下是否有物品
			while (itemUnderFooter())
			{
				// x捡物
				MSDK_keyPress(Keyboard_x, 1);
				Sleep(300);
			}
		}
	}
	penetrate(false);
}

void DailyJobLogic::updateData()
{
	// 提交任务完成类型
	http.updateJob(2);
}

void DailyJobLogic::setGeneralHook()
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

void DailyJobLogic::setBossHook()
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