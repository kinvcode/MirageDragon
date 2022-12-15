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
#include "procData.h"
#include "dungeon.h"
#include "dnfPacket.h"

// 测试线程2
UINT test2Thread(LPVOID pParam)
{
	Log.info(L"线程2开始", true);
	PDATA.cur_job_run = true;
	while (true)
	{
		if (!PDATA.cur_job_run)
		{
			// 主动退出线程
			ExitThread(0);
		}
		programDelay(800, 0);
		break;
	}

	// 从队列中移除当前任务【重要】
	//PDATA.job_queue.pop();
	Log.info(L"线程2完成", true);
	PDATA.cur_job_run = false;
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
			PDATA.dnf.is_running = readInt(0x140000000) == 0x905A4D;
			if (!PDATA.dnf.is_running) {
				PDATA.dnf.PID = getProcessPID(L"DNF.exe");
				PDATA.dnf.dnf_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PDATA.dnf.PID);

				if (PDATA.dnf.PID == 0 || PDATA.dnf.dnf_handle == NULL)
				{
					MainDlg->Log(L"找不到游戏！");
				}
				programDelay(3000, 1);
				continue;
			}

			// 获取DNF窗口信息
			PDATA.dnf.window_top = windowIsTop();

			// 读取游戏状态
			GAME.game_status = readInt(ADDR.x64("C_GAME_STATUS"));

			// 获取角色ID
			if (GAME.game_status == 1)
			{
				if (!GAME.town_info.entered) {
					getRoleID();
				}
			}

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

			// 判断当前角色疲劳值
			if (getUserFatigue() == 0) {
				PDATA.jobs_list.pop();
			} else {
				// 判断当前角色是否有任务
				if (PDATA.jobs_list.front().job.size() == 0) {
					MainDlg->Log(L"当前角色任务已完成");
					PDATA.jobs_list.pop();
				}
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

				// 必须鼠标操控才有效果
				//if (cur_role.favorite == 0)
				//{
				//	// 获取普通角色位置
				//	if (pos == getRolePos(false)) {
				//		is_cur_role = true;
				//	}
				//}
				//else {
				//	// 获取偏爱角色位置
				//	if (pos == getRolePos(true)) {
				//		is_cur_role = true;
				//	}
				//}

				//if (!is_cur_role) {
					if (cur_role.favorite == 0)
					{
						pos += 5;
					}
					roleList();
					programDelay(500, 0);
					selectRole(pos);
					programDelay(500, 0);
				//}
			}
			JOB cur_job = cur_role.job.front();
			int job_category = cur_job.type;
			// 获取队列任务
			job->run(job_category);
		}
	end:
		programDelay(20, 1);
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
		programDelay(50, 1);
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

		// 检查疲劳
		if (getUserFatigue() == 0) {
			Log.info(L"疲劳不足", true);
			break;
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

		programDelay(50, 0);
	}

	// 从队列中移除当前任务【重要】
	//PDATA.job_queue.pop();
	Log.info(L"刷图任务结束", true);
	PDATA.cur_job_run = false;
	return 0;
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
vector<UINT(*)(LPVOID pParam)> Job::job_container = { dungeonThread,test2Thread };

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