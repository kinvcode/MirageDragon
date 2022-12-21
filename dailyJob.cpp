#include "pch.h"
#include "dailyJob.h"
#include "opencv.h"
#include "keyboardDriver.h"
#include "usbhidkeycode.h"
#include "GameData.h"

int DailyJobLogic::dungeon_times = 0;

bool DailyJobLogic::daily_complete = false;

bool DailyJobLogic::com_hook = false; // ��ͨ����HOOK�˺�����

bool DailyJobLogic::boss_hook = false; // ��ͨ����HOOK�˺�����

bool DailyJobLogic::job_faild = false; // ����ʧ��

void DailyJobLogic::initData()
{
	dungeon_times = 0;
	job_faild = false;
	daily_complete = false;
}

// ѡ���ɫ
void DailyJobLogic::selectRole()
{

}

// �ڳ�����
void DailyJobLogic::inTown()
{
	// �ж������Ƿ����
	if (dungeon_times >= 4)
	{
		Log.info(L"����Ƿ������", true);
		if (hasPanel()) {
			while (hasPanel())
			{
				MSDK_keyPress(Keyboard_ESCAPE, 1);
				Sleep(300);
			}
		}

		// ��ÿ��
		Log.info(L"��ÿ�����", true);
		ROOMCOOR coor = OpenCV::getDailyIcon1();
		if (coor.x != 0 && !hasPanel()) {
			MSDK_MoveTo(coor.x, coor.y);
			Sleep(500);
			MSDK_LeftClick();
			Sleep(1000);
			
			bool submit = true;
			while (submit && hasPanel())
			{
				Log.info(L"�����ɰ�ť", true);
				ROOMCOOR coor2 = OpenCV::getDailySubmit();
				if (coor2.x != 0) {
					MSDK_MoveTo(coor2.x, coor2.y);
					Sleep(500);
					MSDK_LeftClick();
					Sleep(1000);
				}
				else {
					Log.info(L"��ɰ�ť�ѵ�����", true);
					submit = false;
					Log.info(L"�˳�����¼�", true);
					daily_complete = true;
					break;
				}
				Sleep(2000);
			}
		}
		return;
	}
	else {
		// �ж�ƣ��
		if (getUserFatigue() > 0) {
			Log.info(L"ƣ�����㣬��ʼѰ�Ҵ���λ��", true);

			Log.info(L"����Ƿ������", true);
			if (hasPanel()) {
				while (hasPanel())
				{
					MSDK_keyPress(Keyboard_ESCAPE, 1);
					Sleep(300);
				}
			}

			// ��ÿ��
			Log.info(L"��ÿ��", true);
			ROOMCOOR coor = OpenCV::getDailyIcon1();
			if (coor.x != 0 && !hasPanel()) {
				MSDK_MoveTo(coor.x, coor.y);
				Sleep(500);
				MSDK_LeftClick();
				Sleep(1000);

				// ������Ͱ�ť
				Log.info(L"�������", true);
				ROOMCOOR coor2 = OpenCV::getDelivery();
				if (coor2.x != 0 && hasPanel()) {
					MSDK_MoveTo(coor2.x, coor2.y);
					Sleep(500);
					MSDK_LeftClick();
					Sleep(1000);
					// ��ȡȷ�ϵ���
					Log.info(L"���ȷ��", true);
					ROOMCOOR coor3 = OpenCV::getDailySpace();
					if (coor3.x != 0 && hasPanel())
					{
						while (GAME.game_status == 1)
						{
							MSDK_keyPress(Keyboard_KongGe, 1);
							Sleep(200);
						}
					}
				}
				else {
					// û�д��Ͱ�ť������ʧ��
					Log.info(L"�Ҳ������Ͱ�ť������ʧ��", true);
					job_faild = true;
					return;
				}
			}
		}
		else {
			return;
		}
	}
}

// ѡ�񸱱�
void DailyJobLogic::selectDungeon()
{
	while (GAME.game_status == 2)
	{
		MSDK_keyPress(Keyboard_KongGe, 1);
		Sleep(200);
	}
}

// �ڸ�����
void DailyJobLogic::inDungeon()
{
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

			// ��ͼ�߼����Զ������¸����䣩
			
			if (GAME.penetrate_status != 1) {
				penetrate(true);
			}
			int map_code = getMapNumber();
			if (map_code == 100002705) {
				if (GAME.dungeon_info.current_room.coordinate.x == 2 || GAME.dungeon_info.current_room.coordinate.x == 3) 
				{
					GAME.use_pass_room_call = true;
				}
			}
			autoNextRoom();
		}
	}
}

// ��ͼ����
void DailyJobLogic::firstRoom()
{
	Log.info(L"������ͼ����");
	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;

	// BUFF�߼�
	{
		// ���Ͽո�
		MSDK_keyPress(Keyboard_UpArrow, 1);
		MSDK_keyPress(Keyboard_UpArrow, 1);
		MSDK_keyPress(Keyboard_KongGe, 1);
		Sleep(350);

		// ���ҿո�
		MSDK_keyPress(Keyboard_RightArrow, 1);
		MSDK_keyPress(Keyboard_RightArrow, 1);
		MSDK_keyPress(Keyboard_KongGe, 1);
		Sleep(350);
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
	panelCall(0);
	Sleep(200);
	panelCall(0);

	penetrate(true);
}

// �ر�ͼ�ڹ���
void DailyJobLogic::closeFunctions()
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

// ����߼�
void DailyJobLogic::attackMonsterLogic()
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
			if (key == Keyboard_x) {
				for (int i = 0; i < 4; i++)
				{
					MSDK_keyPress(key, 1);
					Sleep(300);
				}
			}
			else {
				MSDK_keyPress(key, 1);
			}
		}
	}
}

// ͨ�ش���
void DailyJobLogic::clearanceLogic()
{
	// �ر�ͼ�ڹ���
	closeFunctions();

	// ������������
	dungeon_times++;

	// ÿ���Ƿ����
	bool dungeon_finished = false;
	if (dungeon_times >= 4)
	{
		dungeon_finished = true;
	}

	while (judgeIsBossRoom() && GAME.game_status == 3) {

		finalGatherItems();

		// ���û����
		while (!judgeAwarded())
		{
			// ����ESC����
			MSDK_keyPress(Keyboard_ESCAPE, 1);
			Sleep(500);
		}

		// ��ȡ�̵�����
		int shop_type = getClearanceShop();
		if (shop_type == 1003)
		{
			// �رռӰ����̵�
			MSDK_keyPress(Keyboard_ESCAPE, 1);
			Sleep(1000);
		}

		// �ֽ�װ��
		if (getBackpackLoad() > 50) {
			getPackageOfEq();
		}

		finalGatherItems();

		// �����˺�
		com_hook = false;
		boss_hook = false;

		// ƣ���ж�
		int fatigue = getUserFatigue();
		if (fatigue == 0)
		{
			// ���س��������һ���߼�����
			while (GAME.game_status == 3)
			{
				MSDK_keyPress(Keyboard_F12, 1);
			}
		}

		GAME.dungeonInfoClean();

		// �ж������Ƿ����
		if (dungeon_finished)
		{
			// ���س���
 			while (GAME.game_status == 3)
			{
				MSDK_keyPress(Keyboard_F12, 1);
			}
		}
		else {
			// ������ս��һ������
			while (GAME.game_status == 3)
			{
				MSDK_keyPress(Keyboard_KongGe, 1);
			}
		}
	}
}

// ���ﶵ��
void DailyJobLogic::finalGatherItems()
{
	// ѭ������
	bool has_item = true;
	// �رմ�͸
	penetrate(false);
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
				// x����
				MSDK_keyPress(Keyboard_x, 1);
				Sleep(300);
			}
		}
	}
	penetrate(false);
}

void DailyJobLogic::updateData()
{
	// �ύ�����������
	http.updateJob(2);
}

void DailyJobLogic::setGeneralHook()
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

void DailyJobLogic::setBossHook()
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