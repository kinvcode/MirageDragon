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

struct ROOMCOORDINATE 
{
	int x;
	int y;
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

// ��ǰ����ṹ
struct CURRENTROOM {
	vector<DUNGEONOBJ> dungeon_object_list;	// ͼ�ڶ����б�
	vector<DUNGEONOBJ> monster_list;		// �����б�
	vector<DUNGEONOBJ> item_list;			// ͼ����Ʒ�б�
	ROOMCOORDINATE coordinate;				// ����λ��
	bool room_has_urgent = false;			// �Ƿ����ʱ������
};

// �����ṹ
struct DUNGEONINFO
{
	int next_room = 0; // �¸�����ķ���
	ROOMCOORDINATE boos_room; // BOSS����λ��
	CURRENTROOM current_room; // ��ǰ����
	// �����Ƿ��ѿ���
};

#endif
