#include "pch.h"
#include "common.h"
#include "memory.h"

DWORD PID;

// ��int����
int readInt(__int64 address)
{
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	if (address == 0 || handle == NULL)
	{
		return 0;
	}

	int value = 0;
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
	if (address == 0 || handle == NULL)
	{
		return false;
	}

	if (WriteProcessMemory(handle, (void*)address, &value, 4, NULL)) {
		CloseHandle(handle);
		return true;
	}
	return false;
}

// ��ȡlong����
__int64 readLong(__int64 address)
{
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	if (address == 0 || handle == NULL)
	{
		return -1;
	}

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
	if (address == 0 || handle == NULL)
	{
		return false;
	}

	bool res = WriteProcessMemory(handle, (void*)address, &value, 8, NULL);
	CloseHandle(handle);
	return res;
}

// ��ȡ������
float readFloat(__int64 address)
{
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	if (address == 0 || handle == NULL)
	{
		return 0;
	}

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
	if (address == 0 || handle == NULL)
	{
		return false;
	}
	bool res = WriteProcessMemory(handle, (void*)address, &value, 4, NULL);
	CloseHandle(handle);
	return res;
}

// ��ȡ�ֽ�����
vector<byte> readByteArray(__int64 address, int length)
{
	vector<byte> result;

	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	if (address == 0 || handle == NULL)
	{
		return result;
	}

	byte* tempResult;
	tempResult = new byte[length];
	memset(tempResult, 0, length);
	ReadProcessMemory(handle, (LPCVOID)address, tempResult, length, NULL);
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
	if (address == 0 || handle == NULL)
	{
		return false;
	}

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

// �ֽ�����תwstring
wstring bytesToWstring(vector<byte> data)
{
	// �������ַ���
	wstring wstr;
	// ��ȡ�ַ����ȣ�������\0��;
	size_t length = data.size() - 2;
	// ���������ַ�����С
	wstr.resize(length / 2, 0);
	// ��ȡ�ַ���ָ��
	byte* pByte = (byte*)wstr.data();
	// ����ַ���
	for (size_t i = 0; i < length; i++)
	{
		*pByte = data[i];
		pByte++;
	}

	return wstr;
}

// �����ֽ�����
vector<byte> makeByteArray(vector<byte> data)
{
	return data;
}

// ����ָ�����ȵ��ֽ�����
vector<byte> makeEmptyByteArray(int length)
{
	vector<byte> data;
	for (int i = 0; i < length; i++)
	{
		data.push_back(NULL);
	}

	return data;
}

// intת�ֽ�����
vector<byte> intToBytes(int length) {
	__int64  adr = (__int64)&length;
	vector<byte>c;
	for (size_t i = 0; i < 4; i++) {
		c.push_back(*(byte*)adr++);
	}
	return c;
}

// longת�ֽ�����
vector<byte> longToBytes(__int64 length) {
	__int64  adr = (__int64)&length;
	vector<byte>c;
	for (size_t i = 0; i < 8; i++) {
		c.push_back(*(byte*)adr++);
	}
	return c;
}

// ���������
vector<byte> operator+(vector<byte> a, vector<byte> b)
{
	for (int i = 0; i < b.size(); i++)
	{
		a.push_back(b[i]);
	}

	b.clear();

	return a;
}

