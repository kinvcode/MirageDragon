#include "pch.h"
#include "dnfPacket.h"
#include "dnfBase.h"
#include "memory.h"
#include "dnfData.h"
#include "baseAddress.h"
#include "GameData.h"

std::vector<byte> packetData;

// ����CALL
void bufferCall(int param)
{
	packetData = packetData + makeByteArray({ 72,131,236,96 });
	packetData = packetData + makeByteArray({ 72,185 }) + longToBytes(ADDR.x64("C_PACKET_SEND"));
	packetData = packetData + makeByteArray({ 186 }) + intToBytes(param);
	packetData = packetData + makeByteArray({ 72,184 }) + longToBytes(ADDR.x64("C_BUFFER_CALL"));
	packetData = packetData + makeByteArray({ 255,208 });
}

// ����CALL
void encryptCall(__int64 param, int length)
{
	packetData = packetData + makeByteArray({ 72,185 }) + longToBytes(ADDR.x64("C_PACKET_SEND"));
	packetData = packetData + makeByteArray({ 72,186 }) + longToBytes(param);
	if (length == 1) {
		packetData = packetData + makeByteArray({ 72,184 }) + longToBytes(ADDR.x64("C_ENCRYPT_PACKET_CALL"));
	}
	else if (length == 2) {
		packetData = packetData + makeByteArray({ 72,184 }) + longToBytes(ADDR.x64("C_ENCRYPT_PACKET_CALL2"));
	}
	else if (length == 4) {
		packetData = packetData + makeByteArray({ 72,184 }) + longToBytes(ADDR.x64("C_ENCRYPT_PACKET_CALL4"));
	}
	else if (length == 8) {
		packetData = packetData + makeByteArray({ 72,184 }) + longToBytes(ADDR.x64("C_ENCRYPT_PACKET_CALL8"));
	}
	packetData = packetData + makeByteArray({ 255,208 });
}

// ����CALL
void sendPacketCall()
{
	packetData = packetData + makeByteArray({ 72,184 }) + longToBytes(ADDR.x64("C_PACKET_SEND_CALL"));
	packetData = packetData + makeByteArray({ 255,208 });
	packetData = packetData + makeByteArray({ 72,131,196,96 });
	memoryAssambly(packetData);
	packetData.clear();
}

// �������
void turnOverCard(int x = 0, int y = 0)
{
	bufferCall(69);
	sendPacketCall();
	bufferCall(70);
	sendPacketCall();
	bufferCall(71);
	encryptCall(x, 1);
	encryptCall(y, 1);
	sendPacketCall();
	bufferCall(1426);
	sendPacketCall();
}

// ����ƶ�
void moveOfTown(int world, int area, int x, int y)
{
	bufferCall(36);
	encryptCall(world, 4);
	encryptCall(area, 4);
	encryptCall(x, 2);
	encryptCall(y, 2);
	encryptCall(5, 1);
	encryptCall(38, 4);
	encryptCall(0, 2);
	encryptCall(0, 4);
	encryptCall(0, 1);
	sendPacketCall();
}

// �����ɫ�б�
void roleList()
{
	bufferCall(7);
	sendPacketCall();
}

// ���ѡ���ɫ
void selectRole(int index)
{
	bufferCall(4);
	encryptCall(index, 4);
	sendPacketCall();
}

// �����ͼ���Ѷȣ�0��ͨ 1ð�� 2��ʿ 3���� 4ج��
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

// ���ѡͼ
void selectMap()
{
	bufferCall(15);
	encryptCall(0, 4);
	sendPacketCall();
}

// �����ͼ
void passRoomByPacket(int x, int y)
{
	// �޸�Ϊ�ж��Ƿ���
	if (GLOBAL.dungeon_info.current_room.monster_list.size() > 0)
	{
		return;
	}

	bufferCall(45);
	encryptCall(x, 1);
	encryptCall(y, 1);
	encryptCall(0, 4);
	encryptCall(0, 4);
	encryptCall(0, 1);
	for (int i = 0; i < 9; i++)
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
	encryptCall(0, 4);
	encryptCall(0, 2);
	encryptCall(0, 2);
	encryptCall(0, 2);
	encryptCall(0, 4);
	encryptCall(0, 4);
	encryptCall(0, 4);
	encryptCall(0, 2);
	encryptCall(0, 1);
	sendPacketCall();
}