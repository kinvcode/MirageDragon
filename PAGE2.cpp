// PAGE2.cpp: 实现文件
//

#include "pch.h"
#include "MirageDragon.h"
#include "PAGE2.h"
#include "afxdialogex.h"
#include "MirageDragonDlg.h"
#include "dnfUser.h"


// PAGE2 对话框

IMPLEMENT_DYNAMIC(PAGE2, CDialogEx)

PAGE2::PAGE2(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_PAGE2, pParent)
{

}

PAGE2::~PAGE2()
{
}

void PAGE2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, _attack_speed);
	DDX_Control(pDX, IDC_EDIT3, _casting_speed);
	DDX_Control(pDX, IDC_EDIT4, _move_speed);
	DDX_Control(pDX, IDC_EDIT5, _cool_down);
	DDX_Control(pDX, IDC_EDIT2, _damage_value);
}


BEGIN_MESSAGE_MAP(PAGE2, CDialogEx)
	ON_WM_ERASEBKGND()
	ON_EN_UPDATE(IDC_EDIT2, &PAGE2::OnEnUpdateEdit2)
	ON_EN_KILLFOCUS(IDC_EDIT2, &PAGE2::OnEnKillfocusEdit2)
END_MESSAGE_MAP()


// PAGE2 消息处理程序

BOOL PAGE2::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	_attack_speed.SetWindowText(L"1000");
	_casting_speed.SetWindowText(L"1000");
	_move_speed.SetWindowText(L"1000");
	_cool_down.SetWindowText(L"70");
	_damage_value.SetWindowText(L"196688");

	return TRUE;
}

BOOL PAGE2::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(&rect);
	CBrush myBrush(RGB(0, 255, 255));
	CBrush* pOld = pDC->SelectObject(&myBrush);
	BOOL bRes = pDC->PatBlt(0, 0, rect.Width(), rect.Height(), PATCOPY);
	pDC->SelectObject(pOld);
	return bRes;
}


void PAGE2::OnEnUpdateEdit2()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数，以将 EM_SETEVENTMASK 消息发送到该控件，
	// 同时将 ENM_UPDATE 标志“或”运算到 lParam 掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void PAGE2::OnEnKillfocusEdit2()
{
	CMirageDragonDlg* pParentDlg = (CMirageDragonDlg*)GetParent()->GetParent();
	hookDamage(true);
	pParentDlg->Log(L"HOOK伤害已更新");
}
