#pragma once
//#include <winnt.h>
//#include <vector>
#include <Windows.h>
#include "common.h"

using namespace std;

class GameData
{
public:

	DWORD PID = 0; // 游戏进程ID
	
	bool window_top = false; // 游戏窗口是否置顶状态
	
	bool is_running = false; // 游戏是否正在运行中

	HANDLE MSDK_HANDLE = NULL; // 易键鼠句柄

	bool is_auto_play = false; // 自动开关

	// 备注：自动行为应该使用任务队列
	int auto_play_type = 0; // 自动类型：1刷图；2剧情

	//////////////////////////////////////////////////////////////////////////

	int game_status = 0; // 当前游戏状态：0选择角色；1城镇；2选择副本；3副本中

	int penetrate_status;  // 人物穿透状态：0未穿透；1已穿透

	FUNCTIONSWITCH function_switch = { true,true,true,true,true,true,true }; // 功能列表

	bool use_pass_room_call = false; // 是否强制使用过图CALL

	int play_user_index = 0; // 当前角色索引

	int autoMapNumber = 0; // 副本编号

	DUNGEONINFO dungeon_info; // 当前副本信息
};

extern GameData GLOBAL;