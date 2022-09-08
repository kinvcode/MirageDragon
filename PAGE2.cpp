// PAGE2.cpp: 实现文件
//

#include "pch.h"
#include "MirageDragon.h"
#include "PAGE2.h"
#include "afxdialogex.h"


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
