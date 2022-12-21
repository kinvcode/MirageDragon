#pragma once

class MainLineLogic {
public:
	// 任务信息
	static DNFJOB job_info;

	// 学习技能的等级
	static vector<int> learn_skill_lv;

	// 刷图任务容器
	static vector<CString> dungeon_jobs;

	// 城镇任务容器
	static vector<CString> town_jobs;

	static bool com_hook; // 普通房间HOOK伤害设置

	static bool boss_hook; // 普通房间HOOK伤害设置

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

	// 处理对话
	static void handleDialogue();

	// 处理技能
	static void handleSkill();

	// 是否存在主线任务
	static bool hasMainJob();

	// 获取任务状态
	static int getJobStatus(int code);

	// 处理任务
	static int handleJob();

	// 任务材料处理 -1重新剧情 0材料充足 1材料不足 3剧情材料 5未找到 10其他任务
	static int handleJobMaterial(int code);

	// 退出剧情任务线程
	static void exitMainline();

	// 剧情要求0未知要求 1对话 2刷图
	static int handleJobRequire();

	// 获取剧情地图
	static int getJobMap(int code);

	// 剧情特殊地图
	static int getJobSpecialMap(int code);

	// 获取等级最高地图
	static int getMaxLvMap();

	// 更新任务
	static void updateData();

	// 剧情独立的遍历（有太多特殊类型怪物）
	static void getMainLineDungeonAllObj();

	// 设置HOOK伤害
	static void setGeneralHook();
	static void setBossHook();

	static void test();
};