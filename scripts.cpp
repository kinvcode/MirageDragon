#include "pch.h"
#include "scripts.h"
#include "MirageDragon.h"
#include "MirageDragonDlg.h"
#include "dnfData.h"
#include "dnfMap.h"
#include "keyboardDriver.h"
#include "dnfCALL.h"
#include "dnfUser.h"

// 跑到目标
BOOL runToDestination(int x, int y, bool is_room = false, int target_range = 10)
{
	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;
	InstanceLock wind(mainWindow);

	if (x < 1 || y < 1) {
		{
			InstanceLock lock(mainWindow);
			mainWindow->Log(L"位置太近，放弃跑图");
		}
		return false;
	}

	CString coor;
	coor.Format(L"目标坐标 X:%d Y:%d", x, y);
	{
		InstanceLock lock(mainWindow);
		mainWindow->Log(coor);
	}

	__int64 a, b;
	if (is_room) {
		a = readLong(readLong(readLong(readLong(C_ROOM_NUMBER) + C_TIME_ADDRESS) + C_DOOR_TYPE_OFFSET) + C_CURRENT_ROOM_X);
		b = readLong(readLong(readLong(readLong(C_ROOM_NUMBER) + C_TIME_ADDRESS) + C_DOOR_TYPE_OFFSET) + C_CURRENT_ROOM_Y);
	}

	COORDINATE user_coor = readCoordinate(readLong(C_USER));

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
		if (time(NULL) - cur_time > 5) {
			{
				InstanceLock lock(mainWindow);
				mainWindow->Log(L"跑图超时，退出跑图");
			}
			return false;
		}

		user_coor = readCoordinate(readLong(C_USER));

		if (isFirst) {
			{
				InstanceLock lock(mainWindow);
				mainWindow->Log(L"重新跑图");
			}

			MSDK_keyPress(direction_x, 1);
			MSDK_DelayRandom(50, 100);
			MSDK_KeyDown(direction_x);
			MSDK_DelayRandom(100, 150);
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

		if (game_status != 3) {
			//不在图内，停止跑图;
			{
				InstanceLock lock(mainWindow);
				mainWindow->Log(L"人物已离开图内，停止跑图");
			}
			MSDK_ReleaseAllKey();
			break;
		}

		// 判断人物动作
		//if (decrypt(readLong(C_USER) + C_MOVEMENT_ID) != 14) {
		//	MainDlg->Log(L"当前不是跑步动作，停止跑图");
		//	// 弹起移动按键
		//	keyboardUp(direction_x);
		//	keyboardUp(direction_y);
		//	break;
		//}

		// 判断房间：如果目标是下一个房间
		if (is_room) {
			if (readLong(readLong(readLong(readLong(C_ROOM_NUMBER) + C_TIME_ADDRESS) + C_DOOR_TYPE_OFFSET) + C_CURRENT_ROOM_X) != a ||
				readLong(readLong(readLong(readLong(C_ROOM_NUMBER) + C_TIME_ADDRESS) + C_DOOR_TYPE_OFFSET) + C_CURRENT_ROOM_Y) != b) {
				arrive_next = true;
				{
					InstanceLock lock(mainWindow);
					mainWindow->Log(L"房间发生变化，停止按键");
				}
				// 弹起移动按键
				MSDK_ReleaseAllKey();
				break;
			}
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
					MSDK_ReleaseAllKey();
					break;
				}
			}
			else {
				if (user_coor.y - y > y_range) {
					{
						InstanceLock lock(mainWindow);
						mainWindow->Log(L"向下超出Y范围，停止");
					}
					MSDK_ReleaseAllKey();
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
					MSDK_ReleaseAllKey();
					break;
				}
			}
			else {
				if (x - user_coor.x > target_range) {
					{
						InstanceLock lock(mainWindow);
						mainWindow->Log(L"向左超出X范围，停止");
					}
					MSDK_ReleaseAllKey();
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

			MSDK_ReleaseAllKey();
			return true;
			break;
		}

		//programDelay(100);
	}

	//handleEvents();
	{
		InstanceLock lock(mainWindow);
		mainWindow->Log(L"跑图结束，停止按键");
	}
	MSDK_ReleaseAllKey();

	return false;
}

void autoNextRoom()
{
	if (game_status != 3) {
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
	__int64 pass_room_data = passRoomData(direction);
	__int64 coor_struct = pass_room_data;

	int begin_x, begin_y, end_x, end_y, calc_x, calc_y;
	begin_x = readInt(coor_struct + 0);
	begin_y = readInt(coor_struct + 4);
	end_x = readInt(coor_struct + 8);
	end_y = readInt(coor_struct + 12);

	if (direction == 0)
	{
		calc_x = begin_x + end_x + 20; // 火魂向左顺图
		calc_y = begin_y + end_y / 2;
	}
	if (direction == 1)
	{
		calc_x = begin_x - 20; // 火魂向右顺图
		calc_y = begin_y + end_y / 2;
	}
	if (direction == 2)
	{
		calc_x = begin_x + end_x / 2; // 火魂向上顺图
		calc_y = begin_y + end_y + 20;

	}
	if (direction == 3)
	{
		calc_x = begin_x + end_x / 2; // 火魂向下顺图
		calc_y = begin_y - 20;
	}

	if (calc_x < 0 || calc_y < 0)
	{
		return;
	}

	COORDINATE cur_room;
	cur_room.x = (int)readLong(readLong(readLong(readLong(C_ROOM_NUMBER) + C_TIME_ADDRESS) + C_DOOR_TYPE_OFFSET) + C_CURRENT_ROOM_X);
	cur_room.y = (int)readLong(readLong(readLong(readLong(C_ROOM_NUMBER) + C_TIME_ADDRESS) + C_DOOR_TYPE_OFFSET) + C_CURRENT_ROOM_Y);

	// 跑目标地点，如果不是向上跑图，则优化传送门位置
	if (direction != 2) {
		runToDestination(begin_x + end_x / 2, begin_y + 40, true, 2);
	}
	else {
		runToDestination(begin_x + end_x / 2, begin_y, true, 2);
	}

	// 查看房间是否改变
	if (readLong(readLong(readLong(readLong(C_ROOM_NUMBER) + C_TIME_ADDRESS) + C_DOOR_TYPE_OFFSET) + C_CURRENT_ROOM_X) != cur_room.x ||
		readLong(readLong(readLong(readLong(C_ROOM_NUMBER) + C_TIME_ADDRESS) + C_DOOR_TYPE_OFFSET) + C_CURRENT_ROOM_Y) != cur_room.y)
	{
		{
			CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;
			InstanceLock wind(mainWindow);
			mainWindow->Log(L"房间跑图一次完成");
		}
	}
	else {
		Sleep(100);
		//programDelay(100);
		handleEvents();

		// 远离目标地点（防止卡在入口处）
		runToDestination(calc_x, calc_y, true, 2);

		handleEvents();
	}
}

void firstRoomFunctions()
{
	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;

	// 上上空格
	MSDK_keyPress(Keyboard_UpArrow, 1);
	MSDK_keyPress(Keyboard_UpArrow, 1);
	MSDK_keyPress(Keyboard_KongGe, 1);
	MSDK_DelayRandom(200, 350);

	// 右右空格
	MSDK_keyPress(Keyboard_RightArrow, 1);
	MSDK_keyPress(Keyboard_RightArrow, 1);
	MSDK_keyPress(Keyboard_KongGe, 1);
	MSDK_DelayRandom(200, 350);

	if (function_switch.score)
	{
		superScore();
	}

	if (function_switch.cool_down)
	{
		CString num;
		mainWindow->page2._cool_down.GetWindowText(num);
		float number = (float)_ttof(num);
		skillCoolDown(number);
	}

	if (function_switch.hook_damage)
	{
		hookDamage(true);
	}

	if (function_switch.three_speed)
	{
		CString attack_speed, move_speed, casting_speed;
		mainWindow->page2._attack_speed.GetWindowText(attack_speed);
		mainWindow->page2._move_speed.GetWindowText(move_speed);
		mainWindow->page2._casting_speed.GetWindowText(casting_speed);
		threeSpeed(_ttoi(attack_speed), _ttoi(casting_speed), _ttoi(move_speed));
	}

	if (function_switch.hidden_user)
	{
		hiddenUser();
	}
}