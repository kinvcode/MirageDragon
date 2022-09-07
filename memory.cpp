#include "pch.h"
#include "memory.h"

DWORD PID = 0;

__int64 readLong(__int64 address)
{
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	__int64 value;
	bool res = ReadProcessMemory(handle, (void*)address, &value, 8, NULL);
	if (ReadProcessMemory(handle, (void*)address, &value, 8, NULL))
	{
		return value;
	}
	CloseHandle(handle);
	return 0;
}

bool writeByteArray(__int64 address, vector<byte> Data)
{
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	int length;
	length = (int)Data.size();
	byte* val = new byte[length];
	for (int i = 0; i < length; i++)
	{
		val[i] = Data[i];
	}
	bool res = WriteProcessMemory(handle, (LPVOID)address, (LPCVOID)val, length, NULL);
	CloseHandle(handle);
	return res;
}

vector<byte> cstringToBytes(wstring w_string)
{
	vector<byte> data;
	// 获取宽字符串指针
	const wchar_t* ptr1 = w_string.c_str();
	// 转为字符指针
	byte* pChar = (byte*)ptr1;
	// 获取字符长度（不包含\0）;
	size_t leng = w_string.size() * 2;
	// 字符放入到容器
	for (size_t i = 0; i < leng; i++) {
		data.push_back((byte) * (pChar + i));
	}

	data.push_back(0);
	data.push_back(0);
	return data;
}

