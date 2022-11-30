#include "pch.h"
#include "dnfData.h"
#include "dnfBase.h"
#include "memory.h"
#include "dnfCALL.h"
#include "dnfUser.h"
#include "common.h"
#include "constant.h"
#include "baseAddress.h"

vector<DUNGEONOBJ> monster_list;
vector<DUNGEONOBJ> item_list;
vector<DUNGEONOBJ> dungeon_object_list;

// 获取角色指针
__int64 getUserPointer(__int64 emptyAddress)
{
	vector<byte>asm_code;

	asm_code = makeByteArray({ 72, 131, 236,100 });
	asm_code = asm_code + makeByteArray({ 72,184 }) + longToBytes(ADDR.x64("C_USER_CALL"));
	asm_code = asm_code + makeByteArray({ 255,208 });
	asm_code = asm_code + makeByteArray({ 72,163 }) + longToBytes(emptyAddress);
	asm_code = asm_code + makeByteArray({ 72,131,196,100 });
	memoryAssambly(asm_code);

	return readLong(emptyAddress);
}

// 获取地图名称
wstring getMapName()
{
	vector<byte> data = readByteArray(readLong(readLong(readLong(readLong(ADDR.x64("C_ROOM_NUMBER")) + ADDR.x64("C_TIME_ADDRESS")) + ADDR.x64("C_DOOR_TYPE_OFFSET")) + ADDR.x64("C_MAP_NAME")) + 0, 100);
	return bytesToWstring(data);
}

// 获取地图编号
int getMapNumber()
{
	return readInt(ADDR.x64("C_MAP_NUMBER"));
}

// 读取对象坐标
COORDINATE readCoordinate(__int64 address)
{
	__int64 pointer;
	COORDINATE coor;

	int type = readInt(address + ADDR.x64("C_TYPE_OFFSET"));

	if (type == 0x111)
	{
		pointer = readLong(address + ADDR.x64("C_READ_COORDINATE"));
		coor.x = (int)readFloat(pointer + 0);
		coor.y = (int)readFloat(pointer + 4);
		coor.z = (int)readFloat(pointer + 8);
		return coor;
	}
	else
	{
		pointer = readLong(address + ADDR.x64("C_OBJECT_COORDINATE"));
		coor.x = (int)readFloat(pointer + 32);
		coor.y = (int)readFloat(pointer + 36);
		coor.z = (int)readFloat(pointer + 40);
		return coor;
	}
}

// 遍历图内对象信息
vector<DUNGEONOBJ> getDungeonAllObj()
{
	vector<DUNGEONOBJ> object_list;

	if (game_status != 3)
	{
		return object_list;
	}

	__int64 head_address = readLong(readLong(readLong(readLong(ADDR.x64("C_USER_ADDRESS")) + ADDR.x64("C_MAP_OFFSET")) + 16) + ADDR.x64("C_MAP_HEAD"));
	__int64 end_address = readLong(readLong(readLong(readLong(ADDR.x64("C_USER_ADDRESS")) + ADDR.x64("C_MAP_OFFSET")) + 16) + ADDR.x64("C_MAP_END"));

	if (head_address == 0 || end_address == 0) {
		return object_list;
	}

	int object_quantity = (int)(end_address - head_address) / 32;
	if (object_quantity == 0) {
		return object_list;
	}

	for (__int64 i = 1; i <= object_quantity; i++)
	{
		__int64 monster_address = readLong(readLong(head_address + i * 32) + 16) - 32;
		int monster_camp = readInt(monster_address + ADDR.x64("C_CAMP_OFFSET"));
		int monster_type = readInt(monster_address + ADDR.x64("C_TYPE_OFFSET"));
		int monster_code = readInt(monster_address + ADDR.x64("C_CODE_OFFSET"));
		int monster_blood = readInt(monster_address + ADDR.x64("C_MONSTER_BLOOD"));
		COORDINATE monster_coor = readCoordinate(monster_address);

		// 对象
		DUNGEONOBJ d_object;
		d_object.p = monster_address;
		d_object.blood = monster_blood;
		d_object.type = monster_type;
		d_object.camp = monster_camp;
		d_object.code = monster_code;
		d_object.coor = monster_coor;
		object_list.push_back(d_object);
		handleEvents();
	}

	return object_list;
}

// 更新怪物和物品列表
void getMonsterAndItems()
{
	dungeon_object_list.clear();
	item_list.clear();
	monster_list.clear();

	dungeon_object_list = getDungeonAllObj();

	__int64 object_number = dungeon_object_list.size();

	int monster_max_blood = 0;

	room_has_urgent = false;
	for (__int64 i = 0; i < object_number; i++)
	{
		// 物品
		if (dungeon_object_list[i].type == 289 && dungeon_object_list[i].coor.z == 0)
		{
			item_list.push_back(dungeon_object_list[i]);
		}

		// 怪物、建筑
		if (dungeon_object_list[i].type == 273 || dungeon_object_list[i].type == 529)
		{
			if (dungeon_object_list[i].camp != 0 && dungeon_object_list[i].blood != 0)
			{
				monster_list.push_back(dungeon_object_list[i]);
				if (dungeon_object_list[i].blood > monster_max_blood)
				{
					monster_max_blood = dungeon_object_list[i].blood;
				}
			}
		}

		// 时空旋涡
		if (dungeon_object_list[i].camp == 200 && dungeon_object_list[i].type == 33 && dungeon_object_list[i].code == 490019076)
		{
			room_has_urgent = true;
		}
	}


	// 对怪物容器进行排序(冒泡)
	int i, j, len;
	DUNGEONOBJ temp;
	len = (int)monster_list.size();
	for (i = 0; i < len - 1; i++) {
		for (j = 0; j < len - 1 - i; j++) {
			if (monster_list[j].coor.x > monster_list[j + 1].coor.x) {
				temp = monster_list[j];
				monster_list[j] = monster_list[j + 1];
				monster_list[j + 1] = temp;
			}
		}
	}
}

// 获取BOSS房间位置
COORDINATE getBossRoom()
{
	COORDINATE coor;
	__int64 roomData = readLong(readLong(readLong(ADDR.x64("C_ROOM_NUMBER")) + ADDR.x64("C_TIME_ADDRESS")) + ADDR.x64("C_DOOR_TYPE_OFFSET"));
	coor.x = (int)decrypt(roomData + ADDR.x64("C_BOSS_ROOM_X"));
	coor.y = (int)decrypt(roomData + ADDR.x64("C_BOSS_ROOM_Y"));
	return coor;
}

// 获取当前房间位置
COORDINATE getCurrentRoom()
{
	COORDINATE coor;

	__int64 roomData = readLong(readLong(readLong(ADDR.x64("C_ROOM_NUMBER")) + ADDR.x64("C_TIME_ADDRESS")) + ADDR.x64("C_DOOR_TYPE_OFFSET"));
	coor.x = readInt(roomData + ADDR.x64("C_CURRENT_ROOM_X"));
	coor.y = readInt(roomData + ADDR.x64("C_CURRENT_ROOM_Y"));
	return coor;
}

// 当前是否通关
bool judgeClearance()
{
	__int64 roomData = readLong(readLong(readLong(ADDR.x64("C_ROOM_NUMBER")) + ADDR.x64("C_TIME_ADDRESS")) + ADDR.x64("C_DOOR_TYPE_OFFSET"));
	__int64 result = readInt(roomData + ADDR.x64("C_BONFIRE_JUDGE"));
	if (result == 2 || result == 0)
	{
		return true;
	}
	else {
		return false;
	}
}

// 当前是否开门
bool judgeDoorOpen()
{
	if (decrypt2(readLong(readLong(readLong(ADDR.x64("C_USER_ADDRESS")) + ADDR.x64("C_MAP_OFFSET")) + 16) + ADDR.x64("C_DOOR_OFFSET")) == 0) {
		return true;
	}
	else {
		return false;
	}
}

// 当前是否是BOOS房间
bool judgeIsBossRoom()
{
	COORDINATE cur, boss;
	cur = getCurrentRoom();
	boss = getBossRoom();
	if (cur.x == boss.x && cur.y == boss.y)
	{
		return true;
	}
	else {
		return false;
	}
}

// 聚集物品和怪物
void convergeMonsterAndItems()
{
	COORDINATE monster_coordinate;
	COORDINATE user_coordinate = readCoordinate(readLong(ADDR.x64("C_USER_ADDRESS")));

	if (!function_switch.gather_monster)
	{
		goto item;
	}
	__int64 length = monster_list.size();

	for (__int64 i = 0; i < length; i++)
	{
		monster_coordinate = readCoordinate(monster_list[i].p);
		user_coordinate = readCoordinate(readLong(ADDR.x64("C_USER_ADDRESS")));
		if (abs(monster_coordinate.x - user_coordinate.x) < 20 && abs(monster_coordinate.y - user_coordinate.y) < 20)
		{
			continue;
		}

		if (monster_list[i].code == 109013676)
		{
			// 恩山的机关枪不可移动，
			continue;
			//coorCall(monster_list[i].coor.x, monster_list[i].coor.y, 0);
		}
		else {
			// 移动对象
			writeFloat(readLong(monster_list[i].p + ADDR.x64("C_OBJECT_COORDINATE")) + 32, (float)user_coordinate.x);
			writeFloat(readLong(monster_list[i].p + ADDR.x64("C_OBJECT_COORDINATE")) + 36, (float)user_coordinate.y);
		}


		handleEvents();
	}

item:
	if (!function_switch.gather_items)
	{
		return;
	}
	length = item_list.size();

	for (__int64 i = 0; i < length; i++)
	{
		monster_coordinate = readCoordinate(item_list[i].p);
		user_coordinate = readCoordinate(readLong(ADDR.x64("C_USER_ADDRESS")));
		if (abs(monster_coordinate.x - user_coordinate.x) < 20 && abs(monster_coordinate.y - user_coordinate.y) < 20)
		{
			continue;
		}

		// 移动对象
		writeFloat(readLong(item_list[i].p + ADDR.x64("C_OBJECT_COORDINATE")) + 32, (float)user_coordinate.x);
		writeFloat(readLong(item_list[i].p + ADDR.x64("C_OBJECT_COORDINATE")) + 36, (float)user_coordinate.y);

		handleEvents();
	}
}