// PAGE1.cpp: 实现文件
//

#include "pch.h"
#include "MirageDragon.h"
#include "PAGE1.h"
#include "afxdialogex.h"
#include "MirageDragonDlg.h"

int auto_play_type = 1;
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
}


BEGIN_MESSAGE_MAP(PAGE1, CDialogEx)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_CBN_SELCHANGE(IDC_COMBO2, &PAGE1::OnCbnSelchangeCombo2)
END_MESSAGE_MAP()


// PAGE1 消息处理程序

BOOL PAGE1::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	_switch_score.SetCheck(BST_CHECKED);
	_switch_three_speed.SetCheck(BST_CHECKED);
	_switch_gather_monster.SetCheck(BST_CHECKED);
	_switch_gather_items.SetCheck(BST_CHECKED);
	_switch_cool_down.SetCheck(BST_CHECKED);
	_switch_hook_damage.SetCheck(BST_CHECKED);
	_switch_hidden_user.SetCheck(BST_CHECKED);

	m_ctl_sel_autoplay.InsertString(0, L"自动刷图");
	m_ctl_sel_autoplay.InsertString(1, L"自动剧情");
	m_ctl_sel_autoplay.SetCurSel(0);

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
