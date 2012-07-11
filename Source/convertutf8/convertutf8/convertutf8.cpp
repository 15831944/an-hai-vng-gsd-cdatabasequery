// convertutf8.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "convertutf8.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;

char* UnicodeToUtf8(LPCWSTR s);
LPCWSTR Utf8ToUnicode(char* strUni);

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: code your application's behavior here.
		LPCWSTR root = _T("tôi đi chơi");
		char* kq = UnicodeToUtf8(root);
		LPCWSTR temp = _T("");
		temp = Utf8ToUnicode(kq);
		cout<<temp;

	}

	return nRetCode;
}

char* UnicodeToUtf8(LPCWSTR s)
{
	if (s == NULL) 
		return NULL;

	int iLength= lstrlenW(s);
	if (iLength==0) 
	{
		CHAR *psz = new CHAR[1];
		*psz = '\0';
		return psz;
	}
	int cc = WideCharToMultiByte(CP_UTF8, 0, s, iLength, NULL, 0, NULL, NULL);
	if (cc == 0) 
		return NULL;
	CHAR *psz=new CHAR[cc+1];
	cc = WideCharToMultiByte(CP_UTF8, 0, s, iLength, psz, cc, NULL, NULL);
	if (cc==0) 
	{
		delete[] psz;
		return NULL;
	}
	psz[cc]='\0';
	return psz;
}

LPCWSTR Utf8ToUnicode(char* strUni)
{
	if(strUni == NULL)
		return NULL;
	int iLength = strlen(strUni);
	if(iLength == 0)
	{
		WCHAR *pw = new WCHAR[1];
		*pw = L'\0';
		return pw;
	}
	
	int iSize_needed = MultiByteToWideChar(CP_UTF8, 0, strUni, iLength, NULL, 0);
	WCHAR* strKq = new WCHAR[iSize_needed + 1];
	iSize_needed = MultiByteToWideChar(CP_UTF8, 0, strUni, iLength, (LPWSTR)strKq, iSize_needed);
	strKq[iSize_needed] = L'\0';
	return strKq;
}


