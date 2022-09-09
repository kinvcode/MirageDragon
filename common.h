#pragma once
#include <winnt.h>
#include <vector>

using namespace std;

#ifndef COMMON_H
#define COMMON_H

//////////////////////////////// [ �� �� �� �� ] /////////////////////////////////

struct COORDINATE {
	int x;
	int y;
	int z;
};

// ��ͼ�ṹ
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

// ��ͼͨ���ṹ
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

// ��ͼ�ڵ�ṹ
struct MAPNODE
{
	DWORD f;
	DWORD g;
	DWORD h;
	COORDINATE current;
	COORDINATE final;
};

// ͼ�ڶ������ݽṹ
struct DUNGEONOBJ
{
	__int64 p;
	int type;
	int camp;
	int code;
	int blood;
	COORDINATE coor;
};


// ���ܿ��ؽṹ
struct FUNCTIONSWITCH{
	bool score;
	bool three_speed;
	bool gather_monster;
	bool gather_items;
	bool cool_down;
	bool hook_damage;
	bool hidden_user;
};


//////////////////////////////// [ ȫ �� �� �� ] /////////////////////////////////////////

// ����ID
extern DWORD PID;

// �������
extern HANDLE MSDK_HANDLE;

// ��Ϸ״̬
extern int game_status;

// �����ö�״̬
extern bool window_top;

// ��Ϸ�Ƿ�����
extern bool is_running;

// �Զ�����
extern bool is_auto_play;

// �Զ�����
extern int auto_play_type;

// ���ﴩ͸״̬
extern int penetrate_status;

// ͼ�ڹ����б�
extern vector<DUNGEONOBJ> monster_list;

// ͼ����Ʒ�б�
extern vector<DUNGEONOBJ> item_list;

// ͼ�����ж����б�
extern vector<DUNGEONOBJ> dungeon_object_list;

// ���ܿ���
extern FUNCTIONSWITCH function_switch;

#endif
