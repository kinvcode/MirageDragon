#include "pch.h"
#include "jobs.h"
#include "MirageDragonDlg.h"
#include "dnfData.h"
#include "scripts.h"
#include "keyboardDriver.h"
#include "dnfUser.h"
#include "dnfTask.h"
#include "baseAddress.h"
#include "GameData.h"
#include "http.h"
#include "log.h"
#include "manual.h"

// 测试线程1
UINT test1Thread()
{
	Log.info(L"线程1开始", true);
	programDelay(3000, 0);
	Log.info(L"线程1结束", true);
	return 0;
}
// 测试线程2
UINT test2Thread()
{
	Log.info(L"线程2开始", true);
	programDelay(800, 0);
	Log.info(L"线程2结束", true);
	return 0;
}

UINT updateDataThread(LPVOID pParam)
{
	try
	{
		CMirageDragonDlg* MainDlg = (CMirageDragonDlg*)pParam;

		MainDlg->Log(L"数据更新线程已启动");

		while (true)
		{
			// 游戏读写状态
			GLOBAL.is_running = readInt(0x140000000) == 0x905A4D;
			if (!GLOBAL.is_running) {
				GLOBAL.PID = getProcessPID(L"DNF.exe");
				GLOBAL.dnf_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GLOBAL.PID);

				if (GLOBAL.PID == 0 || GLOBAL.dnf_handle == NULL)
				{
					MainDlg->Log(L"找不到游戏！");
				}
				programDelay(3000, 1);
				continue;
			}

			// 获取DNF窗口信息
			GLOBAL.window_top = windowIsTop();

			// 读取游戏状态
			GLOBAL.game_status = readInt(ADDR.x64("C_GAME_STATUS"));

			programDelay(500, 1);
		}
		return 0;
	}
	catch (const std::exception& e)
	{
		string error = e.what();
		CString msg = CString(error.c_str());
		AfxMessageBox(msg);
	}
	return 0;
}

UINT jobQueueThread(LPVOID pParam)
{
	CMirageDragonDlg* MainDlg = (CMirageDragonDlg*)pParam;
	MainDlg->Log(L"已切换自动动模式");

	Job* job = Job::getInstance();
	while (true)
	{
		// 如果没有队列任务或当前有任务在执行，就继续等待队列任务
		if (GLOBAL.job_queue.size() == 0 || GLOBAL.queue_thread_run)
		{
			goto end;
		}

		// 如果没有队列任务也没有任务在执行，则退出线程
		if (GLOBAL.job_queue.size() == 0 && !GLOBAL.queue_thread_run)
		{
			// 退出线程
			MainDlg->Log(L"当前无任务");
		}

		// 获取队列任务
		int job_category = GLOBAL.job_queue.front();
		// 从队列中移除当前任务
		GLOBAL.job_queue.erase(GLOBAL.job_queue.begin());
		job->run(job_category);

	end:
		programDelay(20, 1);
	}
	return 0;
}

UINT baseThread(LPVOID pParam)
{
	CMirageDragonDlg* MainDlg = (CMirageDragonDlg*)pParam;
	MainDlg->Log(L"已切换手动模式");

	while (true) {
		// 判断运行状态
		if (!GLOBAL.is_running) {
			// 应该停止线程
			goto threadEnd;
		}

		// 游戏不同状态的处理
		switch (GLOBAL.game_status)
		{
		case 0:
			// 选择角色界面
			ManualLogic::selectRole();
			break;
		case 1:
			ManualLogic::inTown();
			break;
		case 2:
			ManualLogic::selectDungeon();
			break;
		case 3:
			ManualLogic::inDungeon();
			break;
		default:
			break;
		}

	threadEnd:
		programDelay(50, 1);
	}
	return 0;
}


UINT playGameThead(LPVOID pParam)
{
	CMirageDragonDlg* MainDlg = (CMirageDragonDlg*)pParam;

	MainDlg->Log(L"刷图线程已启动");

	while (true) {
		// 判断运行状态
		if (!GLOBAL.is_running) {
			// 应该停止线程
			goto threadEnd;
		}

		// 游戏不同状态的处理
		switch (GLOBAL.game_status)
		{
		case 0:
			// 选择角色界面
			selectRoleLogic();
			break;
		case 1:
			townLogic();
			break;
		case 2:
			// 选则副本界面处理（注意加载副本界面也是这个状态）
			selectDungeonLogic();
			break;
		case 3:
			// 副本中的逻辑处理
			dungeonLogic();
			break;
		default:
			break;
		}

		// 自动开关
		if (GLOBAL.is_auto_play)
		{

		}

	threadEnd:
		Sleep(50);
	}

	MainDlg->Log(L"刷图线程已结束");

	return 0;
}

// 选择角色逻辑
void selectRoleLogic()
{
	time_t now_time = time(nullptr);
	if (now_time - GLOBAL.last_update_roles > 60)
	{
		// 更新角色列表
		getRoleList();
	}
}

// 城镇逻辑
void townLogic()
{
	// 如果刚进入城镇
	if (!GLOBAL.town_info.entered) {
		// 初始化城镇信息

		// 关闭其他状态的信息
		GLOBAL.dungeonInfoClean();
		// 关闭图内的功能
		closeDungeonFunctions();

		// 初始化后更改进入状态
		GLOBAL.town_info.entered = true;
	}
}

// 选择副本逻辑
void selectDungeonLogic()
{
	// 如果选图时间过长，则退出城镇
}

// 副本中逻辑
void dungeonLogic()
{
	// 刷图线程开启，关闭选角线程、城镇线程、选图线程

	// 如果刚进入地图
	if (!GLOBAL.dungeon_info.entered) {
		Log.info(L"首次进图，初始化功能");
		// 初始化副本信息
		initDungeonInfo();

		// 关闭其他状态的信息
		GLOBAL.townInfoClean();

		// 开启副本功能
		firstRoomFunctions();

		// 初始化后更改进入状态
		GLOBAL.dungeon_info.entered = true;
	}

	// 更新人物坐标
	updateUserCoor();

	// 判断当前是否是boos房间(并且更新当前房间坐标)
	bool is_boss = judgeIsBossRoom();

	// 判断是否开门
	bool is_open_door = judgeDoorOpen();

	// 遍历地图（人物、物品、怪物...）
	Log.info(L"遍历怪物与物品");
	getDungeonAllObj();

	// 未开门时处理逻辑
	if (is_open_door == false)
	{
		Log.info(L"当前未开门，开始清怪");
		// 打怪逻辑 
		attackMonsterLogic();
	}
	// 开门后的逻辑处理
	else {
		if (is_boss)
		{
			Log.info(L"BOSS房间怪物清理完毕");
			// 判断是否通关（篝火判断）
			bool is_clearance = judgeClearance();
			if (is_clearance)
			{
				Log.info(L"开始通关处理");
				// 通关处理
				clearanceLogic();
			}
		}
		else {
			// 捡物兜底
			Log.info(L"已开门，准备捡物");
			finalGatherItems();
			Log.info(L"准备过图");
			passRoomLogic();
		}
	}
}

void attackMonsterLogic()
{
	if (!GLOBAL.is_auto_play) {
		return;
	}

	// 开启穿透建筑
	penetrate(true);
	while (true)
	{
		// 刷新怪物
		getDungeonAllObj();

		// 获取当前怪物数量
		int monster_num = (int)GLOBAL.dungeon_info.current_room.monster_list.size();

		if (monster_num < 1)
		{
			break;
		}

		// 跟随怪物
		DUNGEONOBJ cur = GLOBAL.dungeon_info.current_room.monster_list.front();
		runToDestination(cur.coor.x, cur.coor.y, false, 20);

		// 攻击怪物
		int key = getCoolDownKey();
		MSDK_keyPress(key, 1);
	}
	// 结束穿透建筑
	penetrate(false);
}

void finalGatherItems()
{
	// 循环策略
	bool has_item = true;
	// 关闭穿透
	penetrate(false);
	while (has_item)
	{
		Log.info(L"刷新怪物与物品");
		getDungeonAllObj();
		has_item = (bool)GLOBAL.dungeon_info.current_room.item_list.size();
		// 如果存在物品
		if (has_item) {
			Log.info(L"存在物品");
			updateUserCoor();
			for (auto item : GLOBAL.dungeon_info.current_room.item_list) {
				gatherAtUser(GLOBAL.dungeon_info.user_coordinate, item);
			}

			// 判断脚下是否有物品
			while (itemUnderFooter())
			{
				Log.info(L"进行捡物");
				// x捡物
				MSDK_keyPress(Keyboard_x, 1);
				Sleep(300);
			}
		}
	}
	Log.info(L"捡物完毕");
	penetrate(true);
}

void passRoomLogic()
{
	//if (GLOBAL.is_auto_play)
	//{
	// // 如果当前没有开启穿透，则启用穿透
	//	{
	//		InstanceLock lock(MainDlg);
	//		MainDlg->Log(L"没有物品，开启穿透");
	//	}
	//	penetrate(true);
	//}

	if (GLOBAL.is_auto_play) {
		// 自动过图
		autoNextRoom();
	}
}

// 通关逻辑
void clearanceLogic()
{
	// 关闭图内功能
	closeDungeonFunctions();
	GLOBAL.dungeonInfoClean();
	while (judgeIsBossRoom() && GLOBAL.game_status == 3) {
		if (GLOBAL.is_auto_play) {

			Log.info(L"BOSS房间捡物");
			finalGatherItems();

			// 如果没翻牌
			if (!judgeAwarded()) {
				// 进行ESC翻牌
				MSDK_keyPress(Keyboard_ESCAPE, 1);
				Sleep(500);
				continue;
			}

			// 获取商店类型
			int shop_type = getClearanceShop();
			if (shop_type == 1003)
			{
				// 关闭加百利商店
				MSDK_keyPress(Keyboard_ESCAPE, 1);
				Sleep(1000);
			}

			// 分解装备

			// 疲劳为空返回城镇
			if (getUserFatigue() < 1)
			{
				// 返回城镇
				MSDK_keyPress(Keyboard_F12, 1);
			}
			if (GLOBAL.auto_play_type == 0) {
				// 再次挑战
				MSDK_keyPress(Keyboard_F10, 1);
			}
		}
	}
}

int roomRepeats(vector<ROOMCOOR> list, ROOMCOOR room)
{
	__int64 length = list.size();
	if (length <= 1)
	{
		return 1;
	}

	int numbers = 0;

	for (__int64 i = 0; i < length; i++)
	{
		if (list[i].x == room.x && list[i].y == room.y)
		{
			numbers++;
		}
	}
	return numbers;
}

void dailyTasks()
{
	// 判断等级=110
	// 判断疲劳值>1
	// 判断名望>=4176

	// 返回城镇

	// 关闭界面

	// 打开活动快捷栏

	// 打开《畅玩任务》界面（两种类型搜索）

	// 点击传送

	// 空格/点击确认传送

	// 进入选图界面，开始选图界面逻辑

	// 进入地图、开始刷图逻辑

	// 通关逻辑、进入下一个任务地图 | 四图完成，返回城镇

	// 打开《畅玩任务》界面（两种类型搜索）

	// 搜索领取按钮并点击

	// 任务退出
}

Job* Job::instance = nullptr;
vector<UINT(*)()> Job::job_container = { test1Thread,test2Thread };

Job* Job::getInstance()
{
	if (instance == nullptr) {
		instance = new Job();
	}
	return instance;
}

void Job::run(int category)
{
	// 锁定线程，保证只有一个线程在运行
	job_run_lock.lock();
	GLOBAL.queue_thread_run = true;

	// 执行当前线程
	job_container[category]();

	// 解锁线程，允许开始下个任务
	GLOBAL.queue_thread_run = false;
	job_run_lock.unlock();
}