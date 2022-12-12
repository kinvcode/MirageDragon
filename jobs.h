#pragma once
#include "dataStruct.h"
#include <mutex>

// 游戏数据更新线程
UINT updateDataThread(LPVOID pParam);

// 任务队列线程
UINT jobQueueThread(LPVOID pParam);

// 基础线程（保证手动功能）
UINT baseThread(LPVOID pParam);

// 刷图线程
UINT dungeonThread(LPVOID pParam);

// 测试线程2
UINT test2Thread(LPVOID pParam);

// 获取当前房间重复次数
int roomRepeats(vector<ROOMCOOR>, ROOMCOOR current_room);

// 每日任务
void dailyTasks();

class Job {
protected:
	Job() {};
	static Job* instance;
public:
	static vector<UINT(*)(LPVOID pParam)> job_container;

	Job(Job& job_p) = delete;

	void operator=(const Job&) = delete;

	mutex job_run_lock;

	static Job* getInstance();

	void run(int category);
};