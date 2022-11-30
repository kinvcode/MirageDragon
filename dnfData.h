#pragma once

// 获取用户指针
__int64 getUserPointer(__int64 emptyAddress);

// 获取副本名称
wstring getMapName();

// 获取副本编号
int getMapNumber();

// 读取坐标
COORDINATE readCoordinate(__int64 address);

// 更新人物坐标
void updateUserCoor();

// 更新当前副本内所有对象信息
void getDungeonAllObj();

void gatherAtUser(COORDINATE user, DUNGEONOBJ item);

// 初始化副本信息
void initDungeonInfo();

// 更新当前房间
void updateCurrentRoom();

// 判断是否通关
bool judgeClearance();

// 判断是否开门
bool judgeDoorOpen();

// 判断是否是boss房间
bool judgeIsBossRoom();