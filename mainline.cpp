#include "pch.h"
#include "mainline.h"
#include "GameData.h"
#include "dnfUser.h"
#include "dnfData.h"
#include "procData.h"
#include "scripts.h"
#include "log.h"
#include "keyboardDriver.h"
#include "usbhidkeycode.h"
#include "dnfCALL.h"
#include "MirageDragonDlg.h"
#include "MirageDragon.h"
#include "dnfPacket.h"
#include "dataStruct.h"
#include "http.h"

vector<int> MainLineLogic::learn_skill_lv = { 5,8,15,20,25,30,35,40,45,65,70,76,80,85 };

void MainLineLogic::selectRole()
{
	if (!GAME.role_panel.entered) {
		GAME.role_panel.entered = true;
		// ���½�ɫ�б�
		getRoleList();
	}
}

void MainLineLogic::inTown()
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

	// �Ի�����
	while (isDialogue())
	{
		MSDK_keyPress(Keyboard_KongGe, 1); // �ո�
		MSDK_keyPress(Keyboard_ESCAPE, 1); // ESC
		programDelay(200, 1);
	}

	// ����ѧϰ
	int cur_lv = getUserLevel();
	if (find(learn_skill_lv.begin(), learn_skill_lv.end(), cur_lv) != learn_skill_lv.end()) {
		// ����ѧϰ������ѧϰ�ȴ����ƣ�
		cleanSkill();
		programDelay(800, 1);
		panelCall(3);
		programDelay(800, 1);
		learSkillCall();
		programDelay(800, 1);
		// ȥ������������׷������Ա任
		removeSkill();
	}

	// ���鴦��
	
	// ��ͼ
}

void MainLineLogic::selectDungeon()
{
	// ���ѡͼʱ����������˳�����
}


void MainLineLogic::inDungeon()
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

void MainLineLogic::firstRoom()
{
	Log.info(L"������ͼ����");
	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;

	// BUFF�߼�
	{
		// ���Ͽո�
		MSDK_keyPress(Keyboard_UpArrow, 1);
		MSDK_keyPress(Keyboard_UpArrow, 1);
		MSDK_keyPress(Keyboard_KongGe, 1);
		programDelay(350, 0);

		// ���ҿո�
		MSDK_keyPress(Keyboard_RightArrow, 1);
		MSDK_keyPress(Keyboard_RightArrow, 1);
		MSDK_keyPress(Keyboard_KongGe, 1);
		programDelay(350, 0);
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

void MainLineLogic::closeFunctions()
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

void MainLineLogic::attackMonsterLogic()
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

void MainLineLogic::clearanceLogic()
{
	// �ر�ͼ�ڹ���
	closeFunctions();

	// 
	bool dungeon_finished = false;

	while (judgeIsBossRoom() && GAME.game_status == 3) {

		Log.info(L"BOSS�������");
		//finalGatherItems();

		// ���û����
		while (!judgeAwarded())
		{
			// ����ESC����
			MSDK_keyPress(Keyboard_ESCAPE, 1);
			programDelay(500, 1);
		}

		// ��ȡ�̵�����
		int shop_type = getClearanceShop();
		if (shop_type == 1003)
		{
			// �رռӰ����̵�
			MSDK_keyPress(Keyboard_ESCAPE, 1);
			programDelay(1000, 1);
		}

		// �ֽ�װ��
		__int64 fatigue = getUserFatigue();

		if (fatigue == 0)
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


void MainLineLogic::finalGatherItems()
{
	// ѭ������
	bool has_item = true;
	// �رմ�͸
	penetrateMap(false);
	while (has_item)
	{
		getDungeonAllObj();
		has_item = (bool)GAME.dungeon_info.current_room.item_list.size();

		// ���������Ʒ
		if (has_item) {
			updateUserCoor();
			for (auto item : GAME.dungeon_info.current_room.item_list) {
				gatherAtUser(GAME.dungeon_info.user_coordinate, item);
			}
			// �жϽ����Ƿ�����Ʒ
			while (itemUnderFooter())
			{
				MSDK_keyPress(Keyboard_x, 1);
				Sleep(300);
			}
		}
	}
	Log.info(L"�������");
	penetrateMap(true);
}