#include "pch.h"
#include "manual.h"
#include "GameData.h"
#include "dnfUser.h"
#include "scripts.h"
#include "dnfData.h"
#include "MirageDragon.h"
#include "MirageDragonDlg.h"
#include "dnfCALL.h"

// ѡ���ɫ
void ManualLogic::selectRole()
{
	time_t now_time = time(nullptr);
	if (now_time - GLOBAL.last_update_roles > 60)
	{
		// ���½�ɫ�б�
		getRoleList();
	}
}

// �ڳ�����
void ManualLogic::inTown()
{
	// ����ս������
	if (!GLOBAL.town_info.entered) {

		// �ر�����״̬����Ϣ
		GLOBAL.dungeonInfoClean();

		// �ر�ͼ�ڵĹ���
		closeDungeonFunctions();

		// ��ʼ������Ľ���״̬
		GLOBAL.town_info.entered = true;
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
	if (!GLOBAL.dungeon_info.entered) {
		// ��ʼ��������Ϣ
		initDungeonInfo();

		// �ر�����״̬����Ϣ
		GLOBAL.townInfoClean();

		// ������������
		firstRoom();

		// ��ʼ������Ľ���״̬
		GLOBAL.dungeon_info.entered = true;
	}

	// ������������
	updateUserCoor();

	// �жϵ�ǰ�Ƿ���boos����(���Ҹ��µ�ǰ��������)
	bool is_boss = judgeIsBossRoom();

	// �ж��Ƿ���
	bool is_open_door = judgeDoorOpen();

	// ������ͼ�������Ʒ������...��
	getDungeonAllObj();

	// ͨ�ش���
	if (is_open_door && is_boss)
	{
		bool is_clearance = judgeClearance();
		if (is_clearance)
		{
			// �ر�ͼ�ڹ���
			closeDungeonFunctions();
			GLOBAL.dungeonInfoClean();
			while (judgeIsBossRoom() && GLOBAL.game_status == 3)
			{
				// �ȴ��뿪BOSS����
				programDelay(1000, 1);
			}
		}
	}
}

void ManualLogic::firstRoom()
{
	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;
	mainWindow->Log(L"������ͼ����");

	if (GLOBAL.function_switch.score)
	{
		superScore();
	}

	if (GLOBAL.function_switch.cool_down)
	{
		CString num;
		mainWindow->page2._cool_down.GetWindowText(num);
		float number = (float)_ttof(num);
		skillCoolDown(number);
	}

	if (GLOBAL.function_switch.hook_damage)
	{
		//hookDamage(true);
	}

	if (GLOBAL.function_switch.three_speed)
	{
		CString attack_speed, move_speed, casting_speed;
		mainWindow->page2._attack_speed.GetWindowText(attack_speed);
		mainWindow->page2._move_speed.GetWindowText(move_speed);
		mainWindow->page2._casting_speed.GetWindowText(casting_speed);
		threeSpeed(_ttoi(attack_speed), _ttoi(casting_speed), _ttoi(move_speed));
	}

	if (GLOBAL.function_switch.hidden_user)
	{
		hiddenUser();
	}
}

void ManualLogic::closeFunctions()
{
	if (GLOBAL.function_switch.cool_down)
	{
		skillCoolDown(0);
	}

	if (GLOBAL.function_switch.hook_damage)
	{
		//hookDamage(false);
	}

	if (GLOBAL.function_switch.three_speed)
	{
		threeSpeed(0, 0, 0);
	}
}