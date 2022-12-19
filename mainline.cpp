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
		// 更新角色列表
		getRoleList();
		GAME.dungeonInfoClean();
		GAME.townInfoClean();
	}
}

void MainLineLogic::inTown()
{
	// 如果刚进入城镇
	if (!GAME.town_info.entered) {
		// 初始化城镇信息

		// 关闭其他状态的信息
		GAME.rolePanelClean();
		GAME.dungeonInfoClean();

		// 关闭图内的功能(防止直接出图)
		closeFunctions();

		// 初始化后更改进入状态
		GAME.town_info.entered = true;

		// 检查疲劳状态
		if (getUserFatigue() == 0) {
			exitMainline();
			return;
		}

		// 处理技能
		handleSkill();
	}

	// 处理对话
	handleDialogue();

handleJobBegin:
	// 剧情处理
	if (hasMainJob()) {
		// 获取剧情完成状态
		int status = getJobStatus(job_info.code);
		switch (status)
		{
		case -1:
			// 未接：接受任务
			acceptJobCall(job_info.code);
			programDelay(300);
			goto handleJobBegin;
			break;
		case 0:
			// 完成：完成任务
			submitJobCall(job_info.code);
			programDelay(300);
			goto handleJobBegin;
			break;
		default:
			// 已接：下个逻辑
			break;
		}

		// 剧情材料判断
		int m_status = handleJobMaterial(job_info.code);
		if (m_status != 0) {
			// 材料不满足，则退出任务
			exitMainline();
		}

		// 剧情条件判断
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
			// 获取任务地图
			int map = getJobMap(job_info.code);
			// 开始刷图

			areaCall(map);
			programDelay(300);
			selectMap();
			entryDungeon(map, 0, 0, 0);
		}

	}
	else {
		// 不存在任务，刷新角色尝试
		flushRole();

		// 不存在主线任务，且人物没有满级，则刷最高等级副本
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
	// 城镇逻辑结束
}

void MainLineLogic::selectDungeon()
{
	// 如果选图时间过长，则退出城镇
}


void MainLineLogic::inDungeon()
{
	// 刷图线程开启，关闭选角线程、城镇线程、选图线程

	// 如果刚进入地图
	if (!GAME.dungeon_info.entered) {

		// 初始化副本信息
		initDungeonInfo();

		// 关闭其他状态的信息
		GAME.townInfoClean();

		// 开启功能
		firstRoom();

		// 初始化后更改进入状态
		GAME.dungeon_info.entered = true;
	}

	// 更新人物坐标
	updateUserCoor();

	// 判断当前是否是boos房间(并且更新当前房间坐标)
	bool is_boss = judgeIsBossRoom();

	// 判断是否开门
	bool is_open_door = judgeDoorOpen();

	// 遍历地图（人物、物品、怪物...）
	getDungeonAllObj();

	// 处理对话
	handleDialogue();

	// 未开门时处理逻辑
	if (is_open_door == false)
	{
		// 打怪逻辑 
		attackMonsterLogic();
	}
	// 开门后的逻辑处理
	else {
		if (is_boss)
		{
			// 判断是否通关（篝火判断）
			bool is_clearance = judgeClearance();
			if (is_clearance)
			{
				// 通关处理
				clearanceLogic();
			}
		}
		else {
			// 捡物兜底
			//finalGatherItems();

			// 过图逻辑（自动进入下个房间）
			autoNextRoom();
		}
	}
}

void MainLineLogic::firstRoom()
{
	Log.info(L"开启首图功能");
	CMirageDragonDlg* mainWindow = (CMirageDragonDlg*)theApp.m_pMainWnd;

	// BUFF逻辑
	{
		// 上上空格
		MSDK_keyPress(Keyboard_UpArrow, 1);
		MSDK_keyPress(Keyboard_UpArrow, 1);
		MSDK_keyPress(Keyboard_KongGe, 1);
		programDelay(350);

		// 右右空格
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

	// 呼出面板，三速生效

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
		// 刷新怪物
		getDungeonAllObj();

		// 获取当前怪物数量
		int monster_num = (int)GAME.dungeon_info.current_room.monster_list.size();

		if (monster_num < 1)
		{
			break;
		}

		// 跟随怪物
		DUNGEONOBJ cur = GAME.dungeon_info.current_room.monster_list.front();
		bool res = runToMonter(cur.coor.x, cur.coor.y);
		if (res) {
			// 攻击怪物
			int key = getCoolDownKey();
			MSDK_keyPress(key, 1);
		}
	}
}

void MainLineLogic::clearanceLogic()
{
	// 关闭图内功能
	closeFunctions();

	while (judgeIsBossRoom() && GAME.game_status == 3) {

		//finalGatherItems();

		// 如果没翻牌
		while (!judgeAwarded())
		{
			// 进行ESC翻牌
			MSDK_keyPress(Keyboard_ESCAPE, 1);
			programDelay(500);
		}

		// 处理对话
		handleDialogue();

		// 分解装备

		GAME.dungeonInfoClean();

		// 如果没有剧情，则继续挑战
		if (!hasMainJob()) {
			while (GAME.game_status == 3)
			{
				// 获取商店类型
				int shop_type = getClearanceShop();
				if (shop_type == 1003)
				{
					// 关闭加百利商店
					MSDK_keyPress(Keyboard_ESCAPE, 1);
					programDelay(200);
				}

				MSDK_keyPress(Keyboard_F10, 1);
			}
		}

		// 返回城镇进行下一轮逻辑处理
		while (GAME.game_status == 3)
		{
			// 获取商店类型
			int shop_type = getClearanceShop();
			if (shop_type == 1003)
			{
				// 关闭加百利商店
				MSDK_keyPress(Keyboard_ESCAPE, 1);
				programDelay(200);
			}

			MSDK_keyPress(Keyboard_F12, 1);
		}

	}
}


void MainLineLogic::finalGatherItems()
{
	// 循环策略
	bool has_item = true;
	// 关闭穿透
	penetrateMap(false);
	while (has_item)
	{
		getDungeonAllObj();
		has_item = (bool)GAME.dungeon_info.current_room.item_list.size();

		// 如果存在物品
		if (has_item) {
			updateUserCoor();
			for (auto item : GAME.dungeon_info.current_room.item_list) {
				gatherAtUser(GAME.dungeon_info.user_coordinate, item);
			}
			// 判断脚下是否有物品
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
	// 对话处理
	while (isDialogue())
	{
		MSDK_keyPress(Keyboard_KongGe, 1); // 空格
		MSDK_keyPress(Keyboard_ESCAPE, 1); // ESC
		programDelay(200);
	}
}

void MainLineLogic::handleSkill()
{
	// 技能学习
	int cur_lv = getUserLevel();
	if (find(learn_skill_lv.begin(), learn_skill_lv.end(), cur_lv) != learn_skill_lv.end()) {
		// 技能学习（智能学习等待完善）
		cleanSkill();
		programDelay(800);
		panelCall(3);
		programDelay(800);
		learSkillCall();
		programDelay(800);
		// 去除后跳、受身蹲伏、属性变换
		removeSkill();
	}
}

bool MainLineLogic::hasMainJob()
{
	__int64 job_address, head_address, end_address, job_pointer;
	int job_number, job_type;
	CString job_name;

	// 任务地址
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

	// 计算
	int arr[3] = { 0,0,0 };
	int remainder;

	// 任务地址
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

			// 数组排序(冒泡排序,从大到小)
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
	// 获取剧情完成状态
	int status = getJobStatus(job_info.code);
	switch (status)
	{
	case -1:
		// 未接：接受任务
		acceptJobCall(job_info.code);
		programDelay(300);
		status = 1;
		break;
	case 0:
		// 完成：完成任务
		submitJobCall(job_info.code);
		programDelay(300);
		break;
	default:
		// 已接：下个逻辑
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
		// 任务：水晶净化
		// 材料：无色小晶块30个
		// 获取背包物品数量，获取仓库物品
		job_item = getItemNum(3037, 7);
		if (job_item >= 30)
		{
			return 0;
		}
		else {
			// 需要购买的材料3037,30
			return 1;
		}

		break;
	case 3209:
		// 任务：艾丽丝的帮助
		// 材料：无色小晶块100个
		job_item = getItemNum(3037, 7);
		if (job_item >= 100)
		{
			return 0;
		}
		else {
			// 需要购买的材料3037,30
			return 1;
		}
		break;
	case 3270:
		// 任务：诺顿的手艺
		// 材料：无色小晶块20个
		job_item = getItemNum(3037, 7);
		if (job_item >= 20)
		{
			return 0;
		}
		else {
			// 需要购买的材料3037,30
			return 1;
		}
		break;
	case 3346:
		// 任务：惊讶的诺顿
		// 材料：黑色小晶块30个；下级元素结晶10个
		job_item1 = getItemNum(3033, 7);
		job_item2 = getItemNum(3166, 3);
		if (job_item1 >= 30 && job_item2 >= 10)
		{
			return 0;
		}
		else {
			// 需要购买的材料3033,30
			// 需要购买的材料3166,10
			return 1;
		}
		break;
	case 3509:
		// 任务：返回赫顿玛尔
		// 材料：毁灭纪小碎片10
		job_item = getItemNum(10164800, 5);
		if (job_item >= 10)
		{
			// 完成CALL
			finishJobCall(job_info.code);
			programDelay(1000);
			return 0;
		}
		else {
			// 需要购买的材料3037,30
			return 3;
		}
		break;
	case 3680:
		// 任务：前往素喃
		// 材料：钢铁片50；黑曜石30
		job_item1 = getItemNum(3150, 3);
		job_item2 = getItemNum(3020, 3);
		if (job_item1 >= 50 && job_item2 >= 30)
		{
			return 0;
		}
		else {
			// 需要购买的材料3150,50
			// 需要购买的材料3020,30
			return 1;
		}
		break;
	case 3622:
		// 任务：时空转移之力
		// 材料：无色小晶块
		job_item = getItemNum(3037, 7);
		if (job_item >= 500)
		{
			finishJobCall(job_info.code);
			programDelay(1000);
			return 0;
		}
		else {
			// 需要购买的材料3037,500
			return 1;
		}

		break;
	case 3763:
		// 任务：危险的研究
		// 材料：黑色小晶块、白色小晶块、红色小晶块、蓝色小晶块200个
		job_item1 = getItemNum(3033, 3);
		job_item2 = getItemNum(3034, 3);
		job_item3 = getItemNum(3035, 3);
		job_item4 = getItemNum(3036, 3);
		if (job_item1 >= 200 && job_item2 >= 200 && job_item3 >= 200 && job_item4 >= 200)
		{
			return 0;
		}
		else {
			// 需要购买的材料3033,200
			// 需要购买的材料3034,200
			// 需要购买的材料3035,200
			// 需要购买的材料3036,200
			return 1;
		}

		break;
	case 5943:
		// 任务：黑市的商人
		// 材料：奇怪的蛇肉串1
		job_item = getItemNum(10305900, 7);
		if (job_item > 0)
		{
			finishJobCall(job_info.code);
			programDelay(1000);
			return 0;
		}
		else {
			// 需要购买的材料10305900,1
			// 移动到黑市
			// 组包购买蛇肉
			// 组包整理背包
			return 1;
		}

		break;
	case 12122:
		// 任务：米娅的通讯器
		// 材料：米娅的通讯器1
		//int job_item = getItemNum(10305900, 7);
		job_item = 0;
		if (job_item > 0)
		{
			finishJobCall(job_info.code);
			programDelay(1000);
			// 跳过CALL
			programDelay(1000);
			return 0;
		}
		else {
			return 3;
		}
		break;
	case 1000002:
		// 任务：二觉材料任务(2)
		return 1;
		break;
	case 1000004:
		// 任务：二觉材料任务(4)
		return 1;
		break;
	case 650:
		//  任务：右槽 魔法石(2)
		return 1;
		break;
	case 649:
		// 任务：右槽_辅助装备(2)
		return 1;
		break;
	case 2636:
		// 任务：耳环(2)
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
	// 主动退出线程
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

	// 任务地址
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
	int lv_map[110] = {5,//0级用于填充数组头
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