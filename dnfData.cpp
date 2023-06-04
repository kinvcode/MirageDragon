#include "pch.h"
#include "dnfData.h"
#include "dnfBase.h"
#include "memory.h"
#include "dnfCALL.h"
#include "dnfUser.h"
#include "dataStruct.h"
#include "baseAddress.h"
#include "GameData.h"
#include "dnfMap.h"
#include "dnfPacket.h"
#include "log.h"

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

// 读取对象坐标
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

int readUserCoorX(__int64 coor_p)
{
	return (int)readFloat(coor_p + 0);
}

int readUserCoorY(__int64 coor_p)
{
	return (int)readFloat(coor_p + 4);
}

void updateUserCoor()
{
	GAME.dungeon_info.user_coordinate = readCoordinate(readLong(C_USER_ADDRESS));
}

// 遍历图内对象信息（怪物、物品）
void getDungeonAllObj()
{
	CURRENTROOM* p_current_room = &GAME.dungeon_info.current_room;
	p_current_room->dungeon_object_list.clear();
	p_current_room->item_list.clear();
	p_current_room->monster_list.clear();
	// 时空旋涡
	bool has_urgent = false;

	if (GAME.game_status != 3)
	{
		return;
	}

	__int64 map_base = readLong(readLong(readLong(GAME.USER_ADDRESS) + C_MAP_OFFSET) + 16);
	__int64 head_address = readLong(map_base + C_MAP_HEAD);
	__int64 end_address = readLong(map_base + C_MAP_END);

	if (head_address == 0 || end_address == 0) {
		return;
	}

	int object_quantity = (int)(end_address - head_address) / 24;
	if (object_quantity == 0 || object_quantity > 1000) {
		return;
	}

	// 怪物列表中血量最大值
	int monster_max_blood = 0;

	for (__int64 i = 1; i <= object_quantity; i++)
	{
		__int64 monster_address = readLong(readLong(head_address + i * 24) + 16) - 32;
		int monster_camp = readInt(monster_address + C_CAMP_OFFSET);
		int monster_type = readInt(monster_address + C_TYPE_OFFSET);
		int monster_code = readInt(monster_address + C_CODE_OFFSET);
		int monster_blood = readInt(monster_address + C_MONSTER_BLOOD);
		COORDINATE monster_coor = readCoordinate(monster_address);

		// 图内对象结构体
		DUNGEONOBJ d_object;
		d_object.p = monster_address;
		d_object.blood = monster_blood;
		d_object.type = monster_type;
		d_object.camp = monster_camp;
		d_object.code = monster_code;
		d_object.coor = monster_coor;
		p_current_room->dungeon_object_list.push_back(d_object);
		// 物品
		if (d_object.type == 289 && d_object.coor.z == 0)
		{
			d_object.code = readInt(readLong(monster_address + C_GROUND_ITEM) + C_CODE_OFFSET);
			// 过滤物品
			if (d_object.code != 0)
			{
				p_current_room->item_list.push_back(d_object);
			}
		}
		// 敌对怪物和人偶类型怪物
		else if (d_object.type == 273 || d_object.type == 529)
		{
			if (d_object.camp == 100 && d_object.blood != 0)
			{
				// 昆法特扫描、永恒之光研究所的门、时空旋涡未知怪物
				bool bug_exists = false;
				if (d_object.code == 109013645 || d_object.code == 109013995 || d_object.code == 407003631)
				{
					bug_exists = true;
				}

				if (bug_exists)
				{
					continue;
				}

				p_current_room->monster_list.push_back(d_object);


				// 最大血量值保存 
				if (d_object.blood > monster_max_blood)
				{
					// 最大血量过滤(破坏之国王冲锋状态)
					if (d_object.code != 109013674)
					{
						monster_max_blood = d_object.blood;
					}
				}

				// 聚怪处理
				if (GAME.function_switch.gather_monster)
				{
					// 恩山的机关枪不可移动
					if (d_object.code != 109013676)
					{
						gatherAtUser(GAME.dungeon_info.user_coordinate, d_object);
					}
				}
			}
		}

		// 如果存在时空旋涡
		if (d_object.camp == 200 && d_object.type == 33 && d_object.code == 490019076)
		{
			//Log.info(L"当前房间存在时空旋涡", true);
			has_urgent = true;
		}
		//handleEvents();
	}

	// 更新时空旋涡状态
	p_current_room->room_has_urgent = has_urgent;

	// 更新最大血量
	GAME.dungeon_info.monster_max_blood = monster_max_blood;

	// 当前房间最大怪物血量
	//ROOMCOOR coor = GAME.dungeon_info.current_room.coordinate;
	//if (GAME.dungeon_info.rooms[coor.x][coor.y].monster_max_blood == 0 && monster_max_blood != 0) {
	//	GAME.dungeon_info.rooms[coor.x][coor.y].monster_max_blood = monster_max_blood;
	//	// 设置HOOK伤害
	//	CString hook_msg;
	//	hook_msg.Format(L"当前房间怪物最大血量为：%d", monster_max_blood);
	//	Log.info(hook_msg, true);
	//}

	// 调用冒泡算法对怪物数组进行排序(根据该房间的入门方向)
	if (GAME.dungeon_info.user_coordinate.x < 500) {
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
	else {
		int i, j, len;
		DUNGEONOBJ temp;
		len = (int)p_current_room->monster_list.size() - 1;
		for (i = len; i > 0; i--) {
			for (j = 0; j < i; j++) {
				if (p_current_room->monster_list[j].coor.x < p_current_room->monster_list[j + 1].coor.x) {
					temp = p_current_room->monster_list[j + 1];
					p_current_room->monster_list[j + 1] = p_current_room->monster_list[j];
					p_current_room->monster_list[j] = temp;
				}
			}
		}
	}
}

// 更新当前房间内所有怪物
void getDungeonMonsters() 
{

}

// 更新当前房间内所有物品
void getDungeonGoods() 
{

}

void gatherAtUser(COORDINATE user, DUNGEONOBJ item)
{
	if (abs(item.coor.x - user.x) < 20 && abs(item.coor.y - user.y) < 20)
	{
		return;
	}
	writeFloat(readLong(item.p + C_OBJECT_COORDINATE) + 32, (float)user.x);
	writeFloat(readLong(item.p + C_OBJECT_COORDINATE) + 36, (float)user.y);
}

void initDungeonInfo()
{
	// 更新时间指针和门型指针（每次进图会变化/副本发生变化时）
	GAME.dungeon_info.time_pointer = readLong(readLong(C_ROOM_NUMBER) + C_TIME_ADDRESS);
	GAME.dungeon_info.door_pointer = readLong(GAME.dungeon_info.time_pointer + C_DOOR_TYPE_OFFSET);

	// 获取当前房间位置
	ROOMCOOR current_coor;
	current_coor.x = readInt(GAME.dungeon_info.time_pointer + C_BEGIN_ROOM_X);
	current_coor.y = readInt(GAME.dungeon_info.time_pointer + C_BEGIN_ROOM_Y);
	GAME.dungeon_info.current_room.coordinate = current_coor;

	// 获取BOSS房间位置
	ROOMCOOR boss_coor;
	boss_coor.x = (int)decrypt(GAME.dungeon_info.door_pointer + C_BOSS_ROOM_X);
	boss_coor.y = (int)decrypt(GAME.dungeon_info.door_pointer + C_BOSS_ROOM_Y);
	GAME.dungeon_info.boos_room = boss_coor;

	// 获取当前副本编号
	GAME.dungeon_info.map_code = getMapNumber();

	// 更新所有房间通向
	updateRooms();
}

// 获取当前房间位置
void updateCurrentRoom()
{
	ROOMCOOR coor;

	__int64 door_offset = GAME.dungeon_info.door_pointer;
	coor.x = readInt(door_offset + C_CURRENT_ROOM_X);
	coor.y = readInt(door_offset + C_CURRENT_ROOM_Y);

	GAME.dungeon_info.current_room.coordinate = coor;
}

// 当前是否通关
bool judgeClearance()
{
	int result = readInt(GAME.dungeon_info.door_pointer + C_BONFIRE_JUDGE);

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
	// 千万别修改!!!!别修改！！！
	// 会导致获取不到门的状态
	if (decrypt2(readLong(readLong(readLong(C_USER_ADDRESS) + C_MAP_OFFSET) + 16) + C_DOOR_OFFSET) == 0) {
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
	if (GAME.dungeon_info.current_room.coordinate.x == GAME.dungeon_info.boos_room.x && GAME.dungeon_info.current_room.coordinate.y == GAME.dungeon_info.boos_room.y)
	{
		return true;
	}
	else {
		return false;
	}
}

bool judgeIsSanction()
{
	int res = readInt(C_SANCTION);
	if (res) {
		return true;
	}
	return false;
}

int getQQAccount()
{
	return readInt(C_ACCOUNT);
}

bool itemUnderFooter()
{
	return (bool)readInt(readLong(C_USER_ADDRESS) + C_ITEM_UNDER_FOOTER);
}

bool isDialogue()
{
	if (readLong(C_DIALOGUE) == 1 || readLong(C_DIALOGUE_B) == 1 || readLong(C_DIALOGUE_ESC) == 1)
	{
		return true;
	}
	return false;
}

bool judgeAwarded()
{
	int status = readInt(readLong(C_CLEARANCE + 8) + C_BONFIRE_JUDGE);
	if (status == 0) {
		return true;
	}
	return false;
}

int getClearanceShop()
{
	return readInt(C_CLEARANCE_SHOP);
}

bool hasPanel()
{
	if (readInt(C_PANEL))
	{
		return true;
	}
	return false;
}

int getBackpackLoad()
{
	__int64 user_address = readLong(C_USER_ADDRESS);
	__int64 package = readLong(user_address + C_PACKAGE);
	int cur = (int)decrypt(package + C_CUR_WEIGHT);
	int max = (int)decrypt(user_address + C_MAX_WEIGHT);
	return cur * 100 / max;
}

void getPackageOfEq()
{
	__int64 check_p = 0;
	// 背包指针
	__int64 package = readLong(C_PACKAGE_ADDRESS);

	// 首地址
	__int64 head = readLong(C_PACKAGE_OFFSET + package);

	// 装备栏首地址
	__int64 eq_head = head + C_PACKAGE1_OF; // 装备栏偏移 

	for (int i = 0; i < 56; i++)
	{
		// 装备指针
		__int64 equipment_p = readLong(readLong(eq_head + i * 32) + 16);
		if (equipment_p != 0)
		{
			// 装备名称
			//CString equipment_name = readString(readLong(equipment_p + C_ITEM_NAME), 50);

			// 装备名望
			//int prestige = readInt(equipment_p + C_EQ_PRESTIGE);

			// 

			// 装备品级：0普通1高级2稀有
			int eq_grade = readInt(equipment_p + C_EQ_GRADE);
			if (eq_grade == 0 || eq_grade == 1 || eq_grade == 2)
			{
				if (equipment_p == check_p)
				{
					continue;
				}
				check_p = equipment_p;
				// 组包分解
				breakDown(i + 9);
				Sleep(30);
			}
		}
	}

	// 组包整理
	sortItems(1, 0, 0);

}

int getItemNum(int code, int type)
{
	switch (type)
	{
	case 1:
		type = C_PACKAGE1_OF;
		break;
	case 2:
		type = C_PACKAGE2_OF;
		break;
	case 3:
		type = C_PACKAGE3_OF;
		break;
	case 4:
		type = C_PACKAGE4_OF;
		break;
	case 5:
		type = C_PACKAGE5_OF;
		break;
	case 6:
		type = C_PACKAGE6_OF;
		break;
	case 7:
		type = C_PACKAGE7_OF;
		break;
	case 8:
		type = C_PACKAGE8_OF;
		break;
	default:
		type = C_PACKAGE1_OF;
		break;
	}
	__int64 address = readLong(readLong(C_PACKAGE_ADDRESS) + C_PACKAGE_OFFSET) + type;

	for (int i = 0; i < 56; i++)
	{
		__int64 item_p = readLong(readLong(address + i * 32) + 16);
		if (item_p != 0)
		{
			//CString equipment_name = readString(readLong(item_p + C_ITEM_NAME), 50);
			int item_code = readInt(item_p + C_ITEM_CODE);
			if (item_code == code) {
				int num = (int)decrypt(item_p + C_ITEM_NUM);
				return num;
			}
		}

	}
	return 0;
}

ROOMCOOR getUrgentCoor()
{
	ROOMCOOR coor = { 0,0 };
	
	int length = (int)GAME.dungeon_info.current_room.dungeon_object_list.size();
	for (int i = 0; i < length; i++) 
	{
		DUNGEONOBJ obj = GAME.dungeon_info.current_room.dungeon_object_list[i];

		if (obj.camp == 200 && obj.type == 33 && obj.code == 490019076)
		{
			coor.x = obj.coor.x;
			coor.y = obj.coor.y;
		}
	}
	return coor;
}

// 获取用户指针
__int64 getUserNewPointer()
{
	__int64 user_p = readLong(C_NEW_USER_ADDRESS);
	if (user_p > 0)
	{
		user_p = user_p - 32;
	}
	return user_p;
}