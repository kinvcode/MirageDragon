#pragma once

// 跑到目标位置
void runToDestination(int x, int y, bool is_room, int target_range);

// 自动进入下个房间
void autoNextRoom();

// 进入指定方向的房间
void runToNextRoom(int direction);

// 首图功能
void firstRoomFunctions();

// 关闭图内所有功能
void closeDungeonFunctions();

// 