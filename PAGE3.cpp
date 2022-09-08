// PAGE3.cpp: 实现文件
//

#include "pch.h"
#include "MirageDragon.h"
#include "PAGE3.h"
#include "afxdialogex.h"


// PAGE3 对话框

IMPLEMENT_DYNAMIC(PAGE3, CDialogEx)

PAGE3::PAGE3(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_PAGE3, pParent)
{

}

PAGE3::~PAGE3()
{
}

void PAGE3::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST3, _ctl_list);
}


BEGIN_MESSAGE_MAP(PAGE3, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &PAGE3::OnBnClickedButton1)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// PAGE3 消息处理程序

BOOL PAGE3::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	_ctl_list.InsertColumn(0, TEXT("怪物阵营"), LVCFMT_CENTER, 100);
	_ctl_list.InsertColumn(1, TEXT("怪物类型"), LVCFMT_CENTER, 100);
	_ctl_list.InsertColumn(2, TEXT("怪物代码"), LVCFMT_CENTER, 100);
	_ctl_list.InsertColumn(3, TEXT("怪物名称"), LVCFMT_CENTER, 200);
	_ctl_list.InsertColumn(4, TEXT("怪物血量"), LVCFMT_CENTER, 200);
	_ctl_list.InsertColumn(5, TEXT("怪物坐标"), LVCFMT_CENTER, 200);

	return TRUE;
}

void PAGE3::OnBnClickedButton1()
{
	// 遍历图内对象

}

BOOL PAGE3::OnEraseBkgnd(CDC* pDC)
{
	CRect rect;
	GetClientRect(&rect);
	CBrush myBrush(RGB(0, 255, 255));
	CBrush* pOld = pDC->SelectObject(&myBrush);
	BOOL bRes = pDC->PatBlt(0, 0, rect.Width(), rect.Height(), PATCOPY);
	pDC->SelectObject(pOld);
	return bRes;
}
