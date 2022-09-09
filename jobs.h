#pragma once

// 定义房间位置结构
struct ROOMCOOR
{
	__int64 x;
	__int64 y;
};

// 游戏数据更新线程
UINT updateDataThread(LPVOID pParam);

// 刷图线程
UINT playGameThead(LPVOID pParam);

// 获取当前房间重复次数
int roomRepeats(vector<ROOMCOOR>, ROOMCOOR current_room);