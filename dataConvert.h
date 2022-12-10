#pragma once
#include <string>
#include "json.h"

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


// string 转 CString
//1. CString = string.c_str()
//2. CString = CString(string.c_str());

// CString 转 const char*
//const char* str = (LPCSTR)(LPCTSTR)cstring;

typedef std::string String;
typedef std::wstring WString;

class DataConvert {
public:
	static String Utf16ToUtf8(const WString& wstr);
	static String Utf16ToUtf8(const wchar_t* wstr);
	static String Utf16ToLocalCp(const WString& wstr);
	static WString Utf8ToUtf16(const String& str);
	static WString Utf8ToUtf16(const char* str);
	static String LocalCpToUtf8(const CString& str);
	static String LocalCpToUtf8(const String& str);
	static String LocalCpToUtf8(const char* str);
	static WString LocalCpToUtf16(const String& wstr);
	static WString LocalCpToUtf16(const char* wstr);
	//static json toJsonRoleList(vector<ROLEINFO> list);
};