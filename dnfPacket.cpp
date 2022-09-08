#include "pch.h"
#include "dnfPacket.h"
#include "dnfBase.h"
#include "memory.h"

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

// 城镇移动
void moveOfTown(int world, int area, int x, int y)
{
	bufferCall(36);
	encryptCall(world, 4);
	encryptCall(area, 1);
	encryptCall(x, 2);
	encryptCall(y, 3);
	encryptCall(5, 1);
	encryptCall(38, 4);
	encryptCall(2, 2);
	encryptCall(0, 4);
	encryptCall(0, 1);
	sendPacketCall();
}