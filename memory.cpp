#include "pch.h"
#include "common.h"
#include "memory.h"

DWORD PID;

// ��int����
int readInt(__int64 address)
{
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	int value;
	if (ReadProcessMemory(handle, (void*)address, &value, 4, NULL))
	{
		return value;
	}
	CloseHandle(handle);
	return 0;
}

// д��int����
bool writeInt(__int64 address, __int64 value)
{
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	bool res = WriteProcessMemory(handle, (void*)address, &value, 4, NULL);
	CloseHandle(handle);
	return res;
}

// ��ȡlong����
__int64 readLong(__int64 address)
{
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	__int64 value;
	if (ReadProcessMemory(handle, (void*)address, &value, 8, NULL))
	{
		return value;
	}
	CloseHandle(handle);
	return 0;
}

// д��long����
bool writeLong(__int64 address, __int64 value)
{
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	bool res = WriteProcessMemory(handle, (void*)address, &value, 8, NULL);
	CloseHandle(handle);
	return res;
}

// ��ȡ������
float readFloat(__int64 address)
{
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	float value;
	if (ReadProcessMemory(handle, (void*)address, &value, 4, NULL))
	{
		return value;
	}
	CloseHandle(handle);
	return 0;
}

// д�븡������
bool writeFloat(__int64 address, float value)
{
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	bool res = WriteProcessMemory(handle, (void*)address, &value, 4, NULL);
	CloseHandle(handle);
	return res;
}

// ��ȡ�ֽ�����
vector<byte> readByteArray(__int64 address, int length)
{
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);

	byte* tempResult;
	tempResult = new byte[length];
	memset(tempResult, 0, length);
	ReadProcessMemory(handle, (LPCVOID)address, tempResult, length, NULL);
	vector<byte> result;
	result.resize(length);
	for (int i = 0; i < length; i++)
	{
		result[i] = tempResult[i];
	}

	CloseHandle(handle);
	return result;
}

// д���ֽ�����
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

// wstringת�ֽ�����
vector<byte> wstringToBytes(wstring w_string)
{
	vector<byte> data;
	// ��ȡ���ַ���ָ��
	const wchar_t* ptr1 = w_string.c_str();
	// תΪ�ַ�ָ��
	byte* pChar = (byte*)ptr1;
	// ��ȡ�ַ����ȣ�������\0��;
	size_t leng = w_string.size() * 2;
	// �ַ����뵽����
	for (size_t i = 0; i < leng; i++) {
		data.push_back((byte) * (pChar + i));
	}

	data.push_back(0);
	data.push_back(0);
	return data;
}

