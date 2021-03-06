_COM_SMARTPTR_TYPEDEF(IShellWindows, IID_IShellWindows);
_COM_SMARTPTR_TYPEDEF(IWebBrowser2, IID_IWebBrowser2);

#pragma once
#include "StdAfx.h"
#include "WinTabBrowser.h"
#include "TrackingIEBrowser.h"
#include "StrSafe.h"
//#include "ExDisp.h"

////#import <shdocvw.dll>  
////#import <mshtml.tlb>
//#include <atlbase.h>  
//CComModule _Module;       // CComDispatchDriver ATL  
//#include <mshtml.h>       // All IHTMLxxxx interface  
//#include <atlcom.h>  




static IShellWindowsPtr jws_shellwindows(0);


CTrackingIEBrowser::CTrackingIEBrowser()
{
}
CTrackingIEBrowser::~CTrackingIEBrowser()
{

}

BOOL CTrackingIEBrowser::GetHistory(vector<STATURL>& lstStatUrl)
{
	
	STATURL url;
	CString strUrl;

	ULONG uFetched;
	IUrlHistoryStg2* history = NULL;
	
	IEnumSTATURL* enumPtr = NULL;
	
	CoInitialize(NULL);
	if(FAILED(CoCreateInstance(CLSID_CUrlHistory, NULL, CLSCTX_INPROC_SERVER, IID_IUrlHistoryStg2,(void**)&history)))
	{
		return FALSE;
	}

	if(FAILED(history->EnumUrls(&enumPtr)))
		return FALSE;

	while(SUCCEEDED(enumPtr->Next(1,&url,&uFetched)))
	{
		if(uFetched==0)
			break;
		//for(int i = 0; i < (int)m_lstTitle.size(); i++)
		//{
		//	
		//	if(NULL != url.pwcsTitle &&  wcscmp(m_lstTitle[i], url.pwcsTitle) == 0 )
		//	{
		//		SYSTEMTIME currentTime;
		//		::GetLocalTime(&currentTime);
		//		currentTime.wMinute++;

		//		FILETIME tempTime;
		//		SystemTimeToFileTime(&currentTime, &tempTime);

		//		LARGE_INTEGER lCurrentTime;
		//		//LARGE_INTEGER lUrlTime;
		//		
		//		lCurrentTime.HighPart = tempTime.dwHighDateTime;
		//		lCurrentTime.LowPart = tempTime.dwLowDateTime;
		//		
		//		FileTimeToSystemTime(&url.ftExpires, &currentTime);
		//		
		//		STATURL tempUrl = url;
		//		lstStatUrl.push_back(tempUrl);
		//		break;
		//	}
		//}
	}
	return TRUE;
}


static void EnumIETab(const std::wstring &parent_name, HWND hwnd, vector<CWinTabBrowser>* plstWinTab)
{
	
	CoInitialize(NULL);
	CComPtr<IShellWindows> spShellWin;  

	HRESULT hr = spShellWin.CoCreateInstance(CLSID_ShellWindows);  
	if ( FAILED ( hr ) )  
	{  
		return;  
	}  

	long nCount = 0;          
	spShellWin->get_Count( &nCount );// Get the count of IE instanceExplorer   
									 //and IExplorer  
	if( 0 == nCount )  
	{  
		return;  
	}  

	for(int i=0; i<nCount; i++)  
	{  
		CComPtr< IDispatch > spDispIE;  
		hr=spShellWin->Item(CComVariant( (long)i ), &spDispIE );  
		if (FAILED(hr))    
			continue;  

		CComQIPtr<IWebBrowser2> spBrowser = spDispIE;  
		if (!spBrowser)       
			continue;  


		/*HWND browser_hwnd;
		if(S_OK != (spBrowser->get_HWND((SHANDLE_PTR *)&browser_hwnd) ) || browser_hwnd != hwnd)
		continue;*/

		CComPtr < IDispatch > spDispDoc;  
		hr = spBrowser->get_Document( &spDispDoc );//check instance  
													//object is or no IE  
		if (FAILED(hr))    
			continue;
	
		CComQIPtr< IHTMLDocument2 > spDocument2 = spDispDoc;
		if (!spDocument2)     continue;
		
		_bstr_t title, uri;
		BSTR title_b, uri_b;
		
		spBrowser->get_LocationURL(&uri_b);
		if(S_OK != spBrowser->get_LocationName(&title_b))
			title = _bstr_t(L"");
		else title.Attach(title_b);

		CWinTabBrowser winTab(title_b, uri_b);
		//CTrackingIEBrowser::m_lstWinTab.push_back(winTab);
		plstWinTab->push_back(winTab);
		/*CString s(title_b);
		CString t(uri_b);
		s.AppendFormat(_T("\n%s"), t);
		AfxMessageBox(s);*/
	} 
	CoUninitialize();//release COM  
  
}
static BOOL CALLBACK EnumWinTabProc(HWND hwnd, LPARAM lParam)
{
	wchar_t title[512];

	DWORD dwStyle = GetWindowLongPtr(hwnd, GWL_STYLE);
	if(!(dwStyle & WS_VISIBLE))
		return TRUE;
	GetWindowText(hwnd, title, _countof(title));
	if(!title[0])
		return TRUE;
	LONG exstyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
	if(exstyle & WS_EX_TOOLWINDOW)
		return TRUE;


	wchar_t className[512];
	if(GetClassName(hwnd, className, _countof(className)))
	{
	  if(!(wcscmp(className, L"IEFrame")))
	  {
		  vector<CWinTabBrowser>* plstWinTab = (vector<CWinTabBrowser>*)lParam;
		  EnumIETab(title, hwnd, plstWinTab );
	  }
	}
	return TRUE;
}

VOID CTrackingIEBrowser::Track()
{
	m_lstWinTab.clear();
	EnumWindows(EnumWinTabProc, (LPARAM)&m_lstWinTab);	
	WriteTrackToFile();
	
}
VOID CTrackingIEBrowser::WriteTrackToFile()
{
	TCHAR strCurrentPath[MAX_PATH] = {0};
	GetCurrentDirectory(MAX_PATH, strCurrentPath);

	StringCchPrintf(strCurrentPath, MAX_PATH, _T("%s\\tracklog.txt"), strCurrentPath);
	
	//for(int i = 0 ;i < 
	
}