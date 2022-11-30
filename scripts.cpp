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

// 跑到目标
void runToDestination(int x, int y, bool is_room = false, int target_range = 10)
{
	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;
	InstanceLock wind(mainWindow);

	if (x < 1 || y < 1) {
		{
			InstanceLock lock(mainWindow);
			mainWindow->Log(L"位置太近，放弃跑图");
		}
		return;
	}

	CString coor;
	coor.Format(L"目标坐标 X:%d Y:%d", x, y);
	{
		InstanceLock lock(mainWindow);
		mainWindow->Log(coor);
	}

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

		// 如果跑图时间超过5秒。则直接退出跑图
		if (time(NULL) - cur_time > 2) {
			{
				InstanceLock lock(mainWindow);
				mainWindow->Log(L"跑图超时，退出跑图");
			}
			return;
		}

		user_coor = readCoordinate(readLong(ADDR.x64("C_USER_ADDRESS")));

		if (isFirst) {
			{
				InstanceLock lock(mainWindow);
				mainWindow->Log(L"开始跑图");
			}

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
			{
				InstanceLock lock(mainWindow);
				mainWindow->Log(L"人物已离开图内，停止跑图");
			}
			MSDK_KeyUp(direction_x);
			MSDK_KeyUp(direction_y);
			break;
		}

		// 判断人物动作
		__int64 user_action = decrypt(readLong(ADDR.x64("C_USER_ADDRESS")) + ADDR.x64("C_MOVEMENT_ID"));
		if ((int)user_action != 14) {
			{
				InstanceLock lock(mainWindow);
				CString msg;
				msg.Format(L"当前动作ID：%ld，非跑动状态已停止跑图", user_action);
				mainWindow->Log(msg);
			}
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
					{
						InstanceLock lock(mainWindow);
						mainWindow->Log(L"向上超出Y范围，停止");
					}
					MSDK_KeyUp(direction_y);
					break;
				}
			}
			else {
				if (user_coor.y - y > y_range) {
					{
						InstanceLock lock(mainWindow);
						mainWindow->Log(L"向下超出Y范围，停止");
					}
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
					{
						InstanceLock lock(mainWindow);
						mainWindow->Log(L"向右超出X范围，停止");
					}
					MSDK_KeyUp(direction_x);
					break;
				}
			}
			else {
				if (x - user_coor.x > target_range) {
					{
						InstanceLock lock(mainWindow);
						mainWindow->Log(L"向左超出X范围，停止");
					}
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
			{
				InstanceLock lock(mainWindow);
				mainWindow->Log(L"到达目标位置，停止按键");
			}
			MSDK_KeyUp(direction_x);
			MSDK_KeyUp(direction_y);
			break;
		}

		handleEvents();
	}

	{
		InstanceLock lock(mainWindow);
		mainWindow->Log(L"跑图结束，停止按键");
	}

	MSDK_KeyUp(direction_x);
	MSDK_KeyUp(direction_y);

	return;
}

void autoNextRoom()
{
	if (GLOBAL.game_status != 3) {
		return;
	}

	COORDINATE room_coor = getCurrentRoom();
	COORDINATE boss_coor = getBossRoom();

	if (room_coor.x == boss_coor.x && room_coor.y == boss_coor.y) {
		return;
	}

	int next_direction = judgeNextRoomDiretion(room_coor, boss_coor);

	{
		CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;
		InstanceLock wind(mainWindow);

		CString msg;

		mainWindow->Log(L"获取下个房间入口");


		switch (next_direction)
		{
		case 0:
			msg = L"向左奔跑";
			break;
		case 1:
			msg = L"向右奔跑";
			break;
		case 2:
			msg = L"向上奔跑";
			break;
		case 3:
			msg = L"向下奔跑";
			break;
		default:
		{
			InstanceLock lock(mainWindow);
			mainWindow->Log(L"获取下个房间入口失败！");
		}
		return;
		}

		{
			InstanceLock lock(mainWindow);
			mainWindow->Log(msg);
		}

	}

	runToNextRoom(next_direction);
}

void runToNextRoom(int direction)
{
	__int64 target_room_x; // 目标的房间X
	__int64 target_room_y; // 目标的房间Y

	target_room_x = readLong(readLong(readLong(readLong(ADDR.x64("C_ROOM_NUMBER")) + ADDR.x64("C_TIME_ADDRESS")) + ADDR.x64("C_DOOR_TYPE_OFFSET")) + ADDR.x64("C_CURRENT_ROOM_X"));
	target_room_y = readLong(readLong(readLong(readLong(ADDR.x64("C_ROOM_NUMBER")) + ADDR.x64("C_TIME_ADDRESS")) + ADDR.x64("C_DOOR_TYPE_OFFSET")) + ADDR.x64("C_CURRENT_ROOM_Y"));

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
	case 0:
		target_room_x--;
		calc_x = begin_x + end_x + far_door;
		calc_y = begin_y + end_y / 2;
		break;
	case 1:
		target_room_x++;
		calc_x = begin_x - far_door;
		calc_y = begin_y + end_y / 2;
		break;
	case 2:
		target_room_y++;
		calc_x = begin_x + end_x / 2;
		calc_y = begin_y + end_y + far_door;
		break;
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
			COORDINATE room = getCurrentRoom();
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
		__int64 current_room_x, current_room_y;
		current_room_x = readLong(readLong(readLong(readLong(ADDR.x64("C_ROOM_NUMBER")) + ADDR.x64("C_TIME_ADDRESS")) + ADDR.x64("C_DOOR_TYPE_OFFSET")) + ADDR.x64("C_CURRENT_ROOM_X"));
		current_room_y = readLong(readLong(readLong(readLong(ADDR.x64("C_ROOM_NUMBER")) + ADDR.x64("C_TIME_ADDRESS")) + ADDR.x64("C_DOOR_TYPE_OFFSET")) + ADDR.x64("C_CURRENT_ROOM_Y"));
		if (target_room_x != current_room_x || target_room_y != current_room_y)
		{
			return;
		}

		programDelay(100, 0);

		// 远离目标地点（防止卡在入口处）
		runToDestination(calc_x, calc_y, true, 2);
	}
}

void firstRoomFunctions()
{
	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;

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
		hookDamage(true);
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

void closeDungeonFunctions()
{
	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;


	if (GLOBAL.function_switch.cool_down)
	{
		skillCoolDown(0);
	}

	if (GLOBAL.function_switch.hook_damage)
	{
		hookDamage(false);
	}

	if (GLOBAL.function_switch.three_speed)
	{
		threeSpeed(0, 0, 0);
	}
}