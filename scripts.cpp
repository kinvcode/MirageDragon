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
#include "procData.h"

// 跑到目标
bool runToDestination(int x, int y, bool is_room = false, int target_range = 10)
{
	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;

	if (x < 1 || y < 1) {
		Log.info(L"位置太近，放弃跑图");
		return true;
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
		if (is_room) {
			// 防止东西没有捡
			//getDungeonAllObj();
			//if (GAME.dungeon_info.current_room.item_list.size() > 0)
			//{
			//	Log.info(L"发现物品，停止跑图");
			//	MSDK_KeyUp(direction_x);
			//	MSDK_KeyUp(direction_y);
			//	return false;
			//	break;
			//}
		}


		// 如果跑图时间超过5秒。则直接退出跑图
		if (time(NULL) - cur_time > 2) {
			Log.info(L"跑图超时，退出跑图");
			MSDK_KeyUp(direction_x);
			MSDK_KeyUp(direction_y);
			return false;
		}

		user_coor = readCoordinate(readLong(ADDR.x64("C_USER_ADDRESS")));

		if (isFirst) {
			Log.info(L"开始跑图");

			MSDK_keyPress(direction_x, 1);
			Sleep(100);
			MSDK_KeyDown(direction_x);
			Sleep(100);
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

		if (GAME.game_status != 3) {
			//不在图内，停止跑图;
			Log.info(L"人物已离开图内，停止跑图");
			MSDK_KeyUp(direction_x);
			MSDK_KeyUp(direction_y);
			return false;
			break;
		}

		// 判断人物动作
		__int64 user_action = decrypt(readLong(ADDR.x64("C_USER_ADDRESS")) + ADDR.x64("C_MOVEMENT_ID"));
		if ((int)user_action != 14) {
			CString msg;
			msg.Format(L"当前动作ID：%ld，非跑动状态已停止跑图", (int)user_action);
			Log.info(msg);
			// 弹起移动按键
			MSDK_KeyUp(direction_x);
			MSDK_KeyUp(direction_y);
			return false;
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
					MSDK_KeyUp(direction_x);
					MSDK_KeyUp(direction_y);
					return false;
					break;
				}
			}
			else {
				if (user_coor.y - y > y_range) {
					Log.info(L"向下超出Y范围，停止");
					MSDK_KeyUp(direction_x);
					MSDK_KeyUp(direction_y);
					return false;
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
					MSDK_KeyUp(direction_y);
					return false;
					break;
				}
			}
			else {
				if (x - user_coor.x > target_range) {
					Log.info(L"向左超出X范围，停止");
					MSDK_KeyUp(direction_x);
					MSDK_KeyUp(direction_y);
					return false;
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
			return true;
			break;
		}
	}

	Log.info(L"跑图结束，停止按键");

	MSDK_KeyUp(direction_x);
	MSDK_KeyUp(direction_y);

	return true;
}

// 跑到门
bool runToDoor(int x, int y)
{
	if (x < 10 || y < 10) {
		Log.info(L"无效的坐标，放弃跑图");
		return true;
	}

	// 基础指针
	__int64 user_pointer = readLong(ADDR.x64("C_USER_ADDRESS"));					// 人物指针
	__int64 user_coor_p = readLong(user_pointer + ADDR.x64("C_READ_COORDINATE"));	// 人物坐标指针
	__int64 user_moveid = user_pointer + ADDR.x64("C_MOVEMENT_ID");					// 动作ID
	COORDINATE user_coor = readCoordinate(user_pointer);

	// 门坐标的偏差
	int range_x = 5;
	int range_y = 5;

	// 计算区域

	// 与目标相差X距离
	int diff_x = abs(user_coor.x - x);
	int diff_y = abs(user_coor.y - y);

	int run_type = 0;

	if (diff_x >= 200 || diff_y >= 200) {
		// 跑动情况
		// 只跑X
		if (diff_y <= 30) {
			run_type = 3;
		}
		else {
			if (diff_x <= 80) {
				run_type = 4;
			}
			else {
				run_type = 5;
			}
		}
	}
	else {
		// 走路情况：
		// 只走X
		if (diff_y <= 30) {
			run_type = 0;
		}
		else {
			// 只走Y
			if (diff_x <= 80) {
				run_type = 1;
			}
			else {
				// 同时走XY
				run_type = 2;
			}
		}
	}


	// 跑动按键判断
	int direction_x, direction_y;
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

	// 需要更新的数据
	int user_x = 0, user_y = 0; // 人物单独的坐标

	// XY是否到达
	bool x_arrived = false, y_arrived = false;

	// 记录当前时间戳
	time_t cur_time = time(NULL);

	// 清空一下按键
	MSDK_KeyUp(direction_x);
	MSDK_KeyUp(direction_y);
	Sleep(50);

	// 跑图起步类型：六种情况
	switch (run_type)
	{
	case 0:
		// 只走X轴
		//Log.info(L"只走X轴");

		MSDK_KeyDown(direction_x);
		Sleep(100);
		break;
	case 1:
		// 只走Y轴
		//Log.info(L"只走Y轴");

		MSDK_KeyDown(direction_y);
		Sleep(100);
		break;
	case 2:
		// 同时走XY轴
		//Log.info(L"同时走XY轴");

		MSDK_KeyDown(direction_x);
		Sleep(100);
		MSDK_KeyDown(direction_y);
		Sleep(100);
		break;
	case 3:
		// 只跑X轴
		//Log.info(L"只跑X轴");

		MSDK_keyPress(direction_x, 1);
		Sleep(100);
		MSDK_KeyDown(direction_x);
		Sleep(100);
		break;
	case 4:
		// 只跑Y轴
		//Log.info(L"只跑Y轴");

		MSDK_keyPress(direction_x, 1);
		Sleep(100);
		MSDK_keyPress(direction_x, 1);
		Sleep(100);
		MSDK_KeyDown(direction_y);
		Sleep(100);
		break;
	case 5:
	default:
		// 同时跑XY轴
		//Log.info(L"同时跑XY");

		MSDK_keyPress(direction_x, 1);
		Sleep(100);
		MSDK_KeyDown(direction_x);
		Sleep(100);
		MSDK_KeyDown(direction_y);
		Sleep(100);
		break;
	}

	// 如果只处理X，则Y轴直接判定到达
	if (run_type == 0 || run_type == 3) {
		//Log.info(L"无需跑Y轴");
		y_arrived = true;
	}

	// 只跑Y轴时，直接判定X轴到达
	if (run_type == 1 || run_type == 4) {
		//Log.info(L"无需跑X轴");
		x_arrived = true;
	}

	// 记录当前房间坐标
	int current_room_x = readInt(GAME.dungeon_info.door_pointer + ADDR.x64("C_CURRENT_ROOM_X"));
	int current_room_y = readInt(GAME.dungeon_info.door_pointer + ADDR.x64("C_CURRENT_ROOM_Y"));

	ROOMCOOR room_coor = { current_room_x,current_room_y };

	while (true) {

		// 防止东西没有捡
		getDungeonAllObj();
		if (GAME.dungeon_info.current_room.item_list.size() > 0)
		{
			Log.info(L"发现物品，停止跑图");
			MSDK_KeyUp2(direction_x);
			Sleep(50);
			MSDK_KeyUp2(direction_y);
			return false;
		}

		// 如果刷新出了怪物，则退出跑图
		if (GAME.dungeon_info.current_room.monster_list.size() > 0) 
		{
			Log.info(L"发现怪物，停止跑图");
			MSDK_KeyUp2(direction_x);
			Sleep(50);
			MSDK_KeyUp2(direction_y);
			return false;
		}

		if (GAME.dungeon_info.current_room.room_has_urgent) 
		{
			Log.info(L"存在时空旋涡，停止跑图",true);
			MSDK_KeyUp2(direction_x);
			Sleep(50);
			MSDK_KeyUp2(direction_y);
			return false;
		}

		// 如果跑图超时处理，防止意外情况
		if (time(NULL) - cur_time > 2) {
			Log.info(L"跑图超时，退出过图跑图");
			MSDK_KeyUp2(direction_x);
			Sleep(50);
			MSDK_KeyUp2(direction_y);
			return false;
		}

		// 如果房间发生改变
		int room_x = readInt(GAME.dungeon_info.door_pointer + ADDR.x64("C_CURRENT_ROOM_X"));
		int room_y = readInt(GAME.dungeon_info.door_pointer + ADDR.x64("C_CURRENT_ROOM_Y"));
		if (room_x != room_coor.x || room_y != room_coor.y)
		{
			Log.info(L"房间发生改变，退出过图跑图", true);
			MSDK_KeyUp2(direction_x);
			Sleep(50);
			MSDK_KeyUp2(direction_y);
			return false;
		}

		// 离开图内时退出跑图
		if (GAME.game_status != 3) {
			MSDK_KeyUp2(direction_x);
			Sleep(50);
			MSDK_KeyUp2(direction_y);
			return false;
		}

		if (run_type == 3 || run_type == 4 || run_type == 5)
		{
			// 非跑动状态退出跑图
			int user_action = (int)decrypt(user_moveid);
			if (user_action != 14) {
				Log.info(L"当前非跑动动作，停止找门跑图", true);
				//	CString msg;
				//	msg.Format(L"当前动作ID：%ld，非跑动状态已停止找怪跑图", (int)user_action);
				//	Log.info(msg);
				MSDK_KeyUp2(direction_x);
				Sleep(50);
				MSDK_KeyUp2(direction_y);
				return false;
			}
		}


		// Y轴到达判断

		if (y_arrived == false) {
			user_y = readUserCoorY(user_coor_p);

			if (direction_y == Keyboard_UpArrow) {
				if (abs(user_y - y) <= range_y) {
					MSDK_KeyUp2(direction_y);
					y_arrived = true;
				}
			}
			else {
				if (abs(y - user_y) <= range_y) {
					MSDK_KeyUp2(direction_y);
					y_arrived = true;
				}
			}
		}

		// X轴到达判断
		if (x_arrived == false) {
			user_x = readUserCoorX(user_coor_p);
			if (direction_x == Keyboard_RightArrow) {
				if (abs(x - user_x) <= range_x) {
					MSDK_KeyUp2(direction_x);
					x_arrived = true;
				}
			}
			else {
				if (abs(user_x - x) <= range_x) {
					MSDK_KeyUp2(direction_x);
					x_arrived = true;
				}
			}
		}

		if (!x_arrived || !y_arrived) {
			continue;
		}

		// 到达目的，退出循环
		break;
	}

	//Log.info(L"到达门位置，停止按键");
	MSDK_KeyUp(direction_x);
	Sleep(50);
	MSDK_KeyUp(direction_y);
	return true;
}

// 跑到怪物
bool runToMonter(int x, int y)
{
	if (x < 10 || y < 10) {
		//Log.info(L"无效的坐标，放弃跑图");
		return true;
	}

	// 基础指针
	__int64 user_pointer = readLong(ADDR.x64("C_USER_ADDRESS"));					// 人物指针
	__int64 user_coor_p = readLong(user_pointer + ADDR.x64("C_READ_COORDINATE"));	// 人物坐标指针
	__int64 user_moveid = user_pointer + ADDR.x64("C_MOVEMENT_ID");					// 动作ID
	COORDINATE user_coor = readCoordinate(user_pointer);

	// 普攻距离默认X80 Y30
	int range_x = 80;
	int range_y = 10;

	// 判断怪物方向0左 1右
	bool monster_dir = false;
	if (user_coor.x < x) {
		monster_dir = true;
	}

	//CString coor2;
	//coor2.Format(L"人物坐标 X:%d Y:%d", user_coor.x, user_coor.y);
	//Log.info(coor2);

	//CString coor;
	//coor.Format(L"怪物坐标 X:%d Y:%d", x, y);
	//Log.info(coor);

	// 计算区域

	// 与目标相差X距离
	int diff_x = abs(user_coor.x - x);
	int diff_y = abs(user_coor.y - y);

	// 大多数人物的最低攻击距离都在100以上，因此低于100以内的距离不需要跑图
	if (diff_x < range_x && diff_y < range_y) {
		//Log.info(L"怪物太近，无需跑图");
	   // 最后调整人物方向，防止攻击方向错误
		if (monster_dir) {
			//Log.info(L"调整人物朝向：右", true);
			MSDK_keyPress(Keyboard_RightArrow, 1);
		}
		else {
			//Log.info(L"调整人物朝向：左", true);
			MSDK_keyPress(Keyboard_LeftArrow, 1);
		}
		return true;
	}


	// 跑动条件：X轴大于130，Y轴大于70
	// 怪物距离情况：X轴超过400，XY同时跑		

	// 跑动类型（默认情况不跑动）：
	// 只走X
	// 只走Y
	// 同时走XY
	// 只跑X
	// 只跑Y
	// 同时跑XY
	// 跑动类型判定条件：
	// 0优先跑X轴，X轴相差超过200，Y轴大于20
	// 1只跑动Y轴，X轴相差不超过80，Y轴超出20
	// 3优先跑Y轴，X轴相差不超过200，不判断

	int run_type = 0;

	if (diff_x >= 200 || diff_y >= 200) {
		// 跑动情况
		// 只跑X
		if (diff_y <= 30) {
			run_type = 3;
		}
		else {
			if (diff_x <= 80) {
				run_type = 4;
			}
			else {
				run_type = 5;
			}
		}
	}
	else {
		// 走路情况：
		// 只走X
		if (diff_y <= 30) {
			run_type = 0;
		}
		else {
			// 只走Y
			if (diff_x <= 80) {
				run_type = 1;
			}
			else {
				// 同时走XY
				run_type = 2;
			}
		}
	}

	// 跑动按键判断
	int direction_x, direction_y;
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

	// 需要更新的数据
	int user_x = 0, user_y = 0; // 人物单独的坐标

	// XY是否到达
	bool x_arrived = false, y_arrived = false;

	// 记录当前时间戳
	time_t cur_time = time(NULL);

	// 清空一下按键
	MSDK_KeyUp(direction_x);
	MSDK_KeyUp(direction_y);
	Sleep(50);

	// 跑图起步类型：六种情况
	switch (run_type)
	{
	case 0:
		// 只走X轴
		//Log.info(L"只走X轴");

		MSDK_KeyDown(direction_x);
		Sleep(100);
		break;
	case 1:
		// 只走Y轴
		//Log.info(L"只走Y轴");

		MSDK_KeyDown(direction_y);
		Sleep(100);
		break;
	case 2:
		// 同时走XY轴
		//Log.info(L"同时走XY轴");

		MSDK_KeyDown(direction_x);
		Sleep(100);
		MSDK_KeyDown(direction_y);
		Sleep(100);
		break;
	case 3:
		// 只跑X轴
		//Log.info(L"只跑X轴");

		MSDK_keyPress(direction_x, 1);
		Sleep(100);
		MSDK_KeyDown(direction_x);
		Sleep(100);
		break;
	case 4:
		// 只跑Y轴
		//Log.info(L"只跑Y轴");

		MSDK_keyPress(direction_x, 1);
		Sleep(100);
		MSDK_keyPress(direction_x, 1);
		Sleep(100);
		MSDK_KeyDown(direction_y);
		Sleep(100);
		break;
	case 5:
	default:
		// 同时跑XY轴
		//Log.info(L"同时跑XY");

		MSDK_keyPress(direction_x, 1);
		Sleep(100);
		MSDK_KeyDown(direction_x);
		Sleep(100);
		MSDK_KeyDown(direction_y);
		Sleep(100);
		break;
	}

	// 如果只处理X，则Y轴直接判定到达
	if (run_type == 0 || run_type == 3) {
		//Log.info(L"无需跑Y轴");
		y_arrived = true;
	}

	// 只跑Y轴时，直接判定X轴到达
	if (run_type == 1 || run_type == 4) {
		//Log.info(L"无需跑X轴");
		x_arrived = true;
	}

	while (true) {

		// 如果跑图超时处理，防止意外情况
		if (time(NULL) - cur_time > 2) {
			//Log.info(L"跑图超时，退出找怪跑图", true);
			MSDK_KeyUp2(direction_x);
			Sleep(50);
			MSDK_KeyUp2(direction_y);
			return false;
		}

		// 离开图内时退出跑图
		if (GAME.game_status != 3) {
			//Log.info(L"已经离开地图，退出跑图", true);
			MSDK_KeyUp2(direction_x);
			Sleep(50);
			MSDK_KeyUp2(direction_y);
			return false;
		}

		if (run_type == 3 || run_type == 4 || run_type == 5)
		{
			// 非跑动状态退出跑图
			int user_action = (int)decrypt(user_moveid);
			if (user_action != 14) {
				//Log.info(L"当前非跑动动作，停止找怪跑图", true);
				//	CString msg;
				//	msg.Format(L"当前动作ID：%ld，非跑动状态已停止找怪跑图", (int)user_action);
				//	Log.info(msg);
				MSDK_KeyUp2(direction_x);
				Sleep(50);
				MSDK_KeyUp2(direction_y);
				return false;
			}
		}

		// Y轴到达判断

		if (y_arrived == false) {
			user_y = readUserCoorY(user_coor_p);

			if (direction_y == Keyboard_UpArrow) {
				if (abs(user_y - y) <= range_y) {
					//Log.info(L"向上已到达Y轴");
					MSDK_KeyUp2(direction_y);
					y_arrived = true;
				}
			}
			else {
				if (abs(y - user_y) <= range_y) {
					//Log.info(L"向下已到达Y轴");
					MSDK_KeyUp2(direction_y);
					y_arrived = true;
				}
			}
		}


		// X轴到达判断
		if (x_arrived == false) {
			user_x = readUserCoorX(user_coor_p);
			if (direction_x == Keyboard_RightArrow) {
				if (abs(x - user_x) <= range_x) {
					MSDK_KeyUp2(direction_x);
					x_arrived = true;
					//Log.info(L"向右已到达X轴");
				}
			}
			else {
				if (abs(user_x - x) <= range_x) {
					MSDK_KeyUp2(direction_x);
					x_arrived = true;
					//Log.info(L"向左已到达X轴");
				}
			}
		}

		if (!x_arrived || !y_arrived) {
			continue;
		}

		// 到达目的，退出循环
		break;
	}

	//Log.info(L"到达怪物位置，停止按键");
	MSDK_KeyUp(direction_x);
	Sleep(50);
	MSDK_KeyUp(direction_y);
	return true;
}

// 跑到物品
bool runToItem(int x, int y)
{
	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;

	if (x < 1 || y < 1) {
		Log.info(L"物品位置太近，放弃跑图");
		return true;
	}

	CString coor;
	coor.Format(L"物品坐标 X:%d Y:%d", x, y);
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

		// 如果跑图时间超过5秒。则直接退出跑图
		if (time(NULL) - cur_time > 2) {
			Log.info(L"找物品超时，退出跑图");
			MSDK_KeyUp(direction_x);
			MSDK_KeyUp(direction_y);
			return false;
		}

		user_coor = readCoordinate(readLong(ADDR.x64("C_USER_ADDRESS")));

		if (isFirst) {
			Log.info(L"开始找物品");

			MSDK_keyPress(direction_x, 1);
			Sleep(100);
			MSDK_KeyDown(direction_x);
			Sleep(100);
			MSDK_KeyDown(direction_y);

		}

		if (GAME.game_status != 3) {
			//不在图内，停止跑图;
			Log.info(L"人物已离开图内，停止找物品");
			MSDK_KeyUp(direction_x);
			MSDK_KeyUp(direction_y);
			return false;
			break;
		}

		// 判断人物动作
		__int64 user_action = decrypt(readLong(ADDR.x64("C_USER_ADDRESS")) + ADDR.x64("C_MOVEMENT_ID"));
		if ((int)user_action != 14) {
			CString msg;
			msg.Format(L"当前动作ID：%ld，非跑动状态已停止找物品", (int)user_action);
			Log.info(msg);
			// 弹起移动按键
			MSDK_KeyUp(direction_x);
			MSDK_KeyUp(direction_y);
			return false;
			break;
		}

		int y_range = 10;

		// 优先到达Y坐标
		if (y_arrived == false) {
			if (direction_y == Keyboard_UpArrow) {
				if (y - user_coor.y > y_range) {
					Log.info(L"向上超出Y范围，停止");
					MSDK_KeyUp(direction_x);
					MSDK_KeyUp(direction_y);
					return false;
					break;
				}
			}
			else {
				if (user_coor.y - y > y_range) {
					Log.info(L"向下超出Y范围，停止");
					MSDK_KeyUp(direction_x);
					MSDK_KeyUp(direction_y);
					return false;
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

		int target_range = 5;

		if (x_arrived == false) {
			if (direction_x == Keyboard_RightArrow) {
				if (user_coor.x - x > target_range) {
					Log.info(L"向右超出X范围，停止");
					MSDK_KeyUp(direction_x);
					MSDK_KeyUp(direction_y);
					return false;
					break;
				}
			}
			else {
				if (x - user_coor.x > target_range) {
					Log.info(L"向左超出X范围，停止");
					MSDK_KeyUp(direction_x);
					MSDK_KeyUp(direction_y);
					return false;
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
			Log.info(L"到达物品位置，停止按键");
			MSDK_KeyUp(direction_x);
			MSDK_KeyUp(direction_y);
			return true;
			break;
		}
	}

	Log.info(L"找物品结束，停止按键");

	MSDK_KeyUp(direction_x);
	MSDK_KeyUp(direction_y);

	return true;
}


void autoNextRoom()
{
	if (GAME.game_status != 3) {
		return;
	}

	ROOMCOOR room_coor = GAME.dungeon_info.current_room.coordinate;
	ROOMCOOR boss_coor = GAME.dungeon_info.boos_room;

	if (room_coor.x == boss_coor.x && room_coor.y == boss_coor.y) {
		return;
	}

	int next_direction = judgeNextRoomDiretion();

	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;
	CString msg;
	msg.Format(L"跑图方向：%d", next_direction);
	Log.info(msg);

	if (GAME.dungeon_info.rooms[room_coor.x][room_coor.y].entry_num > 2) {
		//GAME.use_pass_room_call = true;
	}
	GAME.dungeon_info.rooms[room_coor.x][room_coor.y].entry_num += 1;
	// 左右上下0123
	runToNextRoom(next_direction);
}

void runToNextRoom(int direction)
{
	// 记录过图方向
	GAME.dungeon_info.last_direction = direction;

	int target_room_x = GAME.dungeon_info.current_room.coordinate.x; // 当前房间X坐标
	int target_room_y = GAME.dungeon_info.current_room.coordinate.y; // 当前房间Y坐标

	__int64 pass_room_data = passRoomData(direction);
	__int64 coor_struct = pass_room_data;

	int begin_x, begin_y, end_x, end_y, calc_x, calc_y;
	begin_x = readInt(coor_struct + 0);
	begin_y = readInt(coor_struct + 4);
	end_x = readInt(coor_struct + 8);
	end_y = readInt(coor_struct + 12);

	int far_door;
	if (GAME.use_pass_room_call) {
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

	if (GAME.use_pass_room_call) {
		if (GAME.dungeon_info.current_room.room_has_urgent)
		{
			ROOMCOOR room = GAME.dungeon_info.current_room.coordinate;
			//passRoomByPacket(room.x, room.y);
		}
		else {
			coorCall(calc_x, calc_y, 0);
			Sleep(100);
			coorCall(begin_x + end_x / 2, begin_y, 0);
			GAME.use_pass_room_call = false;
		}
	}
	else {
		// 跑目标地点，如果不是向上跑图，则优化传送门位置
		int new_begin_y = begin_y;
		if (direction != 2) {
			new_begin_y = begin_y + 40;
		}


		//Log.info(L"准备过图", true);
		runToDoor(begin_x + end_x / 2, new_begin_y);
		int current_room_x = readInt(GAME.dungeon_info.door_pointer + ADDR.x64("C_CURRENT_ROOM_X"));
		int current_room_y = readInt(GAME.dungeon_info.door_pointer + ADDR.x64("C_CURRENT_ROOM_Y"));
		if (target_room_x != current_room_x || target_room_y != current_room_y)
		{
			MSDK_closeRunKey();
			return;
		}

		//Sleep(100, 0);
		//Log.info(L"远离门", true);
		// 远离目标地点（防止卡在入口处）
		runToDoor(calc_x, calc_y);
	}
}