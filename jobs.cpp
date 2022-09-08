#include "pch.h"
#include "jobs.h"
#include "MirageDragonDlg.h"
#include "dnfData.h"
#include "Lock.h"

int game_status = 0;
bool window_top = false;
bool is_running = false;
__int64 C_USER = 0;
__int64 C_USER_POINTER = 0;

UINT updateDataThread(LPVOID pParam)
{
	CMirageDragonDlg* MainDlg = (CMirageDragonDlg*)pParam;

	bool statusChange = false;
	__int64 emptyAddress;

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
			MainDlg->Log(L"��Ϸ�ѽ���");
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
				// ����ָ���Ѹı�
				MainDlg->Log(L"����ָ���Ѹı�");
			}
			// ���¶�ȡ����ָ��
		}

		// ��Ϸ��ͬ״̬�Ĵ���
		switch (game_status)
		{
			// ѡ���ɫ����
		case 0:
			// ���¶�ȡ����ָ��
			statusChange = false;
			break;
		case 3:
			// ������Ʒ��Ϣ
			getMonsterAndItems();

			// �жϵ�ǰ�Ƿ���boos����

			// �жϵ�ǰ�Ƿ��ǵ�һ�ν�ͼ

			// �ж��Ƿ���

			// �ж��Ƿ���ͨ��

			// 

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
	CMirageDragonDlg* MainDlg = (CMirageDragonDlg*)pParam;

	{
		InstanceLock lock(MainDlg);
		MainDlg->Log(L"ˢͼ�߳�������");
	}

	while (true) {

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