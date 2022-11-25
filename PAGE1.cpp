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
#include "constant.h"

#include "scripts.h"

int auto_play_type = 1;
int penetrate_status = 0;
FUNCTIONSWITCH function_switch = { true,true,true,true,true,true,true };

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
END_MESSAGE_MAP()


// PAGE1 消息处理程序

BOOL PAGE1::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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
	m_ctl_sel_town.InsertString(0, L"诺顿");
	m_ctl_sel_town.InsertString(1, L"风振");
	m_ctl_sel_town.InsertString(2, L"教堂");
	m_ctl_sel_town.InsertString(3, L"寂静城");
	m_ctl_sel_town.InsertString(4, L"哈林");
	m_ctl_sel_town.InsertString(5, L"诺斯匹斯");
	m_ctl_sel_town.SetCurSel(0);

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
		auto_play_type = 2;
		pParentDlg->Log(L"切换为自动剧情模式");
	}
	else {
		auto_play_type = 1;
		pParentDlg->Log(L"切换为自动刷图模式");
	}
}


void PAGE1::OnBnClickedButton1()
{
	CMirageDragonDlg* pParentDlg = (CMirageDragonDlg*)GetParent()->GetParent();

	if (game_status != 3) {
		pParentDlg->Log(L"请在图内使用该功能");
		return;
	}

	skillCall(readLong(C_USER), 51200, 999999, 500, 500, 0, 0);
}


void PAGE1::OnBnClickedButton2()
{
	CMirageDragonDlg* pParentDlg = (CMirageDragonDlg*)GetParent()->GetParent();

	if (game_status != 3) {
		pParentDlg->Log(L"请在图内使用该功能");
		return;
	}

	if (penetrate_status) {
		penetrate(false);
	}
	else {
		penetrate(true);
	}
}


void PAGE1::OnBnClickedButton3()
{
	CMirageDragonDlg* pParentDlg = (CMirageDragonDlg*)GetParent()->GetParent();

	if (game_status != 3) {
		pParentDlg->Log(L"请在图内使用该功能");
		return;
	}
	// 读取人物坐标
	COORDINATE user = readCoordinate(readLong(C_USER));
	wstring map_name = getMapName();
	int map_number = getMapNumber();
	CString msg;
	msg.Format(L"副本名称：%s，副本编号：%d，坐标X：%d，坐标Y：%d，坐标Z：%d", map_name.c_str(), map_number, user.x, user.y, user.z);
	pParentDlg->Log(msg);
}


void PAGE1::OnBnClickedButton4()
{
	CMirageDragonDlg* pParentDlg = (CMirageDragonDlg*)GetParent()->GetParent();

	if (game_status != 1) {
		pParentDlg->Log(L"请在城镇中使用该功能");
		return;
	}

	int word, area, X, Y;
	word = readInt(C_TOWN_WORLD);
	area = readInt(C_TOWN_AREA);
	X = (int)readFloat(C_TOWN_X);
	Y = (int)readFloat(C_TOWN_Y);

	CString msg;
	msg.Format(L"大区域：%d，小区域：%d，坐标X：%d，坐标Y：%d", word, area, X, Y);
	pParentDlg->Log(msg);
}


void PAGE1::OnCbnSelchangeCombo3()
{
	CMirageDragonDlg* pParentDlg = (CMirageDragonDlg*)GetParent()->GetParent();

	if (game_status != 1) {
		pParentDlg->Log(L"请在城镇中使用该功能");
		return;
	}

	switch (m_ctl_sel_town.GetCurSel())
	{
	case 0:
		moveOfTown(40, 1, 1337, 231);
		break;
	case 1:
		moveOfTown(39, 0, 1809, 256);
	case 2:
		moveOfTown(89, 0, 557, 417);
	case 3:
		moveOfTown(22, 2, 945, 273);
	case 4:
		moveOfTown(54, 2, 972, 227);
		break;
	case 5:
		moveOfTown(137, 1, 864, 196);
		break;
	default:
		pParentDlg->Log(L"未知的移动目标");
		return;
	}
	pParentDlg->Log(L"已完成城镇移动");
}


void PAGE1::OnBnClickedButton5()
{
	is_running = readInt(0x140000000) == 0x905A4D;
	//reloadProcess();
	//firstRoomFunctions();
	// TODO: 在此添加控件通知处理程序代码
}


void PAGE1::OnBnClickedCheck1()
{
	// 三速
	if (_switch_three_speed.GetCheck() == BST_CHECKED)
	{
		function_switch.three_speed = true;
	}
	else {
		function_switch.three_speed = false;
	}
}


void PAGE1::OnBnClickedCheck2()
{
	// 评分
	if (_switch_score.GetCheck() == BST_CHECKED)
	{
		function_switch.score = true;
	}
	else {
		function_switch.score = false;
	}
}


void PAGE1::OnBnClickedCheck3()
{
	// 透明
	if (_switch_hidden_user.GetCheck() == BST_CHECKED)
	{
		function_switch.hidden_user = true;
	}
	else {
		function_switch.hidden_user = false;
	}
}


void PAGE1::OnBnClickedCheck4()
{
	// 聚怪
	if (_switch_gather_monster.GetCheck() == BST_CHECKED)
	{
		function_switch.gather_monster = true;
	}
	else {
		function_switch.gather_monster = false;
	}
}


void PAGE1::OnBnClickedCheck5()
{
	// 聚集物品
	if (_switch_gather_items.GetCheck() == BST_CHECKED)
	{
		function_switch.gather_items = true;
	}
	else {
		function_switch.gather_items = false;
	}
}


void PAGE1::OnBnClickedCheck6()
{
	// 技能冷却
	if (_switch_cool_down.GetCheck() == BST_CHECKED)
	{
		function_switch.cool_down = true;
	}
	else {
		function_switch.cool_down = false;
	}
}


void PAGE1::OnBnClickedCheck7()
{
	// HOOK伤害
	if (_switch_hook_damage.GetCheck() == BST_CHECKED)
	{
		function_switch.hook_damage = true;
	}
	else {
		function_switch.hook_damage = false;
	}
}


void PAGE1::OnEnUpdateEdit2()
{
	CMirageDragonDlg* pParentDlg = (CMirageDragonDlg*)GetParent()->GetParent();
	
	pParentDlg->Log(L"改变了地图值");

	CString code;
	m_ctl_mapNumber.GetWindowText(code);

	autoMapNumber = _wtoi(code);
}
