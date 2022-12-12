#pragma once

// 跑到目标位置
bool runToDestination(int x, int y, bool is_room, int target_range);

// 跑到门
bool runToDoor(int x,int y);

// 跑到怪物
bool runToMonter(int x, int y);

// 跑到物品
bool runToItem(int x, int y);

// 自动进入下个房间
void autoNextRoom();

// 进入指定方向的房间
void runToNextRoom(int direction);