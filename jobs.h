#pragma once

// ���巿��λ�ýṹ
struct ROOMCOOR
{
	__int64 x;
	__int64 y;
};

// ��Ϸ���ݸ����߳�
UINT updateDataThread(LPVOID pParam);

// ˢͼ�߳�
UINT playGameThead(LPVOID pParam);

// ��ȡ��ǰ�����ظ�����
int roomRepeats(vector<ROOMCOOR>, ROOMCOOR current_room);