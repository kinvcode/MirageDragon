#include "pch.h"
#include "dnfCALL.h"
#include "dnfBase.h"
#include "dnfPacket.h"

void skillCall(__int64 pointer, int code, __int64 damage, int x, int y, int z, int skillSize)
{
	__int64 emptyAddress = C_EMPTY_ADDRESS + 1200;
	std::vector<byte>asm_code;

	float size = (float)skillSize;

	writeLong(emptyAddress, pointer);
	writeInt(emptyAddress + 16, code);
	writeLong(emptyAddress + 20, damage);
	writeInt(emptyAddress + 32, x);
	writeInt(emptyAddress + 36, y);
	writeInt(emptyAddress + 40, z);
	writeFloat(emptyAddress + 140, size);
	writeInt(emptyAddress + 144, 65535);
	writeInt(emptyAddress + 148, 65535);

	asm_code = asm_code + makeByteArray({ 72, 129, 236, 0, 2, 0, 0 });
	asm_code = asm_code + makeByteArray({ 72, 185 }) + longToBytes(emptyAddress);
	asm_code = asm_code + makeByteArray({ 72, 184 }) + longToBytes(C_SKILL_CALL);
	asm_code = asm_code + makeByteArray({ 255, 208, 72, 129, 196, 0, 2, 0, 0 });

	memoryAssambly(asm_code);
}

// 坐标CALL(时间久了会掉游戏)
void coorCall(int x, int y, int z)
{
	__int64 target_p = readLong(C_USER);

	if (target_p < 1) {
		return;
	}

	std::vector<byte> asm_code = makeByteArray({ 72,129,236,0,1,0,0 });
	asm_code = asm_code + makeByteArray({ 65,185 }) + intToBytes(z);
	asm_code = asm_code + makeByteArray({ 65,184 }) + intToBytes(y);
	asm_code = asm_code + makeByteArray({ 186 }) + intToBytes(x);
	asm_code = asm_code + makeByteArray({ 72,185 }) + longToBytes(target_p);
	asm_code = asm_code + makeByteArray({ 72,139,1 });
	asm_code = asm_code + makeByteArray({ 255,144 }) + longToBytes(C_COOR_CALL_OFFSET);
	asm_code = asm_code + makeByteArray({ 72,129,196,0,1,0,0 });
	memoryAssambly(asm_code);
}

// 召唤人偶&怪物
void summonFigureOrMonster(int code, int type = 1)
{
	if (type == 1) {
		// 人偶
		__int64 object_type = C_SUMMON_FIGURE;
	}
	else {
		// 怪物
		__int64 object_type = C_SUMMON_MONSTER;
	}

	std::vector<byte>asm_code;

	__int64 emptyAddress = C_EMPTY_ADDRESS + 2900;

	writeLong(emptyAddress, code);
	writeLong(emptyAddress + 4, 135);
	writeLong(emptyAddress + 8, 135);
	writeLong(emptyAddress + 12, 600000);
	writeLong(emptyAddress + 16, 1);

	asm_code = asm_code + makeByteArray({ 72, 131, 236, 32 });
	asm_code = asm_code + makeByteArray({ 73, 184 }) + longToBytes(emptyAddress);
	asm_code = asm_code + makeByteArray({ 72, 186 }) + longToBytes(C_USER);
	asm_code = asm_code + makeByteArray({ 72, 139, 18, 72, 185, 0, 0, 0, 0, 0, 0, 0, 0, 72, 184 }) + longToBytes(C_SUMMON_FIGURE);
	asm_code = asm_code + makeByteArray({ 255, 208, 72, 131, 196, 32 });

	memoryAssambly(asm_code);
}

// 透明CALL
void hiddenUser()
{
	__int64 target = readLong(C_USER);

	std::vector<byte>asm_code;
	asm_code = makeByteArray({ 72, 129, 236, 0, 2, 0, 0 });
	asm_code = asm_code + makeByteArray({ 65, 191, 255, 255, 255, 255 });
	asm_code = asm_code + makeByteArray({ 199, 68, 36, 32, 255, 255, 0, 0 });
	asm_code = asm_code + makeByteArray({ 65, 185, 1, 0, 0, 0 });
	asm_code = asm_code + makeByteArray({ 73, 184, 1, 0, 0, 0, 0, 0, 0, 0 });
	asm_code = asm_code + makeByteArray({ 186, 1, 0, 0, 0 });
	asm_code = asm_code + makeByteArray({ 72, 185 }) + longToBytes(target);
	asm_code = asm_code + makeByteArray({ 72, 184 }) + longToBytes(C_HIDDEN_CALL);
	asm_code = asm_code + makeByteArray({ 255, 208, 72, 129, 196, 0, 2, 0, 0 });

	memoryAssambly(asm_code);
}

// 超级评分
void superScore()
{
	writeLong(readLong(C_SCORE_ADDRESS) + C_CE_SCORE, 999999);
}

// 区域CALL
void areaCall(int code)
{
	if (code == 100000003)
	{
		code = 100000151;
	}

	__int64 area_address = readLong(C_AREA_PARAM);
	__int64 area_call = C_AREA_CALL;

	vector<byte> asm_data = makeByteArray({ 72,131,236,48 });

	asm_data = asm_data + makeByteArray({ 65,184 }) + intToBytes(code);
	asm_data = asm_data + makeByteArray({ 186,174,12,0,0 });
	asm_data = asm_data + makeByteArray({ 72,184,255,255,255,255,0,0,0,0 });
	asm_data = asm_data + makeByteArray({ 72,185 }) + longToBytes(C_AREA_PARAM);
	asm_data = asm_data + makeByteArray({ 72,139,9 });
	asm_data = asm_data + makeByteArray({ 72,184 }) + longToBytes(area_call);
	asm_data = asm_data + makeByteArray({ 255,208,72,131,196,48 });
	memoryAssambly(asm_data);

	programDelay(50, 0);

	int world = readInt(area_address + C_AREA_OFFSET);
	int town = readInt(area_address + C_AREA_OFFSET + 4);
	int x = readInt(area_address + C_AREA_OFFSET + 8);
	int y = readInt(area_address + C_AREA_OFFSET + 12);

	moveOfTown(world, town, x, y);
}