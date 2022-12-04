#include "pch.h"
#include "jobs.h"
#include "MirageDragonDlg.h"
#include "dnfData.h"
#include "Lock.h"
#include "scripts.h"
#include "keyboardDriver.h"
#include "dnfUser.h"
#include "dnfTask.h"
#include "baseAddress.h"
#include "GameData.h"
#include "http.h"

UINT updateDataThread(LPVOID pParam)
{
	CMirageDragonDlg* MainDlg = (CMirageDragonDlg*)pParam;

	{
		InstanceLock lock(MainDlg);
		MainDlg->Log(L"���ݸ����߳�������");
	}

	while (true)
	{
		// ��Ϸ����״̬
		GLOBAL.is_running = readInt(0x140000000) == 0x905A4D;
		if (!GLOBAL.is_running) {
			GLOBAL.PID = getProcessPID(L"DNF.exe");
			{
				InstanceLock lock(MainDlg);
				MainDlg->Log(L"��ʧ��дȨ�ޣ�");
				if (GLOBAL.PID == 0)
				{
					MainDlg->Log(L"��Ϸ�����Ѷ�ʧ��");
					//reloadProcess();
				}
			}
			programDelay(3000, 0);
			continue;
		}

		// ��ȡDNF����״̬
		GLOBAL.window_top = windowIsTop();

		// ��ȡ��Ϸ״̬
		GLOBAL.game_status = readInt(ADDR.x64("C_GAME_STATUS"));

		programDelay(1000, 0);
	}
	return 0;
}

UINT playGameThead(LPVOID pParam)
{
	CMirageDragonDlg* MainDlg = (CMirageDragonDlg*)pParam;

	{
		InstanceLock lock(MainDlg);
		MainDlg->Log(L"ˢͼ�߳�������");
	}


	while (true) {
		// �ж�����״̬
		if (!GLOBAL.is_running) {
			// Ӧ��ֹͣ�߳�
			goto threadEnd;
		}

		// ��Ϸ��ͬ״̬�Ĵ���
		switch (GLOBAL.game_status)
		{
		case 0:
			// ѡ���ɫ����
			selectRoleLogic();
			break;
		case 1:
			townLogic();
			break;
		case 2:
			// ѡ�򸱱����洦��ע����ظ�������Ҳ�����״̬��
			selectDungeonLogic();
			break;
		case 3:
			// �����е��߼�����
			dungeonLogic();
			break;
		default:
			break;
		}


		// �Զ�����
		if (GLOBAL.is_auto_play)
		{

		}

	threadEnd:
		Sleep(50);
		//programDelay(300, 0);
	}

	{
		InstanceLock lock(MainDlg);
		MainDlg->Log(L"ˢͼ�߳��ѽ���");
	}

	return 0;
}

// ѡ���ɫ�߼�
void selectRoleLogic()
{

}

// �����߼�
void townLogic()
{
	//// �����С��ر�ͼ�ڹ���
	//first_room = true;
	//is_clearance = false;
	//allow_next_map = false;
	//first_room_functions = false;

	//if (GLOBAL.is_auto_play)
	//{
	//	pass_room_numbers = 0;
	//	room_history.clear();

	//	// ���鴦��
	//	if (GLOBAL.auto_play_type == 2)
	//	{
	//		// �Ի�����
	//		dialogue();
	//	}

	//	// �Զ���ͼ
	//	autoEntryDungeon();
	//}

}

// ѡ�񸱱��߼�
void selectDungeonLogic()
{
	// ���ѡͼʱ����������˳�����
}

// �������߼�
void dungeonLogic()
{
	// ˢͼ�߳̿������ر�ѡ���̡߳������̡߳�ѡͼ�߳�

	// ����ս����ͼ
	if (!GLOBAL.dungeon_info.entered) {
		// ��ʼ��������Ϣ
		initDungeonInfo();

		// ������������
		firstRoomFunctions();

		// ��ʼ������Ľ���״̬
		GLOBAL.dungeon_info.entered = true;
	}

	// ������������
	updateUserCoor();

	// ������ͼ�������Ʒ������...��
	getDungeonAllObj();

	// �жϵ�ǰ�Ƿ���boos����
	bool is_boss = judgeIsBossRoom();

	// �ж��Ƿ���
	bool is_open_door = judgeDoorOpen();

	// δ����ʱ�����߼�
	if (is_open_door == false)
	{
		// ����߼� 
		attackMonsterLogic();
	}
	// ���ź���߼�����
	else {
		// ���ﶵ��
		finalGatherItems();

		if (is_boss)
		{
			// �ж��Ƿ�ͨ��
			bool is_clearance = judgeClearance();
			if (judgeClearance())
			{
				// ͨ�ش���
				clearanceLogic();
			}
		}
		else {
			passRoomLogic();
		}
	}
}

void attackMonsterLogic() 
{
	//if (GLOBAL.is_auto_play) {
	//	if (GLOBAL.dungeon_info.current_room.monster_list.size() > 0)
	//	{
	//		// �жϼ�����ȴ�б��ͷ��������
	//		int key = getCoolDownKey();
	//		MSDK_keyPress(key, 1);
	//	}
	//}
}

void finalGatherItems() 
{
	// ѭ������

	// ���������Ʒ
	if (GLOBAL.dungeon_info.current_room.item_list.size() > 0) {
		// ���ż����߼� gatherAfterDoor
		//if (GLOBAL.is_auto_play)
		//{
		//	{
		//		InstanceLock lock(MainDlg);
		//		MainDlg->Log(L"������Ʒ���رմ�͸");
		//	}
		//	penetrate(false);
		//}
	}
}

void passRoomLogic() 
{
	//if (GLOBAL.is_auto_play)
	//{
	// // �����ǰû�п�����͸�������ô�͸
	//	{
	//		InstanceLock lock(MainDlg);
	//		MainDlg->Log(L"û����Ʒ��������͸");
	//	}
	//	penetrate(true);
	//}

	// �����¸������߼�
	//if (GLOBAL.is_auto_play) {
	//	{
	//		InstanceLock lock(MainDlg);
	//		MainDlg->Log(L"�����¸�����");
	//	}
	//	// �Զ���ͼ
	//	// ��ȡ��ǰ����λ��
	//	ROOMCOOR current_room;
	//	current_room.x = readLong(readLong(readLong(readLong(ADDR.x64("C_ROOM_NUMBER")) + ADDR.x64("C_TIME_ADDRESS")) + ADDR.x64("C_DOOR_TYPE_OFFSET")) + ADDR.x64("C_CURRENT_ROOM_X"));
	//	current_room.y = readLong(readLong(readLong(readLong(ADDR.x64("C_ROOM_NUMBER")) + ADDR.x64("C_TIME_ADDRESS")) + ADDR.x64("C_DOOR_TYPE_OFFSET")) + ADDR.x64("C_CURRENT_ROOM_Y"));
	//	room_history.push_back(current_room);
	//	autoNextRoom();
	//	pass_room_numbers++;
	//}
}

// ͨ���߼�
void clearanceLogic() 
{
	// �ر�ͼ�ڹ���
	closeDungeonFunctions();

	//if (GLOBAL.is_auto_play) {
	//	// �ر��Զ�����
	//	GLOBAL.use_pass_room_call = false;
	//	if (GLOBAL.dungeon_info.current_room.item_list.size() < 1) {
	//		// ����
	//		MSDK_keyPress(1, 1);
	//		programDelay(1000, 0);
	//		// �ֽ�װ��
	//		// ESC
	//		MSDK_keyPress(Keyboard_ESCAPE, 1);
	//		programDelay(600, 0);
	//		// ƣ��Ϊ�շ��س���
	//		if (allow_next_map) {
	//			if (getUserFatigue() < 1)
	//			{
	//				// ���س���
	//				MSDK_keyPress(Keyboard_F12, 1);
	//			}
	//			if (GLOBAL.auto_play_type == 1) {
	//				// �ٴ���ս
	//				MSDK_keyPress(Keyboard_F10, 1);
	//			}
	//		}
	//	}
	//}
}

int roomRepeats(vector<ROOMCOOR> list, ROOMCOOR room)
{
	__int64 length = list.size();
	if (length <= 1)
	{
		return 1;
	}

	int numbers = 0;

	for (__int64 i = 0; i < length; i++)
	{
		if (list[i].x == room.x && list[i].y == room.y)
		{
			numbers++;
		}
	}
	return numbers;
}