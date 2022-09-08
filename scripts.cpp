#include "pch.h"
#include "scripts.h"
#include "MirageDragon.h"
#include "MirageDragonDlg.h"
#include "dnfData.h"
#include "dnfMap.h"
#include "keyboardDriver.h"

// �ܵ�Ŀ��
BOOL runToDestination(int x, int y, bool is_room = false, int target_range = 10)
{
	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;
	InstanceLock wind(mainWindow);

	if (x < 1 || y < 1) {
		mainWindow->Log(L"λ��̫����������ͼ");
		return false;
	}

	CString coor;
	coor.Format(L"Ŀ������ X:%d Y:%d", x, y);
	mainWindow->Log(coor);

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

	// ��¼��ǰʱ���
	time_t cur_time = time(NULL);

	while (true) {

		// �����ͼʱ�䳬��5�롣��ֱ���˳���ͼ
		if (time(NULL) - cur_time > 5) {
			mainWindow->Log(L"��ͼ��ʱ���˳���ͼ");
			return false;
		}

		user_coor = readCoordinate(readLong(C_USER));

		if (isFirst) {
			mainWindow->Log(L"������ͼ");
			M_KeyPress(MSDK_HANDLE, direction_x, 1);
			M_DelayRandom(50, 100);
			M_KeyDown(MSDK_HANDLE, direction_x);
			M_DelayRandom(100, 150);
			M_KeyDown(MSDK_HANDLE, direction_y);

			// ���򷿼�������Y�ᣬ��ֹ���������������
			if (is_room) {
				int x_diff = abs(x - user_coor.x);
				int y_diff = abs(y - user_coor.y);

				// ����������ƶ��������̵������ҷ����
				if (x_diff < y_diff) {
					M_KeyUp(MSDK_HANDLE, direction_x);
				}
			}
		}

		if (game_status != 3) {
			//����ͼ�ڣ�ֹͣ��ͼ;
			mainWindow->Log(L"�������뿪ͼ�ڣ�ֹͣ��ͼ");
			M_ReleaseAllKey(MSDK_HANDLE);
			break;
		}

		// �ж����ﶯ��
		//if (decrypt(readLong(C_USER) + C_MOVEMENT_ID) != 14) {
		//	MainDlg->Log(L"��ǰ�����ܲ�������ֹͣ��ͼ");
		//	// �����ƶ�����
		//	keyboardUp(direction_x);
		//	keyboardUp(direction_y);
		//	break;
		//}

		// �жϷ��䣺���Ŀ������һ������
		if (is_room) {
			if (readLong(readLong(readLong(readLong(C_ROOM_NUMBER) + C_TIME_ADDRESS) + C_DOOR_TYPE_OFFSET) + C_CURRENT_ROOM_X) != a ||
				readLong(readLong(readLong(readLong(C_ROOM_NUMBER) + C_TIME_ADDRESS) + C_DOOR_TYPE_OFFSET) + C_CURRENT_ROOM_Y) != b) {
				arrive_next = true;
				mainWindow->Log(L"���䷢���仯��ֹͣ����");
				// �����ƶ�����
				M_ReleaseAllKey(MSDK_HANDLE);
				break;
			}
		}

		int y_range = 10;
		if (is_room) {
			y_range = 3;
		}

		// ���ȵ���Y����
		if (y_arrived == false) {
			if (direction_y == Keyboard_UpArrow) {
				if (y - user_coor.y > y_range) {
					mainWindow->Log(L"���ϳ���Y��Χ��ֹͣ");
					M_ReleaseAllKey(MSDK_HANDLE);
					break;
				}
			}
			else {
				if (user_coor.y - y > y_range) {
					mainWindow->Log(L"���³���Y��Χ��ֹͣ");
					M_ReleaseAllKey(MSDK_HANDLE);
					break;
				}
			}

			if (abs(user_coor.y - y) > y_range) {
				isFirst = false;
				continue;
			}
			else {
				keyboardUp(direction_y);
				y_arrived = true;
			}
		}

		if (x_arrived == false) {
			if (direction_x == Keyboard_RightArrow) {
				if (user_coor.x - x > target_range) {
					mainWindow->Log(L"���ҳ���X��Χ��ֹͣ");
					M_ReleaseAllKey(MSDK_HANDLE);
					break;
				}
			}
			else {
				if (x - user_coor.x > target_range) {
					mainWindow->Log(L"���󳬳�X��Χ��ֹͣ");
					M_ReleaseAllKey(MSDK_HANDLE);
					break;
				}
			}
			if (abs(user_coor.x - x) > target_range) {
				isFirst = false;
				continue;
			}
			else {
				keyboardUp(direction_x);
				x_arrived = true;
			}
		}

		if (x_arrived && y_arrived) {
			mainWindow->Log(L"����Ŀ��λ�ã�ֹͣ����");

			M_ReleaseAllKey(MSDK_HANDLE);
			return true;
			break;
		}

		//programDelay(100);
	}

	//handleEvents();

	mainWindow->Log(L"��ͼ������ֹͣ����");
	M_ReleaseAllKey(MSDK_HANDLE);

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
		switch (next_direction)
		{
		case 0:
			mainWindow->Log(L"������");
			break;
		case 1:
			mainWindow->Log(L"���ұ���");
			break;
		case 2:
			mainWindow->Log(L"���ϱ���");
			break;
		case 3:
			mainWindow->Log(L"���±���");
			break;
		default:
			mainWindow->Log(L"��ȡ�¸��������ʧ�ܣ�");
			return;
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
		calc_x = begin_x + end_x + 20; // �������˳ͼ
		calc_y = begin_y + end_y / 2;
	}
	if (direction == 1)
	{
		calc_x = begin_x - 20; // �������˳ͼ
		calc_y = begin_y + end_y / 2;
	}
	if (direction == 2)
	{
		calc_x = begin_x + end_x / 2; // �������˳ͼ
		calc_y = begin_y + end_y + 20;

	}
	if (direction == 3)
	{
		calc_x = begin_x + end_x / 2; // �������˳ͼ
		calc_y = begin_y - 20;
	}

	if (calc_x < 0 || calc_y < 0)
	{
		return;
	}

	COORDINATE cur_room;
	cur_room.x = (int)readLong(readLong(readLong(readLong(C_ROOM_NUMBER) + C_TIME_ADDRESS) + C_DOOR_TYPE_OFFSET) + C_CURRENT_ROOM_X);
	cur_room.y = (int)readLong(readLong(readLong(readLong(C_ROOM_NUMBER) + C_TIME_ADDRESS) + C_DOOR_TYPE_OFFSET) + C_CURRENT_ROOM_Y);

	// ��Ŀ��ص㣬�������������ͼ�����Ż�������λ��
	if (direction != 2) {
		runToDestination(begin_x + end_x / 2, begin_y + 40, true, 2);
	}
	else {
		runToDestination(begin_x + end_x / 2, begin_y, true, 2);
	}

	// �鿴�����Ƿ�ı�
	if (readLong(readLong(readLong(readLong(C_ROOM_NUMBER) + C_TIME_ADDRESS) + C_DOOR_TYPE_OFFSET) + C_CURRENT_ROOM_X) != cur_room.x ||
		readLong(readLong(readLong(readLong(C_ROOM_NUMBER) + C_TIME_ADDRESS) + C_DOOR_TYPE_OFFSET) + C_CURRENT_ROOM_Y) != cur_room.y)
	{
		{
			CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;
			InstanceLock wind(mainWindow);
			mainWindow->Log(L"������ͼһ�����");
		}
	}
	else {
		Sleep(100);
		//programDelay(100);
		handleEvents();

		// Զ��Ŀ��ص㣨��ֹ������ڴ���
		runToDestination(calc_x, calc_y, true, 2);

		handleEvents();
	}
}

void firstRoomFunctions()
{
	// ���Ͽո�
	//MSDK_keyPress(Keyboard_UpArrow, 1);
	//MSDK_keyPress(Keyboard_UpArrow, 1);
	MSDK_keyPress(Keyboard_RightArrow, 1);
	MSDK_keyPress(Keyboard_RightArrow, 1);
	MSDK_keyPress(Keyboard_KongGe, 1);

	//M_KeyPress(MSDK_HANDLE, Keyboard_UpArrow, 1);
	//M_KeyPress(MSDK_HANDLE, Keyboard_UpArrow, 1);
	//M_KeyPress(MSDK_HANDLE, Keyboard_KongGe, 1);
	//M_DelayRandom(200, 350);

	// ���ҿո�
	//M_KeyPress(MSDK_HANDLE, Keyboard_RightArrow, 1);
	//M_KeyPress(MSDK_HANDLE, Keyboard_RightArrow, 1);
	//M_KeyPress(MSDK_HANDLE, Keyboard_KongGe, 1);
	//M_DelayRandom(200, 350);

	//if (MainDlg->page1._switch_score.GetCheck() == BST_CHECKED)
	//{
	//	superScore();
	//}

	//if (MainDlg->page1._switch_cool_down.GetCheck() == BST_CHECKED)
	//{
	//	CString num;
	//	MainDlg->page3._cool_down.GetWindowText(num);
	//	float number = (float)_ttof(num);
	//	skillCoolDown(number);
	//}

	//if (MainDlg->page1._switch_hook_damage.GetCheck() == BST_CHECKED)
	//{
	//	hookDamage(true);
	//}

	//if (MainDlg->page1._switch_three_speed.GetCheck() == BST_CHECKED)
	//{
	//	CString attack_speed, move_speed, casting_speed;
	//	MainDlg->page3._attack_speed.GetWindowText(attack_speed);
	//	MainDlg->page3._move_speed.GetWindowText(move_speed);
	//	MainDlg->page3._casting_speed.GetWindowText(casting_speed);
	//	threeSpeed(_ttoi(attack_speed), _ttoi(casting_speed), _ttoi(move_speed));
	//}

	//if (MainDlg->page1._switch_hidden_user.GetCheck() == BST_CHECKED)
	//{
	//	hiddenUser();
	//}
}