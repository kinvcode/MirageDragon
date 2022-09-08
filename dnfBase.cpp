#include "pch.h"
#include "dnfBase.h"

void memoryAssambly(vector<byte>asm_code)
{
	// HOOK数据和原始数据
	vector<byte>HOOKdata, HOOKraw;
	
	//异步执行
	bool asyncExecute = false;
	
	//汇编中转,空白地址,判断地址
	__int64 asmTransfer, emptyAddress, logicAddress, HOOKasm, HOOKret;
	asmTransfer = C_EMPTY_ADDRESS + 300;
	emptyAddress = C_EMPTY_ADDRESS + 500;
	logicAddress = emptyAddress - 100;
	if (asyncExecute) {
		return;
	}
	
	asyncExecute = true;
	HOOKasm = C_ASM_CALL;
	HOOKasm += 144;
	HOOKret = HOOKasm + 19;
	HOOKdata = readByteArray(HOOKasm, 19);
	HOOKraw = HOOKdata;
	HOOKdata = HOOKdata + makeByteArray({ 72,184 }) + longToBytes(logicAddress);
	HOOKdata = HOOKdata + makeByteArray({ 131, 56, 1, 117, 42, 72, 129, 236, 0, 3, 0, 0 });
	HOOKdata = HOOKdata + makeByteArray({ 72,187 }) + longToBytes(emptyAddress);
	HOOKdata = HOOKdata + makeByteArray({ 255, 211 });
	HOOKdata = HOOKdata + makeByteArray({ 72,184 }) + longToBytes(logicAddress);
	HOOKdata = HOOKdata + makeByteArray({ 199, 0, 3, 0, 0, 0 });
	HOOKdata = HOOKdata + makeByteArray({ 72, 129, 196, 0, 3, 0, 0 });
	HOOKdata = HOOKdata + makeByteArray({ 255, 37, 0, 0, 0, 0 }) + longToBytes(HOOKret);
	
	if (readInt(asmTransfer) == 0)
	{
		writeByteArray(asmTransfer, HOOKdata);
	}

	writeByteArray(emptyAddress, asm_code + makeByteArray({ 195 }));
	writeByteArray(HOOKasm, makeByteArray({ 255, 37, 0, 0, 0, 0 }) + longToBytes(asmTransfer) + makeByteArray({ 144, 144, 144, 144, 144 }));
	writeInt(logicAddress, 1);
	while (readInt(logicAddress) == 1)
	{
		Sleep(5);
		handleEvents();
	}
	writeByteArray(HOOKasm, HOOKraw);
	writeByteArray(emptyAddress, makeEmptyByteArray((int)asm_code.size() + 16));
	asyncExecute = false;
}

// 加密
void encrypt(__int64 address, int value)
{
	__int64 data = value + 4;
	data = 0x1F2A025C ^ data;
	// 写长整数
	writeLong(address, data);
}

// 解密
__int64 decrypt(__int64 address)
{
	__int64 data = readLong(address) ^ 0x1F2A025C;
	data -= 4;
	return data;
}

// 解密方法2
__int64 decrypt2(__int64 address)
{
	__int64 value = readInt(address);
	value = value ^ 0x1F2A025C & 0xffffffff;
	value -= 4;
	return value;
}