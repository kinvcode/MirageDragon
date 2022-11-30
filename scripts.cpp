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

// �ܵ�Ŀ��
void runToDestination(int x, int y, bool is_room = false, int target_range = 10)
{
	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;
	InstanceLock wind(mainWindow);

	if (x < 1 || y < 1) {
		{
			InstanceLock lock(mainWindow);
			mainWindow->Log(L"λ��̫����������ͼ");
		}
		return;
	}

	CString coor;
	coor.Format(L"Ŀ������ X:%d Y:%d", x, y);
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

	// ��¼��ǰʱ���
	time_t cur_time = time(NULL);

	while (true) {

		// �����ͼʱ�䳬��5�롣��ֱ���˳���ͼ
		if (time(NULL) - cur_time > 2) {
			{
				InstanceLock lock(mainWindow);
				mainWindow->Log(L"��ͼ��ʱ���˳���ͼ");
			}
			return;
		}

		user_coor = readCoordinate(readLong(ADDR.x64("C_USER_ADDRESS")));

		if (isFirst) {
			{
				InstanceLock lock(mainWindow);
				mainWindow->Log(L"��ʼ��ͼ");
			}

			MSDK_keyPress(direction_x, 1);
			programDelay(100, 0);
			MSDK_KeyDown(direction_x);
			programDelay(100, 0);
			MSDK_KeyDown(direction_y);

			// ���򷿼�������Y�ᣬ��ֹ���������������
			if (is_room) {
				int x_diff = abs(x - user_coor.x);
				int y_diff = abs(y - user_coor.y);

				// ����������ƶ��������̵������ҷ����
				if (x_diff < y_diff) {
					MSDK_KeyUp(direction_x);
				}
			}
		}

		if (GLOBAL.game_status != 3) {
			//����ͼ�ڣ�ֹͣ��ͼ;
			{
				InstanceLock lock(mainWindow);
				mainWindow->Log(L"�������뿪ͼ�ڣ�ֹͣ��ͼ");
			}
			MSDK_KeyUp(direction_x);
			MSDK_KeyUp(direction_y);
			break;
		}

		// �ж����ﶯ��
		__int64 user_action = decrypt(readLong(ADDR.x64("C_USER_ADDRESS")) + ADDR.x64("C_MOVEMENT_ID"));
		if ((int)user_action != 14) {
			{
				InstanceLock lock(mainWindow);
				CString msg;
				msg.Format(L"��ǰ����ID��%ld�����ܶ�״̬��ֹͣ��ͼ", user_action);
				mainWindow->Log(msg);
			}
			// �����ƶ�����
			MSDK_KeyUp(direction_x);
			MSDK_KeyUp(direction_y);
			break;
		}

		int y_range = 10;
		if (is_room) {
			y_range = 3;
		}

		// ���ȵ���Y����
		if (y_arrived == false) {
			if (direction_y == Keyboard_UpArrow) {
				if (y - user_coor.y > y_range) {
					{
						InstanceLock lock(mainWindow);
						mainWindow->Log(L"���ϳ���Y��Χ��ֹͣ");
					}
					MSDK_KeyUp(direction_y);
					break;
				}
			}
			else {
				if (user_coor.y - y > y_range) {
					{
						InstanceLock lock(mainWindow);
						mainWindow->Log(L"���³���Y��Χ��ֹͣ");
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
						mainWindow->Log(L"���ҳ���X��Χ��ֹͣ");
					}
					MSDK_KeyUp(direction_x);
					break;
				}
			}
			else {
				if (x - user_coor.x > target_range) {
					{
						InstanceLock lock(mainWindow);
						mainWindow->Log(L"���󳬳�X��Χ��ֹͣ");
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
				mainWindow->Log(L"����Ŀ��λ�ã�ֹͣ����");
			}
			MSDK_KeyUp(direction_x);
			MSDK_KeyUp(direction_y);
			break;
		}

		handleEvents();
	}

	{
		InstanceLock lock(mainWindow);
		mainWindow->Log(L"��ͼ������ֹͣ����");
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

		mainWindow->Log(L"��ȡ�¸��������");


		switch (next_direction)
		{
		case 0:
			msg = L"������";
			break;
		case 1:
			msg = L"���ұ���";
			break;
		case 2:
			msg = L"���ϱ���";
			break;
		case 3:
			msg = L"���±���";
			break;
		default:
		{
			InstanceLock lock(mainWindow);
			mainWindow->Log(L"��ȡ�¸��������ʧ�ܣ�");
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
	__int64 target_room_x; // Ŀ��ķ���X
	__int64 target_room_y; // Ŀ��ķ���Y

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
		far_door = 100; // Զ���ŵľ��룬��ֹ����
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
		// ��Ŀ��ص㣬�������������ͼ�����Ż�������λ��
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

		// Զ��Ŀ��ص㣨��ֹ������ڴ���
		runToDestination(calc_x, calc_y, true, 2);
	}
}

void firstRoomFunctions()
{
	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;

	if (GLOBAL.is_auto_play)
	{
		// ���Ͽո�
		MSDK_keyPress(Keyboard_UpArrow, 1);
		MSDK_keyPress(Keyboard_UpArrow, 1);
		MSDK_keyPress(Keyboard_KongGe, 1);
		programDelay(350, 0);

		// ���ҿո�
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