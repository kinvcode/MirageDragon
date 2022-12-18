#include "pch.h"
#include "dungeon.h"
#include "procData.h"
#include "dnfUser.h"
#include "scripts.h"
#include "GameData.h"
#include "log.h"
#include "dnfData.h"
#include "keyboardDriver.h"
#include "usbhidkeycode.h"
#include "dnfCALL.h"
#include "MirageDragonDlg.h"
#include "MirageDragon.h"
#include "dnfPacket.h"
#include "dataStruct.h"
#include "http.h"

queue<DUNGEONJOB> DungeonLogic::dg_list = {};

void DungeonLogic::selectRole()
{
	if (!GAME.role_panel.entered) {
		//time_t now_time = time(nullptr);
		//if (now_time - PDATA.last_update_roles > 60)
		//{
		//}

		GAME.role_panel.entered = true;

		// ���½�ɫ�б�
		getRoleList();
	}

}

void DungeonLogic::inTown()
{
	// ����ս������
	if (!GAME.town_info.entered) {
		// ��ʼ��������Ϣ

		// �ر�����״̬����Ϣ
		GAME.rolePanelClean();
		GAME.dungeonInfoClean();

		// �ر�ͼ�ڵĹ���(��ֱֹ�ӳ�ͼ)
		closeFunctions();

		// ��ʼ������Ľ���״̬
		GAME.town_info.entered = true;

		// ���ƣ��״̬
		if (getUserFatigue() == 0) {
			return;
		}
	}

	// û�������򲻴���
	if (dg_list.size() < 1) {
		return;
	}

	// ��ȡ��ǰҪ����ĸ���ID
	int code = dg_list.front().dungeon_code;

	//// ��������
	areaCall(code);
	// ���ѡͼ
	selectMap();
	// �����ͼ
	entryDungeon(code, 0, 0, 0);
}

void DungeonLogic::selectDungeon()
{
	// ���ѡͼʱ����������˳�����
}
void DungeonLogic::inDungeon()
{
	// ˢͼ�߳̿������ر�ѡ���̡߳������̡߳�ѡͼ�߳�

	// ����ս����ͼ
	if (!GAME.dungeon_info.entered) {

		// ��ʼ��������Ϣ
		initDungeonInfo();

		// �ر�����״̬����Ϣ
		GAME.townInfoClean();

		// ��������
		firstRoom();

		// ��ʼ������Ľ���״̬
		GAME.dungeon_info.entered = true;
	}

	// ������������
	updateUserCoor();

	// �жϵ�ǰ�Ƿ���boos����(���Ҹ��µ�ǰ��������)
	bool is_boss = judgeIsBossRoom();

	// �ж��Ƿ���
	bool is_open_door = judgeDoorOpen();

	// ������ͼ�������Ʒ������...��
	Log.info(L"������������Ʒ");
	getDungeonAllObj();

	// δ����ʱ�����߼�
	if (is_open_door == false)
	{
		Log.info(L"��ǰδ���ţ���ʼ���");
		// ����߼� 
		attackMonsterLogic();
	}
	// ���ź���߼�����
	else {
		if (is_boss)
		{
			Log.info(L"BOSS��������������");
			// �ж��Ƿ�ͨ�أ������жϣ�
			bool is_clearance = judgeClearance();
			if (is_clearance)
			{
				Log.info(L"��ʼͨ�ش���");
				// ͨ�ش���
				clearanceLogic();
			}
		}
		else {
			// ���ﶵ��
			Log.info(L"�ѿ��ţ�׼������");
			//finalGatherItems();
			Log.info(L"׼����ͼ");

			// ��ͼ�߼����Զ������¸����䣩
			autoNextRoom();
		}
	}
}

void DungeonLogic::firstRoom()
{
	Log.info(L"������ͼ����");
	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;

	// BUFF�߼�
	{
		// ���Ͽո�
		MSDK_keyPress(Keyboard_UpArrow, 1);
		MSDK_keyPress(Keyboard_UpArrow, 1);
		MSDK_keyPress(Keyboard_KongGe, 1);
		programDelay(350);

		// ���ҿո�
		MSDK_keyPress(Keyboard_RightArrow, 1);
		MSDK_keyPress(Keyboard_RightArrow, 1);
		MSDK_keyPress(Keyboard_KongGe, 1);
		programDelay(350);
	}

	if (GAME.function_switch.score)
	{
		superScore();
	}

	if (GAME.function_switch.cool_down)
	{
		CString num;
		mainWindow->page2._cool_down.GetWindowText(num);
		float number = (float)_ttof(num);
		skillCoolDown(number);
	}

	if (GAME.function_switch.three_speed)
	{
		CString attack_speed, move_speed, casting_speed;
		mainWindow->page2._attack_speed.GetWindowText(attack_speed);
		mainWindow->page2._move_speed.GetWindowText(move_speed);
		mainWindow->page2._casting_speed.GetWindowText(casting_speed);
		threeSpeed(_ttoi(attack_speed), _ttoi(casting_speed), _ttoi(move_speed));
	}

	if (GAME.function_switch.hidden_user)
	{
		hiddenUser();
	}

	// ������壬������Ч

	while (!hasPanel())
	{
		MSDK_keyPress(Keyboard_m, 1);
		Sleep(500);
	}
	while (hasPanel())
	{
		MSDK_keyPress(Keyboard_m, 1);
		Sleep(500);
	}

	penetrate(true);
}

void DungeonLogic::closeFunctions()
{
	if (GAME.function_switch.cool_down)
	{
		skillCoolDown(0);
	}

	if (GAME.function_switch.three_speed)
	{
		threeSpeed(0, 0, 0);
	}

	penetrate(false);
}

void DungeonLogic::attackMonsterLogic()
{
	while (true)
	{
		// ˢ�¹���
		getDungeonAllObj();

		// ��ȡ��ǰ��������
		int monster_num = (int)GAME.dungeon_info.current_room.monster_list.size();

		if (monster_num < 1)
		{
			break;
		}

		// �������
		DUNGEONOBJ cur = GAME.dungeon_info.current_room.monster_list.front();
		bool res = runToMonter(cur.coor.x, cur.coor.y);
		if (res) {
			// ��������
			//int key = getCoolDownKey();
			MSDK_keyPress(Keyboard_x, 1);
		}
	}
}

void DungeonLogic::clearanceLogic()
{
	// �ر�ͼ�ڹ���
	closeFunctions();

	DUNGEONJOB* job = &dg_list.front();

	// ������������
	if (job->times != -1 && job->times != 0) {
		job->times--;
	}

	bool dungeon_finished = false;

	if (job->times == 0)
	{
		dg_list.pop();
		dungeon_finished = true;
	}

	// ����������Ϣ
	updateData();

	while (judgeIsBossRoom() && GAME.game_status == 3) {

		Log.info(L"BOSS�������");
		//finalGatherItems();

		// ���û����
		while (!judgeAwarded())
		{
			// ����ESC����
			MSDK_keyPress(Keyboard_ESCAPE, 1);
			programDelay(500);
		}

		// ��ȡ�̵�����
		int shop_type = getClearanceShop();
		if (shop_type == 1003)
		{
			// �رռӰ����̵�
			MSDK_keyPress(Keyboard_ESCAPE, 1);
			programDelay(1000);
		}

		// �ֽ�װ��
		__int64 fatigue = getUserFatigue();

		if (fatigue == 0 && job->times == -1)
		{
			dungeon_finished = true;
		}

		GAME.dungeonInfoClean();
		// �ж������Ƿ����
		if (dungeon_finished)
		{
			Log.info(L"��ǰ�������������", true);
			// ���س���
			while (GAME.game_status == 3)
			{
				MSDK_keyPress(Keyboard_F12, 1);
			}
		}
		else {
			// ƣ��Ϊ�շ��س���
			if (fatigue < 1)
			{
				Log.info(L"��ǰ��������ʧ�ܣ�ƣ�Ͳ���", true);
				// ����ʧ�ܣ����س���
				while (GAME.game_status == 3)
				{
					MSDK_keyPress(Keyboard_F12, 1);
				};
			}
			// �ٴ���ս
			while (GAME.game_status == 3)
			{
				MSDK_keyPress(Keyboard_F10, 1);
			}
		}
	}
}

void DungeonLogic::finalGatherItems()
{
	// ѭ������
	bool has_item = true;
	// �رմ�͸
	penetrateMap(false);
	while (has_item)
	{
		Log.info(L"ˢ�¹�������Ʒ");
		getDungeonAllObj();
		has_item = (bool)GAME.dungeon_info.current_room.item_list.size();
		// ���������Ʒ
		if (has_item) {
			Log.info(L"������Ʒ");
			updateUserCoor();
			for (auto item : GAME.dungeon_info.current_room.item_list) {
				gatherAtUser(GAME.dungeon_info.user_coordinate, item);
			}

			// �жϽ����Ƿ�����Ʒ
			while (itemUnderFooter())
			{
				Log.info(L"���м���");
				// x����
				MSDK_keyPress(Keyboard_x, 1);
				Sleep(300);
			}
		}
	}
	Log.info(L"�������");
	penetrateMap(true);
}

void DungeonLogic::initDG()
{
	JOB dungeon_array = PDATA.jobs_list.front().job.front();
	json json_data = dungeon_array.data;

	for (json::iterator it = json_data.begin(); it != json_data.end(); ++it) {
		dg_list.push(it->get<DUNGEONJOB>());
	}
}

void DungeonLogic::updateData() 
{
	// �ύ�����������
	http.updateJob(0);
}