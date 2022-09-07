#pragma once


// PAGE2 对话框

class PAGE2 : public CDialogEx
{
	DECLARE_DYNAMIC(PAGE2)

public:
	PAGE2(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~PAGE2();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_PAGE2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	//afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	CEdit _attack_speed;
	CEdit _casting_speed;
	CEdit _move_speed;
	CEdit _cool_down;
	CEdit _damage_value;
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
