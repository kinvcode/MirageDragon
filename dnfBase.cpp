#include "pch.h"
#include "dnfBase.h"

void memoryAssambly(vector<byte>asm_code)
{
	//// HOOK数据和原始数据
	//vector<byte>HOOKdata, HOOKraw;
	////异步执行
	//bool asyncExecute = false;
	////汇编中转,空白地址,判断地址
	//__int64 asmTransfer, emptyAddress, logicAddress, HOOKasm, HOOKret;
	//asmTransfer = C_EMPTY_ADDRESS + 300;
	//emptyAddress = C_EMPTY_ADDRESS + 500;
	//logicAddress = emptyAddress - 100;
	//if (asyncExecute) {
	//	return;
	//}
	//asyncExecute = true;
	//HOOKasm = C_ASM_CALL;
	//HOOKasm += 144;
	//HOOKret = HOOKasm + 19;
	//HOOKdata = readByteArray(HOOKasm, 19);
	//HOOKraw = HOOKdata;
	//HOOKdata = HOOKdata + makeByteArray({ 72,184 }) + intToBytes(logicAddress);
	//HOOKdata = HOOKdata + makeByteArray({ 131, 56, 1, 117, 42, 72, 129, 236, 0, 3, 0, 0 });
	//HOOKdata = HOOKdata + makeByteArray({ 72,187 }) + intToBytes(emptyAddress);
	//HOOKdata = HOOKdata + makeByteArray({ 255, 211 });
	//HOOKdata = HOOKdata + makeByteArray({ 72,184 }) + intToBytes(logicAddress);
	//HOOKdata = HOOKdata + makeByteArray({ 199, 0, 3, 0, 0, 0 });
	//HOOKdata = HOOKdata + makeByteArray({ 72, 129, 196, 0, 3, 0, 0 });
	//HOOKdata = HOOKdata + makeByteArray({ 255, 37, 0, 0, 0, 0 }) + intToBytes(HOOKret);
	//if (readInt(asmTransfer) == 0)
	//{
	//	writeByteArray(asmTransfer, HOOKdata);
	//}
	//writeByteArray(emptyAddress, asm_code + makeByteArray({ 195 }));
	//writeByteArray(HOOKasm, makeByteArray({ 255, 37, 0, 0, 0, 0 }) + intToBytes(asmTransfer) + makeByteArray({ 144, 144, 144, 144, 144 }));
	//writeLong(logicAddress, 1);
	//while (readLong(logicAddress) == 1)
	//{
	//	Sleep(5);
	//	handleEvents();
	//}
	//writeByteArray(HOOKasm, HOOKraw);
	//writeByteArray(emptyAddress, makeEmptyByteArray((int)asm_code.size() + 16));
	//asyncExecute = false;
}