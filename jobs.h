#pragma once

// 定义房间位置结构
struct ROOMCOOR
{
	__int64 x;
	__int64 y;
};

// 游戏数据更新线程
UINT updateDataThread(LPVOID pParam);

// 刷图线程
UINT playGameThead(LPVOID pParam);

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