#include "pch.h"
#include "jobs.h"
#include "MirageDragonDlg.h"
#include "dnfData.h"
#include "scripts.h"
#include "keyboardDriver.h"
#include "dnfUser.h"
#include "baseAddress.h"
#include "GameData.h"
#include "http.h"
#include "log.h"
#include "manual.h"
#include "procData.h"
#include "dungeon.h"
#include "dnfPacket.h"
#include "mainline.h"
#include "dailyJob.h"

UINT updateDataThread(LPVOID pParam)
{
	try
	{
		CMirageDragonDlg* MainDlg = (CMirageDragonDlg*)pParam;

		MainDlg->Log(L"数据更新线程已启动");

		while (true)
		{
			// 游戏读写状态
			PDATA.dnf.is_running = readInt(0x140000000) == 0x905A4D;
			if (!PDATA.dnf.is_running) {
				PDATA.dnf.PID = getProcessPID(L"DNF.exe");
				PDATA.dnf.dnf_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PDATA.dnf.PID);

				if (PDATA.dnf.PID == 0 || PDATA.dnf.dnf_handle == NULL)
				{
					MainDlg->Log(L"找不到游戏！");
				}
				Sleep(3000);
				continue;
			}

			// 获取DNF窗口信息
			PDATA.dnf.window_top = windowIsTop();
			if (!PDATA.dnf.window_top) 
			{
				MSDK_ReleaseAllKey();
			}

			// 读取游戏状态
			GAME.game_status = readInt(ADDR.x64("C_GAME_STATUS"));

			Sleep(500);
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
	MainDlg->Log(L"已切换自动模式");

	PDATA.queue_thread_run = true;

	Job* job = Job::getInstance();
	while (true)
	{
		if (!PDATA.queue_thread_run)
		{
			ExitThread(0);
		}

		// 如果没有队列任务或当前有任务在执行，就继续等待队列任务
		if (PDATA.cur_job_run)
		{
			goto end;
		}
		else {

			// 判断当前角色是否有任务
			if (PDATA.jobs_list.front().job.size() == 0) {
				MainDlg->Log(L"当前角色任务已完成");
				PDATA.jobs_list.pop();
			}


			// 如果没有队列任务也没有任务在执行，则退出线程
			if (PDATA.jobs_list.size() == 0)
			{
				// 退出线程
				MainDlg->Log(L"当前无任务");
				break;
			}

			// 选择当前角色
			ROLEJOB cur_role = PDATA.jobs_list.front();

			// 选择角色逻辑
			if (GAME.game_status != 0)
			{
				bool is_cur_role = false;
				int pos = cur_role.pos;
				//if (cur_role.favorite == 0)
				//{
				//	pos += 5;
				//}

				if (GAME.role_panel.index != pos) {
					roleList();
					Sleep(500);
					selectRole(pos);
					Sleep(500);
					GAME.role_panel.index = pos;
				}
			}
			JOB cur_job = cur_role.job.front();
			int job_category = cur_job.type;
			// 获取队列任务
			job->run(job_category);
		}
	end:
		Sleep(1000);
	}

	PDATA.queue_thread_run = false;
	return 0;
}

UINT baseThread(LPVOID pParam)
{
	CMirageDragonDlg* MainDlg = (CMirageDragonDlg*)pParam;
	MainDlg->Log(L"已切换手动模式");

	PDATA.manual_thread_run = true;

	while (true) {

		if (!PDATA.manual_thread_run)
		{
			ExitThread(0);
		}

		// 判断运行状态
		if (!PDATA.dnf.is_running) {
			// 应该停止线程
			goto threadEnd;
		}

		// 游戏不同状态的处理
		switch (GAME.game_status)
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
		Sleep(50);
	}

	PDATA.manual_thread_run = false;
	return 0;
}

UINT dungeonThread(LPVOID pParam)
{
	Log.info(L"刷图任务开始", true);
	PDATA.cur_job_run = true;

	// 初始化刷图数据
	DungeonLogic::initDG();

	while (true)
	{
		if (!PDATA.cur_job_run)
		{
			// 主动退出线程
			ExitThread(0);
		}

		// 检测副本列表任务完成
		if (DungeonLogic::dg_list.size() == 0)
		{
			PDATA.jobs_list.front().job.pop();
			break;
		}

		// 任务列表 - 任务完成状态更新、如果失败，更新失败信息

		// 游戏不同状态的处理
		switch (GAME.game_status)
		{
		case 0:
			// 选择角色界面
			DungeonLogic::selectRole();
			break;
		case 1:
			DungeonLogic::inTown();
			break;
		case 2:
			// 选则副本界面处理（注意加载副本界面也是这个状态）
			DungeonLogic::selectDungeon();
			break;
		case 3:
			// 副本中的逻辑处理
			DungeonLogic::inDungeon();
			break;
		default:
			break;
		}

		// 检查疲劳
		if (getUserFatigue() == 0) {
			Log.info(L"疲劳不足", true);
			break;
		}

		Sleep(50);
	}

	Log.info(L"刷图任务结束", true);
	PDATA.cur_job_run = false;
	return 0;
}

UINT mainLineThread(LPVOID pParam)
{
	Log.info(L"剧情任务开始", true);
	PDATA.cur_job_run = true;
	GAME.town_info.entered = false;
	while (true)
	{
		if (!PDATA.cur_job_run)
		{
			// 主动退出线程
			break;
			//ExitThread(0);
		}

		// 游戏不同状态的处理
		switch (GAME.game_status)
		{
		case 0:
			// 选择角色界面
			MainLineLogic::selectRole();
			break;
		case 1:
			MainLineLogic::inTown();
			break;
		case 2:
			// 选则副本界面处理（注意加载副本界面也是这个状态）
			MainLineLogic::selectDungeon();
			break;
		case 3:
			// 副本中的逻辑处理
			MainLineLogic::inDungeon();
			break;
		default:
			break;
		}

		Sleep(50);
		//break;
	}

	Log.info(L"剧情任务结束", true);
	PDATA.jobs_list.front().job.pop();
	PDATA.cur_job_run = false;
	return 0;
}

UINT dailyTaskThread(LPVOID pParam)
{
	Log.info(L"每日任务开始", true);
	PDATA.cur_job_run = true;

	// 判断等级=110
	int level = getUserLevel();
	if (level != 110) {
		Log.info(L"等级不满足要求，任务无法执行", true);
		goto daily_end;
	}
	// 判断名望>=4176
	int prestige = getUserPrestige();
	if (prestige < 4176) {
		Log.info(L"名望不满足要求，任务无法执行", true);
		goto daily_end;
	}
	// 判断疲劳值>1
	int fatigue = getUserFatigue();
	if (fatigue < 1) {
		Log.info(L"疲劳不满足要求，任务无法执行", true);
		goto daily_end;
	}

	// 初始化刷图数据
	DailyJobLogic::initData();

	while (true)
	{
		if (!PDATA.cur_job_run)
		{
			// 主动退出线程
			ExitThread(0);
		}

		// 游戏不同状态的处理
		switch (GAME.game_status)
		{
		case 0:
			// 选择角色界面
			DailyJobLogic::selectRole();
			break;
		case 1:
			// 检测任务是否完成
			Log.info(L"检测任务是否完成", true);
			if (DailyJobLogic::dungeon_times >= 4 && DailyJobLogic::daily_complete)
			{
				goto daily_end;
				break;
			}

			if (DailyJobLogic::job_faild) {
				Log.info(L"任务失败", true);
				goto daily_end;
				break;
			}

			if (getUserFatigue() < 1)
			{
				Log.info(L"疲劳不足，任务失败", true);
				goto daily_end;
				break;
			}

			DailyJobLogic::inTown();
			break;
		case 2:
			DailyJobLogic::selectDungeon();
			break;
		case 3:
			// 副本中的逻辑处理
			DailyJobLogic::inDungeon();
			break;
		default:
			break;
		}

		// 检查疲劳
		if (getUserFatigue() == 0) {
			Log.info(L"疲劳不足", true);
			break;
		}

		Sleep(50);
	}
daily_end:
	// 更新任务信息
	DailyJobLogic::updateData();
	PDATA.jobs_list.front().job.pop();
	Log.info(L"每日任务结束", true);
	PDATA.cur_job_run = false;
	return 0;
}

Job* Job::instance = nullptr;
vector<UINT(*)(LPVOID pParam)> Job::job_container = { dungeonThread,mainLineThread,dailyTaskThread };

Job* Job::getInstance()
{
	if (instance == nullptr) {
		instance = new Job();
	}
	return instance;
}

void Job::run(int category)
{
	// 执行当前线程
	PDATA.cur_job_thread = AfxBeginThread(job_container[category], this);
}