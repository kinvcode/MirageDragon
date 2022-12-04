#pragma once
#include <string>

using namespace std;

// const string 转 const char*
//const char* s2ccp(const string&str) 
//{
//	return str.c_str();
//}

// CString 转 string
// unicode:
//CString str1 = L"山东大卵男";
//string str2 = CW2A(str1.GetString());
// assic:


// string 转 CString
//1. CString = string.c_str()
//2. CString = CString(string.c_str());

// CString 转 const char*
//const char* str = (LPCSTR)(LPCTSTR)cstring;