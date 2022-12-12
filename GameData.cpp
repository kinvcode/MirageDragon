#include "pch.h"
#include "GameData.h"

void GameData::dungeonInfoClean() 
{
	// 清空进图状态
	dungeon_info.entered = false;
	
	// 清空时间指针和门型指针
	dungeon_info.time_pointer = 0;
	dungeon_info.door_pointer = 0;

	// 清空BOSS房间坐标
	dungeon_info.boos_room.x = 0;
	dungeon_info.boos_room.y = 0;

	// 清空当前房间坐标
	dungeon_info.current_room.coordinate.x = 0;
	dungeon_info.current_room.coordinate.y = 0;

	// 清空当前房间对象列表、物品列表、怪物列表、是否存在时空旋涡
	dungeon_info.current_room.dungeon_object_list.clear();
	dungeon_info.current_room.item_list.clear();
	dungeon_info.current_room.monster_list.clear();
	dungeon_info.current_room.room_has_urgent = false;

	// 所有房间信息清空
	dungeon_info.rooms.clear();
}

void GameData::townInfoClean() 
{
	town_info.entered = false;
}

GameData GAME;