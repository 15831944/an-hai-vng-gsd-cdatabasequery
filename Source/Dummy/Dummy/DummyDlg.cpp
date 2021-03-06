// DummyDlg.cpp : implementation file
//
#pragma once
#include "stdafx.h"
#include "Dummy.h"
#include "DummyDlg.h"
#include "WinINET/HTTPRequester.h"
#include "WinINET/HTTPDownloader.h"
#include "BlackListReader.h"
#include "Utility/PathUtility.h"
#include "Utility/MyUtility.h"
#include "AppUtility.h"
#include "MyUtils.h"
#pragma comment (lib, "Encrypt/mddencrypt.lib")
extern "C" __declspec(dllimport) void Decode(CString strFileIn, CString strFileOut, char roStrKeyData[]);

#pragma warning (disable : 4996 )


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define		TIMER_PING_SERVER			100
#define		TIMER_UPDATE_BLACK_LIST		101
#define		TIMER_STARTUP_INFOSOFT		102


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


// CDummyDlg dialog




CDummyDlg::CDummyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDummyDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bIsRegistered = FALSE;
	m_bIsVisibleDlg = FALSE;
	memset(&m_stLastModifyBlackList, 0, sizeof(m_stLastModifyBlackList));

}

void CDummyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLORER_REGISTER, m_webRegister);
}

BEGIN_MESSAGE_MAP(CDummyDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_TRAY_ICON_NOTIFY, OnTrayNotification)
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_WM_CLOSE()
	ON_WM_WINDOWPOSCHANGING()
	ON_COMMAND(IDM_EXIT, &CDummyDlg::OnExit)
	ON_COMMAND(IDM_REGISTER_INFO, &CDummyDlg::OnRegisterInfo)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDummyDlg message handlers

BOOL CDummyDlg::OnInitDialog()
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

	// Set App startup with windows.
	CAppUtility appUtility;
	appUtility.SetStartUp();

	AddIconToSystemTray();
	
	SetTimer(TIMER_PING_SERVER, 60*1000, NULL);
	SetTimer(TIMER_UPDATE_BLACK_LIST, 60*1000, NULL);
	

	if(g_strMachineCode.IsEmpty())
	{
		if(g_bIsAutoStart)
		{
			SetTimer(TIMER_STARTUP_INFOSOFT, 30*1000, NULL);
		}	
		else
		{
			g_strMachineCode = CMyUtils::GetMachineCode();
			PostMessage(WM_CHECK_SHOW_REGISTER_INFO);
		}
	}
	else
	{
		PostMessage(WM_CHECK_SHOW_REGISTER_INFO);
	}

	m_webRegister.Navigate(_T("about:blank"), NULL, NULL, NULL, NULL);

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
		CDialog::OnSysCommand(nID, lParam);
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
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDummyDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


LRESULT CDummyDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if (message == WM_CHECK_SHOW_REGISTER_INFO)
	{
		CAppUtility appUtility;
		m_bIsRegistered = appUtility.CheckRegister();
		
		if (!m_bIsRegistered)
		{
			ShowRegisterInfo();
			
		}
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void CDummyDlg::OnWindowPosChanging(WINDOWPOS *lpwndpos)
{
	if(!m_bIsVisibleDlg && lpwndpos->flags & SWP_SHOWWINDOW)
	{
		lpwndpos->flags &= ~SWP_SHOWWINDOW;
	}

	CDialog::OnWindowPosChanging(lpwndpos);
}


LRESULT CDummyDlg::OnTrayNotification(WPARAM uID, LPARAM lEvent)
{
	switch (lEvent ) 
	{
		case WM_LBUTTONUP:
			break;

		case WM_RBUTTONUP:
			{
				CMenu Menu;

				VERIFY(Menu.LoadMenu(IDR_TRAY_MENU));
				int iMenuID = 0;
				CMenu* pPopup = Menu.GetSubMenu(iMenuID);
				ASSERT(pPopup != NULL);

				POINT point;
				GetCursorPos(&point);
				SetForegroundWindow();

				pPopup->TrackPopupMenu(TPM_RIGHTALIGN |TPM_RIGHTBUTTON, point.x, point.y, this);
			}
			break;
		case WM_LBUTTONDBLCLK:
			{
				OnRegisterInfo();
				//PostMessage(WM_CHECK_SHOW_REGISTER_INFO);
			}
			break;
	}

	return 1;
}

void CDummyDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
	RemoveIconFromSystemTray();
}

void CDummyDlg::AddIconToSystemTray()
{
	NOTIFYICONDATA NotifyIcon;

	memset(&NotifyIcon, 0, sizeof(NotifyIcon));
	NotifyIcon.cbSize = sizeof(NotifyIcon);
	NotifyIcon.hIcon = m_hIcon;
	NotifyIcon.hWnd	= m_hWnd;
	_tcscpy(NotifyIcon.szTip, _T("InfoSoft"));
	NotifyIcon.uCallbackMessage	= WM_TRAY_ICON_NOTIFY;
	NotifyIcon.uFlags	=	NIF_ICON | NIF_MESSAGE | NIF_TIP;
	NotifyIcon.uID		=	IDR_TRAY_MENU;
	Shell_NotifyIcon(NIM_ADD, &NotifyIcon);
}

void CDummyDlg::RemoveIconFromSystemTray()
{
	NOTIFYICONDATA NotifyIcon;

	memset(&NotifyIcon, 0, sizeof(NotifyIcon));
	NotifyIcon.cbSize	=	sizeof(NotifyIcon);
	NotifyIcon.hWnd	=	m_hWnd;
	NotifyIcon.uID		=	IDR_TRAY_MENU;
	Shell_NotifyIcon(NIM_DELETE, &NotifyIcon);
}

void CDummyDlg::ShowRegisterInfo()
{	
	VARIANT vr = {0};
	CString  strPostParam;
	strPostParam.Format(_T("mc=%s&ctid=%s"), g_strMachineCode, CITYID_CODE);
	CString strHeader = _T("Content-Type: application/x-www-form-urlencoded\r\n");
	vr = CMyUtils::CreateVariantPostData(strPostParam);

	DeleteUrlCacheEntry(URL_REGISTER);
	m_webRegister.HideScrollBars();
	
	m_webRegister.Navigate(URL_REGISTER, NULL, NULL, &vr, COleVariant(strHeader));
	
	

	m_bIsVisibleDlg = TRUE;
	
	
	ShowWindow(SW_SHOW);
	//SetForegroundWindow();
	SetActiveWindow();
	//SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE| SWP_NOMOVE );
	SetFocus();
}

void CDummyDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	ShowWindow(SW_HIDE);
	//CDialog::OnClose();
}

void CDummyDlg::OnExit()
{
	// TODO: Add your command handler code here
	//PostMessage(WM_CLOSE);
	PostQuitMessage(0);
}

void CDummyDlg::OnRegisterInfo()
{
	// TODO: Add your command handler code here
	if (g_strMachineCode.IsEmpty())
	{
		KillTimer(TIMER_STARTUP_INFOSOFT);
		g_strMachineCode = CMyUtils::GetMachineCode();
	}
	ShowRegisterInfo();
}

void CDummyDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch (nIDEvent)
	{
		case TIMER_PING_SERVER:
			{
				static bool bFirst = true;
				if (bFirst)
				{
					SetTimer(TIMER_PING_SERVER, 5*60*1000, NULL);
					bFirst = false;
				}
				PingServer();
			}
			break;

		case TIMER_UPDATE_BLACK_LIST:
			{
				static bool bFirst = true;
				if (bFirst)
				{
					SetTimer(TIMER_UPDATE_BLACK_LIST, 60*60*1000, NULL);
					bFirst = false;
				}
				UpdateBlackList();
				break;
			}
		case TIMER_STARTUP_INFOSOFT:
			{
				g_strMachineCode = CMyUtils::GetMachineCode();
				KillTimer(TIMER_STARTUP_INFOSOFT);
				PostMessage(WM_CHECK_SHOW_REGISTER_INFO);
			}

	}
	CDialog::OnTimer(nIDEvent);
}




BEGIN_EVENTSINK_MAP(CDummyDlg, CDialog)
	ON_EVENT(CDummyDlg, IDC_EXPLORER_REGISTER, 250, CDummyDlg::BeforeNavigate2ExplorerRegister, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	ON_EVENT(CDummyDlg, IDC_EXPLORER_REGISTER, 252, CDummyDlg::NavigateComplete2ExplorerRegister, VTS_DISPATCH VTS_PVARIANT)
	ON_EVENT(CDummyDlg, IDC_EXPLORER_REGISTER, 263, CDummyDlg::WindowClosingExplorerRegister, VTS_BOOL VTS_PBOOL)
END_EVENTSINK_MAP()

void CDummyDlg::BeforeNavigate2ExplorerRegister(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, BOOL* Cancel)
{
	// TODO: Add your message handler code here
	CString strURL(URL->bstrVal);
	if(strURL.Find(LINK_REDIRECT_CLOSE, 0) >= 0)
	{
		
		PostMessage(WM_CLOSE);
		m_webRegister.Navigate(_T("about:blank"), NULL, NULL, NULL, NULL);
	}
}

void CDummyDlg::NavigateComplete2ExplorerRegister(LPDISPATCH pDisp, VARIANT* URL)
{
	// TODO: Add your message handler code here
	CString strURL(URL->bstrVal);

	int iIndex = strURL.Find(_T("regcode="), 0);
	if(iIndex < 0 || iIndex >= strURL.GetLength())
	{
		return;
	}
	int iLength = strURL.Delete(0, iIndex + wcslen(_T("regcode=")));

	if(iLength <= 0)
		return;

	iIndex = strURL.Find(_T("&"));
	if(iIndex >= 0 && iIndex < strURL.GetLength())
	{
		strURL.Delete(iIndex, strURL.GetLength() - iIndex);
	}
	
	if(strURL.Compare(_T("1")) == 0)
	{
		CAppUtility appUtility;
		appUtility.UpdateRegistered(TRUE);
	}
	
	
}

void CDummyDlg::WindowClosingExplorerRegister(BOOL IsChildWindow, BOOL* Cancel)
{
	// TODO: Add your message handler code here
	*Cancel = 1;
	PostMessage(WM_CLOSE, 0, 0);
}

BOOL CDummyDlg::UpdateBlackList()
{
	CString strBlackListURL;

	if (FALSE == IsNeedDownloadBlackList(strBlackListURL))
	{
		return TRUE;
	}

	CString strBlackListFilePath = theApp.GetExeDirectory() + L"\\wblistex.vcs";
	CHTTPDownloader httpDownloader;

	if (FALSE == httpDownloader.Download(strBlackListURL, strBlackListFilePath))
	{
		return FALSE;
	}

	CString strTempFilePath;

	if (FALSE == CMyUtility::CreateTempFilePath(strTempFilePath))
	{
		return FALSE;
	}

	Decode(strBlackListFilePath, strTempFilePath, "abc@123");

	CBlackListReader blackListReader;
	if (FALSE == blackListReader.Read(strTempFilePath))
	{
		::DeleteFile(strTempFilePath);
		return FALSE;
	}

	::DeleteFile(strTempFilePath);

	if (FALSE == UpdateFileTime(strBlackListFilePath))
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CDummyDlg::PingServer()
{
	int iNumClientsActive = CMyUtils::GetNumClientActive();

	CString strData;
	strData.Format(L"mc=%s&ctid=%s&nca=%d&ip=%s", g_strMachineCode, CITYID_CODE, iNumClientsActive, g_strIPGateway);

	CHTTPRequester httpRequester;

	if (FALSE == httpRequester.PostRequest(URL_PING_MASTER, strData))
	{
		return FALSE;
	}

	CString strResponse = httpRequester.GetResponsedString();

	if (strResponse == L"1")
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CDummyDlg::IsNeedDownloadBlackList(CString &strBlackListUrl )
{

	CString strBlackListFilePath = theApp.GetExeDirectory() + L"\\wblistex.vcs";
	FILETIME ftFileTime;

	CString strFileTime;
	
	if (FALSE == CMyUtility::GetModifyTime(strBlackListFilePath, ftFileTime))
	{
		strFileTime = L"00000000000000";
	}
	else
	{
		if(m_stLastModifyBlackList.wYear == 0)
		{
			SYSTEMTIME stFileTime;
			if (FALSE == ::FileTimeToSystemTime(&ftFileTime, &stFileTime))
			{
				return FALSE;
			}
			strFileTime.Format(L"%04u%02u%02u%02u%02u%02u", stFileTime.wYear, stFileTime.wMonth, stFileTime.wDay, stFileTime.wHour, stFileTime.wMinute, stFileTime.wSecond);
		}
		else
		{
			strFileTime.Format(L"%04u%02u%02u%02u%02u%02u",
								m_stLastModifyBlackList.wYear,
								m_stLastModifyBlackList.wMonth,
								m_stLastModifyBlackList.wDay,
								m_stLastModifyBlackList.wHour,
								m_stLastModifyBlackList.wMinute,
								m_stLastModifyBlackList.wSecond);
		}
	}
	
	CString strData;
	strData.Format(L"mc=%s&ctid=%s&mt=%s", g_strMachineCode, CITYID_CODE, strFileTime);

	CHTTPRequester requester;
	if (FALSE == requester.PostRequest(URL_CHECK_BLACK_LIST, strData))
	{
		return FALSE;
	}
	
	strBlackListUrl = requester.GetResponsedString();

	if(strBlackListUrl.GetLength() > 0)
	{		
		strFileTime.Empty();
		strFileTime.Append(strBlackListUrl);
		int iIndex = strFileTime.Find(_T(";"));
		strBlackListUrl.Delete(iIndex, strBlackListUrl.GetLength() - iIndex);
		strFileTime.Delete(0, iIndex +1);
		return CMyUtils::ConvertStringToSystemTime(m_stLastModifyBlackList, strFileTime);
	}

	return FALSE;
}

BOOL CDummyDlg::UpdateFileTime( LPCTSTR strFileURL, LPCTSTR strFilePath )
{
	SYSTEMTIME stMasterFileTime;
	if (FALSE == CMyUtility::QueryMasterModifyTime(strFileURL, &stMasterFileTime))
	{
		return FALSE;
	}

	FILETIME ftMasterFileTime;
	if (FALSE ==::SystemTimeToFileTime(&stMasterFileTime, &ftMasterFileTime))
	{
		return FALSE;
	}

	if (FALSE == CMyUtility::SetModifyTime(strFilePath, ftMasterFileTime))
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CDummyDlg::UpdateFileTime(LPCTSTR strFilePath)
{
	
	FILETIME ftMasterFileTime;
	if (FALSE ==::SystemTimeToFileTime(&m_stLastModifyBlackList, &ftMasterFileTime))
	{
		return FALSE;
	}

	if (FALSE == CMyUtility::SetModifyTime(strFilePath, ftMasterFileTime))
	{
		return FALSE;
	}
	return TRUE;
}