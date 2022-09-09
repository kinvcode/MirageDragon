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
				MainDlg->Log(L"��Ϸ�ѽ���");
			}

			// ֹͣ�߳�
			return 0;
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

		programDelay(300, 0);
	}
	return 0;
}

UINT playGameThead(LPVOID pParam)
{
	bool is_boss = false; // ��ǰΪBOOS����
	bool first_room = false; // ��ǰΪ��һ������
	bool is_open_door = false; // �Ƿ���Խ����¸�����
	bool is_clearance = false; // �ж��Ƿ���ͨ��

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
					if(is_auto_play)
					{
						{
							InstanceLock lock(MainDlg);
							MainDlg->Log(L"������Ʒ���رմ�͸");
						}
						penetrate(false);
					}
				}
				else {
					if(is_auto_play)
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
							autoNextRoom();
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
						//closeDungeonFunctions();
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

		programDelay(300, 0);
	}

	{
		InstanceLock lock(MainDlg);
		MainDlg->Log(L"ˢͼ�߳��ѽ���");
	}

	return 0;
}