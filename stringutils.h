#pragma once

typedef std::string String;
typedef std::wstring WString;

class StringUtils
{
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
};