
// MirageDragon.cpp: 定义应用程序的类行为。
//

#include "pch.h"
#include "framework.h"
#include "MirageDragon.h"
#include "MirageDragonDlg.h"
#include "json.h"
#include "http.h"
#include "baseAddress.h"
#include "jobs.h"
#include "log.h"
#include "dataConvert.h"
#include "dataStruct.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMirageDragonApp

BEGIN_MESSAGE_MAP(CMirageDragonApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMirageDragonApp 构造

CMirageDragonApp::CMirageDragonApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的 CMirageDragonApp 对象

CMirageDragonApp theApp;

// CMirageDragonApp 初始化

BOOL CMirageDragonApp::InitInstance()
{

	// 提取启动方式
	//asSystemRunning();
	asDNFUserRunning();

	// 初始化基址
	initBaseAddress();

	// 如果一个运行在 Windows XP 上的应用程序清单指定要使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	// 基类初始化
	CWinApp::InitInstance();
	// OLE初始化
	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager* pShellManager = new CShellManager;

	// 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 绑定主窗口
	CMirageDragonDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		//  “确定”来关闭对话框的代码
}
	else if (nResponse == IDCANCEL)
	{
		//  “取消”来关闭对话框的代码
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，而不是启动应用程序的消息泵。
	return FALSE;
}

void CMirageDragonApp::initBaseAddress()
{
	// 加载基址
	json data = http.getBaseAddress();

	if (data.size() < 1)
	{
		MessageBoxW(NULL, L"基址列表未更新！", L"幻镜龙", MB_OK);
		ExitProcess(0);
	}

	for (json::iterator it = data.begin(); it != data.end(); ++it) {
		ADDR.address_list.push_back(json::object_t::value_type(it.value().at("const_name"), it.value().at("address")));
	}
}