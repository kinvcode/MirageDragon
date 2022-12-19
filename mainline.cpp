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

vector<int> MainLineLogic::learn_skill_lv = { 5,8,15,20,25,30,35,40,45,65,70,76,80,85 };

DNFJOB MainLineLogic::job_info = { -1,NULL,NULL };

vector<CString> MainLineLogic::dungeon_jobs = { L"[hunt monster]" , L"[hunt enemy]" , L"[condition under clear]" , L"[clear map]" , L"[seeking]" };

vector<CString> MainLineLogic::town_jobs = { L"[meet npc]" , L"[seek n meet npc]" , L"[reach the range]" , L"[look cinematic]" , L"[question]" , L"[quest clear]" };

void MainLineLogic::selectRole()
{
	if (!GAME.role_panel.entered) {
		GAME.role_panel.entered = true;
		// ���½�ɫ�б�
		getRoleList();
		GAME.dungeonInfoClean();
		GAME.townInfoClean();
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
			exitMainline();
			return;
		}

		// ������
		handleSkill();
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
			programDelay(300);
			goto handleJobBegin;
			break;
		case 0:
			// ��ɣ��������
			submitJobCall(job_info.code);
			programDelay(300);
			goto handleJobBegin;
			break;
		default:
			// �ѽӣ��¸��߼�
			break;
		}

		// ��������ж�
		int m_status = handleJobMaterial(job_info.code);
		if (m_status != 0) {
			// ���ϲ����㣬���˳�����
			exitMainline();
		}

		// ���������ж�
		int job_type = handleJobRequire();
		if (job_type == 0)
		{
			exitMainline();
		}
		if (job_type == 1) {
			finishJobCall(job_info.code);
			programDelay(300);
			goto handleJobBegin;
		}
		if (job_type == 2) {
			// ��ȡ�����ͼ
			int map = getJobMap(job_info.code);
			// ��ʼˢͼ

			areaCall(map);
			programDelay(300);
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
			programDelay(300);
			selectMap();
			entryDungeon(map, 0, 0, 0);
			//exitMainline();
		}
	}
	// �����߼�����
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
	getDungeonAllObj();

	// ����Ի�
	handleDialogue();

	// δ����ʱ�����߼�
	if (is_open_door == false)
	{
		// ����߼� 
		attackMonsterLogic();
	}
	// ���ź���߼�����
	else {
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
			//finalGatherItems();

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

	while (judgeIsBossRoom() && GAME.game_status == 3) {

		//finalGatherItems();

		// ���û����
		while (!judgeAwarded())
		{
			// ����ESC����
			MSDK_keyPress(Keyboard_ESCAPE, 1);
			programDelay(500);
		}

		// ����Ի�
		handleDialogue();

		// �ֽ�װ��

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
					programDelay(200);
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
				programDelay(200);
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
	penetrateMap(true);
}

void MainLineLogic::handleDialogue()
{
	// �Ի�����
	while (isDialogue())
	{
		MSDK_keyPress(Keyboard_KongGe, 1); // �ո�
		MSDK_keyPress(Keyboard_ESCAPE, 1); // ESC
		programDelay(200);
	}
}

void MainLineLogic::handleSkill()
{
	// ����ѧϰ
	int cur_lv = getUserLevel();
	if (find(learn_skill_lv.begin(), learn_skill_lv.end(), cur_lv) != learn_skill_lv.end()) {
		// ����ѧϰ������ѧϰ�ȴ����ƣ�
		cleanSkill();
		programDelay(800);
		panelCall(3);
		programDelay(800);
		learSkillCall();
		programDelay(800);
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
		programDelay(300);
		status = 1;
		break;
	case 0:
		// ��ɣ��������
		submitJobCall(job_info.code);
		programDelay(300);
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
		// ���񣺾��ȵ�ŵ��
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
			programDelay(1000);
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
			programDelay(1000);
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
			programDelay(1000);
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
		//int job_item = getItemNum(10305900, 7);
		job_item = 0;
		if (job_item > 0)
		{
			finishJobCall(job_info.code);
			programDelay(1000);
			// ����CALL
			programDelay(1000);
			return 0;
		}
		else {
			return 3;
		}
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
	ExitThread(0);
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
	int lv_map[110] = {5,//0�������������ͷ
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

void MainLineLogic::test()
{

	hasMainJob();
	int res = handleJobRequire();
	if (res == 0)
	{
		exit(0);
	}
}