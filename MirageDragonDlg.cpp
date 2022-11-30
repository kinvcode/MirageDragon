
// MirageDragonDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MirageDragon.h"
#include "MirageDragonDlg.h"
#include "afxdialogex.h"
#include "jobs.h"
#include "common.h"
#include "keyboardDriver.h"
#include "baseAddress.h"
#include "GameData.h"

#include <gdiplus.h>
#pragma comment(lib,"gdiplus.lib")


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HANDLE MSDK_HANDLE;
bool is_auto_play = false;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMirageDragonDlg 对话框



CMirageDragonDlg::CMirageDragonDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MIRAGEDRAGON_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON2);
}

void CMirageDragonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_ctl_tab);
	DDX_Control(pDX, IDC_EDIT1, m_ctl_edit_console);
	DDX_Control(pDX, IDC_BUTTON1, m_ctl_btn_init);
}

BEGIN_MESSAGE_MAP(CMirageDragonDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMirageDragonDlg::OnBnClickedButton1)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CMirageDragonDlg::OnTcnSelchangeTab1)
	ON_WM_CTLCOLOR()
	ON_WM_HOTKEY()
END_MESSAGE_MAP()


// CMirageDragonDlg 消息处理程序

BOOL CMirageDragonDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// 设置窗口标题
	SetWindowText(L"幻镜龙");

	// 设置日志框字符数
	m_ctl_edit_console.SetLimitText(UINT_MAX);

	// 开启易键鼠按键功能
	MSDK_HANDLE = M_Open(1);
	if (MSDK_HANDLE != NULL) 
	{
		this->Log(L"键鼠设备已安装！");
	}

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMirageDragonDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMirageDragonDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		// 背景图片
		CPaintDC dc(this);
		CRect rect;
		GetClientRect(&rect);
		CBitmap bmpBackground;
		bmpBackground.LoadBitmap(IDB_BITMAP1);
		BITMAP m_bitmap;
		bmpBackground.GetBitmap(&m_bitmap);
		CDC dcBmp;
		dcBmp.CreateCompatibleDC(&dc);
		dcBmp.SelectObject(&bmpBackground);
		dc.SetStretchBltMode(HALFTONE);
		dc.StretchBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dcBmp, 0, 0, m_bitmap.bmWidth, m_bitmap.bmHeight, SRCCOPY);
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMirageDragonDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMirageDragonDlg::OnBnClickedButton1()
{
	// 获取DNF进程ID
	GLOBAL.PID = getProcessPID(L"DNF.exe");
	if (GLOBAL.PID == 0) {
		AfxMessageBox(L"请先运行游戏");
		return;
	}

	if (readInt(0x140000000) != 0x905A4D) {
		AfxMessageBox(L"无读写权限，请获取图标!");
		return;
	}

	// 显示控件
	m_ctl_btn_init.ShowWindow(SW_HIDE);
	m_ctl_edit_console.ShowWindow(SW_SHOW);
	m_ctl_tab.ShowWindow(SW_SHOW);

	// 初始化tab控件
	initTabCtl();

	// 热键管理
	// 初始化热键
	RegisterHotKey(GetSafeHwnd(), 1, NULL, VK_END); // 自动开关

	// 线程池管理
	// 启动数据更新线程
	//theApp.thread_update = AfxBeginThread(updateDataThread, this);
	// 启动刷图线程
	//theApp.thread_play = AfxBeginThread(playGameThead, this);
	
	//Log(L"初始化完毕");
	// 初始化线程
	initThreads();
}

void CMirageDragonDlg::initTabCtl()
{
	// 初始化tab控件
	m_ctl_tab.InsertItem(0, L"主界面");
	m_ctl_tab.InsertItem(1, L"配置界面");
	m_ctl_tab.InsertItem(2, L"地图遍历");
	page1.Create(IDD_DIALOG_PAGE1, &m_ctl_tab);
	page2.Create(IDD_DIALOG_PAGE2, &m_ctl_tab);
	page3.Create(IDD_DIALOG_PAGE3, &m_ctl_tab);
	CRect rc;
	m_ctl_tab.GetClientRect(rc);
	rc.top += 26;
	rc.bottom -= 0;
	rc.left += 0;
	rc.right -= 0;
	page1.MoveWindow(&rc);
	page2.MoveWindow(&rc);
	page3.MoveWindow(&rc);
	page1.ShowWindow(SW_SHOW);
	page2.ShowWindow(SW_HIDE);
	page3.ShowWindow(SW_HIDE);
	m_tab_box[0] = &page1;
	m_tab_box[1] = &page2;
	m_tab_box[2] = &page3;
	m_cur_tab_index = 0;
}


void CMirageDragonDlg::OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult)
{
	int index = m_ctl_tab.GetCurSel();
	int count = m_ctl_tab.GetItemCount();

	for (int i = 0; i < count; i++)
	{
		m_tab_box[i]->ShowWindow(SW_HIDE);
	}

	m_tab_box[index]->ShowWindow(SW_SHOW);

	*pResult = 0;
}

void CMirageDragonDlg::Log(CString msg)
{
	m_log_time = CTime::GetCurrentTime();
	CString time = m_log_time.Format("%H:%M:%S");
	msg = time + L" " + msg;
	m_ctl_edit_console.SetSel(-1);
	m_ctl_edit_console.ReplaceSel(msg);
	m_ctl_edit_console.SetSel(-1);
	m_ctl_edit_console.ReplaceSel(L"\r\n");
}

HBRUSH CMirageDragonDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	if (pWnd->GetDlgCtrlID() == IDC_EDIT1)
	{
		pDC->SetTextColor(RGB(171, 192, 35));//设置编辑框字体的颜色
		pDC->SetBkColor(RGB(43, 43, 43));//设置字体背景颜色
		return CreateSolidBrush(RGB(43, 43, 43));
	}

	if (pWnd->GetDlgCtrlID() == IDC_TAB1)
	{
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(0, 255, 0));
		return (HBRUSH)GetStockObject(HOLLOW_BRUSH);
	}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


void CMirageDragonDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	switch (nHotKeyId)
	{
	case 1:
		// 自动开关切换
		if (is_auto_play) {
			is_auto_play = false;
			closeRunKey();
			Log(L"关闭自动");
		}
		else {
			is_auto_play = true;
			Log(L"开启自动");
		}
		break;
	default:
		break;
	}

	__super::OnHotKey(nHotKeyId, nKey1, nKey2);
}

void CMirageDragonDlg::initThreads()
{
	// 数据线程
	thread_update = AfxBeginThread(updateDataThread, this);
	// 刷图线程
	thread_play = AfxBeginThread(playGameThead, this);
}
