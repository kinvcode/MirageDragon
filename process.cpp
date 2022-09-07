#include "pch.h"
#include "process.h"

using namespace std;

int getProcessPID(wchar_t Name[])
{

	PROCESSENTRY32 Processinformation = { 0 };
	BOOL Processhandle;
	HANDLE Processsnapshot;

	Processsnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (Processsnapshot != 0)
	{
		Processinformation.dwSize = sizeof(PROCESSENTRY32);
		Processhandle = Process32First(Processsnapshot, &Processinformation);
		while (Processhandle)
		{
			if (lstrcmpi(Name, Processinformation.szExeFile) == 0)
			{
				CloseHandle(Processsnapshot);

				return Processinformation.th32ProcessID;
			}
			Processhandle = Process32Next(Processsnapshot, &Processinformation);
		}
		CloseHandle(Processsnapshot);

	}
	return 0;
}

int asSystem(int Pid, char FileName[])
{

	BOOLEAN  nEn = 0;
	SIZE_T cbAttributeListSize = 0;
	HANDLE ProcessHandle = 0;
	PPROC_THREAD_ATTRIBUTE_LIST ps = 0;
	STARTUPINFOEXA si;
	ZeroMemory(&si, sizeof(si));
	si.StartupInfo.cb = sizeof(si);

	typedef LONG(*type_RtlAdjustPrivilege)(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);
	type_RtlAdjustPrivilege RtlAdjustPrivilege = (type_RtlAdjustPrivilege)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "RtlAdjustPrivilege");
	RtlAdjustPrivilege(20, 1, 0, &nEn);
	ProcessHandle = OpenProcess(2035711, FALSE, Pid);
	InitializeProcThreadAttributeList(NULL, 1, 0, &cbAttributeListSize);
	char* temp = new char[cbAttributeListSize];
	PPROC_THREAD_ATTRIBUTE_LIST AttributeList = (LPPROC_THREAD_ATTRIBUTE_LIST)temp;
	InitializeProcThreadAttributeList(AttributeList, 1, 0, &cbAttributeListSize);

	if (!UpdateProcThreadAttribute(AttributeList, 0, 131072, &ProcessHandle, sizeof(HANDLE), NULL, NULL))
	{
		MessageBoxW(NULL, L"UpdateProcThreadAttribute Ê§°Ü !", L"Á¹³Ç", MB_OK);
		ExitProcess(0);
	}
	si.lpAttributeList = AttributeList;
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
	si.StartupInfo.cb = 68;
	si.StartupInfo.dwFlags = 0;
	si.StartupInfo.wShowWindow = 1;
	if (CreateProcessAsUserA(NULL, 0, FileName, 0, 0, 0, 524288, 0, 0, (LPSTARTUPINFOA)&si, &pi))
	{
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else
	{
		MessageBoxW(NULL, L"CreateProcessAsUserA Ê§°Ü!", L"Á¹³Ç", MB_OK);
		ExitProcess(0);
	}
	CloseHandle(ProcessHandle);
	DeleteProcThreadAttributeList(AttributeList);
	delete temp;
	return pi.dwProcessId;
}

void asSystemRunning()
{
	CString sysName = L"SYSTEM";
	DWORD dwSize = MAX_PATH;
	TCHAR* pszName = new TCHAR[dwSize];
	GetUserName(pszName, &dwSize);
	if (pszName != sysName) {
		// ¹Ø±Õcmd
		TerminateProcess(OpenProcess(PROCESS_ALL_ACCESS, FALSE, getProcessPID(L"cmd.exe")), 0);
		char path[MAX_PATH];
		memset(path, 0, MAX_PATH);
		DWORD nSize = ::GetModuleFileNameA(NULL, path, MAX_PATH);
		//WinExec("C:\\Windows\\System32\\cmd.exe", SW_HIDE);
		CreateProcess(L"C:\\Windows\\System32\\cmd.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, 0, 0);
		asSystem(getProcessPID(L"winlogon.exe"), path);
		ExitProcess(0);
		TerminateProcess(OpenProcess(PROCESS_ALL_ACCESS, FALSE, getProcessPID(L"cmd.exe")), 0);
	}
}