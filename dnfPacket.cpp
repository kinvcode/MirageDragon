#include "pch.h"
#include "dnfPacket.h"
#include "dnfBase.h"
#include "memory.h"
#include "dnfData.h"

std::vector<byte> packetData;

// 缓冲CALL
void bufferCall(int param)
{
	packetData = packetData + makeByteArray({ 72,131,236,96 });
	packetData = packetData + makeByteArray({ 186 }) + intToBytes(param);
	packetData = packetData + makeByteArray({ 72,185 }) + longToBytes(C_PACKET_SEND);
	packetData = packetData + makeByteArray({ 72,139,9 });
	packetData = packetData + makeByteArray({ 72,184 }) + longToBytes(C_BUFFER_CALL);
	packetData = packetData + makeByteArray({ 255,208 });
}

// 加密CALL
void encryptCall(__int64 param, int length)
{
	packetData = packetData + makeByteArray({ 73,199,192 }) + intToBytes(length);
	packetData = packetData + makeByteArray({ 72,184 }) + longToBytes(param);
	packetData = packetData + makeByteArray({ 72,137,68,36,32 });
	packetData = packetData + makeByteArray({ 72,141,84,36,32 });
	packetData = packetData + makeByteArray({ 72,185 }) + longToBytes(C_PACKET_SEND);
	packetData = packetData + makeByteArray({ 72,139,9 });
	packetData = packetData + makeByteArray({ 72,184 }) + longToBytes(C_ENCRYPT_CALL);
	packetData = packetData + makeByteArray({ 255,208 });
}

// 发包CALL
void sendPacketCall()
{
	packetData = packetData + makeByteArray({ 72,184 }) + longToBytes(C_PACKET_SEND_CALL);
	packetData = packetData + makeByteArray({ 255,208 });
	packetData = packetData + makeByteArray({ 72,131,196,96 });
	memoryAssambly(packetData);
	packetData.clear();
}

// 组包移动
void moveOfTown(int world, int area, int x, int y)
{
	bufferCall(36);
	encryptCall(world, 4);
	encryptCall(area, 1);
	encryptCall(x, 2);
	encryptCall(y, 2);
	encryptCall(5, 1);
	encryptCall(38, 4);
	encryptCall(2, 2);
	encryptCall(0, 4);
	encryptCall(0, 1);
	sendPacketCall();
}

// 组包角色列表
void roleList()
{
	bufferCall(7);
	sendPacketCall();
}

// 组包选择角色
void selectRole(int index)
{
	bufferCall(4);
	encryptCall(index, 4);
	sendPacketCall();
}

// 组包进图，难度：0普通 1冒险 2勇士 3王者 4噩梦
void entryDungeon(int id, int difficulty, int abyss = 0, int practice = 0)
{
	if (id == 100000151)
	{
		abyss = 1;
	}

	if (id == 100000212 || id == 100000215 || id == 100000199 || id == 100000209 || id == 100000214)
	{
		abyss = 6;
	}

	bufferCall(16);
	encryptCall(id, 4);
	encryptCall(difficulty, 1);
	encryptCall(0, 2);
	encryptCall(abyss, 1);
	encryptCall(practice, 1);
	encryptCall(65535, 2);
	encryptCall(0, 4);
	encryptCall(1, 1);
	encryptCall(0, 4);
	encryptCall(0, 1);
	encryptCall(-1, 4);
	sendPacketCall();
}

// 组包选图
void selectMap()
{
	bufferCall(15);
	encryptCall(0, 4);
	sendPacketCall();
}

// 组包过图
void passRoomByPacket(int x,int y)
{
	if (monster_list.size() > 0)
	{
		return;
	}

	bufferCall(45);
	encryptCall(x,1);
	encryptCall(y,1);
	encryptCall(0,4);
	encryptCall(0,4);
	encryptCall(0,1);
	for (int i=0;i<9;i++)
	{
		encryptCall(0, 2);
	}
	for (int j = 0; j < 8; j++)
	{
		encryptCall(0, 4);
	}
	for (int k = 0; k < 7; k++)
	{
		encryptCall(0, 2);
	}
	encryptCall(0,4);
	encryptCall(0,2);
	encryptCall(0,2);
	encryptCall(0,2);
	encryptCall(0, 4);
	encryptCall(0, 4);
	encryptCall(0, 4);
	encryptCall(0,2);
	encryptCall(0,1);
	sendPacketCall();
}