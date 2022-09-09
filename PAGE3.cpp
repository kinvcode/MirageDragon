// PAGE3.cpp: 实现文件
//

#include "pch.h"
#include "MirageDragon.h"
#include "PAGE3.h"
#include "afxdialogex.h"
#include "dnfData.h"


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
	_ctl_list.DeleteAllItems();

	// 遍历怪物
	if (game_status != 3)
	{
		return;
	}

	vector<DUNGEONOBJ> items = getDungeonAllObj();

	int row = 0;
	__int64 length = items.size();
	for (__int64 i = 0; i < length; i++)
	{
		CString monster_name = bytesToWstring(readByteArray(readLong(items[i].p + C_NAME_OFFSET), 50)).c_str();

		COORDINATE monster_coor = readCoordinate(items[i].p);

		CString str_camp, str_type, str_code, str_blood, str_name, str_coor;

		str_camp.Format(_T("%d"), items[i].camp);
		str_type.Format(_T("%d"), items[i].type);
		str_code.Format(_T("%d"), items[i].code);
		str_blood.Format(_T("%d"), items[i].blood);
		str_coor.Format(_T("%d,%d,%d"), monster_coor.x, monster_coor.y, monster_coor.z);

		_ctl_list.InsertItem(row, str_camp);
		_ctl_list.SetItemText(row, 1, str_type);
		_ctl_list.SetItemText(row, 2, str_code);
		_ctl_list.SetItemText(row, 3, monster_name);
		_ctl_list.SetItemText(row, 4, str_blood);
		_ctl_list.SetItemText(row, 5, str_coor);
		row++;

		handleEvents();
	}
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
