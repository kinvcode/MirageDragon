#pragma once
#include "common.h"
#include <vector>
#include "string"


using namespace std;

int readInt(__int64 address);

bool writeInt(__int64 address, __int64 value);

__int64 readLong(__int64 address);

bool writeLong(__int64 address, __int64 value);

float readFloat(__int64 address);

bool writeFloat(__int64 address, float value);

vector<byte> readByteArray(__int64 address, int length);

bool writeByteArray(__int64 address, vector<byte> Data);

CString readString(__int64 address, int length);

bool writeString(__int64 address, CString text);

vector<byte> wstringToBytes(wstring w_string);

wstring bytesToWstring(vector<byte> data);

vector<byte> makeByteArray(vector<byte> data);

vector<byte> makeEmptyByteArray(int length);

vector<byte> intToBytes(int length);

vector<byte> longToBytes(__int64 length);

vector<byte> operator+(vector<byte> a, vector<byte> b);
