// PAGE1.cpp: 实现文件
//

#include "pch.h"
#include "MirageDragon.h"
#include "PAGE1.h"
#include "afxdialogex.h"
#include "MirageDragonDlg.h"
#include "dnfCALL.h"
#include "dnfUser.h"
#include "dnfPacket.h"
#include "dnfData.h"
#include "scripts.h"
#include "baseAddress.h"
#include "GameData.h"
#include "http.h"
#include "keyboardDriver.h"
#include "log.h"
#include "screenshot.h"
#include "opencv.h"
#include "procData.h"
#include "dnfBase.h"
///////////// 测 试 区 域 /////////////////
#include "mainline.h"

// PAGE1 对话框

IMPLEMENT_DYNAMIC(PAGE1, CDialogEx)

PAGE1::PAGE1(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_PAGE1, pParent)
{

}

PAGE1::~PAGE1()
{
}

void PAGE1::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK1, _switch_three_speed);
	DDX_Control(pDX, IDC_CHECK2, _switch_score);
	DDX_Control(pDX, IDC_CHECK3, _switch_hidden_user);
	DDX_Control(pDX, IDC_CHECK6, _switch_cool_down);
	DDX_Control(pDX, IDC_CHECK7, _switch_hook_damage);
	DDX_Control(pDX, IDC_CHECK4, _switch_gather_monster);
	DDX_Control(pDX, IDC_CHECK5, _switch_gather_items);
	DDX_Control(pDX, IDC_COMBO2, m_ctl_sel_autoplay);
	DDX_Control(pDX, IDC_COMBO3, m_ctl_sel_town);
	DDX_Control(pDX, IDC_EDIT2, m_ctl_mapNumber);
}


BEGIN_MESSAGE_MAP(PAGE1, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_CBN_SELCHANGE(IDC_COMBO2, &PAGE1::OnCbnSelchangeCombo2)
	ON_BN_CLICKED(IDC_BUTTON1, &PAGE1::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &PAGE1::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &PAGE1::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &PAGE1::OnBnClickedButton4)
	ON_CBN_SELCHANGE(IDC_COMBO3, &PAGE1::OnCbnSelchangeCombo3)
	ON_BN_CLICKED(IDC_BUTTON5, &PAGE1::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_CHECK1, &PAGE1::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK2, &PAGE1::OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_CHECK3, &PAGE1::OnBnClickedCheck3)
	ON_BN_CLICKED(IDC_CHECK4, &PAGE1::OnBnClickedCheck4)
	ON_BN_CLICKED(IDC_CHECK5, &PAGE1::OnBnClickedCheck5)
	ON_BN_CLICKED(IDC_CHECK6, &PAGE1::OnBnClickedCheck6)
	ON_BN_CLICKED(IDC_CHECK7, &PAGE1::OnBnClickedCheck7)
	ON_EN_UPDATE(IDC_EDIT2, &PAGE1::OnEnUpdateEdit2)
	ON_BN_CLICKED(IDC_BUTTON6, &PAGE1::OnBnClickedButton6)
END_MESSAGE_MAP()


// PAGE1 消息处理程序

BOOL PAGE1::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 加载功能配置
	// 初始化图内功能
	_switch_score.SetCheck(BST_CHECKED);
	_switch_three_speed.SetCheck(BST_CHECKED);
	_switch_gather_monster.SetCheck(BST_CHECKED);
	_switch_gather_items.SetCheck(BST_CHECKED);
	_switch_cool_down.SetCheck(BST_CHECKED);
	_switch_hook_damage.SetCheck(BST_CHECKED);
	_switch_hidden_user.SetCheck(BST_CHECKED);

	// 初始化刷图方式
	m_ctl_sel_autoplay.InsertString(0, L"自动刷图");
	m_ctl_sel_autoplay.InsertString(1, L"自动剧情");
	m_ctl_sel_autoplay.SetCurSel(0);

	// 初始化城镇列表
	initTown();

	// 初始化副本列表
	m_ctl_mapNumber.SetWindowText(L"100002962");

	return TRUE;
}

BOOL PAGE1::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(&rect);
	CBrush myBrush(RGB(0, 255, 255));
	CBrush* pOld = pDC->SelectObject(&myBrush);
	BOOL bRes = pDC->PatBlt(0, 0, rect.Width(), rect.Height(), PATCOPY);
	pDC->SelectObject(pOld);
	return bRes;
}


void PAGE1::OnCbnSelchangeCombo2()
{
	CMirageDragonDlg* pParentDlg = (CMirageDragonDlg*)GetParent()->GetParent();

	if (m_ctl_sel_autoplay.GetCurSel() == 1) {
		pParentDlg->Log(L"切换为自动剧情模式");
	}
	else {
		pParentDlg->Log(L"切换为自动刷图模式");
	}
}


void PAGE1::OnBnClickedButton1()
{
	CMirageDragonDlg* pParentDlg = (CMirageDragonDlg*)GetParent()->GetParent();

	if (GAME.game_status != 3) {
		pParentDlg->Log(L"请在图内使用该功能");
		return;
	}

	skillCall(readLong(ADDR.x64("C_USER_ADDRESS")), 51200, 999999, 500, 500, 0, 0);
}


void PAGE1::OnBnClickedButton2()
{
	CMirageDragonDlg* pParentDlg = (CMirageDragonDlg*)GetParent()->GetParent();

	if (GAME.game_status != 3) {
		pParentDlg->Log(L"请在图内使用该功能");
		return;
	}

	if (GAME.penetrate_status) {
		penetrate(false);
		GAME.penetrate_status = false;
	}
	else {
		penetrate(true);
		GAME.penetrate_status = true;
	}
}


void PAGE1::OnBnClickedButton3()
{
	CMirageDragonDlg* pParentDlg = (CMirageDragonDlg*)GetParent()->GetParent();

	if (GAME.game_status != 3) {
		pParentDlg->Log(L"请在图内使用该功能");
		return;
	}
	// 读取人物坐标
	COORDINATE user = readCoordinate(readLong(ADDR.x64("C_USER_ADDRESS")));
	wstring map_name = getMapName();
	int map_number = getMapNumber();
	CString msg;
	msg.Format(L"副本名称：%s，副本编号：%d，坐标X：%d，坐标Y：%d，坐标Z：%d", map_name.c_str(), map_number, user.x, user.y, user.z);
	pParentDlg->Log(msg);
}


void PAGE1::OnBnClickedButton4()
{
	CMirageDragonDlg* pParentDlg = (CMirageDragonDlg*)GetParent()->GetParent();

	if (GAME.game_status != 1) {
		pParentDlg->Log(L"请在城镇中使用该功能");
		return;
	}

	int word, area, X, Y;
	word = readInt(ADDR.x64("C_TOWN_WORLD"));
	area = readInt(ADDR.x64("C_TOWN_AREA"));
	X = (int)readFloat(ADDR.x64("C_TOWN_X"));
	Y = (int)readFloat(ADDR.x64("C_TOWN_Y"));

	CString msg;
	msg.Format(L"大区域：%d，小区域：%d，坐标X：%d，坐标Y：%d", word, area, X, Y);
	pParentDlg->Log(msg);
}


void PAGE1::OnCbnSelchangeCombo3()
{
	CMirageDragonDlg* pParentDlg = (CMirageDragonDlg*)GetParent()->GetParent();

	if (GAME.game_status != 1) {
		pParentDlg->Log(L"请在城镇中使用该功能");
		return;
	}

	int index = m_ctl_sel_town.GetCurSel();
	TOWNCOOR town = town_list[index];
	moveOfTown(town.word, town.area, town.x, town.y);
	pParentDlg->Log(L"已完成城镇移动");
}


// 测试按钮
void PAGE1::OnBnClickedButton5()
{
	CMirageDragonDlg* pParentDlg = (CMirageDragonDlg*)GetParent()->GetParent();

	// 获取角色名称
	//CString name = readString(readLong(readLong(0x149CCF220)+0x850), 12);
	//getRoleList();
	//ScreenShot::savePic();

	//OpenCV::read();

	//Log.info(L"开始移动", true);
	//for (int i = 0; i < 10; i++)
	//{
	//	Log.info(L"向右移动", true);
	//	MSDK_MoveTo(rect.left - 100, rect.top);
	//	Sleep(200);
	//	Log.info(L"向左移动", true);
	//	MSDK_MoveTo(rect.left - 100, rect.bottom);
	//	programDelay(1500, 0);
	//}
	//return;

	MainLineLogic::test();
}


void PAGE1::OnBnClickedCheck1()
{
	// 三速
	if (_switch_three_speed.GetCheck() == BST_CHECKED)
	{
		GAME.function_switch.three_speed = true;
	}
	else {
		GAME.function_switch.three_speed = false;
	}
}


void PAGE1::OnBnClickedCheck2()
{
	// 评分
	if (_switch_score.GetCheck() == BST_CHECKED)
	{
		GAME.function_switch.score = true;
	}
	else {
		GAME.function_switch.score = false;
	}
}


void PAGE1::OnBnClickedCheck3()
{
	// 透明
	if (_switch_hidden_user.GetCheck() == BST_CHECKED)
	{
		GAME.function_switch.hidden_user = true;
	}
	else {
		GAME.function_switch.hidden_user = false;
	}
}


void PAGE1::OnBnClickedCheck4()
{
	// 聚怪
	if (_switch_gather_monster.GetCheck() == BST_CHECKED)
	{
		GAME.function_switch.gather_monster = true;
	}
	else {
		GAME.function_switch.gather_monster = false;
	}
}


void PAGE1::OnBnClickedCheck5()
{
	// 聚集物品
	if (_switch_gather_items.GetCheck() == BST_CHECKED)
	{
		GAME.function_switch.gather_items = true;
	}
	else {
		GAME.function_switch.gather_items = false;
	}
}


void PAGE1::OnBnClickedCheck6()
{
	// 技能冷却
	if (_switch_cool_down.GetCheck() == BST_CHECKED)
	{
		GAME.function_switch.cool_down = true;
	}
	else {
		GAME.function_switch.cool_down = false;
	}
}


void PAGE1::OnBnClickedCheck7()
{
	// HOOK伤害
	if (_switch_hook_damage.GetCheck() == BST_CHECKED)
	{
		GAME.function_switch.hook_damage = true;
		hookDamage(true);
	}
	else {
		hookDamage(false);
		GAME.function_switch.hook_damage = false;
	}
}


void PAGE1::OnEnUpdateEdit2()
{
	CMirageDragonDlg* pParentDlg = (CMirageDragonDlg*)GetParent()->GetParent();

	pParentDlg->Log(L"改变了地图值");

	CString code;
	m_ctl_mapNumber.GetWindowText(code);

	GAME.autoMapNumber = _wtoi(code);
}


void PAGE1::OnBnClickedButton6()
{
	// TODO: 在此添加控件通知处理程序代码

	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PDATA.dnf.PID);
	if (handle == NULL)
	{
		return;
	}
	int length = 200 * 1024 * 1024;
	byte* tempResult;
	tempResult = new byte[length];
	memset(tempResult, 0, length);
	ReadProcessMemory(handle, (LPCVOID)0x140000000, tempResult, length, NULL);

	CFile dump;
	if (!dump.Open(L"dump.exe", CFile::modeWrite | CFile::modeNoTruncate | CFile::modeCreate))
	{
		return;
	}

	dump.Write(tempResult, length);
	dump.Close();
	delete[] tempResult;
	CloseHandle(handle);
	AfxMessageBox(L"提取完毕！");
}

void PAGE1::initTown()
{
	// 获取API数据
	json list = http.getTownCoordinates();
	// 自动转换
	town_list = list.get<vector<TOWNCOOR>>();

	int length = (int)town_list.size();
	for (int i = 0; i < length; i++)
	{
		m_ctl_sel_town.InsertString(i, town_list[i].name);
	}

	m_ctl_sel_town.SetCurSel(0);
}