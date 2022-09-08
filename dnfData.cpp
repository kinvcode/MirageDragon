#include "pch.h"
#include "dnfData.h"
#include "dnfBase.h"
#include "memory.h"

vector<DUNGEONOBJ> monster_list;
vector<DUNGEONOBJ> item_list;
vector<DUNGEONOBJ> dungeon_object_list;

// 获取角色指针
__int64 getUserPointer(__int64 emptyAddress)
{
	vector<byte>asm_code;

	asm_code = makeByteArray({ 72, 131, 236,100 });
	asm_code = asm_code + makeByteArray({ 72,184 }) + longToBytes(C_USER_CALL);
	asm_code = asm_code + makeByteArray({ 255,208 });
	asm_code = asm_code + makeByteArray({ 72,163 }) + longToBytes(emptyAddress);
	asm_code = asm_code + makeByteArray({ 72,131,196,100 });
	memoryAssambly(asm_code);

	return readLong(emptyAddress);
}

// 获取地图名称
wstring getMapName()
{
	vector<byte> data = readByteArray(readLong(readLong(readLong(readLong(C_ROOM_NUMBER) + C_TIME_ADDRESS) + C_DOOR_TYPE_OFFSET) + C_MAP_NAME) + 0, 100);
	return bytesToWstring(data);
}

// 获取地图编号
int getMapNumber()
{
	return readInt(C_MAP_NUMBER);
}

COORDINATE readCoordinate(__int64 address)
{
	__int64 pointer;
	COORDINATE coor;

	int type = readInt(address + C_TYPE_OFFSET);

	if (type == 0x111)
	{
		pointer = readLong(address + C_READ_COORDINATE);
		coor.x = (int)readFloat(pointer + 0);
		coor.y = (int)readFloat(pointer + 4);
		coor.z = (int)readFloat(pointer + 8);
		return coor;
	}
	else
	{
		pointer = readLong(address + C_OBJECT_COORDINATE);
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

	__int64 head_address = readLong(readLong(readLong(readLong(C_USER) + C_MAP_OFFSET) + 16) + C_MAP_HEAD);
	__int64 end_address = readLong(readLong(readLong(readLong(C_USER) + C_MAP_OFFSET) + 16) + C_MAP_END);

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
		int monster_camp = readInt(monster_address + C_CAMP_OFFSET);
		int monster_type = readInt(monster_address + C_TYPE_OFFSET);
		int monster_code = readInt(monster_address + C_CODE_OFFSET);
		int monster_blood = readInt(monster_address + C_MONSTER_BLOOD);
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
				if (dungeon_object_list[i].coor.x != 0 && dungeon_object_list[i].coor.y != 0)
				{
					monster_list.push_back(dungeon_object_list[i]);
				}
			}
		}
	}
}