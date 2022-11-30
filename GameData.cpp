#include "pch.h"
#include "GameData.h"

void GameData::dungeonInfoClean() 
{
	dungeon_info.entered = false;
	
	dungeon_info.time_pointer = 0;
	dungeon_info.door_pointer = 0;

	dungeon_info.boos_room.x = 0;
	dungeon_info.boos_room.y = 0;

	dungeon_info.current_room.coordinate.x = 0;
	dungeon_info.current_room.coordinate.y = 0;

	dungeon_info.current_room.dungeon_object_list.clear();
	dungeon_info.current_room.item_list.clear();
	dungeon_info.current_room.monster_list.clear();
	dungeon_info.current_room.room_has_urgent = false;

	dungeon_info.next_room = -1;
}

GameData GLOBAL;