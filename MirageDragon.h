
// MirageDragon.h: PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含 'pch.h' 以生成 PCH"
#endif

#include "resource.h"		// 主符号


// CMirageDragonApp:
// 有关此类的实现，请参阅 MirageDragon.cpp
//

class CMirageDragonApp : public CWinApp
{
public:
	CMirageDragonApp();

	// 进程数据：进程ID、进程运行状态、窗口置顶状态
	// 游戏数据：qq号、角色ID、角色疲劳值（总值、当前值）
	// 当前副本信息：副本信息 + 当前房间信息

	// 主窗口控件

// 重写
public:
	virtual BOOL InitInstance();
	
	void initBaseAddress();
// 实现

	DECLARE_MESSAGE_MAP()
};

extern CMirageDragonApp theApp;
