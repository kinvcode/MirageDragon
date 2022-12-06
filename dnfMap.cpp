#include "pch.h"
#include "dnfMap.h"
#include "dnfBase.h"
#include "baseAddress.h"
#include "GameData.h"

__int64 passRoomData(int direction)
{
	__int64 empty_address = ADDR.x64("C_EMPTY_ADDRESS") + 1450;
	__int64 room_data = readLong(readLong(readLong(ADDR.x64("C_ROOM_NUMBER")) + ADDR.x64("C_TIME_ADDRESS")) + ADDR.x64("C_PASS_ROOM_OFFSET"));

	vector<byte>asm_code;
	asm_code = makeByteArray({ 72,129,236,0,1,0,0 });
	asm_code = asm_code + makeByteArray({ 72,185 }) + longToBytes(room_data);
	asm_code = asm_code + makeByteArray({ 186 }) + intToBytes(direction);
	asm_code = asm_code + makeByteArray({ 72,184 }) + longToBytes(ADDR.x64("C_COORDINATE_PASS_ROOM"));
	asm_code = asm_code + makeByteArray({ 255,208 });
	asm_code = asm_code + makeByteArray({ 72,163 }) + longToBytes(empty_address);
	asm_code = asm_code + makeByteArray({ 72,129,196,0,1,0,0 });
	memoryAssambly(asm_code);
	return readLong(empty_address);
}

int judgeNextRoomDiretion(ROOMCOORDINATE current, ROOMCOORDINATE boss)
{
	int x, y;

	// 获取当前房间数据
	DUNGEONMAP map_data = mapData();
	if (map_data.fatigue < 1) {
		return -1;
	}

	// ??
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

	// 获取门型偏移
	__int64 door_type = GLOBAL.dungeon_info.door_pointer;
	// 获取索引
	__int64 room_index = decrypt2(door_type + ADDR.x64("C_MAP_CODE"));

	// 获取地图X轴房间数量
	map_data.width = readInt(readLong(door_type + ADDR.x64("C_WH_OFFSET")) + room_index * 8 + 0);
	// 获取地图Y轴房间数量
	map_data.height = readInt(readLong(door_type + ADDR.x64("C_WH_OFFSET")) + room_index * 8 + 4);
	// 获取通道偏移
	map_data.tmp = readLong(readLong(door_type + ADDR.x64("C_AISLE_OFFSET")) + room_index * 32 + 8);
	// 获取通道数量
	map_data.aisle_num = map_data.width * map_data.height;

	for (__int64 i = 0; i < map_data.aisle_num; i++)
	{
		// 所有房间存放到通道容器中；读取的是房间索引？
		map_data.aisle.insert(map_data.aisle.begin() + i, readInt(map_data.tmp + i * 4));
	}

	// 起始房间(就是当前房间的坐标)
	map_data.begin.x = readInt(readLong(readLong(ADDR.x64("C_ROOM_NUMBER")) + ADDR.x64("C_TIME_ADDRESS")) + ADDR.x64("C_BEGIN_ROOM_X")) + 1;
	map_data.begin.y = readInt(readLong(readLong(ADDR.x64("C_ROOM_NUMBER")) + ADDR.x64("C_TIME_ADDRESS")) + ADDR.x64("C_BEGIN_ROOM_Y")) + 1;
	map_data.end.x = (int)GLOBAL.dungeon_info.boos_room.x + 1;
	map_data.end.y = (int)GLOBAL.dungeon_info.boos_room.y + 1;

	// 判断整个地图就一个房间？
	if (map_data.begin.x == map_data.end.x && map_data.begin.y == map_data.end.y) {
		return map_data;
	}

	// 获取路径、疲劳消耗量？
	map_data.fatigue = getWay(map_data.aisle, map_data.width, map_data.height, map_data.begin, map_data.end, map_data.way);
	return map_data;
}

int getWay(vector<int> aisle, int width, int height, COORDINATE begin, COORDINATE end, vector<COORDINATE>& way)
{
	COORDINATE begin_coor, end_coor;
	vector<vector<AISLEDATA>> map_tag, map_array;
	vector<COORDINATE> cross_way;

	// 判断整个地图就一个房间？
	if (begin.x == end.x && begin.y == end.y)
	{
		way.clear();
		way.resize(0);
		return 0;
	}

	createMap(width, height, aisle, map_array);
	showMap(map_array, width, height, map_tag);

	// 设置房间在算法地图（带门的房间数组）中的位置
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
	// 重置一维数组大小（地图X）
	map.resize(width);
	for (int x = 0; x < width; x++)
	{
		// 重置二维数组大小（地图Y）
		map[x].resize(height);
	}

	int i = 0;
	int x, y;
	// 以左上角房间为起点，遍历所有房间（从左往右依次存放）
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			map[x][y].coor.x = x;		// 房间X坐标
			map[x][y].coor.y = y;		// 房间Y坐标
			map[x][y].aisle = aisle[i];	// 通道数据
			map[x][y].left = judgeDirection(aisle[i], 0);	// 左门是否存在
			map[x][y].right = judgeDirection(aisle[i], 1);	// 右门是否存在
			map[x][y].top = judgeDirection(aisle[i], 2);	// 上门是否存在
			map[x][y].bottom = judgeDirection(aisle[i], 3);	// 下门是否存在
			map[x][y].bg = 0xFFFFFF; // 默认白色背景（代表有门）
			i++;
			// 如果该房间的【门分布】类型为0，则该房间没有任何门
			if (map[x][y].aisle == 0)
			{
				// 设置房间背景色为黑色（代表没有门）
				map[x][y].bg = 0x000000;
			}
		}
	}
}

void showMap(vector<vector<AISLEDATA>> map, int width, int height, vector<vector<AISLEDATA>>& tag)
{
	tag.clear();
	// 重置一维数组大小
	tag.resize(width * 3);
	
	// tag数组原理如下
	// 以二维平面图为设计原理
	// 创建一个二维数组，在每个房间的上下左右四个方向都设置一个元素，代表房间的四个门
	// 房间周围，只有上下左右元素有效（代表门）
	// 房间四个角是无效的数据，也用不到（只是为了方便创建平面图）

	for (int x = 0; x < width * 3; x++)
	{
		// 重置二维数组大小
		tag[x].resize(height * 3);
	}
	// tag此时表示每个房间都有上下左右四个门

	int x, y;

	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			// 房间本身标记为白色
			tag[(x + 1) * 3 - 2][(y + 1) * 3 - 2].bg = 0xFFFFFF;

			// 如果左边有门
			if (map[x][y].left == true)
			{
				// 房间左边标记为白色
				tag[(x + 1) * 3 - 3][(y + 1) * 3 - 2].bg = 0xFFFFFF;
			}

			// 如果右边有门
			if (map[x][y].right == true)
			{
				// 房间右边标记为白色
				tag[(x + 1) * 3 - 1][(y + 1) * 3 - 2].bg = 0xFFFFFF;
			}

			// 如果上边有门
			if (map[x][y].top == true)
			{
				// 房间上边标记为白色
				tag[(x + 1) * 3 - 2][(y + 1) * 3 - 3].bg = 0xFFFFFF;
			}

			// 如果下边有门
			if (map[x][y].bottom == true)
			{
				// 房间下边标记为白色
				tag[(x + 1) * 3 - 2][(y + 1) * 3 - 1].bg = 0xFFFFFF;
			}
		}
	}

}

bool judgeDirection(int aisle, int direction)
{
	// 房间理论通向（上下左右四个门）
	unsigned char direction_arr[4];
	// 房间门出现的所有情况
	unsigned char direction_set[16][4] =
	{
		{ 0, 0, 0, 0 }, // 没有门
		{ 0, 1, 0, 0 }, // 右门
		{ 0, 0, 1, 0 }, // 上门
		{ 0, 1, 1, 0 }, // 右、上门
		{ 1, 0, 0, 0 }, // 左门
		{ 1, 1, 0, 0 }, // 左、右门
		{ 1, 0, 1, 0 }, // 左、上门
		{ 1, 1, 1, 0 }, // 左、右、上门
		{ 0, 0, 0, 1 }, // 下门
		{ 0, 1, 0, 1 }, // 右门
		{ 0, 0, 1, 1 }, // 上、下门
		{ 0, 1, 1, 1 }, // 右、上、下门
		{ 1, 0, 0, 1 }, // 左、下门
		{ 1, 1, 0, 1 }, // 左、右下门
		{ 1, 0, 1, 1 }, // 左、上、下门
		{ 1, 1, 1, 1 }  // 左、右、上、下门
	};
	
	vector<byte> arr;
	// 如果该房间通道有正常数据
	if (aisle >= 0 && aisle <= 15)
	{
		// 设置该房间的门分布
		for (int i = 0; i < 4; i++)
		{
			direction_arr[i] = direction_set[aisle][i];
		}
	}
	else {
		// 该房间上下左右都没有门
		for (int i = 0; i < 4; i++)
		{
			direction_arr[i] = 0;
		}
	}
	// 该方向有门
	if (direction_arr[direction] == 1) {
		return true;
	}
	// 没有门
	return false;
}

// 迷宫算法：BFS（时间复杂度小、空间复杂度大）；DFS（时间复杂度大、空间复杂度小）

// 作者算法（采用门图的方式：在每个房间的上下左右位置加入坐标。二维数组的大小是房间图数组的3倍）
void pathCalc(vector<vector<AISLEDATA>> map_tag, COORDINATE begin, COORDINATE end, int width, int height, vector<COORDINATE>& cross_way)
{
	int x, y;			// 只是声明变量，没有实际意义
	DWORD min_f;		// 最小f值，值越小表示距离boss越近
	int estimate_g;		// 预测g值，
	MAPNODE tmp_node;	// 存放最小编号的节点，该节点距离BOSS房间最近
	int min_number = 0;			// 最小编号，门图的索引。该编号的节点距离BOSS房间最近
	bool exists_open_list;		// 将要计算的节点是否在已计算列表中
	MAPNODE not_check_node;		// 将要计算的节点
	bool exists_close_list;		// 将要计算的节点是否存在于路径中，如果存在就跳过计算
	COORDINATE not_check_coor;	// 一个临时坐标，表示将要进行计算的坐标
	vector<MAPNODE> open_list;  // 表示已参加计算的坐标
	vector<MAPNODE> close_list; // 存放的是最短路径中的节点。表示通往BOSS房间的路径
	

	tmp_node.current.x = begin.x;
	tmp_node.current.y = begin.y;
	map_tag[begin.x][begin.y].bg = 0x00FF00; // 绿色
	map_tag[end.x][end.y].bg = 0x0000FF; // 红色
	
	// 存放起始房间坐标
	open_list.insert(open_list.begin(), tmp_node);

	do
	{
		min_f = 0;
		// 找最小f值的坐标（优先级为：上左右下）
		for (y = 0; y < open_list.size(); y++)
		{
			// 先设置一个最小值，方便与其他元素进行对比
			if (min_f == 0)
			{
				min_f = open_list[0].f;
				min_number = y;
			}
			// 比较最小f值
			if (open_list[y].f < min_f)
			{
				min_f = open_list[y].f;
				min_number = y;
			}
		}

		// tmp_node存放最小编号的地图节点
		tmp_node = open_list[min_number];
		// open_list移除最小编号节点
		open_list.erase(open_list.begin() + min_number);
		// close_list加入最小编号的节点
		close_list.insert(close_list.begin(), tmp_node);

		// 如果最小编号节点不是起始节点
		if (tmp_node.current.x != begin.x || tmp_node.current.y != begin.y)
		{
			// 如果最小编号节点不是boss节点
			if (tmp_node.current.x != end.x || tmp_node.current.y != end.y)
			{
				// 最小编号的背景色设置为橙黄
				map_tag[tmp_node.current.x][tmp_node.current.y].bg = 0x0080FF; // 橙黄
			}
		}

		// 遍历路径节点
		for (y = 0; y < close_list.size(); y++)
		{
			// 如果路径节点中存在BOSS房间节点，说明现在是最后一次循环
			if (close_list[y].current.x == end.x && close_list[y].current.y == end.y)
			{
				// 保存最后一个节点，注意这个节点不是BOSS房间节点，而是BOSS房间的上个节点（BOSS房间之前那个房间的门节点）
				not_check_node = close_list[y];
				do {
					// 遍历路径节点（按照距离boss近到远进行排序）
					for (unsigned int x = 0; x < close_list.size(); x++)
					{
						// 如果当前节点与当前保存的节点的中心相等
						if (close_list[x].current.x == not_check_node.final.x && close_list[x].current.y == not_check_node.final.y)
						{
							// 保存当前节点
							not_check_node = close_list[x];
							break;
						}
					}
					// 如果当前房间节点不是起始房间节点
					if (not_check_node.current.x != begin.x || not_check_node.current.y != begin.y)
					{
						// 设置当前房间节点背景色为嫩黄
						map_tag[not_check_node.current.x][not_check_node.current.y].bg = 0x00D8D8; //嫩黄
						// 过图路径容器插入当前节点
						cross_way.insert(cross_way.begin(), not_check_node.current);
					}
					// 循环到起始房间结束
				} while (not_check_node.current.x != begin.x || not_check_node.current.y != begin.y);
				
				// 保存起始节点
				cross_way.insert(cross_way.begin(), begin);
				// 保存boss节点
				cross_way.insert(cross_way.end(), end);
				return;
			}
		}

		// 遍历最小编号节点的四个方向（节点），找出距离BOSS房间最近的那个节点
		for (y = 0; y < 4; y++)
		{
			if (y == 0)
			{
				// 上
				not_check_coor.x = tmp_node.current.x;
				not_check_coor.y = tmp_node.current.y - 1;
			}
			else if (y == 1) {
				// 左
				not_check_coor.x = tmp_node.current.x - 1;
				not_check_coor.y = tmp_node.current.y;
			}
			else if (y == 2) {
				// 右
				not_check_coor.x = tmp_node.current.x + 1;
				not_check_coor.y = tmp_node.current.y;
			}
			else {
				// 下
				not_check_coor.x = tmp_node.current.x;
				not_check_coor.y = tmp_node.current.y + 1;
			}

			// 如果该节点不在图中,继续循环
			if (not_check_coor.x < 0 || not_check_coor.x>(width - 1) || not_check_coor.y<0 || not_check_coor.y>(height - 1))
			{
				continue;
			}
			
			// 如果该节点没有门，继续循环
			if (map_tag[not_check_coor.x][not_check_coor.y].bg == 0x000000)
			{
				continue;
			}
			
			exists_close_list = false;
			for (x = 0; x < close_list.size(); x++)
			{
				// 如果当前的要检测的节点在路径节点中存在，则进行下一轮循环。不再对该节点进行计算（因为已经计算过f值）
				if (close_list[x].current.x == not_check_coor.x && close_list[x].current.y == not_check_coor.y)
				{
					exists_close_list = true;
					break;
				}
			}
			// 跳过本次循环（跳过该节点的计算）
			if (exists_close_list) {
				continue;
			}

			exists_open_list = false;
			for (x = 0; x < open_list.size(); x++)
			{
				// 如果当前要检测的节点已参与过计算
				if (open_list[x].current.x == not_check_coor.x && open_list[x].current.y == not_check_coor.y)
				{
					// 如果当前要检测的节点与最小编号节点 X轴或Y轴相同。则改变g值（大）
					if (not_check_coor.x != tmp_node.current.x || not_check_coor.y != tmp_node.current.y)
					{
						estimate_g = 14;
					}
					else
					{
						estimate_g = 10;
					}
					// 如果最小编号的节点与g值相加之和，小于当前循环节点的g值
					if (tmp_node.g + estimate_g < open_list[x].g) {
						open_list[x].final = tmp_node.current;
					}
					// 跳出本次wheel循环（因为该节点已进行过计算，不用重复计算）
					exists_open_list = true;
					break;
				}
			}

			// 当前要检测的节点还没参与过计算，则进行计算
			if (exists_open_list == false)
			{
				// 如果要检测的节点与最小编号的节点 X轴或Y轴相同，则修改g值（小）
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
	// 遍历路径节点
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
	return readInt(readLong(readLong(ADDR.x64("C_MAP_SELECTED"))) + ADDR.x64("C_MAP_CODE_SELECTED"));
}

// 选中地图难度
int mapDifficultyOfSelected()
{
	return readInt(readLong(ADDR.x64("C_MAP2_SELECTED")) + ADDR.x64("C_MAP_DIFFICULTY_SELECTED"));
}