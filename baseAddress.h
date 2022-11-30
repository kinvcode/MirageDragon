#pragma once
#include "json.h"

#ifndef ADDR_H
#define ADDR_H

typedef __int64 DNF64;
typedef int DNF32;
typedef short DNF16;
typedef char DNF8;

#endif

class BaseAddress 
{
public:
	json address_list;

	DNF64 x64(const string& key);
	DNF32 x32(const string& key);
	DNF16 x16(const string& key);
	DNF8 x8(const string& key);
};

extern BaseAddress ADDR;