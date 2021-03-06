#include "StdAfx.h"
#include "AutoUpdateTool.h"
#include "FileServices.h"
#include "MyPath.h"
#include "FolderCloner.h"
#include "FolderCompare.h"
#include "FolderRemoving.h"
#include "MyUtils.h"
#include "GameSourceCompare.h"
#include "GlobalClass.h"


CAutoUpdateTool::CAutoUpdateTool()
{
	m_pAutoLauncher = NULL;
}

BOOL CAutoUpdateTool::Create( CLauncher *pAutoLauncher, LPCTSTR strToken )
{
	if (NULL == pAutoLauncher) {
		return FALSE;
	}

	m_strSourcePath = pAutoLauncher->GetSourcePath();
	
	m_strToken = strToken;
	m_pAutoLauncher = pAutoLauncher;

	CMyPath myPath;

	m_strLogFilePath = myPath.GetExeFilePath() + CString(L"\\log") + m_strToken + CString(L".txt");
	return TRUE;
}

BOOL CAutoUpdateTool::Create( CLauncher *pAutoLauncher, LPCTSTR strToken, LPCTSTR strCCSource )
{
	m_strTempSourcePath = strCCSource;
	return Create(pAutoLauncher, strToken);
}


CAutoUpdateTool::~CAutoUpdateTool(void)
{
	if (NULL != m_pAutoLauncher) {
		delete m_pAutoLauncher;
	}
}

BOOL CAutoUpdateTool::Update()
{
	CString strLog;

	if (NULL == m_pAutoLauncher) {

		strLog = L"Launcher chua duoc cai dat...";
		WriteErrorLog(strLog);
		WriteErrorLog(L"");

		return FALSE;
	}

	if (FALSE ==  RunAutoLauncher()) {
		WriteErrorLog(L"That bai... Duong truyen bi loi hoac launcher da bi thay doi");
	}

	if (FALSE == Compare()) {
		WriteErrorLog(L"So sanh source game that bai. Vui long kiem tra lai duong dan source game");
	}

	strLog = m_pAutoLauncher->GetName() + L" update hoan tat...";
	WriteDebugLog(strLog);
	WriteDebugLog(L"");

	return TRUE;

}

BOOL CAutoUpdateTool::CloneSource()
{
	return TRUE;
}

BOOL CAutoUpdateTool::RunAutoLauncher()
{
	CString strLog = m_pAutoLauncher->GetName() + L" dang tu dong chay launcher ...";
	WriteDebugLog(strLog);

	if (FALSE == m_pAutoLauncher->Run()) {
		return FALSE;
	}
	
	return TRUE;
}

BOOL CAutoUpdateTool::Compare()
{
	CString strLog = m_pAutoLauncher->GetName() + L" dang so sanh source ...";
	WriteDebugLog(strLog);

	CMyPath myPath;

	CString strReportDirectory = myPath.GetExeFilePath() + L"\\Report";
	if (FALSE == myPath.IsDirectory(strReportDirectory)) {
		::CreateDirectory(strReportDirectory, NULL);
	}

	CString strResultFilePath;
	//strResultFilePath.Format(L"%s\\%s %s.txt",strReportDirectory,  m_pAutoLauncher->GetName(), m_strToken);
	strResultFilePath.Format(L"%s\\%s.txt",strReportDirectory,  m_pAutoLauncher->GetName());
	CGameSourceCompare *pComparer = m_pAutoLauncher->GetComparer(m_strSourcePath, m_strTempSourcePath);

	if (NULL == pComparer)
	{
		CMyUtils::WriteErrorLog(L"Failure to get comparer");
		return FALSE;
	}

	if (FALSE == pComparer->Compare(strResultFilePath))
	{
		CMyUtils::WriteErrorLog(L"Failure to Compare source game");
		return FALSE;
	}
	
	if (FALSE == pComparer->HasUpdate())
	{
		if (FALSE == pComparer->DeleteResultFile())
		{
			CMyUtils::WriteErrorLog(L"Failure to delete result file when there isn't update");
			return FALSE;
		}
	} 
	
	return TRUE;
}

BOOL CAutoUpdateTool::RemoveTempSource()
{
	return TRUE;
}

CString CAutoUpdateTool::GetName()
{
	if (NULL == m_pAutoLauncher) {
		return L"";
	}
	return m_pAutoLauncher->GetName();
}

void CAutoUpdateTool::WriteDebugLog( const CString &strLog )
{
	_tprintf(strLog + L"\n");
	CGlobalClass::GetInstance()->GetLogWriter()->WriteLog(LOG_TYPE_FYI, strLog);
}

void CAutoUpdateTool::WriteErrorLog( const CString &strLog )
{
	_tprintf(strLog + L"\n");
	CGlobalClass::GetInstance()->GetLogWriter()->WriteLog(LOG_TYPE_ERROR, strLog);
}
