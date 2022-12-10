#pragma once
//#include <winnt.h>
//#include <vector>
#include <Windows.h>
#include "dataStruct.h"

using namespace std;

class GameData
{
public:

	DWORD PID = 0; // 游戏进程ID

	HANDLE dnf_handle = NULL; // 游戏进程句柄
	
	HWND dnf_window = NULL; // 游戏窗口句柄

	RECT  dnf_rect; // 游戏窗口位置

	//HDC dnf_hDc = NULL; // 游戏窗口DC

	bool window_top = false; // 游戏窗口是否置顶状态
	
	bool is_running = false; // 游戏是否正在运行中

	HANDLE MSDK_HANDLE = NULL; // 易键鼠句柄

	CString cur_dir; // 当前工作目录

	CString log_file; // 日志路径

	CString screenshot_dir; // 截图存放目录

	string screenshot_file_s; // 截图文件(string)

	CString screenshot_file_c; // 截图文件(CString)

	///////////////////////////////////////////////////////////////////////////

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

	TOWNINFO town_info; // 当前城镇信息

	void dungeonInfoClean(); // 清除副本信息

	void townInfoClean(); // 清除城镇信息

	////////////////////////////////////////////////////////////////////////////////////

	// 上次更新角色列表时间
	time_t last_update_roles = 0;
};

extern GameData GLOBAL;