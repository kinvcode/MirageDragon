#pragma once
//#include <winnt.h>
//#include <vector>
#include <Windows.h>
#include "dataStruct.h"

using namespace std;

class GameData
{
public:
	FUNCTIONSWITCH function_switch = { true,true,true,true,true,true,true }; // 功能列表

	int game_status = 0; // 当前游戏状态：0选择角色；1城镇；2选择副本；3副本中

	int penetrate_status;  // 人物穿透状态：0未穿透；1已穿透

	bool use_pass_room_call = false; // 是否强制使用过图CALL

	int play_user_index = 0; // 当前角色索引

	int autoMapNumber = 0; // 副本编号

	DUNGEONINFO dungeon_info; // 当前副本信息

	TOWNINFO town_info; // 当前城镇信息

	void dungeonInfoClean(); // 清除副本信息

	void townInfoClean(); // 清除城镇信息
};

extern GameData GAME;