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

// 判断账号是否制裁
bool judgeIsSanction();

// 读账号基址
int getQQAccount();

// 判断脚底是否有物品
bool itemUnderFooter();

// 判断是否对话中（弹窗或对话）
bool isDialogue();

// 判断是否翻牌
bool judgeAwarded();

// 获取通关商店类型:0未打开;1000大妈;1003加百利
int getClearanceShop();

// 是否存在面板
bool hasPanel();

// 获取背包负重
int getBackpackLoad();

// 获取装备物品栏
void getPackageOfEq();