#include "pch.h"
#include "jobs.h"
#include "MirageDragonDlg.h"
#include "dnfData.h"
#include "Lock.h"
#include "scripts.h"
#include "keyboardDriver.h"
#include "dnfUser.h"

int game_status = 0;
bool window_top = false;
bool is_running = false;
__int64 C_USER = 0;
__int64 C_USER_POINTER = 0;
bool use_pass_room_call = false;

UINT updateDataThread(LPVOID pParam)
{
	CMirageDragonDlg* MainDlg = (CMirageDragonDlg*)pParam;

	__int64 emptyAddress;
	bool statusChange = false; // ����ָ���Ѹı�

	{
		InstanceLock lock(MainDlg);
		MainDlg->Log(L"���ݸ����߳�������");
	}

	while (true)
	{
		// ��ȡDNF����״̬
		window_top = windowIsTop();

		// ��Ϸ����״̬
		is_running = readInt(0x140000000) == 0x905A4D;
		if (!is_running) {
			// ��Ϸ����
			statusChange = false;
			{
				InstanceLock lock(MainDlg);
				MainDlg->Log(L"��ʧ��дȨ�ޣ�����");
			}
			Sleep(3000);
			continue;
		}

		// ��ȡ��Ϸ״̬
		game_status = readInt(C_GAME_STATUS);

		if (game_status >= 1 && statusChange == false) {
			emptyAddress = C_EMPTY_ADDRESS + 4000;
			C_USER_POINTER = getUserPointer(emptyAddress);
			C_USER = emptyAddress;
			if (C_USER_POINTER != 0)
			{
				statusChange = true;
				{
					InstanceLock lock(MainDlg);
					MainDlg->Log(L"����ָ���Ѹı�");
				}
			}
			// ���¶�ȡ����ָ��
		}

		// ��Ϸ��ͬ״̬�Ĵ���
		switch (game_status)
		{
		case 0:
			// ѡ���ɫ����
			statusChange = false;
			break;
		case 1:
			// �����С��ر�ͼ�ڹ���
			break;
		case 2:
			// ѡ�񸱱�����
			break;
		case 3:
			// ͼ����
			break;
		default:
			break;
		}
		Sleep(300);
		//programDelay(300, 0);
	}
	return 0;
}

UINT playGameThead(LPVOID pParam)
{
	bool is_boss = false; // ��ǰΪBOOS����
	bool first_room = false; // ��ǰΪ��һ������
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
		// ��Ϸ��ͬ״̬�Ĵ���
		switch (game_status)
		{
		case 0:
			// ѡ���ɫ����
			break;
		case 1:
			// �����С��ر�ͼ�ڹ���
			first_room = false;
			is_clearance = false;
			allow_next_map = false;
			use_pass_room_call = false;
			pass_room_numbers = 0;
			room_history.clear();
			break;
		case 3:
			// ������Ʒ��Ϣ
			getMonsterAndItems();

			// �ۼ�����
			convergeMonsterAndItems();

			// �жϵ�ǰ�Ƿ���boos����
			is_boss = judgeIsBossRoom();

			// �жϵ�ǰ�Ƿ��ǵ�һ�ν�ͼ
			if (first_room == false && is_clearance == false)
			{
				{
					InstanceLock lock(MainDlg);
					MainDlg->Log(L"������ͼ����");
				}
				first_room = true;
				firstRoomFunctions();
				first_room_functions = true;
			}

			// �ж��Ƿ���
			is_open_door = judgeDoorOpen();

			// δ����ʱ�����߼�
			if (is_open_door == false)
			{
				if (is_auto_play) {
					// �жϼ�����ȴ�б��ͷ��������
					int key = getCoolDownKey();
					MSDK_keyPress(key, 1);
				}

			}
			else {
				// ���ź���߼�����

				if (item_list.size() > 0) {
					//gather_item_times++;
					if (is_auto_play)
					{
						{
							InstanceLock lock(MainDlg);
							MainDlg->Log(L"������Ʒ���رմ�͸");
						}
						penetrate(false);
					}
				}
				else {
					if (is_auto_play)
					{
						{
							InstanceLock lock(MainDlg);
							MainDlg->Log(L"û����Ʒ��������͸");
						}
						penetrate(true);
					}
					// ��ͨ���������ͼ
					if (!is_boss) {
						if (is_auto_play) {
							{
								InstanceLock lock(MainDlg);
								MainDlg->Log(L"�����¸�����");
							}
							// �Զ���ͼ
							// ��ȡ��ǰ����λ��
							ROOMCOOR current_room;
							current_room.x = readLong(readLong(readLong(readLong(C_ROOM_NUMBER) + C_TIME_ADDRESS) + C_DOOR_TYPE_OFFSET) + C_CURRENT_ROOM_X);
							current_room.y = readLong(readLong(readLong(readLong(C_ROOM_NUMBER) + C_TIME_ADDRESS) + C_DOOR_TYPE_OFFSET) + C_CURRENT_ROOM_Y);
							room_history.push_back(current_room);
							if (roomRepeats(room_history, current_room) > 2)
							{
								use_pass_room_call = true;
							}
							else {
								use_pass_room_call = false;
							}
							autoNextRoom();
							pass_room_numbers++;
						}
					}
				}

				if (is_boss)
				{
					// �ж��Ƿ�ͨ��
					is_clearance = judgeClearance();
					if (is_clearance)
					{
						{
							InstanceLock lock(MainDlg);
							MainDlg->Log(L"�ر�ͼ�ڹ���");
						}

						if (first_room_functions == true)
						{
							//closeDungeonFunctions();
							first_room_functions = false;
						}

						if (is_auto_play) {

							// �ر��Զ�����
							use_pass_room_call = false;
							pass_room_numbers = 0;
							room_history.clear();

							if (item_list.size() < 1) {
								// ����
								MSDK_keyPress(1, 1);
								MSDK_DelayRandom(500, 1000);
								// �ֽ�װ��

								allow_next_map = true;
								// ESC
								//MSDK_keyPress(Keyboard_ESCAPE, 1);

								if (allow_next_map) {
									if (auto_play_type == 1) {
										// �ٴ���ս
										MSDK_keyPress(Keyboard_F10, 1);
									}
									else {
										// ���س���
										MSDK_keyPress(Keyboard_F12, 1);
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
		if (is_auto_play)
		{

		}
		Sleep(300);
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