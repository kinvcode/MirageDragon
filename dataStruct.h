#pragma once
#include <winnt.h>
#include <vector>
#include "json.h"
#include "dataConvert.h"

using namespace std;

#ifndef COMMON_H
#define COMMON_H

//////////////////////////////// [ 数 据 结 构 ] /////////////////////////////////

// 坐标
struct COORDINATE {
	int x;
	int y;
	int z;
};

// 房间位置（以左上角的房间为起点）
struct ROOMCOORDINATE
{
	int x;
	int y;
};

// 地图结构
struct DUNGEONMAP
{
	char* map_name; // 地图名称
	int map_number; // 地图编号
	vector<int> aisle; // 通道容器（理论房间容器）
	COORDINATE begin; // 起始房间（当前房间？入图房间？）
	COORDINATE end; // 结束房间(BOSS房间)
	int width; // 地图X轴房间数
	int height; // 地图Y轴房间数
	vector<COORDINATE> way; // 路径
	int fatigue; // 消耗疲劳
	int aisle_num; // 通道数量（理论房间数量 = X*Y）
	__int64 tmp; // 临时变量（通道偏移）
};

// 地图通道结构
struct AISLEDATA
{
	COORDINATE coor; // 房间坐标
	bool left; // 
	bool right;
	bool top;
	bool bottom;
	int aisle; // 通道数据
	int bg;
};

// 地图节点结构
struct MAPNODE
{
	DWORD f;
	DWORD g;
	DWORD h;
	COORDINATE current; // 该节点的坐标
	COORDINATE final;	// 该节点的房间坐标
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
struct FUNCTIONSWITCH {
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
	bool entered;				// 是否已进入房间
	int next_room;				// 下个房间的方向
	__int64 time_pointer;		// 时间指针
	__int64 door_pointer;		// 门型指针
	COORDINATE user_coordinate; // 人物坐标
	ROOMCOORDINATE boos_room;	// BOSS房间位置
	ROOMCOORDINATE start_room;	// 起始房间位置
	CURRENTROOM current_room;	// 当前房间
};

// 角色列表中的角色信息
struct ROLEINFO
{
	CString name;		// 角色名称
	int account;		// QQ账号
	int character;		// 角色职业
	int advancement;	// 角色转职
	int awakening;		// 觉醒次数
	int level;			// 角色等级
	int prestige;		// 角色名望
	int position;		// 角色位置
};

// 城镇坐标结构
struct TOWNCOOR
{
	CString name;	// 坐标名称
	int word;		// 大区域编号
	int area;		// 小区域编号
	int x;			// X坐标
	int y;			// Y坐标
};

// JSON序列器(转JSON) - 角色信息
void to_json(json& j, const ROLEINFO& p);

// JSON序列器(转JSON) - 城镇坐标
void to_json(json& j, const TOWNCOOR& p);

// JSON序列器(转TOWNCOOR) - 城镇坐标
void from_json(const json& j, TOWNCOOR& p);

#endif
