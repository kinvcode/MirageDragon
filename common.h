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

struct ROOMCOORDINATE 
{
	int x;
	int y;
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

// 当前房间结构
struct CURRENTROOM {
	vector<DUNGEONOBJ> dungeon_object_list;	// 图内对象列表
	vector<DUNGEONOBJ> monster_list;		// 怪物列表
	vector<DUNGEONOBJ> item_list;			// 图内物品列表
	ROOMCOORDINATE coordinate;				// 房间位置
	bool room_has_urgent = false;			// 是否存在时空旋涡
};

// 副本结构
struct DUNGEONINFO
{
	int next_room = 0; // 下个房间的方向
	ROOMCOORDINATE boos_room; // BOSS房间位置
	CURRENTROOM current_room; // 当前房间
	// 功能是否已开启
};

#endif
