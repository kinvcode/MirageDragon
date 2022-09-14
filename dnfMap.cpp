#include "pch.h"
#include "dnfMap.h"
#include "dnfBase.h"

__int64 passRoomData(int direction)
{
	__int64 empty_address = C_EMPTY_ADDRESS + 1450;
	__int64 room_data = readLong(readLong(readLong(C_ROOM_NUMBER) + C_TIME_ADDRESS) + C_PASS_ROOM_OFFSET);

	vector<byte>asm_code;
	asm_code = makeByteArray({ 72,129,236,0,1,0,0 });
	asm_code = asm_code + makeByteArray({ 72,185 }) + longToBytes(room_data);
	asm_code = asm_code + makeByteArray({ 186 }) + intToBytes(direction);
	asm_code = asm_code + makeByteArray({ 72,184 }) + longToBytes(C_COORDINATE_PASS_ROOM);
	asm_code = asm_code + makeByteArray({ 255,208 });
	asm_code = asm_code + makeByteArray({ 72,163 }) + longToBytes(empty_address);
	asm_code = asm_code + makeByteArray({ 72,129,196,0,1,0,0 });
	memoryAssambly(asm_code);
	return readLong(empty_address);
}

int judgeNextRoomDiretion(COORDINATE current, COORDINATE boss)
{
	int x, y;

	DUNGEONMAP map_data = mapData();
	if (map_data.fatigue < 1) {
		return -1;
	}
	x = map_data.way[0].x - map_data.way[1].x;
	y = map_data.way[0].y - map_data.way[1].y;


	if (x == 0)
	{
		if (y == 1)
		{
			return 2;// 上
		}
		else
		{
			return 3;// 下
		}
	}
	else if (y == 0)
	{
		if (x == 1)
		{
			return 0;// 左
		}
		else
		{
			return 1;// 右
		}
	}

	return -1;
}

DUNGEONMAP mapData()
{
	DUNGEONMAP map_data;

	__int64 room_data = readLong(readLong(readLong(C_ROOM_NUMBER) + C_TIME_ADDRESS) + C_DOOR_TYPE_OFFSET);
	__int64 room_index = decrypt2(room_data + C_MAP_CODE);

	map_data.width = readInt(readLong(room_data + C_WH_OFFSET) + room_index * 8 + 0);
	map_data.height = readInt(readLong(room_data + C_WH_OFFSET) + room_index * 8 + 4);
	map_data.tmp = readLong(readLong(room_data + C_AISLE_OFFSET) + 32 * room_index + 8);

	map_data.aisle_num = map_data.width * map_data.height;

	for (__int64 i = 0; i < map_data.aisle_num; i++)
	{
		map_data.aisle.insert(map_data.aisle.begin() + i, readInt(map_data.tmp + i * 4));
	}

	map_data.begin.x = readInt(readLong(readLong(C_ROOM_NUMBER) + C_TIME_ADDRESS) + C_BEGIN_ROOM_X) + 1;
	map_data.begin.y = readInt(readLong(readLong(C_ROOM_NUMBER) + C_TIME_ADDRESS) + C_BEGIN_ROOM_Y) + 1;
	map_data.end.x = (int)decrypt2(room_data + C_BOSS_ROOM_X) + 1;
	map_data.end.y = (int)decrypt2(room_data + C_BOSS_ROOM_Y) + 1;

	if (map_data.begin.x == map_data.end.x && map_data.begin.y == map_data.end.y) {
		return map_data;
	}

	map_data.fatigue = getWay(map_data.aisle, map_data.width, map_data.height, map_data.begin, map_data.end, map_data.way);
	return map_data;
}

int getWay(vector<int> aisle, int width, int height, COORDINATE begin, COORDINATE end, vector<COORDINATE>& way)
{
	COORDINATE begin_coor, end_coor;
	vector<vector<AISLEDATA>> map_tag, map_array;
	vector<COORDINATE> cross_way;

	if (begin.x == end.x && begin.y == end.y)
	{
		way.clear();
		way.resize(0);
		return 0;
	}

	createMap(width, height, aisle, map_array);
	showMap(map_array, width, height, map_tag);

	begin_coor.x = begin.x * 3 - 2;
	begin_coor.y = begin.y * 3 - 2;
	end_coor.x = end.x * 3 - 2;
	end_coor.y = end.y * 3 - 2;

	// 路径算法
	pathCalc(map_tag, begin_coor, end_coor, width * 3, height * 3, cross_way);

	// 整理坐标
	return arrangeCoor(cross_way, way);
}

void createMap(int width, int height, vector<int> aisle, vector<vector<AISLEDATA>>& map)
{
	map.clear();
	map.resize(width);
	for (int x = 0; x < width; x++)
	{
		map[x].resize(height);
	}

	int i = 0;
	int x, y;
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			map[x][y].coor.x = x;
			map[x][y].coor.y = y;
			map[x][y].aisle = aisle[i];
			map[x][y].left = judgeDirection(aisle[i], 0);
			map[x][y].right = judgeDirection(aisle[i], 1);
			map[x][y].top = judgeDirection(aisle[i], 2);
			map[x][y].bottom = judgeDirection(aisle[i], 3);
			map[x][y].bg = 0xFFFFFF;
			i++;
			if (map[x][y].aisle == 0)
			{
				map[x][y].bg = 0x000000;
			}
		}
	}
}

void showMap(vector<vector<AISLEDATA>> map, int width, int height, vector<vector<AISLEDATA>>& tag)
{
	tag.clear();
	tag.resize(width * 3);

	for (int x = 0; x < width * 3; x++)
	{
		tag[x].resize(height * 3);
	}

	int x, y;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			tag[(x + 1) * 3 - 2][(y + 1) * 3 - 2].bg = 0xFFFFFF;
			if (map[x][y].left == true)
			{
				tag[(x + 1) * 3 - 3][(y + 1) * 3 - 2].bg = 0xFFFFFF;
			}
			if (map[x][y].right == true)
			{
				tag[(x + 1) * 3 - 1][(y + 1) * 3 - 2].bg = 0xFFFFFF;
			}
			if (map[x][y].top == true)
			{
				tag[(x + 1) * 3 - 2][(y + 1) * 3 - 3].bg = 0xFFFFFF;
			}
			if (map[x][y].bottom == true)
			{
				tag[(x + 1) * 3 - 2][(y + 1) * 3 - 1].bg = 0xFFFFFF;
			}
		}
	}

}

bool judgeDirection(int aisle, int direction)
{
	unsigned char direction_arr[4];
	unsigned char direction_set[16][4] =
	{
		{ 0, 0, 0, 0 },
		{ 0, 1, 0, 0 },
		{ 0, 0, 1, 0 },
		{ 0, 1, 1, 0 },
		{ 1, 0, 0, 0 },
		{ 1, 1, 0, 0 },
		{ 1, 0, 1, 0 },
		{ 1, 1, 1, 0 },
		{ 0, 0, 0, 1 },
		{ 0, 1, 0, 1 },
		{ 0, 0, 1, 1 },
		{ 0, 1, 1, 1 },
		{ 1, 0, 0, 1 },
		{ 1, 1, 0, 1 },
		{ 1, 0, 1, 1 },
		{ 1, 1, 1, 1 }
	};
	std::vector<byte> arr;
	if (aisle >= 0 && aisle <= 15)
	{
		for (int i = 0; i < 4; i++)
		{
			direction_arr[i] = direction_set[aisle][i];
		}
	}
	else {
		for (int i = 0; i < 4; i++)
		{
			direction_arr[i] = 0;
		}
	}
	if (direction_arr[direction] == 1) {
		return true;
	}
	return false;
}

void pathCalc(vector<vector<AISLEDATA>> map_tag, COORDINATE begin, COORDINATE end, int width, int height, vector<COORDINATE>& cross_way)
{
	bool exists_open_list, exists_close_list;
	COORDINATE not_check_coor;
	MAPNODE not_check_node, tmp_node;
	vector<MAPNODE> open_list, close_list;
	int min_number = 0;
	DWORD min_f;
	int estimate_g;
	int x, y;

	tmp_node.current.x = begin.x;
	tmp_node.current.y = begin.y;
	map_tag[begin.x][begin.y].bg = 0x00FF00;
	map_tag[end.x][end.y].bg = 0x0000FF;
	open_list.insert(open_list.begin(), tmp_node);

	do
	{
		min_f = 0;
		for (y = 0; y < open_list.size(); y++)
		{
			if (min_f == 0)
			{
				min_f = open_list[0].f;
				min_number = y;
			}
			if (open_list[y].f < min_f)
			{
				min_f = open_list[y].f;
				min_number = y;
			}
		}
		tmp_node = open_list[min_number];
		open_list.erase(open_list.begin() + min_number);
		close_list.insert(close_list.begin(), tmp_node);

		if (tmp_node.current.x != begin.x || tmp_node.current.y != begin.y)
		{
			if (tmp_node.current.x != end.x || tmp_node.current.y != end.y)
			{
				map_tag[tmp_node.current.x][tmp_node.current.y].bg = 0x0080FF;
			}
		}

		for (y = 0; y < close_list.size(); y++)
		{
			if (close_list[y].current.x == end.x && close_list[y].current.y == end.y)
			{
				not_check_node = close_list[y];
				do {
					for (unsigned int x = 0; x < close_list.size(); x++)
					{
						if (close_list[x].current.x == not_check_node.final.x && close_list[x].current.y == not_check_node.final.y)
						{
							not_check_node = close_list[x];
							break;
						}
					}
					if (not_check_node.current.x != begin.x || not_check_node.current.y != begin.y)
					{
						map_tag[not_check_node.current.x][not_check_node.current.y].bg = 0x00D8D8;
						cross_way.insert(cross_way.begin(), not_check_node.current);
					}

				} while (not_check_node.current.x != begin.x || not_check_node.current.y != begin.y);
				cross_way.insert(cross_way.begin(), begin);
				cross_way.insert(cross_way.end(), end);
				return;
			}
		}

		for (y = 0; y < 4; y++)
		{
			if (y == 0)
			{
				not_check_coor.x = tmp_node.current.x;
				not_check_coor.y = tmp_node.current.y - 1;
			}
			else if (y == 1) {
				not_check_coor.x = tmp_node.current.x - 1;
				not_check_coor.y = tmp_node.current.y;
			}
			else if (y == 2) {
				not_check_coor.x = tmp_node.current.x + 1;
				not_check_coor.y = tmp_node.current.y;
			}
			else {
				not_check_coor.x = tmp_node.current.x;
				not_check_coor.y = tmp_node.current.y + 1;
			}

			if (not_check_coor.x < 0 || not_check_coor.x>(width - 1) || not_check_coor.y<0 || not_check_coor.y>(height - 1))
			{
				continue;
			}
			if (map_tag[not_check_coor.x][not_check_coor.y].bg == 0x000000)
			{
				continue;
			}
			exists_close_list = false;

			for (x = 0; x < close_list.size(); x++)
			{
				if (close_list[x].current.x == not_check_coor.x && close_list[x].current.y == not_check_coor.y)
				{
					exists_close_list = true;
					break;
				}
			}
			if (exists_close_list) {
				continue;
			}
			exists_open_list = false;

			for (x = 0; x < open_list.size(); x++)
			{
				if (open_list[x].current.x == not_check_coor.x && open_list[x].current.y == not_check_coor.y)
				{
					if (not_check_coor.x != tmp_node.current.x || not_check_coor.y != tmp_node.current.y)
					{
						estimate_g = 14;
					}
					else
					{
						estimate_g = 10;
					}
					if (tmp_node.g + estimate_g < open_list[x].g) {
						open_list[x].final = tmp_node.current;
					}
					exists_open_list = true;
					break;
				}
			}

			if (exists_open_list == false)
			{
				if (not_check_coor.x == tmp_node.current.x || not_check_coor.y == tmp_node.current.y) {
					estimate_g = 10;
				}
				else {
					estimate_g = 14;
				}
				not_check_node.g = tmp_node.g + estimate_g;
				not_check_node.h = abs(end.x - not_check_coor.x) * 10 + abs(end.y - not_check_coor.y) * 10;
				not_check_node.f = not_check_node.g + not_check_node.h;
				not_check_node.current = not_check_coor;
				not_check_node.final = tmp_node.current;
				open_list.insert(open_list.begin(), not_check_node);
			}
		}
	} while (open_list.size() != 0);
}

int arrangeCoor(vector<COORDINATE>imitation, vector<COORDINATE>& real_cross)
{
	int x, y, k = 0;
	COORDINATE tmp_coor;
	for (int i = 0; i < imitation.size(); i++)
	{
		x = (imitation[i].x + 2) % 3;
		y = (imitation[i].y + 2) % 3;
		if (x == 0 && y == 0)
		{
			tmp_coor.x = (imitation[i].x + 2) / 3 - 1;
			tmp_coor.y = (imitation[i].y + 2) / 3 - 1;
			real_cross.insert(real_cross.begin() + k, tmp_coor);
			k++;
		}
	}
	return(k);
}

// 选中地图编号
int mapCodeOfSelected()
{
	return readInt(readLong(readLong(C_MAP_SELECTED)) + C_MAP_CODE_SELECTED);
}

// 选中地图难度
int mapDifficultyOfSelected()
{
	readLong(readLong(C_MAP2_SELECTED) + C_MAP_DIFFICULTY_SELECTED);
}