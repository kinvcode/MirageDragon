#include "pch.h"
#include "jobs.h"
#include "MirageDragonDlg.h"

UINT userPointerThread(LPVOID pParam)
{
	CMirageDragonDlg* MainDlg = (CMirageDragonDlg*)pParam;

	bool statusChange = false;
	__int64 emptyAddress;

	while (true)
	{
		// ����л�����̨
		//if (!dnf->windowIsTop()) {
		//	//�л�����̨��ֹͣ��ͼ;
		//	M_ReleaseAllKey(MainDlg->msdk_handle);
		//}

		if (readInt(0x140000000) != 0x905A4D) {
			// ��Ϸ����
			statusChange = false;
			MainDlg->Log(L"��Ϸ�ѽ���");
			break;

		}
		else {
			MainDlg->Log(L"��������ָ��");
		}

		//if (judgeGameStatus() >= 1 && statusChange == false)
		//{
		//	emptyAddress = C_EMPTY_ADDRESS + 4000;
		//	C_USER_POINTER = getUserPointer(emptyAddress);
		//	C_USER = emptyAddress;
		//	if (C_USER_POINTER == 0)
		//	{
		//		// ���¶�ȡ����ָ��
		//		continue;
		//	}
		//	statusChange = true;
		//	// ����ָ���Ѹı�
		//	MainDlg->Log(L"����ָ���Ѹı�");
		//}
		//else if (judgeGameStatus() == 0)
		//{
		//	// ѡ���ɫ
		//	statusChange = false;
		//}
		//dnf->programDelay(300);
		Sleep(300);
	}
	return 0;
}