#include "pch.h"
#include "scripts.h"
#include "MirageDragon.h"
#include "MirageDragonDlg.h"
#include "dnfData.h"
#include "dnfMap.h"
#include "keyboardDriver.h"
#include "dnfCALL.h"
#include "dnfUser.h"
#include "dnfBase.h"
#include "dnfPacket.h"
#include "baseAddress.h"
#include "GameData.h"
#include "log.h"

// 跑到目标
void runToDestination(int x, int y, bool is_room = false, int target_range = 10)
{
	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;

	if (x < 1 || y < 1) {
		Log.info(L"位置太近，放弃跑图");
		return;
	}

	CString coor;
	coor.Format(L"目标坐标 X:%d Y:%d", x, y);
	Log.info(coor);


	COORDINATE user_coor = readCoordinate(readLong(ADDR.x64("C_USER_ADDRESS")));

	bool x_arrived = false, y_arrived = false, isFirst = true, arrive_next = false;

	int direction_x;
	int direction_y;

	if (x > user_coor.x) {
		direction_x = Keyboard_RightArrow;
	}
	else {
		direction_x = Keyboard_LeftArrow;
	}

	if (y > user_coor.y) {
		direction_y = Keyboard_DownArrow;
	}
	else {
		direction_y = Keyboard_UpArrow;
	}

	// 记录当前时间戳
	time_t cur_time = time(NULL);

	while (true) {
		// 防止东西没有捡
		getDungeonAllObj();
		if (GLOBAL.dungeon_info.current_room.item_list.size() > 0) 
		{
			Log.info(L"发现物品，停止跑图");
			break;
		}

		// 如果跑图时间超过5秒。则直接退出跑图
		if (time(NULL) - cur_time > 2) {
			Log.info(L"跑图超时，退出跑图");
			return;
		}

		user_coor = readCoordinate(readLong(ADDR.x64("C_USER_ADDRESS")));

		if (isFirst) {
			Log.info(L"开始跑图");

			MSDK_keyPress(direction_x, 1);
			programDelay(100, 0);
			MSDK_KeyDown(direction_x);
			programDelay(100, 0);
			MSDK_KeyDown(direction_y);

			// 跑向房间优先跑Y轴，防止意外进入其他房间
			if (is_room) {
				int x_diff = abs(x - user_coor.x);
				int y_diff = abs(y - user_coor.y);

				// 如果是上下移动，则立刻弹起左右方向键
				if (x_diff < y_diff) {
					MSDK_KeyUp(direction_x);
				}
			}
		}

		if (GLOBAL.game_status != 3) {
			//不在图内，停止跑图;
			Log.info(L"人物已离开图内，停止跑图");
			MSDK_KeyUp(direction_x);
			MSDK_KeyUp(direction_y);
			break;
		}

		// 判断人物动作
		__int64 user_action = decrypt(readLong(ADDR.x64("C_USER_ADDRESS")) + ADDR.x64("C_MOVEMENT_ID"));
		if ((int)user_action != 14) {
			CString msg;
			msg.Format(L"当前动作ID：%ld，非跑动状态已停止跑图", user_action);
			Log.info(msg);
			// 弹起移动按键
			MSDK_KeyUp(direction_x);
			MSDK_KeyUp(direction_y);
			break;
		}

		int y_range = 10;
		if (is_room) {
			y_range = 3;
		}

		// 优先到达Y坐标
		if (y_arrived == false) {
			if (direction_y == Keyboard_UpArrow) {
				if (y - user_coor.y > y_range) {
					Log.info(L"向上超出Y范围，停止");
					MSDK_KeyUp(direction_y);
					break;
				}
			}
			else {
				if (user_coor.y - y > y_range) {
					Log.info(L"向下超出Y范围，停止");
					MSDK_KeyUp(direction_y);
					break;
				}
			}

			if (abs(user_coor.y - y) > y_range) {
				isFirst = false;
				continue;
			}
			else {
				MSDK_KeyUp(direction_y);
				y_arrived = true;
			}
		}

		if (x_arrived == false) {
			if (direction_x == Keyboard_RightArrow) {
				if (user_coor.x - x > target_range) {
					Log.info(L"向右超出X范围，停止");
					MSDK_KeyUp(direction_x);
					break;
				}
			}
			else {
				if (x - user_coor.x > target_range) {
					Log.info(L"向左超出X范围，停止");
					MSDK_KeyUp(direction_x);
					break;
				}
			}
			if (abs(user_coor.x - x) > target_range) {
				isFirst = false;
				continue;
			}
			else {
				MSDK_KeyUp(direction_x);
				x_arrived = true;
			}
		}

		if (x_arrived && y_arrived) {
			Log.info(L"到达目标位置，停止按键");
			MSDK_KeyUp(direction_x);
			MSDK_KeyUp(direction_y);
			break;
		}
	}

	Log.info(L"跑图结束，停止按键");

	MSDK_KeyUp(direction_x);
	MSDK_KeyUp(direction_y);

	return;
}

void autoNextRoom()
{
	if (GLOBAL.game_status != 3) {
		return;
	}

	ROOMCOOR room_coor = GLOBAL.dungeon_info.current_room.coordinate;
	ROOMCOOR boss_coor = GLOBAL.dungeon_info.boos_room;

	if (room_coor.x == boss_coor.x && room_coor.y == boss_coor.y) {
		return;
	}

	int next_direction = judgeNextRoomDiretion();

	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;
	CString msg;
	msg.Format(L"跑图方向：%d", next_direction);
	Log.info(msg);

	// 左右上下0123
	runToNextRoom(next_direction);
}

void runToNextRoom(int direction)
{
	int target_room_x = GLOBAL.dungeon_info.current_room.coordinate.x; // 当前房间X坐标
	int target_room_y = GLOBAL.dungeon_info.current_room.coordinate.y; // 当前房间Y坐标

	__int64 pass_room_data = passRoomData(direction);
	__int64 coor_struct = pass_room_data;

	int begin_x, begin_y, end_x, end_y, calc_x, calc_y;
	begin_x = readInt(coor_struct + 0);
	begin_y = readInt(coor_struct + 4);
	end_x = readInt(coor_struct + 8);
	end_y = readInt(coor_struct + 12);

	int far_door;
	if (GLOBAL.use_pass_room_call) {
		far_door = 20;
	}
	else {
		far_door = 100; // 远离门的距离，防止卡门
	}

	switch (direction)
	{
		// 左
	case 0:
		target_room_x--;
		calc_x = begin_x + end_x + far_door;
		calc_y = begin_y + end_y / 2;
		break;
		// 右
	case 1:
		target_room_x++;
		calc_x = begin_x - far_door;
		calc_y = begin_y + end_y / 2;
		break;
		// 下
	case 2:
		target_room_y++;
		calc_x = begin_x + end_x / 2;
		calc_y = begin_y + end_y + far_door;
		break;
		// 上
	case 3:
		target_room_y--;
		calc_x = begin_x + end_x / 2;
		calc_y = begin_y - far_door;
		break;
	default:
		return;
		break;
	}

	if (calc_x < 0 || calc_y < 0)
	{
		return;
	}

	if (GLOBAL.use_pass_room_call) {
		if (GLOBAL.dungeon_info.current_room.room_has_urgent)
		{
			ROOMCOOR room = GLOBAL.dungeon_info.current_room.coordinate;
			passRoomByPacket(room.x, room.y);
		}
		else {
			coorCall(calc_x, calc_y, 0);
			programDelay(100, 0);
			coorCall(begin_x + end_x / 2, begin_y, 0);
		}
	}
	else {
		// 跑目标地点，如果不是向上跑图，则优化传送门位置
		int new_begin_y = begin_y;
		if (direction != 2) {
			new_begin_y = begin_y + 40;
		}

		runToDestination(begin_x + end_x / 2, new_begin_y, true, 2);

		int current_room_x = GLOBAL.dungeon_info.current_room.coordinate.x; // 当前房间X坐标
		int current_room_y = GLOBAL.dungeon_info.current_room.coordinate.y; // 当前房间Y坐标
		if (target_room_x != current_room_x || target_room_y != current_room_y)
		{
			return;
		}

		//programDelay(100, 0);

		// 远离目标地点（防止卡在入口处）
		runToDestination(calc_x, calc_y, true, 2);
	}
}

void firstRoomFunctions()
{
	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;

	mainWindow->Log(L"开启首图功能");

	if (GLOBAL.is_auto_play)
	{
		// 上上空格
		MSDK_keyPress(Keyboard_UpArrow, 1);
		MSDK_keyPress(Keyboard_UpArrow, 1);
		MSDK_keyPress(Keyboard_KongGe, 1);
		programDelay(350, 0);

		// 右右空格
		MSDK_keyPress(Keyboard_RightArrow, 1);
		MSDK_keyPress(Keyboard_RightArrow, 1);
		MSDK_keyPress(Keyboard_KongGe, 1);
		programDelay(350, 0);
	}

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

	// 呼出面板，三速生效
	if (GLOBAL.is_auto_play)
	{
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
	}
}

void closeDungeonFunctions()
{
	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;


	if (GLOBAL.function_switch.cool_down)
	{
		skillCoolDown(0);
	}

	if (GLOBAL.function_switch.three_speed)
	{
		threeSpeed(0, 0, 0);
	}
}