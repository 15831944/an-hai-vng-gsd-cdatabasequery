
// MyWebBrowserDlg.h : header file
//

#pragma once
#include "Explorer.h"


// CMyWebBrowserDlg dialog
class CMyWebBrowserDlg : public CDialog
{
// Construction
public:
	CMyWebBrowserDlg(CWnd* pParent = NULL);	
	enum { IDD = IDD_MYWEBBROWSER_DIALOG };


protected:
	HICON m_hIcon;
	CExplorer1 *m_pExplorer;

	virtual BOOL PreTranslateMessage( MSG* pMsg);
	virtual BOOL OnInitDialog();

	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnNavigate( WPARAM, LPARAM );
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
public:

	DECLARE_EVENTSINK_MAP()


	void DocumentCompleteExplorer(LPDISPATCH pDisp, VARIANT* URL);
	void BeforeNavigate2Explorer(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers, BOOL* Cancel);
//	void DownloadBeginExplorer();
};
