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
#include "baseAddress.h"
#include "dnfBase.h"
#include "dnfMap.h"

vector<int> MainLineLogic::learn_skill_lv = { 5,8,15,20,25,30,35,40,45,65,70,76,80,85 };

DNFJOB MainLineLogic::job_info = { -1,NULL,NULL };

vector<CString> MainLineLogic::dungeon_jobs = { L"[hunt monster]" , L"[hunt enemy]" , L"[condition under clear]" , L"[clear map]" , L"[seeking]" };

vector<CString> MainLineLogic::town_jobs = { L"[meet npc]" , L"[seek n meet npc]" , L"[reach the range]" , L"[look cinematic]" , L"[question]" , L"[quest clear]" };

bool MainLineLogic::com_hook = false; // ��ͨ����HOOK�˺�����

bool MainLineLogic::boss_hook = false; // ��ͨ����HOOK�˺�����

void MainLineLogic::selectRole()
{
	if (!GAME.role_panel.entered) {
		// ���½�ɫ�б�
		//getRoleList();
		GAME.dungeonInfoClean();
		GAME.townInfoClean();
		GAME.role_panel.entered = true;
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
		GAME.seldungeonClean();

		// �ر�ͼ�ڵĹ���(��ֱֹ�ӳ�ͼ)
		closeFunctions();

		// ��ʼ������Ľ���״̬
		GAME.town_info.entered = true;

		// ��ȡ��ɫID
		//getRoleID();

		// ���ƣ��״̬
		if (getUserFatigue() == 0) {
			Log.info(L"ƣ������գ�������������", true);
			updateData();
			exitMainline();
			return;
		}

		// ������
		//handleSkill();
	}

	// ����Ի�
	handleDialogue();

handleJobBegin:
	// ���鴦��
	if (hasMainJob()) {
		// ��ȡ�������״̬
		int status = getJobStatus(job_info.code);
		switch (status)
		{
		case -1:
			// δ�ӣ���������
			acceptJobCall(job_info.code);
			Sleep(300);
			goto handleJobBegin;
			break;
		case 0:
			// ��ɣ��������
			submitJobCall(job_info.code);
			Sleep(300);
			goto handleJobBegin;
			break;
		default:
			// �ѽӣ��¸��߼�
			break;
		}

		// ����������

		// ��������ж�
		int m_status = handleJobMaterial(job_info.code);

		if (m_status != 0) {
			Log.info(L"������ϲ��㣬�˳�����", true);
			exitMainline();
			return;
		}

		// ���������ж�
		int job_type = handleJobRequire();
		if (job_type == 0)
		{
			Log.info(L"δ֪���������ͣ��˳�����", true);
			exitMainline();
			return;
		}
		if (job_type == 1) {
			finishJobCall(job_info.code);
			Sleep(300);
			goto handleJobBegin;
		}
		if (job_type == 2) {
			// ��ȡ�����ͼ
			int map = getJobMap(job_info.code);
			// ��ʼˢͼ

			areaCall(map);
			Sleep(300);
			selectMap();
			entryDungeon(map, 0, 0, 0);
		}

	}
	else {
		// ����������ˢ�½�ɫ����
		flushRole();

		// ��������������������û����������ˢ��ߵȼ�����
		int lv = getUserLevel();
		if (!hasMainJob() && lv < 110)
		{
			int map = getMaxLvMap();
			areaCall(map);
			Sleep(300);
			selectMap();
			entryDungeon(map, 0, 0, 0);
		}
	}
	// �����߼�����
}

void MainLineLogic::selectDungeon()
{
	int begin_time = 0;
	if (!GAME.sel_dungeon.entered) {
		GAME.townInfoClean();
		GAME.sel_dungeon.entered = true;

		begin_time = (int)time(NULL);
	}
	else {
		if (time(NULL) - begin_time > 2) {
			MSDK_keyPress(Keyboard_ESCAPE, 1);
		}
	}

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
	getMainLineDungeonAllObj();

	// ����Ի�
	handleDialogue();

	// δ����ʱ�����߼�
	if (is_open_door == false)
	{
		if (is_boss) {
			// HOOK�˺�����
			setBossHook();
		}
		else {
			// HOOK�˺�����
			setGeneralHook();

			//// ��ɫ��ؿ����䣨��������3,0��
			//if (getMapNumber() == 100002724 && GAME.dungeon_info.current_room.coordinate.x == 3 && GAME.dungeon_info.current_room.coordinate.y == 0)
			//{
			//	runToMonter(2000, 500);
			//}
		}

		// ����߼� 
		attackMonsterLogic();
	}
	// ���ź���߼�����
	else {

		// ������BOSS�������
		if (GAME.dungeon_info.map_code == 51)
		{
			ROOMCOOR boss_coor;
			boss_coor.x = (int)decrypt(GAME.dungeon_info.door_pointer + ADDR.x64("C_BOSS_ROOM_X"));
			boss_coor.y = (int)decrypt(GAME.dungeon_info.door_pointer + ADDR.x64("C_BOSS_ROOM_Y"));
			GAME.dungeon_info.boos_room = boss_coor;
			updateRooms();
			return;
		}

		if (is_boss)
		{
			// �ж��Ƿ�ͨ�أ������жϣ�
			bool is_clearance = judgeClearance();
			if (is_clearance)
			{
				// ͨ�ش���
				clearanceLogic();
			}
		}
		else {
			// ���ﶵ��
			finalGatherItems();

			// ��ͼ����������д��
			//if (GAME.dungeon_info.map_code == 15) {
			//	if (GAME.dungeon_info.current_room.coordinate.y == 1) {
			//		passRoomByPacket(0, 0);
			//	}
			//}

			// ����˺��֮��-��ͼ
			if (job_info.code == 12940) {
				if (GAME.dungeon_info.current_room.coordinate.x == 6 && GAME.dungeon_info.current_room.coordinate.y == 1)
				{
					passRoomByPacket(6, 0);
					return;
				}
			}

			// �������ڲ����߻�-��ͼ2,1
			if (job_info.code == 13570) {
				if (GAME.dungeon_info.current_room.coordinate.x == 1 && GAME.dungeon_info.current_room.coordinate.y == 1)
				{
					passRoomByPacket(2, 1);
					return;
				}
			}

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
		// �Ի�����
		handleDialogue();

		// ˢ�¹���
		getMainLineDungeonAllObj();

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

	while (judgeIsBossRoom() && GAME.game_status == 3) {

		finalGatherItems();

		// ���û����
		while (!judgeAwarded())
		{
			// ����ESC����
			MSDK_keyPress(Keyboard_ESCAPE, 1);
			Sleep(500);
		}

		// ����Ի�
		handleDialogue();

		// �ֽ�װ��
		if (getBackpackLoad() > 50) {
			getPackageOfEq();
		}

		finalGatherItems();

		// �����˺�
		com_hook = false;
		boss_hook = false;

		GAME.dungeonInfoClean();

		// ���û�о��飬�������ս
		if (!hasMainJob()) {
			while (GAME.game_status == 3)
			{
				// ��ȡ�̵�����
				int shop_type = getClearanceShop();
				if (shop_type == 1003)
				{
					// �رռӰ����̵�
					MSDK_keyPress(Keyboard_ESCAPE, 1);
					Sleep(200);
				}

				MSDK_keyPress(Keyboard_F10, 1);
			}
		}

		// ���س��������һ���߼�����
		while (GAME.game_status == 3)
		{
			// ��ȡ�̵�����
			int shop_type = getClearanceShop();
			if (shop_type == 1003)
			{
				// �رռӰ����̵�
				MSDK_keyPress(Keyboard_ESCAPE, 1);
				Sleep(200);
			}

			MSDK_keyPress(Keyboard_F12, 1);
		}

	}
}


void MainLineLogic::finalGatherItems()
{
	// ѭ������
	bool has_item = true;
	// �رմ�͸
	penetrate(false);
	while (has_item)
	{
		getMainLineDungeonAllObj();
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
	penetrate(true);
}

void MainLineLogic::handleDialogue()
{
	// �Ի�����
	while (isDialogue())
	{
		MSDK_keyPress(Keyboard_KongGe, 1); // �ո�
		MSDK_keyPress(Keyboard_ESCAPE, 1); // ESC
		Sleep(200);
	}
}

void MainLineLogic::handleSkill()
{
	// ����ѧϰ
	int cur_lv = getUserLevel();
	if (find(learn_skill_lv.begin(), learn_skill_lv.end(), cur_lv) != learn_skill_lv.end()) {
		// ����ѧϰ������ѧϰ�ȴ����ƣ�
		cleanSkill();
		Sleep(800);
		panelCall(3);
		Sleep(800);
		learSkillCall();
		Sleep(800);
		// ȥ������������׷������Ա任
		removeSkill();
	}
}

bool MainLineLogic::hasMainJob()
{
	__int64 job_address, head_address, end_address, job_pointer;
	int job_number, job_type;
	CString job_name;

	// �����ַ
	job_address = readLong(ADDR.x64("C_JOB_ADDRESS"));
	head_address = readLong(job_address + ADDR.x64("C_ALL_JOB_HEAD"));
	end_address = readLong(job_address + ADDR.x64("C_ALL_JOB_END"));
	job_number = (int)((end_address - head_address) / 8);

	for (int i = 0; i < job_number; i++)
	{
		job_pointer = readLong(head_address + i * 8);
		job_type = readInt(job_pointer + ADDR.x64("C_JOB_TYPE"));

		if (job_type == 0)
		{
			int job_length = readInt(job_pointer + 40);
			if (job_length > 7) {
				job_name = readString(readLong(job_pointer + 16), 50);
			}
			else {
				job_name = readString(job_pointer + 16, 50);
			}
			CString job_require = readString(readLong(job_pointer + ADDR.x64("C_JOB_REQUIRE")), 50);
			int job_code = readInt(job_pointer);

			job_info.name = job_name;
			job_info.require = job_require;
			job_info.code = job_code;
			return true;
		}
	}

	return false;
}

int MainLineLogic::getJobStatus(int code)
{
	__int64 job_address, head_address, end_address, job_pointer;
	int job_number;
	CString job_name;

	// ����
	int arr[3] = { 0,0,0 };
	int remainder;

	// �����ַ
	job_address = readLong(ADDR.x64("C_JOB_ADDRESS"));
	head_address = readLong(job_address + ADDR.x64("C_ACCEPTED_JOB_HEAD"));
	end_address = readLong(job_address + ADDR.x64("C_ACCEPTED_JOB_END"));
	job_number = (int)((end_address - head_address) / 16);

	for (int i = 0; i < job_number; i++)
	{
		job_pointer = readLong(head_address + i * 16);
		if (readInt(job_pointer) == code)
		{
			int times = (int)decrypt(head_address + i * 16 + 8);
			if (times < 512) {
				return times;
			}
			if (times == 512) {
				return 1;
			}
			arr[0] = times % 512;
			remainder = times - arr[0];
			if (remainder < 262144) {
				arr[1] = remainder / 512;
			}
			else {
				arr[1] = remainder % 262144 / 512;
			}
			remainder = remainder - arr[0] * 512;
			if (remainder < 262144)
			{
				arr[2] = 0;
			}
			else
			{
				arr[2] = remainder / 262144;
			}

			// ��������(ð������,�Ӵ�С)
			int k, j, len, temp;
			len = 3;
			for (k = len; k > 0; k--) {
				for (j = 0; j < k; j++) {
					if (arr[j] < arr[j + 1]) {
						temp = arr[j + 1];
						arr[j + 1] = arr[j];
						arr[j] = temp;
					}
				}
			}

			if (arr[0] == 0) {
				arr[0] = 1;
			}
			return arr[0];
		}
	}
	return -1;
}

int MainLineLogic::handleJob()
{
	// ��ȡ�������״̬
	int status = getJobStatus(job_info.code);
	switch (status)
	{
	case -1:
		// δ�ӣ���������
		acceptJobCall(job_info.code);
		Sleep(300);
		status = 1;
		break;
	case 0:
		// ��ɣ��������
		submitJobCall(job_info.code);
		Sleep(300);
		break;
	default:
		// �ѽӣ��¸��߼�
		break;
	}
	return status;
}

int MainLineLogic::handleJobMaterial(int code)
{
	int job_item, job_item1, job_item2, job_item3, job_item4;
	switch (code)
	{
	case 3182:
		// ����ˮ������
		// ���ϣ���ɫС����30��
		// ��ȡ������Ʒ��������ȡ�ֿ���Ʒ
		job_item = getItemNum(3037, 7);
		if (job_item >= 30)
		{
			return 0;
		}
		else {
			// ��Ҫ����Ĳ���3037,30
			return 1;
		}

		break;
	case 3209:
		// ���񣺰���˿�İ���
		// ���ϣ���ɫС����100��
		job_item = getItemNum(3037, 7);
		if (job_item >= 100)
		{
			return 0;
		}
		else {
			// ��Ҫ����Ĳ���3037,30
			return 1;
		}
		break;
	case 3270:
		// ����ŵ�ٵ�����
		// ���ϣ���ɫС����20��
		job_item = getItemNum(3037, 7);
		if (job_item >= 20)
		{
			return 0;
		}
		else {
			// ��Ҫ����Ĳ���3037,30
			return 1;
		}
		break;
	case 3346:
		// ���񣺾��ȵ�ŵ��(50���Ѿ�����)
		// ���ϣ���ɫС����30�����¼�Ԫ�ؽᾧ10��
		job_item1 = getItemNum(3033, 7);
		job_item2 = getItemNum(3166, 3);
		if (job_item1 >= 30 && job_item2 >= 10)
		{
			return 0;
		}
		else {
			// ��Ҫ����Ĳ���3033,30
			// ��Ҫ����Ĳ���3166,10
			return 1;
		}
		break;
	case 3509:
		// ���񣺷��غն����
		// ���ϣ������С��Ƭ10
		job_item = getItemNum(10164800, 5);
		if (job_item >= 10)
		{
			// ���CALL
			finishJobCall(job_info.code);
			Sleep(1000);
			return 0;
		}
		else {
			// ��Ҫ����Ĳ���3037,30
			return 3;
		}
		break;
	case 3680:
		// ����ǰ�����
		// ���ϣ�����Ƭ50������ʯ30
		job_item1 = getItemNum(3150, 3);
		job_item2 = getItemNum(3020, 3);
		if (job_item1 >= 50 && job_item2 >= 30)
		{
			return 0;
		}
		else {
			// ��Ҫ����Ĳ���3150,50
			// ��Ҫ����Ĳ���3020,30
			return 1;
		}
		break;
	case 3622:
		// ����ʱ��ת��֮��
		// ���ϣ���ɫС����
		job_item = getItemNum(3037, 7);
		if (job_item >= 500)
		{
			finishJobCall(job_info.code);
			Sleep(1000);
			return 0;
		}
		else {
			// ��Ҫ����Ĳ���3037,500
			return 1;
		}

		break;
	case 3763:
		// ����Σ�յ��о�
		// ���ϣ���ɫС���顢��ɫС���顢��ɫС���顢��ɫС����200��
		job_item1 = getItemNum(3033, 3);
		job_item2 = getItemNum(3034, 3);
		job_item3 = getItemNum(3035, 3);
		job_item4 = getItemNum(3036, 3);
		if (job_item1 >= 200 && job_item2 >= 200 && job_item3 >= 200 && job_item4 >= 200)
		{
			return 0;
		}
		else {
			// ��Ҫ����Ĳ���3033,200
			// ��Ҫ����Ĳ���3034,200
			// ��Ҫ����Ĳ���3035,200
			// ��Ҫ����Ĳ���3036,200
			return 1;
		}

		break;
	case 5943:
		// ���񣺺��е�����
		// ���ϣ���ֵ����⴮1
		job_item = getItemNum(10305900, 7);
		if (job_item > 0)
		{
			finishJobCall(job_info.code);
			Sleep(1000);
			return 0;
		}
		else {
			// ��Ҫ����Ĳ���10305900,1
			// �ƶ�������
			// �����������
			// ���������
			return 1;
		}

		break;
	case 12122:
		// ������櫵�ͨѶ��
		// ���ϣ���櫵�ͨѶ��1
		finishJobCall(job_info.code);
		Sleep(1000);
		break;
	case 1000002:
		// ���񣺶�����������(2)
		return 1;
		break;
	case 1000004:
		// ���񣺶�����������(4)
		return 1;
		break;
	case 650:
		//  �����Ҳ� ħ��ʯ(2)
		return 1;
		break;
	case 649:
		// �����Ҳ�_����װ��(2)
		return 1;
		break;
	case 2636:
		// ���񣺶���(2)
		return 1;
		break;
	default:
		return 0;
		break;
	}
	return 10;
}

void MainLineLogic::exitMainline()
{
	// �����˳��߳�
	PDATA.cur_job_run = false;
	PDATA.jobs_list.front().job.pop();
}

int MainLineLogic::handleJobRequire()
{
	for (auto item : town_jobs)
	{
		if (item == job_info.require)
		{
			return 1;
		}
	}

	for (auto item : dungeon_jobs)
	{
		if (item == job_info.require)
		{
			return 2;
		}
	}
	return 0;
}

int MainLineLogic::getJobMap(int code)
{
	__int64 job_address, head_address, end_address, job_pointer;
	int number;

	// �����ַ
	job_address = readLong(ADDR.x64("C_JOB_ADDRESS"));
	head_address = readLong(job_address + ADDR.x64("C_ALL_JOB_HEAD"));
	end_address = readLong(job_address + ADDR.x64("C_ALL_JOB_END"));
	number = (int)((end_address - head_address) / 8);
	for (int i = 0; i < number; i++)
	{
		job_pointer = readLong(head_address + i * 8);
		if (readInt(job_pointer) == code)
		{
			int job_data = getJobSpecialMap(code);
			if (job_data == -1) {
				job_data = readInt(readLong(job_pointer + ADDR.x64("C_JOB_MAP")));
				return job_data;
			}
			else {
				return job_data;
			}
		}
	}
	return 0;
}

int MainLineLogic::getJobSpecialMap(int code)
{
	if (code == 675) {
		return 82;
	}
	if (code == 674) {
		return 163;
	}
	if (code == 2635)
	{
		return 311;
		//return 314;
	}

	return -1;
}

int MainLineLogic::getMaxLvMap()
{
	int level = getUserLevel();
	int lv_map[110] = { 5,//0�������������ͷ
		3,5,5,5,5,6,6,6,9,9,//1~10
		9,7,7,8,8,1000,1000,1000,12,13,//11~20
		14,17,15,15,22,23,24,25,26,26,//21~30
		32,150,151,35,34,34,153,154,154,154,//31~40
		41,42,43,141,141,141,50,51,53,53,//41~50
		145,146,148,148,157,158,159,160,160,163,//51~60
		164,164,164,81,82,88,88,83,84,85,//61~70
		85,87,92,93,93,71,72,74,75,76,//71~80
		76,103,104,104,104,192,310,312,314,314,//81~90
		291100293,291100293,291100293,291100293,291100293,291100293,291100293,291100293,291100293,291100293,//91~100
		291100293,291100293,291100293,291100293,291100293,291100293,291100293,291100293,291100293 //101~109
	};
	if (level < 110) {
		return lv_map[level];
	}
	return 5;
}

void MainLineLogic::updateData()
{
	// �ύ�����������
	http.updateJob(1);
}

void MainLineLogic::test()
{

	hasMainJob();
	int res = handleJobRequire();
	if (res == 0)
	{
		exit(0);
	}
}

// ����ͼ�ڶ�����Ϣ�������Ʒ��
void MainLineLogic::getMainLineDungeonAllObj()
{
	CURRENTROOM* p_current_room = &GAME.dungeon_info.current_room;
	p_current_room->dungeon_object_list.clear();
	p_current_room->item_list.clear();
	p_current_room->monster_list.clear();

	if (GAME.game_status != 3)
	{
		return;
	}

	__int64 map_base = readLong(readLong(readLong(ADDR.x64("C_USER_ADDRESS")) + ADDR.x64("C_MAP_OFFSET")) + 16);
	__int64 head_address = readLong(map_base + ADDR.x64("C_MAP_HEAD"));
	__int64 end_address = readLong(map_base + ADDR.x64("C_MAP_END"));

	if (head_address == 0 || end_address == 0) {
		return;
	}

	int object_quantity = (int)(end_address - head_address) / 32;
	if (object_quantity == 0 || object_quantity > 1000) {
		return;
	}

	// �����б���Ѫ�����ֵ
	int monster_max_blood = 0;

	for (__int64 i = 1; i <= object_quantity; i++)
	{
		__int64 monster_address = readLong(readLong(head_address + i * 32) + 16) - 32;
		int monster_camp = readInt(monster_address + ADDR.x64("C_CAMP_OFFSET"));
		int monster_type = readInt(monster_address + ADDR.x64("C_TYPE_OFFSET"));
		int monster_code = readInt(monster_address + ADDR.x64("C_CODE_OFFSET"));
		int monster_blood = readInt(monster_address + ADDR.x64("C_MONSTER_BLOOD"));
		COORDINATE monster_coor = readCoordinate(monster_address);

		// ͼ�ڶ���ṹ��
		DUNGEONOBJ d_object;
		d_object.p = monster_address;
		d_object.blood = monster_blood;
		d_object.type = monster_type;
		d_object.camp = monster_camp;
		d_object.code = monster_code;
		d_object.coor = monster_coor;

		// ��Ʒ
		if (d_object.type == 289 && d_object.coor.z == 0)
		{
			d_object.code = readInt(readLong(monster_address + ADDR.x64("C_GROUND_ITEM")) + ADDR.x64("C_CODE_OFFSET"));
			// ������Ʒ
			if (d_object.code != 0)
			{
				p_current_room->item_list.push_back(d_object);
			}
		}
		// �жԹ������ż���͹���
		else if (d_object.type == 273 || d_object.type == 529 || d_object.type == 545)
		{
			// ����BUG�ĵط�
			bool bug_exists = true;

			// Ͷʯ��Ѫ����0
			if (d_object.type == 545)
			{
				goto mon_push;
			}

			if ((d_object.camp == 100 || d_object.camp == 101 || d_object.camp == 110 || d_object.camp == 120 || d_object.camp == 75) && d_object.blood != 0)
			{

				if (d_object.code == 70137 && d_object.coor.y > 600) {
					// ������̳��BOSS
					bug_exists = false;
				}

				if (d_object.code == 63821 && d_object.coor.x > 1000) {
					// ��ɫ��ؿ�BOSS
					bug_exists = false;
				}

				if (d_object.code == 109010762 && d_object.coor.y > 600)
				{
					// �����ո�ս������
					bug_exists = false;
				}

				if (d_object.code == 1097 && d_object.coor.x == 0)
				{	
					// �¹��ֿ�����
					bug_exists = false;
				}

				//// �¹���-�ƶ��ĵ���
				//if (d_object.type == 1057 && d_object.code != 30533)
				//{

				//	bug_exists = false;
				//}

			mon_push:
				if (bug_exists) {
					p_current_room->monster_list.push_back(d_object);
				}


				// ���Ѫ��ֵ���� 
				if (d_object.blood > monster_max_blood)
				{
					// ���Ѫ������(�ƻ�֮�������״̬)
					if (d_object.code != 109013674)
					{
						monster_max_blood = d_object.blood;
					}
				}

				// �۹ִ���
				if (GAME.function_switch.gather_monster)
				{
					// ��ɽ�Ļ���ǹ�����ƶ�
					if (d_object.code != 109013676)
					{
						gatherAtUser(GAME.dungeon_info.user_coordinate, d_object);
					}
				}
			}
		}

		// �������ʱ������
		if (d_object.camp == 200 && d_object.type == 33 && d_object.code == 490019076)
		{
			Log.info(L"��ǰ�������ʱ������", true);
			p_current_room->room_has_urgent = true;
		}
		//handleEvents();
	}

	// �������Ѫ��
	GAME.dungeon_info.monster_max_blood = monster_max_blood;

	// ����ð���㷨�Թ��������������(���ݸ÷�������ŷ���)
	if (GAME.dungeon_info.user_coordinate.x < 500) {
		int i, j, len;
		DUNGEONOBJ temp;
		len = (int)p_current_room->monster_list.size() - 1;
		for (i = len; i > 0; i--) {
			for (j = 0; j < i; j++) {
				if (p_current_room->monster_list[j].coor.x > p_current_room->monster_list[j + 1].coor.x) {
					temp = p_current_room->monster_list[j + 1];
					p_current_room->monster_list[j + 1] = p_current_room->monster_list[j];
					p_current_room->monster_list[j] = temp;
				}
			}
		}
	}
	else {
		int i, j, len;
		DUNGEONOBJ temp;
		len = (int)p_current_room->monster_list.size() - 1;
		for (i = len; i > 0; i--) {
			for (j = 0; j < i; j++) {
				if (p_current_room->monster_list[j].coor.x < p_current_room->monster_list[j + 1].coor.x) {
					temp = p_current_room->monster_list[j + 1];
					p_current_room->monster_list[j + 1] = p_current_room->monster_list[j];
					p_current_room->monster_list[j] = temp;
				}
			}
		}
	}
}

void MainLineLogic::setGeneralHook()
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

void MainLineLogic::setBossHook()
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