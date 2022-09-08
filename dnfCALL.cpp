#include "pch.h"
#include "dnfCALL.h"
#include "dnfBase.h"

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

// ×ø±êCALL
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