#include "pch.h"
#include "dnfCALL.h"
#include "dnfBase.h"
#include "dnfPacket.h"
#include "baseAddress.h"

void skillCall(__int64 pointer, int code, __int64 damage, int x, int y, int z, int skillSize)
{
	__int64 emptyAddress = ADDR.x64("C_EMPTY_ADDRESS") + 1200;
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
	asm_code = asm_code + makeByteArray({ 72, 184 }) + longToBytes(ADDR.x64("C_SKILL_CALL"));
	asm_code = asm_code + makeByteArray({ 255, 208, 72, 129, 196, 0, 2, 0, 0 });

	memoryAssambly(asm_code);
}

// 坐标CALL(时间久了会掉游戏)
void coorCall(int x, int y, int z)
{
	__int64 target_p = readLong(ADDR.x64("C_USER_ADDRESS"));

	if (target_p < 1) {
		return;
	}

	std::vector<byte> asm_code = makeByteArray({ 72,129,236,0,1,0,0 });
	asm_code = asm_code + makeByteArray({ 65,185 }) + intToBytes(z);
	asm_code = asm_code + makeByteArray({ 65,184 }) + intToBytes(y);
	asm_code = asm_code + makeByteArray({ 186 }) + intToBytes(x);
	asm_code = asm_code + makeByteArray({ 72,185 }) + longToBytes(target_p);
	asm_code = asm_code + makeByteArray({ 72,139,1 });
	asm_code = asm_code + makeByteArray({ 255,144 }) + longToBytes(ADDR.x64("C_COOR_CALL_OFFSET"));
	asm_code = asm_code + makeByteArray({ 72,129,196,0,1,0,0 });
	memoryAssambly(asm_code);
}

// 召唤人偶&怪物
void summonFigureOrMonster(int code, int type = 1)
{
	if (type == 1) {
		// 人偶
		__int64 object_type = ADDR.x64("C_SUMMON_FIGURE");
	}
	else {
		// 怪物
		__int64 object_type = ADDR.x64("C_SUMMON_MONSTER");
	}

	std::vector<byte>asm_code;

	__int64 emptyAddress = ADDR.x64("C_EMPTY_ADDRESS") + 2900;

	writeLong(emptyAddress, code);
	writeLong(emptyAddress + 4, 135);
	writeLong(emptyAddress + 8, 135);
	writeLong(emptyAddress + 12, 600000);
	writeLong(emptyAddress + 16, 1);

	asm_code = asm_code + makeByteArray({ 72, 131, 236, 32 });
	asm_code = asm_code + makeByteArray({ 73, 184 }) + longToBytes(emptyAddress);
	asm_code = asm_code + makeByteArray({ 72, 186 }) + longToBytes(ADDR.x64("C_USER_ADDRESS"));
	asm_code = asm_code + makeByteArray({ 72, 139, 18, 72, 185, 0, 0, 0, 0, 0, 0, 0, 0, 72, 184 }) + longToBytes(ADDR.x64("C_SUMMON_FIGURE"));
	asm_code = asm_code + makeByteArray({ 255, 208, 72, 131, 196, 32 });

	memoryAssambly(asm_code);
}

// 透明CALL
void hiddenUser()
{
	__int64 target = readLong(ADDR.x64("C_USER_ADDRESS"));

	std::vector<byte>asm_code;
	asm_code = makeByteArray({ 72, 129, 236, 0, 2, 0, 0 });
	asm_code = asm_code + makeByteArray({ 65, 191, 255, 255, 255, 255 });
	asm_code = asm_code + makeByteArray({ 199, 68, 36, 32, 255, 255, 0, 0 });
	asm_code = asm_code + makeByteArray({ 65, 185, 1, 0, 0, 0 });
	asm_code = asm_code + makeByteArray({ 73, 184, 1, 0, 0, 0, 0, 0, 0, 0 });
	asm_code = asm_code + makeByteArray({ 186, 1, 0, 0, 0 });
	asm_code = asm_code + makeByteArray({ 72, 185 }) + longToBytes(target);
	asm_code = asm_code + makeByteArray({ 72, 184 }) + longToBytes(ADDR.x64("C_HIDDEN_CALL"));
	asm_code = asm_code + makeByteArray({ 255, 208, 72, 129, 196, 0, 2, 0, 0 });

	memoryAssambly(asm_code);
}

// 超级评分
void superScore()
{
	writeLong(readLong(ADDR.x64("C_SCORE_ADDRESS")) + ADDR.x16("C_CE_SCORE"), 999999);
}

// 区域CALL
void areaCall(int code)
{
	if (code == 100000003)
	{
		code = 100000151;
	}

	__int64 area_address = readLong(ADDR.x64("C_AREA_PARAM"));
	__int64 area_call = ADDR.x64("C_AREA_CALL");

	vector<byte> asm_data = makeByteArray({ 72,131,236,48 });

	asm_data = asm_data + makeByteArray({ 65,184 }) + intToBytes(code);
	asm_data = asm_data + makeByteArray({ 186,174,12,0,0 });
	asm_data = asm_data + makeByteArray({ 72,184,255,255,255,255,0,0,0,0 });
	asm_data = asm_data + makeByteArray({ 72,185 }) + longToBytes(ADDR.x64("C_AREA_PARAM"));
	asm_data = asm_data + makeByteArray({ 72,139,9 });
	asm_data = asm_data + makeByteArray({ 72,184 }) + longToBytes(area_call);
	asm_data = asm_data + makeByteArray({ 255,208,72,131,196,48 });
	memoryAssambly(asm_data);

	Sleep(50);

	int world = readInt(area_address + ADDR.x32("C_AREA_OFFSET"));
	int town = readInt(area_address + ADDR.x32("C_AREA_OFFSET") + 4);
	int x = readInt(area_address + ADDR.x32("C_AREA_OFFSET") + 8);
	int y = readInt(area_address + ADDR.x32("C_AREA_OFFSET") + 12);

	moveOfTown(world, town, x, y);
}

void panelCall(int id)
{
	__int64 pointer = readLong(ADDR.x64("C_PANEL_RCX"));
	vector<byte>asm_code;
	asm_code = subRsp(256);
	asm_code = asm_code + makeByteArray({ 186 }) + intToBytes(id);
	asm_code = asm_code + makeByteArray({ 72,185 }) + longToBytes(pointer);
	asm_code = asm_code + makeByteArray({ 255, 21, 2, 0, 0, 0, 235, 8 }) + longToBytes(ADDR.x64("C_PANEL_CALL"));
	asm_code = asm_code + addRsp(256);
	memoryAssambly(asm_code);
}

void learSkillCall()
{
	__int64 pointer = readLong(readLong(readLong(ADDR.x64("C_SHOP")) + ADDR.x64("C_LEARN_SKILL_OFFSET")));

	vector<byte> asm_code;
	asm_code = subRsp(256);
	asm_code = asm_code + makeByteArray({ 65, 184, 1, 0, 0, 0 });
	asm_code = asm_code + makeByteArray({ 186, 0, 0, 0, 0 });
	asm_code = asm_code + makeByteArray({ 72, 185 }) + longToBytes(pointer);
	asm_code = asm_code + makeByteArray({ 255, 21, 2, 0, 0, 0, 235, 8 }) + longToBytes(ADDR.x64("C_LEARN_SKILL_CALL"));
	asm_code = asm_code + addRsp(256);
	memoryAssambly(asm_code);
}

int getSkillNULPos()
{
	__int64 skill_p = readLong(readLong(readLong(ADDR.x64("C_USER_ADDRESS")) + ADDR.x64("C_SKILL_LIST")) + ADDR.x64("C_SKILL_LIST_OFFSET"));

	for (int i = 0; i < 407;i++) 
	{
		__int64 tmp = readLong(skill_p + i * 8);
		if((int)tmp == 0)
		{
			if (i > 13) 
			{
				return i;
			}
		}
	}
	return -1;
}

void moveSkillCall(int source, int target)
{
	vector<byte> asm_code = makeByteArray({ 72, 131, 236, 40 });
	asm_code = asm_code + makeByteArray({ 199, 68, 36, 32, 1, 0, 0, 0 });
	asm_code = asm_code + makeByteArray({ 65, 185, 4, 0, 0, 0 });
	asm_code = asm_code + makeByteArray({ 65, 184 }) + intToBytes(source);
	asm_code = asm_code + makeByteArray({ 186 }) + intToBytes(target);
	asm_code = asm_code + makeByteArray({ 72, 185 }) + longToBytes(ADDR.x64("C_USER_ADDRESS"));
	asm_code = asm_code + makeByteArray({ 72, 139, 9 });
	asm_code = asm_code + makeByteArray({ 72, 139, 137 }) + intToBytes(ADDR.x32("C_SKILL_LIST"));
	asm_code = asm_code + makeByteArray({ 255, 21, 2, 0, 0, 0, 235, 8 }) + longToBytes(ADDR.x64("C_MOV_SKILL_CALL"));
	asm_code = asm_code + makeByteArray({ 72, 131, 196, 40 });
	memoryAssambly(asm_code);
}

void finishJobCall(int id,int times) 
{
	for (int i=0;i<times;i++)
	{
		vector<byte> asm_code = makeByteArray({ 72, 129, 236, 0, 2, 0, 0 });
		asm_code = asm_code + makeByteArray({ 179, 255 });
		asm_code = asm_code + makeByteArray({ 68, 15, 182, 203 });
		asm_code = asm_code + makeByteArray({ 65, 176, 255 });
		asm_code = asm_code + makeByteArray({ 186 }) + intToBytes(id);
		asm_code = asm_code + makeByteArray({ 255, 21, 2, 0, 0, 0, 235, 8 }) + longToBytes(ADDR.x64("C_FINISH_CALL"));
		asm_code = asm_code + makeByteArray({ 72, 129, 196, 0, 2, 0, 0 });
		memoryAssambly(asm_code);
	}
}

void submitJobCall(int id) 
{
	vector<byte> asm_code = makeByteArray({ 72, 131, 236, 48 });
	asm_code = asm_code + makeByteArray({ 65, 189, 1, 0, 0, 0 });
	asm_code = asm_code + makeByteArray({ 65, 190, 255, 255, 255, 255 });
	asm_code = asm_code + makeByteArray({ 69, 139, 205 });
	asm_code = asm_code + makeByteArray({ 69, 139, 198 });
	asm_code = asm_code + makeByteArray({ 72, 185 }) + longToBytes(ADDR.x64("C_JOB_ADDRESS"));
	asm_code = asm_code + makeByteArray({ 72, 139, 9 });
	asm_code = asm_code + makeByteArray({ 186 }) + intToBytes(id);
	asm_code = asm_code + makeByteArray({ 255, 21, 2, 0, 0, 0, 235, 8 }) + longToBytes(ADDR.x64("C_SUBMIT_CALL"));
	asm_code = asm_code + makeByteArray({ 72, 131, 196, 48 });
	memoryAssambly(asm_code);
}

void acceptJobCall(int id) 
{
	vector<byte> asm_code = makeByteArray({ 72, 131, 236, 40 });
	asm_code = asm_code + makeByteArray({ 186 }) + intToBytes(id);
	asm_code = asm_code + makeByteArray({ 255, 21, 2, 0, 0, 0, 235, 8 }) + longToBytes(ADDR.x64("C_ACCEPT_CALL"));
	asm_code = asm_code + makeByteArray({ 72, 131, 196, 40 });
	memoryAssambly(asm_code);
}