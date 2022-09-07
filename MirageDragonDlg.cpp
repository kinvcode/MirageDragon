
// MirageDragonDlg.cpp: ʵ���ļ�
//

#include "pch.h"
#include "framework.h"
#include "MirageDragon.h"
#include "MirageDragonDlg.h"
#include "afxdialogex.h"
#include "jobs.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HANDLE MSDK_HANDLE;

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMirageDragonDlg �Ի���



CMirageDragonDlg::CMirageDragonDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MIRAGEDRAGON_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON2);
}

void CMirageDragonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_ctl_tab);
	DDX_Control(pDX, IDC_EDIT1, m_ctl_edit_console);
	DDX_Control(pDX, IDC_BUTTON1, m_ctl_btn_init);
}

BEGIN_MESSAGE_MAP(CMirageDragonDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMirageDragonDlg::OnBnClickedButton1)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CMirageDragonDlg::OnTcnSelchangeTab1)
END_MESSAGE_MAP()


// CMirageDragonDlg ��Ϣ�������

BOOL CMirageDragonDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// ���ô��ڱ���
	SetWindowText(L"�þ���");

	// ������־���ַ���
	m_ctl_edit_console.SetLimitText(UINT_MAX);

	// ����������������
	MSDK_HANDLE = M_Open(1);

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMirageDragonDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMirageDragonDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMirageDragonDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMirageDragonDlg::OnBnClickedButton1()
{
	// ��ȡDNF����ID
	PID = getProcessPID(L"DNF.exe");
	if (PID == 0) {
		AfxMessageBox(L"����������Ϸ");
		return;
	}

	if (readInt(0x140000000) != 0x905A4D) {
		AfxMessageBox(L"�޶�дȨ�ޣ����ȡͼ��!");
		return;
	}

	// ��ʾ�ؼ�
	m_ctl_btn_init.ShowWindow(SW_HIDE);
	m_ctl_edit_console.ShowWindow(SW_SHOW);
	m_ctl_tab.ShowWindow(SW_SHOW);

	// ��ʼ��tab�ؼ�
	initTabCtl();
	
	Log(L"��ʼ�����");

	// �����߳�
	AfxBeginThread(userPointerThread, this);
}

void CMirageDragonDlg::initTabCtl() 
{
	// ��ʼ��tab�ؼ�
	m_ctl_tab.InsertItem(0, L"������");
	m_ctl_tab.InsertItem(1, L"��ͼ����");
	m_ctl_tab.InsertItem(2, L"���ý���");
	page1.Create(IDD_DIALOG_PAGE1, &m_ctl_tab);
	page2.Create(IDD_DIALOG_PAGE2, &m_ctl_tab);
	page3.Create(IDD_DIALOG_PAGE3, &m_ctl_tab);
	CRect rc;
	m_ctl_tab.GetClientRect(rc);
	rc.top += 26;
	rc.bottom -= 0;
	rc.left += 0;
	rc.right -= 0;
	page1.MoveWindow(&rc);
	page2.MoveWindow(&rc);
	page3.MoveWindow(&rc);
	page1.ShowWindow(SW_SHOW);
	page2.ShowWindow(SW_HIDE);
	page3.ShowWindow(SW_HIDE);
	m_tab_box[0] = &page1;
	m_tab_box[1] = &page2;
	m_tab_box[2] = &page3;
	m_cur_tab_index = 0;
}


void CMirageDragonDlg::OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult)
{
	int index = m_ctl_tab.GetCurSel();
	int count = m_ctl_tab.GetItemCount();

	for (int i = 0; i < count; i++)
	{
		m_tab_box[i]->ShowWindow(SW_HIDE);
	}

	m_tab_box[index]->ShowWindow(SW_SHOW);

	*pResult = 0;
}

void CMirageDragonDlg::Log(CString msg)
{
	m_ctl_edit_console.SetSel(-1);
	m_ctl_edit_console.ReplaceSel(msg);
	m_ctl_edit_console.SetSel(-1);
	m_ctl_edit_console.ReplaceSel(L"\r\n");
}