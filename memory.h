#pragma once
#include "common.h"
#include <vector>
#include "string"


using namespace std;

__int64 readLong(__int64 address);

bool writeByteArray(__int64 address, vector<byte> Data);

vector<byte> cstringToBytes(wstring str);