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
#include "dnfBase.h"
#include "baseAddress.h"

queue<DUNGEONJOB> DungeonLogic::dg_list = {};

bool DungeonLogic::com_hook = false; // ��ͨ����HOOK�˺�����

bool DungeonLogic::boss_hook = false; // ��ͨ����HOOK�˺�����

void DungeonLogic::selectRole()
{
	if (!GAME.role_panel.entered) {
		//time_t now_time = time(nullptr);
		//if (now_time - PDATA.last_update_roles > 60)
		//{
		//}

		// �ر�����״̬����Ϣ
		GAME.dungeonInfoClean();
		GAME.townInfoClean();

		GAME.role_panel.entered = true;

		// ���½�ɫ�б�
		//getRoleList();
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

		// ��ȡ��ɫID
		//getRoleID();

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

	// ��ȡ����ȼ�
	int level = getUserLevel();
	// ��ȡ��������
	int prestige = getUserPrestige();
	// ��ͼ�Ѷ� 0��ͨ 1ð�� 2��ʿ 3���� 4ج��
	int difficulty = 0;
	if (level >= 110)
	{
		if (prestige >= 25837)
		{
			// Ӣ��
			if (prestige >= 29369) {
				difficulty = 1;
			}
			if (prestige >= 30946) {
				difficulty = 2;
			}
			if (prestige >= 32523) {
				difficulty = 3;
			}
			if (prestige >= 33989) {
				difficulty = 4;
			}
		}
		else {
			// ��ͨ
			if (prestige >= 8602) {
				difficulty = 1;
			}
			if (prestige >= 13195) {
				difficulty = 2;
			}
			if (prestige >= 21675) {
				difficulty = 3;
			}
			if (prestige >= 23259) {
				difficulty = 4;
			}
		}
	}

	//// ��������
	areaCall(code);
	Sleep(500);
	// ���ѡͼ
	selectMap();
	Sleep(500);
	// �����ͼ
	entryDungeon(code, difficulty, 0, 0);
	Sleep(500);
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
		Log.info(L"׼����ʼ������", true);
		// ������BOSS�����ж�(��ֹ�ٴ���ս����)
		if (!dungeonRoomIsBoss())
		{
			Log.info(L"��ʼ������", true);
			// ��ʼ��������Ϣ
			initDungeonInfo();

			// �ر�����״̬����Ϣ
			GAME.townInfoClean();

			// ��������
			firstRoom();

			// ��ʼ������Ľ���״̬
			GAME.dungeon_info.entered = true;
		}
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

		if (is_boss) {
			// HOOK�˺�����
			setBossHook();
		}
		else {
			// HOOK�˺�����
			setGeneralHook();
		}

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
			finalGatherItems();
			Log.info(L"׼����ͼ");

			// �������ʱ������
			if (GAME.dungeon_info.current_room.room_has_urgent)
			{
				// ȡ��ʱ������λ��
				ROOMCOOR urgent_coor = getUrgentCoor();
				// �ܵ�ʱ�����д�
				runToMonter(urgent_coor.x, urgent_coor.y);
				return;
			}
			else {
				// ��ͼ�߼����Զ������¸����䣩
				autoNextRoom();
			}
		}
	}
}

void DungeonLogic::firstRoom()
{
	Log.info(L"������ͼ����");
	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;

	if (GAME.function_switch.hidden_user)
	{
		hiddenUser();
	}

	if (GAME.function_switch.score)
	{
		superScore();
	}

	// �����;�
	lockDurable();

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

	// ������壬������Ч
	panelCall(0);
	Sleep(200);
	panelCall(0);

	penetrate(true);

	// BUFF�߼�
	{
		// ���Ͽո�
		MSDK_keyPress(Keyboard_UpArrow, 1);
		Sleep(100);
		MSDK_keyPress(Keyboard_UpArrow, 1);
		Sleep(100);
		MSDK_keyPress(Keyboard_KongGe, 1);
		Sleep(800);

		// ���ҿո�
		MSDK_keyPress(Keyboard_RightArrow, 1);
		Sleep(100);
		MSDK_keyPress(Keyboard_RightArrow, 1);
		Sleep(100);
		MSDK_keyPress(Keyboard_KongGe, 1);
		Sleep(500);
	}
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
			int key = getCoolDownKey();
			MSDK_keyPress(key, 1);
		}
	}
}

void DungeonLogic::clearanceLogic()
{
	// �ر�ͼ�ڹ���
	closeFunctions();

	DUNGEONJOB* job = &dg_list.front();

	// ������������
	if (job->times > 0) {
		job->times--;
	}

	bool dungeon_finished = false;

	if (job->times == 0)
	{
		if (getUserFatigue() > 0)
		{
			dg_list.pop();
			// ����������Ϣ
			updateData();
		}
		dungeon_finished = true;
	}

	if (judgeIsBossRoom() && GAME.game_status == 3) {

		// ���û����
		while (!judgeAwarded())
		{
			// ����ESC����
			Log.info(L"���з���", true);
			MSDK_keyPress(Keyboard_ESCAPE, 1);
			Sleep(500);
		}

		// ��ȡ�̵�����
		int shop_type = getClearanceShop();
		if (shop_type == 1003)
		{
			Log.info(L"�ر��̵�", true);
			// �رռӰ����̵�
			MSDK_keyPress(Keyboard_ESCAPE, 1);
			Sleep(1000);
		}

		// �ֽ�װ��
		if (getBackpackLoad() > 50) {
			Log.info(L"�ֽ�װ��", true);
			getPackageOfEq();
		}

		Log.info(L"BOSS�������", true);
		finalGatherItems();

		// �����˺�
		com_hook = false;
		boss_hook = false;

		// ƣ���ж�
		int fatigue = getUserFatigue();
		if (fatigue == 0 && job->times == -1)
		{
			dungeon_finished = true;
		}

		// �ж������Ƿ����
		if (dungeon_finished)
		{
			Log.info(L"��ǰ�������������", true);
			// ���س���
			MSDK_keyPress(Keyboard_F12, 1);
			Sleep(333);
		}
		else {
			// ƣ��Ϊ�շ��س���
			if (fatigue < 1)
			{
				Log.info(L"��ǰ��������ʧ�ܣ�ƣ�Ͳ���", true);
				// ����ʧ�ܣ����س���
				MSDK_keyPress(Keyboard_F12, 1);
				Sleep(333);
			}
			else {
				Log.info(L"�ٴ���ս", true);
				MSDK_keyPress(Keyboard_F10, 1);
				Sleep(5000);
			}
		}
	}
	GAME.dungeonInfoClean();
}

void DungeonLogic::finalGatherItems()
{
	// ѭ������
	bool has_item = true;
	// �رմ�͸
	penetrate(false);
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
	penetrate(true);
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

void DungeonLogic::setGeneralHook()
{
	if (!com_hook) {
		int hook_value = GAME.dungeon_info.monster_max_blood;
		if (hook_value != 0) {
			hook_value = hook_value / 4;
			com_hook = true;
			CString msg;
			msg.Format(L"��������ͨ����HOOK�˺�Ϊ��%d", hook_value);
			Log.info(msg, true);
			updateHookValue(hook_value);
		}

	}
}

void DungeonLogic::setBossHook()
{
	if (!boss_hook)
	{
		int hook_value = GAME.dungeon_info.monster_max_blood;
		if (hook_value != 0) {
			hook_value = hook_value / 8;
			com_hook = true;
			CString msg;
			msg.Format(L"������BOSS����HOOK�˺�Ϊ��%d", hook_value);
			Log.info(msg, true);
			updateHookValue(hook_value);
		}
	}
}

bool DungeonLogic::dungeonRoomIsBoss()
{
	// ����ʱ��ָ�������ָ�루ÿ�ν�ͼ��仯/���������仯ʱ��
	__int64 time_pointer = readLong(readLong(ADDR.x64("C_ROOM_NUMBER")) + ADDR.x64("C_TIME_ADDRESS"));
	__int64 door_pointer = readLong(time_pointer + ADDR.x64("C_DOOR_TYPE_OFFSET"));
	// ��ȡ��ǰ����λ��
	ROOMCOOR current_coor;
	current_coor.x = readInt(time_pointer + ADDR.x64("C_BEGIN_ROOM_X"));
	current_coor.y = readInt(time_pointer + ADDR.x64("C_BEGIN_ROOM_Y"));

	// ��ȡBOSS����λ��
	ROOMCOOR boss_coor;
	boss_coor.x = (int)decrypt(door_pointer + ADDR.x64("C_BOSS_ROOM_X"));
	boss_coor.y = (int)decrypt(door_pointer + ADDR.x64("C_BOSS_ROOM_Y"));
	if (current_coor.x == boss_coor.x && current_coor.y == boss_coor.y)
	{
		return true;
	}

	return false;
}