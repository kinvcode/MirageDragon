#pragma once
#include "dataStruct.h"
#include <mutex>

// 游戏数据更新线程
UINT updateDataThread(LPVOID pParam);

// 任务队列线程
UINT jobQueueThread(LPVOID pParam);

// 刷图线程
UINT playGameThead(LPVOID pParam);

// 基础线程（保证手动功能）
UINT baseThread(LPVOID pParam);

// 测试线程1
UINT test1Thread(LPVOID pParam);

// 测试线程2
UINT test2Thread(LPVOID pParam);

// 选择角色逻辑
void selectRoleLogic();

// 城镇逻辑
void townLogic();

// 选择副本逻辑
void selectDungeonLogic();

// 副本中逻辑
void dungeonLogic();

// 清理怪物逻辑
void attackMonsterLogic();

// 捡物兜底
void finalGatherItems();

// 过图逻辑
void passRoomLogic();

// 通关逻辑
void clearanceLogic();

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