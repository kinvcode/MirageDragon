#include "pch.h"
#include "manual.h"
#include "GameData.h"
#include "dnfUser.h"
#include "scripts.h"
#include "dnfData.h"
#include "MirageDragon.h"
#include "MirageDragonDlg.h"
#include "dnfCALL.h"
#include "procData.h"
#include "log.h"
#include "keyboardDriver.h"
#include "usbhidkeycode.h"

// ѡ���ɫ
void ManualLogic::selectRole()
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

// �ڳ�����
void ManualLogic::inTown()
{
	// ����ս������
	if (!GAME.town_info.entered) {

		// �ر�����״̬����Ϣ
		GAME.rolePanelClean();
		GAME.dungeonInfoClean();

		// �ر�ͼ�ڵĹ���
		closeFunctions();

		// ��ʼ������Ľ���״̬
		GAME.town_info.entered = true;

		// ��ȡ��ɫID
		//Log.info(L"���½�ɫID", true);
		//getRoleID();
	}
}

// ѡ�񸱱�
void ManualLogic::selectDungeon()
{

}

// �ڸ�����
void ManualLogic::inDungeon()
{
	// ˢͼ�߳̿������ر�ѡ���̡߳������̡߳�ѡͼ�߳�

	// ����ս����ͼ
	if (!GAME.dungeon_info.entered) {
		// ��ʼ��������Ϣ
		initDungeonInfo();

		// �ر�����״̬����Ϣ
		GAME.townInfoClean();

		// ������������
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
	getDungeonAllObj();

	//// ͨ�ش���
	//if (is_open_door && is_boss)
	//{
	//	bool is_clearance = judgeClearance();
	//	if (is_clearance)
	//	{
	//		// �ر�ͼ�ڹ���
	//		closeFunctions();
	//		GAME.dungeonInfoClean();
	//		while (judgeIsBossRoom() && GAME.game_status == 3)
	//		{
	//			// �ȴ��뿪BOSS����
	//			Sleep(1000);
	//		}
	//	}
	//}

		// δ����ʱ�����߼�
	if (is_open_door == false)
	{
		// ����߼� 
		if (PDATA.play_model) 
		{
			attackMonsterLogic();
		}
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
				// �ر�ͼ�ڹ���
				closeFunctions();
				GAME.dungeonInfoClean();
			}
		}
		else {

			if (PDATA.play_model)
			{
				// ���ﶵ��
				finalGatherItems();

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

}

void ManualLogic::firstRoom()
{
	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;
	mainWindow->Log(L"������ͼ����");

	if (GAME.function_switch.score)
	{
		superScore();
	}

	// �����;�
	//lockDurable();

	if (GAME.function_switch.cool_down)
	{
		CString num;
		mainWindow->page2._cool_down.GetWindowText(num);
		float number = (float)_ttof(num);
		skillCoolDown(number);
	}

	if (GAME.function_switch.hook_damage)
	{
		//hookDamage(true);
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
}

void ManualLogic::closeFunctions()
{
	if (GAME.function_switch.cool_down)
	{
		skillCoolDown(0);
	}

	if (GAME.function_switch.hook_damage)
	{
		//hookDamage(false);
	}

	if (GAME.function_switch.three_speed)
	{
		threeSpeed(0, 0, 0);
	}
}


// ����߼�
void ManualLogic::attackMonsterLogic() 
{
	while (true)
	{
		if (!PDATA.play_model) {
			break;
		}
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

void ManualLogic::finalGatherItems()
{
	// ѭ������
	bool has_item = true;
	while (has_item)
	{
		if (!PDATA.play_model) {
			break;
		}
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
}