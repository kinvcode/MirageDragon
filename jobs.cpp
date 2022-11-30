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

UINT updateDataThread(LPVOID pParam)
{
	CMirageDragonDlg* MainDlg = (CMirageDragonDlg*)pParam;

	bool statusChange = false; // ����ָ���Ѹı�

	{
		InstanceLock lock(MainDlg);
		MainDlg->Log(L"���ݸ����߳�������");
	}

	while (true)
	{
		// ��ȡDNF����״̬
		GLOBAL.window_top = windowIsTop();

		// ��Ϸ����״̬
		GLOBAL.is_running = readInt(0x140000000) == 0x905A4D;
		if (!GLOBAL.is_running) {
			statusChange = false;
			GLOBAL.PID = getProcessPID(L"DNF.exe");
			{
				InstanceLock lock(MainDlg);
				MainDlg->Log(L"��ʧ��дȨ�ޣ�����");
				if (GLOBAL.PID == 0)
				{
					MainDlg->Log(L"��������Ӧ��");
					//reloadProcess();
				}
			}
			programDelay(3000, 0);
			continue;
		}

		// ��ȡ��Ϸ״̬
		GLOBAL.game_status = readInt(ADDR.x64("C_GAME_STATUS"));

		// ��Ϸ��ͬ״̬�Ĵ���
		//switch (game_status)
		//{
		//case 0:
		//	// ѡ���ɫ����
		//	statusChange = false;
		//	break;
		//case 1:
		//	// �����С��ر�ͼ�ڹ���
		//	break;
		//case 2:
		//	// ѡ�񸱱�����
		//	break;
		//case 3:
		//	// ͼ����
		//	break;
		//default:
		//	break;
		//}

		programDelay(1000, 0);
	}
	return 0;
}

UINT playGameThead(LPVOID pParam)
{
	bool is_boss = false; // ��ǰΪBOOS����
	bool first_room = true; // ��ǰΪ��һ������
	bool first_room_functions = false; // ��ͼ�����Ƿ���
	bool is_open_door = false; // �Ƿ���Խ����¸�����
	bool is_clearance = false; // �ж��Ƿ���ͨ��
	bool allow_next_map = false; // ��������¸���ͼ
	int pass_room_numbers = 0; // �����¸��������
	vector<ROOMCOOR> room_history; // ·���ķ�����ʷ����

	CMirageDragonDlg* MainDlg = (CMirageDragonDlg*)pParam;

	{
		InstanceLock lock(MainDlg);
		MainDlg->Log(L"ˢͼ�߳�������");
	}


	while (true) {
		// �ж�����״̬
		if (!GLOBAL.is_running) {
			goto threadEnd;
		}

		// ��Ϸ��ͬ״̬�Ĵ���
		switch (GLOBAL.game_status)
		{
		case 0:
			// ѡ���ɫ����
			break;
		case 1:
			// �����С��ر�ͼ�ڹ���
			first_room = true;
			is_clearance = false;
			allow_next_map = false;
			first_room_functions = false;

			if (GLOBAL.is_auto_play)
			{
				pass_room_numbers = 0;
				room_history.clear();

				// ���鴦��
				if (GLOBAL.auto_play_type == 2)
				{
					// �Ի�����
					dialogue();
				}

				// �Զ���ͼ
				autoEntryDungeon();
			}

			break;
		case 2:
			// ѡͼ���洦��

			// ���ѡͼʱ����������˳�����

			break;
		case 3:
			// ˢͼ�߳̿������ر�ѡ���̡߳������̡߳�ѡͼ�߳�

		roomBegin:
			// ������Ʒ�͹�����Ϣ
			getMonsterAndItems();

			// �ۼ�����
			convergeMonsterAndItems();

			// �жϵ�ǰ�Ƿ���boos����
			is_boss = judgeIsBossRoom();

			// �ж��Ƿ�ͨ��
			is_clearance = judgeClearance();

			// �жϵ�ǰ�Ƿ��ǵ�һ�ν�ͼ
			if (first_room == true && is_clearance == false)
			{
				{
					InstanceLock lock(MainDlg);
					MainDlg->Log(L"������ͼ����");
				}
				first_room = false;
				firstRoomFunctions();
				first_room_functions = true;

				if (GLOBAL.is_auto_play)
				{
					MSDK_keyPress(Keyboard_m, 1);
					programDelay(400, 0);
					MSDK_keyPress(Keyboard_m, 1);
				}

			}

			// �ж��Ƿ���
			is_open_door = judgeDoorOpen();

			// δ����ʱ�����߼�
			if (is_open_door == false)
			{
				if (GLOBAL.is_auto_play) {

					if (GLOBAL.dungeon_info.current_room.monster_list.size() > 0)
					{
						// �жϼ�����ȴ�б��ͷ��������
						int key = getCoolDownKey();
						MSDK_keyPress(key, 1);
					}
				}

			}
			else {
				// ���ź���߼�����
				programDelay(1000, 0);
				getMonsterAndItems();
				convergeMonsterAndItems();
				if (GLOBAL.dungeon_info.current_room.item_list.size() > 0 || GLOBAL.dungeon_info.current_room.monster_list.size() > 0) {
					if (GLOBAL.is_auto_play)
					{
						{
							InstanceLock lock(MainDlg);
							MainDlg->Log(L"������Ʒ���رմ�͸");
						}
						penetrate(false);
					}
					if (!is_boss) {
						// �����ӳ٣���ֹû�м���Ʒ
						goto roomBegin;
					}
				}
				else {
					if (GLOBAL.is_auto_play)
					{
						{
							InstanceLock lock(MainDlg);
							MainDlg->Log(L"û����Ʒ��������͸");
						}
						penetrate(true);
					}
					// ��ͨ���������ͼ
					if (!is_boss) {
						if (GLOBAL.is_auto_play) {
							{
								InstanceLock lock(MainDlg);
								MainDlg->Log(L"�����¸�����");
							}
							// �Զ���ͼ
							// ��ȡ��ǰ����λ��
							ROOMCOOR current_room;
							current_room.x = readLong(readLong(readLong(readLong(ADDR.x64("C_ROOM_NUMBER")) + ADDR.x64("C_TIME_ADDRESS")) + ADDR.x64("C_DOOR_TYPE_OFFSET")) + ADDR.x64("C_CURRENT_ROOM_X"));
							current_room.y = readLong(readLong(readLong(readLong(ADDR.x64("C_ROOM_NUMBER")) + ADDR.x64("C_TIME_ADDRESS")) + ADDR.x64("C_DOOR_TYPE_OFFSET")) + ADDR.x64("C_CURRENT_ROOM_Y"));
							room_history.push_back(current_room);
							autoNextRoom();
							pass_room_numbers++;
						}
					}
				}

				if (is_boss)
				{
					if (is_clearance)
					{
						first_room = true;
						if (first_room_functions == true)
						{
							{
								InstanceLock lock(MainDlg);
								MainDlg->Log(L"�ر�ͼ�ڹ���");
							}
							closeDungeonFunctions();
							first_room_functions = false;
						}

						if (GLOBAL.is_auto_play) {

							// �ر��Զ�����
							GLOBAL.use_pass_room_call = false;
							pass_room_numbers = 0;
							room_history.clear();

							if (GLOBAL.dungeon_info.current_room.item_list.size() < 1) {
								// ����
								MSDK_keyPress(1, 1);
								programDelay(1000, 0);
								// �ֽ�װ��

								allow_next_map = true;

								// ESC
								MSDK_keyPress(Keyboard_ESCAPE, 1);
								programDelay(600, 0);

								// ƣ��Ϊ�շ��س���

								if (allow_next_map) {
									if (getUserFatigue() < 1)
									{
										// ���س���
										MSDK_keyPress(Keyboard_F12, 1);
									}

									if (GLOBAL.auto_play_type == 1) {
										// �ٴ���ս
										MSDK_keyPress(Keyboard_F10, 1);
									}
								}
							}
						}
					}
				}

			}

			// �ж��Ƿ���ͨ��

			// 

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