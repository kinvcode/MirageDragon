#pragma once
//#include <winnt.h>
//#include <vector>
#include <Windows.h>
#include "common.h"

using namespace std;

class GameData
{
public:

	DWORD PID = 0; // ��Ϸ����ID
	
	bool window_top = false; // ��Ϸ�����Ƿ��ö�״̬
	
	bool is_running = false; // ��Ϸ�Ƿ�����������

	HANDLE MSDK_HANDLE = NULL; // �׼�����

	bool is_auto_play = false; // �Զ�����

	// ��ע���Զ���ΪӦ��ʹ���������
	int auto_play_type = 0; // �Զ����ͣ�1ˢͼ��2����

	//////////////////////////////////////////////////////////////////////////

	int game_status = 0; // ��ǰ��Ϸ״̬��0ѡ���ɫ��1����2ѡ�񸱱���3������

	int penetrate_status;  // ���ﴩ͸״̬��0δ��͸��1�Ѵ�͸

	FUNCTIONSWITCH function_switch = { true,true,true,true,true,true,true }; // �����б�

	bool use_pass_room_call = false; // �Ƿ�ǿ��ʹ�ù�ͼCALL

	int play_user_index = 0; // ��ǰ��ɫ����

	int autoMapNumber = 0; // �������

	DUNGEONINFO dungeon_info; // ��ǰ������Ϣ
};

extern GameData GLOBAL;