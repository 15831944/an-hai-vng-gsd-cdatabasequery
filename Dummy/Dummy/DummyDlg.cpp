
// DummyDlg.cpp : implementation file
//
#pragma once
#include "stdafx.h"
#include "Dummy.h"
#include "DummyDlg.h"
#include "DummyManager.h"
#include "BlackListDAO.h"

#include "MySQLDataAccessHelper.h"

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
	//DECLARE_INTERFACE()
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


// CDummyDlg dialog




CDummyDlg::CDummyDlg(CWnd* pParent /*=NULL*/)
	: CTrayDialog(CDummyDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDummyDlg::DoDataExchange(CDataExchange* pDX)
{
	CTrayDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLORER_REGINFO, m_webBrowerRegInfo);
}

BEGIN_MESSAGE_MAP(CDummyDlg, CTrayDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP

	ON_COMMAND(ID_SYSTEMTRAYPOPUP_ABOUT, OnTrayMenuAbout)
	ON_COMMAND(ID_SYSTEMTRAYPOPUP_REGINFO, OnTrayMenuRegisterInfo)
	ON_COMMAND(ID_SYSTEMTRAYPOPUP_EXIT, OnTrayMenuExit)
END_MESSAGE_MAP()


// CDummyDlg message handlers

BOOL CDummyDlg::OnInitDialog()
{
	CTrayDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	

	CMySQLDataAccessHelper te("", "", "","");
	

	TraySetIcon(IDR_MAINFRAME);
	TraySetToolTip(_T("Dummy"));
	TraySetMenu(IDR_MENU_STPOPUP);

	if(m_bIsRegistered)
		::PostMessage(this->m_hWnd, WM_SYSCOMMAND, (WPARAM)SC_MINIMIZE, 0);

	NavigateWeb();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDummyDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CTrayDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDummyDlg::OnPaint()
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
		CTrayDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDummyDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

VOID CDummyDlg::NavigateWeb()
{
	if(m_bIsRegistered)
	{
		m_webBrowerRegInfo.Navigate(_T("vng.com.vn"), NULL, NULL, NULL, NULL);
	}
	else
		m_webBrowerRegInfo.Navigate(_T("file:///C:/Users/Administrator/Desktop/test.html"), NULL, NULL, NULL, NULL);
}

VOID CDummyDlg::SetRegistered(BOOL bIsRegistered)
{
	m_bIsRegistered = bIsRegistered;
}

void CDummyDlg::OnTrayMenuRegisterInfo()
{
	this->ShowWindow(SW_SHOW);
}

void CDummyDlg::OnTrayMenuAbout()
{
	CAboutDlg about;
	about.DoModal();
}

void CDummyDlg::OnTrayMenuExit()
{
	OnCancel();
}

BEGIN_EVENTSINK_MAP(CDummyDlg, CTrayDialog)
	ON_EVENT(CDummyDlg, IDC_EXPLORER_REGINFO, 250, CDummyDlg::BeforeNavigate2ExplorerReginfo, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
END_EVENTSINK_MAP()

void CDummyDlg::BeforeNavigate2ExplorerReginfo(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, BOOL* Cancel)
{
	CString strURL(*URL);
	if(strURL.Find(_T("dummysuccess.com"), 0) >= 0)
	{
		CDummyManager::UpdateRegistered(TRUE);
		AfxMessageBox(_T("Đăng ký thành công"));
		
	}
	else if(strURL.Find(_T("dummyclose.com"), 0) >= 0)
	{
		CDummyManager::UpdateRegistered(FALSE);
		PostMessage(WM_CLOSE);
	}
}
