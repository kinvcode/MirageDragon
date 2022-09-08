
// MirageDragonDlg.h: 头文件
//

#pragma once
#include "msdk.h"
#include "UsbHidKeyCode.h"
#include "PAGE1.h"
#include "PAGE2.h"
#include "PAGE3.h"
#include "Lock.h"

// CMirageDragonDlg 对话框
class CMirageDragonDlg : public CDialogEx, public InstanceLockBase
{
	// 构造
public:
	CBrush m_brush;
	CMirageDragonDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MIRAGEDRAGON_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// tab控件容器
	CDialog* m_tab_box[3];
	// tab控件当前索引
	int m_cur_tab_index;
	// 日志输出时间
	CTime m_log_time;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	// 功能
	void initTabCtl();
public:
	// tab页面
	PAGE1 page1;
	PAGE2 page2;
	PAGE3 page3;

	void CMirageDragonDlg::Log(CString msg);

	afx_msg void OnBnClickedButton1();
	CTabCtrl m_ctl_tab;
	//CMFCTabCtrl m_ctl_tab;
	CEdit m_ctl_edit_console;
	CButton m_ctl_btn_init;
	afx_msg void OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
