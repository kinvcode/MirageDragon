#pragma once
#include "dnfData.h"
#include "log.h"
#include "scripts.h"
#include "dnfUser.h"
#include "dnfCALL.h"
#include "MirageDragonDlg.h"
#include "MirageDragon.h"
#include "http.h"

class DailyJobLogic 
{
public:
	static int dungeon_times; // 刷图次数

	static bool daily_complete; // 任务完成

	static bool com_hook; // 普通房间HOOK伤害设置

	static bool boss_hook; // 普通房间HOOK伤害设置

	static bool job_faild; // 任务是否失败

	// 初始化数据
	static void initData();

	// 选择角色
	static void selectRole();

	// 在城镇中
	static void inTown();

	// 选择副本
	static void selectDungeon();

	// 在副本中
	static void inDungeon();

	// 首图功能
	static void firstRoom();

	// 关闭图内功能
	static void closeFunctions();

	// 打怪逻辑
	static void attackMonsterLogic();

	// 通关处理
	static void clearanceLogic();

	// 捡物兜底
	static void finalGatherItems();

	// 更新数据
	static void updateData();

	// 设置HOOK伤害
	static void setGeneralHook();
	static void setBossHook();
};