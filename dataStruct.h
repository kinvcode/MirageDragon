#pragma once
#include <winnt.h>
#include <vector>
#include "json.h"
#include "dataConvert.h"

using namespace std;

#ifndef COMMON_H
#define COMMON_H

//////////////////////////////// [ 数 据 结 构 ] /////////////////////////////////

// 三维坐标
struct COORDINATE {
	int x;
	int y;
	int z;
};

// 二维坐标
struct ROOMCOOR
{
	int x;
	int y;
};

// BFS队列
struct queueNode
{
	ROOMCOOR coor;
};

// 房间顺图方向
struct ROOMDIRECT {
	ROOMCOOR coor; // 房间坐标
	int direct; // 该房间顺图方向
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
	ROOMCOOR coordinate;				// 房间位置
	bool room_has_urgent = false;			// 是否存在时空旋涡
};

// 用于房间集合的结构
struct ROOMINFO {
	char direct = -1; // 默认-1，无效的方向
};

// 副本结构
struct DUNGEONINFO
{
	bool entered;				// 是否已进入图内
	__int64 time_pointer;		// 时间指针
	__int64 door_pointer;		// 门型指针
	COORDINATE user_coordinate; // 人物坐标
	ROOMCOOR boos_room;	// BOSS房间位置
	CURRENTROOM current_room;	// 当前房间结构
	vector<vector<ROOMINFO>> rooms; // 所有房间的信息
};

// 城镇结构
struct TOWNINFO
{
	bool entered; // 是否已进入城镇
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
