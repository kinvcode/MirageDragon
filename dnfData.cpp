#include "pch.h"
#include "dnfData.h"
#include "dnfBase.h"
#include "memory.h"
#include "dnfCALL.h"
#include "dnfUser.h"
#include "common.h"
#include "constant.h"
#include "baseAddress.h"
#include "GameData.h"

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

void updateUserCoor()
{
	GLOBAL.dungeon_info.user_coordinate = readCoordinate(readLong(ADDR.x64("C_USER_ADDRESS")));
}

// 遍历图内对象信息
void getDungeonAllObj()
{
	CURRENTROOM* p_current_room = &GLOBAL.dungeon_info.current_room;
	p_current_room->dungeon_object_list.clear();
	p_current_room->item_list.clear();
	p_current_room->monster_list.clear();

	if (GLOBAL.game_status != 3)
	{
		return;
	}

	__int64 map_base = readLong(readLong(readLong(ADDR.x64("C_USER_ADDRESS")) + ADDR.x64("C_MAP_OFFSET")) + 16);
	__int64 head_address = readLong(map_base + ADDR.x64("C_MAP_HEAD"));
	__int64 end_address = readLong(map_base + ADDR.x64("C_MAP_END"));

	if (head_address == 0 || end_address == 0) {
		return;
	}

	int object_quantity = (int)(end_address - head_address) / 32;
	if (object_quantity == 0 || object_quantity > 1000) {
		return;
	}

	// 怪物列表中血量最大值
	int monster_max_blood = 0;

	for (__int64 i = 1; i <= object_quantity; i++)
	{
		__int64 monster_address = readLong(readLong(head_address + i * 32) + 16) - 32;
		int monster_camp = readInt(monster_address + ADDR.x64("C_CAMP_OFFSET"));
		int monster_type = readInt(monster_address + ADDR.x64("C_TYPE_OFFSET"));
		int monster_code = readInt(monster_address + ADDR.x64("C_CODE_OFFSET"));
		int monster_blood = readInt(monster_address + ADDR.x64("C_MONSTER_BLOOD"));
		COORDINATE monster_coor = readCoordinate(monster_address);

		// 图内对象结构体
		DUNGEONOBJ d_object;
		d_object.p = monster_address;
		d_object.blood = monster_blood;
		d_object.type = monster_type;
		d_object.camp = monster_camp;
		d_object.code = monster_code;
		d_object.coor = monster_coor;

		// 物品
		if (d_object.type == 289 && d_object.coor.z == 0)
		{
			p_current_room->item_list.push_back(d_object);

			// 聚物处理（应该放到开门后处理）
			if (GLOBAL.function_switch.gather_monster)
			{
				gatherAtUser(GLOBAL.dungeon_info.user_coordinate, d_object);
			}
		}
		// 敌对怪物和人偶类型怪物
		else if (d_object.type == 273 || d_object.type == 529)
		{
			if (d_object.camp == 100 && d_object.blood != 0)
			{
				p_current_room->monster_list.push_back(d_object);
				
				// 最大血量值保存 
				if (d_object.blood > monster_max_blood)
				{
					monster_max_blood = d_object.blood;
				}

				// 聚怪处理
				if (!GLOBAL.function_switch.gather_items)
				{
					// 恩山的机关枪不可移动
					if (p_current_room->monster_list[i].code != 109013676)
					{
						gatherAtUser(GLOBAL.dungeon_info.user_coordinate, d_object);
					}
				}
			}
		}

		// 如果存在时空旋涡
		if (d_object.camp == 200 && d_object.type == 33 && d_object.code == 490019076)
		{
			p_current_room->room_has_urgent = true;
		}
		//handleEvents();
	}

	// 调用冒泡算法对怪物数组进行排序
	int i, j, len;
	DUNGEONOBJ temp;
	len = (int)p_current_room->monster_list.size() - 1;
	for (i = len; i > 0; i--) {
		for (j = 0; j < i; j++) {
			if (p_current_room->monster_list[j].coor.x > p_current_room->monster_list[j + 1].coor.x) {
				temp = p_current_room->monster_list[j + 1];
				p_current_room->monster_list[j + 1] = p_current_room->monster_list[j];
				p_current_room->monster_list[j] = temp;
			}
		}
	}
}

void gatherAtUser(COORDINATE user, DUNGEONOBJ item)
{
	if (abs(item.coor.x - user.x) < 20 && abs(item.coor.y - user.y) < 20)
	{
		return;
	}
	writeFloat(readLong(item.p + ADDR.x64("C_OBJECT_COORDINATE")) + 32, (float)user.x);
	writeFloat(readLong(item.p + ADDR.x64("C_OBJECT_COORDINATE")) + 36, (float)user.y);
}

void initDungeonInfo()
{
	// 更新时间指针和门型指针（每次进图会变化）
	GLOBAL.dungeon_info.time_pointer = readLong(readLong(ADDR.x64("C_ROOM_NUMBER")) + ADDR.x64("C_TIME_ADDRESS"));
	GLOBAL.dungeon_info.door_pointer = readLong(GLOBAL.dungeon_info.time_pointer + ADDR.x64("C_DOOR_TYPE_OFFSET"));

	// 获取起始房间位置
	ROOMCOORDINATE start_coor;
	start_coor.x = readInt(GLOBAL.dungeon_info.time_pointer + ADDR.x64("C_BEGIN_ROOM_X"));
	start_coor.y = readInt(GLOBAL.dungeon_info.time_pointer + ADDR.x64("C_BEGIN_ROOM_Y"));
	GLOBAL.dungeon_info.start_room = start_coor;

	// 获取BOSS房间位置
	ROOMCOORDINATE boss_coor;
	boss_coor.x = (int)decrypt(GLOBAL.dungeon_info.door_pointer + ADDR.x64("C_BOSS_ROOM_X"));
	boss_coor.y = (int)decrypt(GLOBAL.dungeon_info.door_pointer + ADDR.x64("C_BOSS_ROOM_Y"));
	GLOBAL.dungeon_info.boos_room = boss_coor;
}

// 获取当前房间位置
void updateCurrentRoom()
{
	ROOMCOORDINATE coor;

	__int64 roomData = readLong(readLong(readLong(ADDR.x64("C_ROOM_NUMBER")) + ADDR.x64("C_TIME_ADDRESS")) + ADDR.x64("C_DOOR_TYPE_OFFSET"));
	coor.x = readInt(roomData + ADDR.x64("C_CURRENT_ROOM_X"));
	coor.y = readInt(roomData + ADDR.x64("C_CURRENT_ROOM_Y"));

	GLOBAL.dungeon_info.current_room.coordinate = coor;
}

// 当前是否通关
bool judgeClearance()
{
	int result = readInt(GLOBAL.dungeon_info.door_pointer + ADDR.x64("C_BONFIRE_JUDGE"));

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
	// 每进入一个房间需要重新读取
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
	updateCurrentRoom();
	if (GLOBAL.dungeon_info.current_room.coordinate.x == GLOBAL.dungeon_info.boos_room.x && GLOBAL.dungeon_info.current_room.coordinate.y == GLOBAL.dungeon_info.boos_room.y)
	{
		return true;
	}
	else {
		return false;
	}
}