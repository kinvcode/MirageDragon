#pragma once

// 跑到目标位置
void runToDestination(int x, int y, bool is_room, int target_range);

// 自动进入下个房间
void autoNextRoom();

// 进入指定方向的房间
void runToNextRoom(int direction);

void firstRoomFunctions();

void closeDungeonFunctions();