#include "pch.h"
#include "dnfData.h"
#include "dnfBase.h"
#include "memory.h"

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
	vector<byte> data = readByteArray(readLong(readLong(readLong(readLong(C_ROOM_NUMBER) + C_TIME_ADDRESS) + C_DOOR_OFFSET) + C_MAP_NAME) + 0, 100);
	return bytesToWstring(data);
}

// 获取地图编号
int getMapNumber()
{
	return readInt(C_MAP_NUMBER);
}