#pragma once
#include <winnt.h>

#ifndef COMMON_H
#define COMMON_H

// 进程ID
extern DWORD PID;

// 按键句柄
extern HANDLE MSDK_HANDLE;

// 游戏状态
extern int game_status;

// 窗口置顶状态
extern bool window_top;

// 游戏是否运行
extern bool is_running;

// 自动开关
extern bool is_auto_play;

// 自动类型
extern int auto_play_type;

#endif
