#include "pch.h"
#include "process.h"
#include "MirageDragonDlg.h"
#include "GameData.h"
#include "log.h"
#include "procData.h"

using namespace std;

CString LastErrorMsg(CString lpszFunction)
{
	CString lpMsgBuf; // 错误原因
	DWORD dw = GetLastError();
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	CString msg;
	msg.Format(L"函数：%s 错误码：%d 错误消息：%s", lpszFunction.GetString(), dw, lpMsgBuf.GetString());

	return msg;
}

int getProcessPID(wchar_t Name[])
{
	//CMirageDragonDlg* pParentDlg = (CMirageDragonDlg*)GetParent()->GetParent();
	CString err_msg;

	PROCESSENTRY32 Processinformation = { 0 };
	BOOL Processhandle;
	HANDLE Processsnapshot;

	Processsnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (Processsnapshot == INVALID_HANDLE_VALUE) {
		// 进程快照获取失败
		err_msg = LastErrorMsg(L"CreateToolhelp32Snapshot");
		//pParentDlg->Log(err_msg);
	}

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

	return 0;
}

int asSystem(int Pid, char FileName[])
{

	BOOLEAN  nEn = 0;
	SIZE_T cbAttributeListSize = 0; // 接收所需缓存区大小（用于存放提权属性列表大小）
	HANDLE ProcessHandle = 0;
	PPROC_THREAD_ATTRIBUTE_LIST ps = 0;
	STARTUPINFOEXA si;
	ZeroMemory(&si, sizeof(si));
	si.StartupInfo.cb = sizeof(si);

	// 获取提权函数
	typedef LONG(*type_RtlAdjustPrivilege)(ULONG Privilege, BOOLEAN Enable, BOOLEAN CurrentThread, PBOOLEAN Enabled);
	type_RtlAdjustPrivilege RtlAdjustPrivilege = (type_RtlAdjustPrivilege)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "RtlAdjustPrivilege");
	RtlAdjustPrivilege(20, 1, 0, &nEn);

	// 打开winlogon进程
	ProcessHandle = OpenProcess(2035711, FALSE, Pid);
	// 获取提权属性大小
	InitializeProcThreadAttributeList(NULL, 1, 0, &cbAttributeListSize);
	// 申请属性列表空间
	char* temp = new char[cbAttributeListSize];
	// 属性类型转换
	PPROC_THREAD_ATTRIBUTE_LIST AttributeList = (LPPROC_THREAD_ATTRIBUTE_LIST)temp;
	// ??
	InitializeProcThreadAttributeList(AttributeList, 1, 0, &cbAttributeListSize);
	// 更新权限为131072
	if (!UpdateProcThreadAttribute(AttributeList, 0, 131072, &ProcessHandle, sizeof(HANDLE), NULL, NULL))
	{
		MessageBoxW(NULL, L"更新进程属性失败 !", L"幻镜龙", MB_OK);
		ExitProcess(0);
	}
	// 
	si.lpAttributeList = AttributeList;
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
	si.StartupInfo.cb = 68;
	si.StartupInfo.dwFlags = 0;
	si.StartupInfo.wShowWindow = 1;

	// 创建目标进程
	if (CreateProcessAsUserA(NULL, 0, FileName, 0, 0, 0, 524288, 0, 0, (LPSTARTUPINFOA)&si, &pi))
	{
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else
	{
		MessageBoxW(NULL, L"创建新进程失败!", L"幻镜龙", MB_OK);
		ExitProcess(0);
	}
	CloseHandle(ProcessHandle);
	DeleteProcThreadAttributeList(AttributeList);
	delete[] temp;
	return pi.dwProcessId;
}

void asSystemRunning()
{
	CString sysName = L"SYSTEM";
	DWORD dwSize = MAX_PATH;
	TCHAR* pszName = new TCHAR[dwSize];
	GetUserName(pszName, &dwSize);
	if (pszName != sysName) {
		// 关闭cmd
		TerminateProcess(OpenProcess(PROCESS_ALL_ACCESS, FALSE, getProcessPID(L"cmd.exe")), 0);
		char path[MAX_PATH];
		memset(path, 0, MAX_PATH);
		DWORD nSize = ::GetModuleFileNameA(NULL, path, MAX_PATH);
		//WinExec("C:\\Windows\\System32\\cmd.exe", SW_HIDE);
		CreateProcess(L"C:\\Windows\\System32\\cmd.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, 0, 0);
		asSystem(getProcessPID(L"winlogon.exe"), path);
		TerminateProcess(OpenProcess(PROCESS_ALL_ACCESS, FALSE, getProcessPID(L"cmd.exe")), 0);
		ExitProcess(0);
	}

	delete[] pszName;
}

void asDNFUserRunning()
{
	int dnf_pid = getProcessPID(L"DNF.exe");
	if (dnf_pid == 0)
	{
		AfxMessageBox(L"DNF未启动");
		ExitProcess(0);
	}

	bool is_dnf_child = false;

	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe = { 0 };
	pe.dwSize = sizeof(PROCESSENTRY32);
	int self_pid = GetCurrentProcessId();

	if (Process32First(h, &pe)) {
		do {
			if (pe.th32ProcessID == self_pid) {
				if (dnf_pid == pe.th32ParentProcessID)
				{
					is_dnf_child = true;
				}
			}
		} while (Process32Next(h, &pe));
	}

	if (!is_dnf_child) {
		// 关闭cmd
		TerminateProcess(OpenProcess(PROCESS_ALL_ACCESS, FALSE, getProcessPID(L"cmd.exe")), 0);
		char path[MAX_PATH];
		memset(path, 0, MAX_PATH);
		DWORD nSize = ::GetModuleFileNameA(NULL, path, MAX_PATH);
		//WinExec("C:\\Windows\\System32\\cmd.exe", SW_HIDE);
		CreateProcess(L"C:\\Windows\\System32\\cmd.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, 0, 0);
		asSystem(dnf_pid, path);
		TerminateProcess(OpenProcess(PROCESS_ALL_ACCESS, FALSE, getProcessPID(L"cmd.exe")), 0);
		ExitProcess(0);
	}
}

// 判断DNF是否是置顶窗口
bool windowIsTop()
{
	HWND cur_top_window = GetForegroundWindow();
	PDATA.dnf.dnf_window = FindWindowA(NULL, "地下城与勇士");


	GetWindowRect(PDATA.dnf.dnf_window, &PDATA.dnf.dnf_rect);

	//HDC hDc = NULL;
	//GAME.dnf_hDc = GetDC(GAME.dnf_window); //获取DC
	//if (!GetWindowRect(GAME.dnf_window, &GAME.dnf_rect))
	//{
	//	Log.info(L"获取DNF窗口位置失败", true);
	//}

	return cur_top_window == PDATA.dnf.dnf_window;
}

// 程序延迟
bool programDelay(int time, int delayUnit)
{
	INT_PTR Interval = time, Unit = 0;
	if (!Interval && !Unit)
	{
		Interval = 5000 * 60 * 60;
		Unit = 5000 * 60 * 60;
	}
	else {
		switch (delayUnit)
		{
		case 0:
			Unit = 1;
			break;
		case 1:
			Unit = 1000;
			break;
		case 2:
			Unit = 1000 * 60;
			break;
		case 3:
			Unit = 1000 * 60 * 60;
			break;
		default:
			break;
		}
	}

	HANDLE handle[1];
	handle[0] = CreateWaitableTimerW(NULL, false, NULL);
	LARGE_INTEGER lpDueTime;
	lpDueTime.QuadPart = -10 * Interval * 1000 * Unit;
	SetWaitableTimer(handle[0], &lpDueTime, 0, NULL, NULL, false);
	DWORD nCount = sizeof(handle) / sizeof(HANDLE);
	while (MsgWaitForMultipleObjects(nCount, handle, false, -1, (QS_KEY | QS_MOUSEMOVE | QS_MOUSEBUTTON | QS_POSTMESSAGE | QS_TIMER | QS_PAINT | QS_SENDMESSAGE | QS_HOTKEY)) != WAIT_OBJECT_0)
	{
		// 系统处理事件
		MSG msg;
		while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	CloseHandle(handle[0]);
	return true;
}

// 事件处理
void handleEvents()
{
	CWinThread* pWinThread = AfxGetThread();
	if (pWinThread != NULL)
	{
		MSG msg;
		while (::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE) && msg.message != WM_QUIT)
		{
			AfxGetThread()->PumpMessage();
		}
	}
}

// 重新启动应用
void reloadProcess()
{
	ExitProcess(0);

	//char path[MAX_PATH];
	//memset(path, 0, MAX_PATH);
	//DWORD nSize = ::GetModuleFileNameA(NULL, path, MAX_PATH);
	//if (nSize != 0)
	//{
	//	UINT status = WinExec(path, SW_SHOW);
	//	ExitProcess(0);
	//}
}

CString getCurDir() 
{
	TCHAR szFilePath[MAX_PATH + 1] = { 0 };
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	(_tcsrchr(szFilePath, _T('\\')))[1] = 0;
	CString str_url = szFilePath;
	return str_url;
}