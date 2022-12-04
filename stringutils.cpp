#include "pch.h"
#include "stringutils.h"

//Conversion functions taken from: http://www.nubaria.com/en/blog/?p=289
String StringUtils::Utf16ToUtf8(const WString& wstr)
{
	return Utf16ToUtf8(wstr.c_str());
}

String StringUtils::Utf16ToUtf8(const wchar_t* wstr)
{
	String convertedString;
	if (!wstr || !*wstr)
		return convertedString;
	auto requiredSize = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, nullptr, 0, nullptr, nullptr);
	if (requiredSize > 0)
	{
		convertedString.resize(requiredSize - 1);
		if (!WideCharToMultiByte(CP_UTF8, 0, wstr, -1, (char*)convertedString.c_str(), requiredSize, nullptr, nullptr))
			convertedString.clear();
	}
	return convertedString;
}

WString StringUtils::Utf8ToUtf16(const String& str)
{
	return Utf8ToUtf16(str.c_str());
}

WString StringUtils::Utf8ToUtf16(const char* str)
{
	WString convertedString;
	if (!str || !*str)
		return convertedString;
	int requiredSize = MultiByteToWideChar(CP_UTF8, 0, str, -1, nullptr, 0);
	if (requiredSize > 0)
	{
		convertedString.resize(requiredSize - 1);
		if (!MultiByteToWideChar(CP_UTF8, 0, str, -1, (wchar_t*)convertedString.c_str(), requiredSize))
			convertedString.clear();
	}
	return convertedString;
}

String StringUtils::LocalCpToUtf8(const CString& str)
{
	String convertedString = CW2A(str);
	return LocalCpToUtf8(convertedString.c_str());
}

String StringUtils::LocalCpToUtf8(const String& str)
{
	return LocalCpToUtf8(str.c_str());
}

String StringUtils::LocalCpToUtf8(const char* str)
{
	return Utf16ToUtf8(LocalCpToUtf16(str).c_str());
}

WString StringUtils::LocalCpToUtf16(const String& str)
{
	return LocalCpToUtf16(str.c_str());
}

WString StringUtils::LocalCpToUtf16(const char* str)
{
	WString convertedString;
	if (!str || !*str)
		return convertedString;
	int requiredSize = MultiByteToWideChar(CP_ACP, 0, str, -1, nullptr, 0);
	if (requiredSize > 0)
	{
		convertedString.resize(requiredSize - 1);
		if (!MultiByteToWideChar(CP_ACP, 0, str, -1, (wchar_t*)convertedString.c_str(), requiredSize))
			convertedString.clear();
	}
	return convertedString;
}

String StringUtils::Utf16ToLocalCp(const WString& str)
{
	String convertedString;
	if (str.size() == 0)
		return convertedString;
	int requiredSize = WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (requiredSize > 0)
	{
		convertedString.resize(requiredSize - 1);
		if (!WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, (char*)convertedString.c_str(), requiredSize, nullptr, nullptr))
			convertedString.clear();
	}
	return convertedString;
}
