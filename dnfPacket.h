#pragma once

// 缓冲CALL
void bufferCall(int param);

// 加密CALL
void encryptCall(__int64 param, int length);

// 发包CALL
void sendPacketCall();

// 组包翻牌
void turnOverCard(int x, int y);

// 组包移动
void moveOfTown(int world, int area, int x, int y);

// 组包角色列表
void roleList();

// 组包选择角色
void selectRole(int index);

// 组包进图，难度：0普通 1冒险 2勇士 3王者 4噩梦
void entryDungeon(int id, int difficulty, int abyss, int practice);

// 组包选图
void selectMap();

// 组包过图
void passRoomByPacket(int x, int y);

// 组包分解
void breakDown(int pos);

// 组包整理
/**
 * 背包区域：1装备2消耗品3材料4人物10副职业
 * 存储类型：0背包2个人仓库12账号仓库
 * 排序类型：0栏位排序1品级排序2等级排序3部位排序
 */
void sortItems(int package_type,int store_type,int sort_type);