#pragma once


// PAGE1 对话框

class PAGE1 : public CDialogEx
{
	DECLARE_DYNAMIC(PAGE1)

public:
	PAGE1(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~PAGE1();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PAGE1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CButton _switch_three_speed;
	CButton _switch_score;
	CButton _switch_hidden_user;
	CButton _switch_cool_down;
	CButton _switch_hook_damage;
	CButton _switch_gather_monster;
	CButton _switch_gather_items;
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
