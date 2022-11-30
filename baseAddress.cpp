#include "pch.h"
#include "baseAddress.h"

DNF64 BaseAddress::x64(const string& key)
{
	return address_list.at(key).get<DNF64>();
}

DNF32 BaseAddress::x32(const string& key)
{
	return address_list.at(key).get<DNF32>();
}

DNF16 BaseAddress::x16(const string& key)
{
	return address_list.at(key).get<DNF16>();
}

DNF8 BaseAddress::x8(const string& key)
{
	return address_list.at(key).get<DNF8>();
}

BaseAddress ADDR;