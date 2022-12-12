#pragma once

class DungeonLogic
{
public:

	static queue<DUNGEONJOB> dg_list; // 副本列表

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

	// 初始化副本列表
	static void initDG();
};