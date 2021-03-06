// CSMServerSimulatorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CSMServerSimulator.h"
#include "CSMServerSimulatorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CCSMServerSimulatorDlg dialog




CCSMServerSimulatorDlg::CCSMServerSimulatorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCSMServerSimulatorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCSMServerSimulatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LST_WEB_HISTORY, m_lstWebHistory);
}

BEGIN_MESSAGE_MAP(CCSMServerSimulatorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_SOCKET, OnAsyncSocketNotify)
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CCSMServerSimulatorDlg message handlers

BOOL CCSMServerSimulatorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	g_Listener.Start(m_hWnd);
	InitListCtrWebHistory();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCSMServerSimulatorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCSMServerSimulatorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCSMServerSimulatorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CCSMServerSimulatorDlg::OnAsyncSocketNotify(WPARAM wParam, LPARAM lParam)
{
	g_Listener.ProcessSocketMsg(wParam, lParam);
	return 1;
}

void CCSMServerSimulatorDlg::OnDestroy()
{
	CDialog::OnDestroy();
	
	g_Listener.Stop();
	// TODO: Add your message handler code here
}

VOID CCSMServerSimulatorDlg::InitListCtrWebHistory()
{
	CRect rect;
	m_lstWebHistory.GetClientRect(&rect);
	int nColInterval = rect.Width()/2;

	m_lstWebHistory.InsertColumn(0, _T("Website truy cập"));
	m_lstWebHistory.SetColumnWidth(0, (int)(nColInterval));

	m_lstWebHistory.InsertColumn(1, _T("Thời điểm truy cập"));
	m_lstWebHistory.SetColumnWidth(1, (int)(nColInterval));

	m_lstWebHistory.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_BORDERSELECT | LVS_EX_FULLROWSELECT | LVS_EX_UNDERLINECOLD | LVS_EX_FULLROWSELECT);

}

VOID CCSMServerSimulatorDlg::InsertItemHistory( const CString& strURL, const CString& strTime /*= _T("")*/ )
{
	int iIndex = m_lstWebHistory.GetItemCount();

	LVITEM lvItemHistory;
	lvItemHistory.mask = LVIF_TEXT | LVIF_STATE;
	lvItemHistory.state = LVIS_OVERLAYMASK;
	lvItemHistory.stateMask = LVIS_SELECTED;
	lvItemHistory.iItem = iIndex;

	lvItemHistory.iSubItem = 0;
	lvItemHistory.pszText = (LPTSTR)(LPCTSTR)strURL;
	m_lstWebHistory.InsertItem(&lvItemHistory);

	lvItemHistory.iSubItem = 1;
	lvItemHistory.pszText = (LPTSTR)(LPCTSTR)strTime;
	m_lstWebHistory.SetItem(&lvItemHistory);

}

LRESULT CCSMServerSimulatorDlg::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if(message == VCM_WEB_HISTORY)	
	{
		LPCTSTR strTemp = (LPCTSTR)lParam;
		
		InsertItemHistory(strTemp);
	}

	return CDialog::WindowProc(message, wParam, lParam);
}
