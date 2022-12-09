// PAGE3.cpp: 实现文件
//

#include "pch.h"
#include "MirageDragon.h"
#include "PAGE3.h"
#include "afxdialogex.h"
#include "dnfData.h"
#include "baseAddress.h"
#include "GameData.h"

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

	if (GLOBAL.game_status != 3)
	{
		return;
	}

	__int64 map_base = readLong(readLong(readLong(ADDR.x64("C_USER_ADDRESS")) + ADDR.x64("C_MAP_OFFSET")) + 16);
	__int64 head_address = readLong(map_base + ADDR.x64("C_MAP_HEAD"));
	__int64 end_address = readLong(map_base + ADDR.x64("C_MAP_END"));

	if (head_address == 0 || end_address == 0) {
		return;
	}

	int object_quantity = (int)(end_address - head_address) / 32;
	if (object_quantity == 0 || object_quantity > 1000) {
		return;
	}

	int row = 0;
	for (__int64 i = 1; i <= object_quantity; i++)
	{
		__int64 monster_address = readLong(readLong(head_address + i * 32) + 16) - 32;
		int monster_camp = readInt(monster_address + ADDR.x64("C_CAMP_OFFSET"));
		int monster_type = readInt(monster_address + ADDR.x64("C_TYPE_OFFSET"));
		int monster_blood = readInt(monster_address + ADDR.x64("C_MONSTER_BLOOD"));
		COORDINATE monster_coor = readCoordinate(monster_address);

		CString monster_name;
		int monster_code;
		if (monster_type == 289) {
			monster_name = readString(readLong(readLong(monster_address + ADDR.x64("C_GROUND_ITEM")) + ADDR.x64("C_ITEM_NAME")), 20);
			monster_code = readInt(readLong(monster_address + ADDR.x64("C_GROUND_ITEM")) + ADDR.x64("C_CODE_OFFSET"));
		}
		else {
			monster_name = readString(readLong(monster_address + ADDR.x64("C_NAME_OFFSET")), 20);
			monster_code = readInt(monster_address + ADDR.x64("C_CODE_OFFSET"));
		}

		CString str_camp, str_type, str_code, str_blood, str_name, str_coor;
		str_camp.Format(_T("%d"), monster_camp);
		str_type.Format(_T("%d"), monster_type);
		str_code.Format(_T("%d"), monster_code);
		str_blood.Format(_T("%d"), monster_blood);
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
