#pragma once
#include <winnt.h>
#include <vector>

using namespace std;

#ifndef COMMON_H
#define COMMON_H

//////////////////////////////// [ 数 据 结 构 ] /////////////////////////////////

struct COORDINATE {
	int x;
	int y;
	int z;
};

// 地图结构
struct DUNGEONMAP
{
	char* map_name;
	int map_number;
	vector<int> aisle;
	COORDINATE begin;
	COORDINATE end;
	int width;
	int height;
	vector<COORDINATE> way;
	int fatigue;
	int aisle_num;
	__int64 tmp;
};

// 地图通道结构
struct AISLEDATA
{
	COORDINATE coor;
	bool left;
	bool right;
	bool top;
	bool bottom;
	int aisle;
	int bg;
};

// 地图节点结构
struct MAPNODE
{
	DWORD f;
	DWORD g;
	DWORD h;
	COORDINATE current;
	COORDINATE final;
};

// 图内对象数据结构
struct DUNGEONOBJ
{
	__int64 p;
	int type;
	int camp;
	int code;
	int blood;
	COORDINATE coor;
};


// 功能开关结构
struct FUNCTIONSWITCH{
	bool score;
	bool three_speed;
	bool gather_monster;
	bool gather_items;
	bool cool_down;
	bool hook_damage;
	bool hidden_user;
};


//////////////////////////////// [ 全 局 变 量 ] /////////////////////////////////////////

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

// 人物穿透状态
extern int penetrate_status;

// 图内怪物列表
extern vector<DUNGEONOBJ> monster_list;

// 图内物品列表
extern vector<DUNGEONOBJ> item_list;

// 图内所有对象列表
extern vector<DUNGEONOBJ> dungeon_object_list;

// 功能开关
extern FUNCTIONSWITCH function_switch;

#endif
