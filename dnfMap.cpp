#include "pch.h"
#include "dnfMap.h"
#include "dnfBase.h"
#include "baseAddress.h"
#include "GameData.h"
#include <queue>

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

int judgeNextRoomDiretion(ROOMCOOR current, ROOMCOOR boss)
{
	// 获取当前房间坐标
	ROOMCOOR coor = GLOBAL.dungeon_info.current_room.coordinate;
	// 获取当前房间通向
	char direct = GLOBAL.dungeon_info.rooms[coor.x][coor.y].direct;
	// 如果当前房间没有通向数据
	if (direct == -1) {
		// 更新所有房间通向
		updateRooms();
	}
	return GLOBAL.dungeon_info.rooms[coor.x][coor.y].direct;
}

void updateRooms()
{
	// 获取门型偏移
	__int64 door_type = GLOBAL.dungeon_info.door_pointer;
	// 获取索引
	__int64 room_index = decrypt2(door_type + ADDR.x64("C_MAP_CODE"));

	// 获取地图X轴房间数量
	int width = readInt(readLong(door_type + ADDR.x64("C_WH_OFFSET")) + room_index * 8 + 0);
	// 获取地图Y轴房间数量
	int height = readInt(readLong(door_type + ADDR.x64("C_WH_OFFSET")) + room_index * 8 + 4);
	// 获取通道指针
	__int64 aisle = readLong(readLong(door_type + ADDR.x64("C_AISLE_OFFSET")) + room_index * 32 + 8);

	// 创建房间地图，默认所有房间不可进入
	vector<vector<int>> rooms(height, vector<int>(width, 0));

	int k = 0;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			// 获取房间通道类型
			rooms[i][j] = readInt(aisle + k * 4);
			k++;
		}
	}

	// 起始房间(就是当前房间的坐标)
	ROOMCOOR begin, end;
	begin.y = GLOBAL.dungeon_info.current_room.coordinate.x;
	begin.x = GLOBAL.dungeon_info.current_room.coordinate.y;
	end.y = GLOBAL.dungeon_info.boos_room.x;
	end.x = GLOBAL.dungeon_info.boos_room.y;

	// 整个地图就一个房间不用计算路径
	if (begin.x == end.x && begin.y == end.y) {
		return;
	}

	// 获取房间过图方向
	vector<ROOMDIRECT> list = mazeBFS(rooms, begin, end, height, width);

	// 反方向数组(用于纠正)
	int reverse[4] = { 1,0,3,2 };

	if (GLOBAL.dungeon_info.rooms.size() < 1)
	{
		// 创建所有房间
		vector<vector<ROOMINFO>> dungeon_rooms(width, vector<ROOMINFO>(height));
		GLOBAL.dungeon_info.rooms = dungeon_rooms;
	}

	// 遍历过图方向
	for (auto item : list)
	{
		if (GLOBAL.dungeon_info.rooms[item.coor.y][item.coor.x].direct == -1) 
		{
			GLOBAL.dungeon_info.rooms[item.coor.y][item.coor.x].direct = item.direct;
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

// BFS算法
vector<ROOMDIRECT> mazeBFS(vector<vector<int>> mat, ROOMCOOR src, ROOMCOOR dest, int width, int height)
{
	// 方向 左右上下 0,1,2,3 
	vector<ROOMCOOR> direct = { {0,-1},{0,1},{-1,0}, {1,0} };

	// 房间通道类型
	unsigned char direction_set[16][4] =
	{
		{ 0, 0, 0, 0 }, // 0没有门
		{ 0, 1, 0, 0 }, // 1右门
		{ 0, 0, 1, 0 }, // 2上门
		{ 0, 1, 1, 0 }, // 3右、上门
		{ 1, 0, 0, 0 }, // 4左门
		{ 1, 1, 0, 0 }, // 5左、右门
		{ 1, 0, 1, 0 }, // 6左、上门
		{ 1, 1, 1, 0 }, // 7左、右、上门
		{ 0, 0, 0, 1 }, // 8下门
		{ 0, 1, 0, 1 }, // 9右门
		{ 0, 0, 1, 1 }, // 10上、下门
		{ 0, 1, 1, 1 }, // 11右、上、下门
		{ 1, 0, 0, 1 }, // 12左、下门
		{ 1, 1, 0, 1 }, // 13左、右下门
		{ 1, 0, 1, 1 }, // 14左、上、下门
		{ 1, 1, 1, 1 }  // 15左、右、上、下门
	};

	vector<ROOMDIRECT> room_direct_list;

	// 如果起点和结束点不可到达，则不进行计算
	if (!mat[src.x][src.y] || !mat[dest.x][dest.y])
		return room_direct_list;

	// 访问单元标记
	vector<vector<bool>> visited(width, vector<bool>(height, false));

	// 起点默认被访问
	visited[src.x][src.y] = true;

	// 创建BFS队列
	queue<queueNode> q;

	// 创建树
	vector<vector<ROOMCOOR>> cell_map(width, vector<ROOMCOOR>(height));

	// 从起点开始进行搜索
	queueNode s = { src };
	q.push(s);

	cell_map[src.x][src.y] = { -1,-1 };

	// BFS从起始节点开始
	while (!q.empty())
	{
		// 获取当前队列首单元
		queueNode curr = q.front();
		ROOMCOOR curr_coor = curr.coor;

		// 如果到达结束节点算法结束
		if (curr_coor.x == dest.x && curr_coor.y == dest.y)
		{
			ROOMCOOR coor = dest;
			while (coor.x != -1 && coor.y != -1)
			{
				// 上一个房间坐标
				ROOMCOOR pre = cell_map[coor.x][coor.y];
				// 计算方向
				for (int k = 0; k < 4; k++)
				{
					if (pre.x - coor.x == direct[k].x && pre.y - coor.y == direct[k].y)
					{
						int dir = k;
						ROOMDIRECT tmp_room_dir = { {pre.x,pre.y},dir };
						room_direct_list.push_back(tmp_room_dir);
						break;
					}
				}
				coor = cell_map[coor.x][coor.y];
			}
			return room_direct_list;
		}

		// 队列中移除这个单元
		q.pop();

		// 遍历位于该单元四个方向的单元
		for (int i = 0; i < 4; i++)
		{
			int row = curr_coor.x + direct[i].x;
			int col = curr_coor.y + direct[i].y;

			// 如果位于此方向的单元在合法范围内 且单元可访问 且单元未被访问
			if (row >= 0 && row < width && col >= 0 && col < height && mat[row][col] && !visited[row][col])
			{
				unsigned char res = direction_set[mat[curr_coor.x][curr_coor.y]][i];
				if (!res) {
					continue;
				}
				// mark cell as visited and enqueue it
				visited[row][col] = true;
				queueNode Adjcell = { {row, col} };
				// 保存父节点位置
				cell_map[row][col] = { curr_coor.x,curr_coor.y };
				q.push(Adjcell);
			}
		}
	}

	return room_direct_list;
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