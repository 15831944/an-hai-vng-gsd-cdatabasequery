#pragma once
#include "StdAfx.h"
#include "AccessConvertDB.h"
#include "BlackListDAO.h"
#include "UserDAO.h"
#include "ServiceDAO.h"
#include "Strsafe.h"
#include "StringConverter.h"


CAccessConvertDB::CAccessConvertDB(void)
{
}

CAccessConvertDB::~CAccessConvertDB(void)
{
	
}

BOOL CAccessConvertDB::Connect(const TCHAR* strPath)
{
	
	CString strConnect;
	strConnect.Format(_T("ODBC;Driver={Microsoft Access Driver (*.mdb, *.accdb)};DBQ=%s ;Uid=Admin;Pwd=MASOTHUY0101148362;"), strPath );
	//return m_db.OpenEx(strConnect.GetBuffer(), CDatabase::openReadOnly |CDatabase::noOdbcDialog);
	if(! m_db.Open(NULL, false, false, strConnect))
	{
		strConnect.Format(_T("Driver={Microsoft Access Driver (*.mdb)};Dbq=%s ;Uid=Admin;Pwd=MASOTHUY0101148362;"), strPath );
		return m_db.Open(NULL, false, false, strConnect);
	}
	return TRUE;
}

BOOL CAccessConvertDB::Convert(const TCHAR* strPath)
{
	if(!Connect(strPath))
	{
		::MessageBox(NULL, _T("Không thể kết nối cơ sở dữ liệu đến NetCafe"), _T("Thông báo"),MB_ICONSTOP); 
		return FALSE;
	}
	
	ConvertBlackList();
	
	ConvertService();
	
	ConvertUser();
	
	return TRUE;

}

BOOL CAccessConvertDB::ConvertBlackList()
{
	LPCTSTR query = _T("SELECT BlackList.[Address],BlackList.[DateAdd], BlackList.[Status], BlackList.[Admin]  FROM BlackList;");
	TRY
	{
		CRecordset rs(&m_db);
		if(!rs.Open(CRecordset::dynaset, query, CRecordset::none))
		{
			return FALSE;
		}
		short sFieldCount = rs.GetODBCFieldCount();

		CDBVariant var;
		CString strValue;

		CBlackListDAO blackListDAO;
		if(!blackListDAO.ConnectToDB(m_ConnectStringToMYSQL.m_strUsername,
									m_ConnectStringToMYSQL.m_strPasssword,
									m_ConnectStringToMYSQL.m_strServerAddress,
									m_ConnectStringToMYSQL.m_strDatabaseName)
			)
		{
			::MessageBox(NULL, _T("Không thể kết nối cơ sở dữ liệu đến csm server"), _T("Thông báo"),MB_ICONSTOP); 
			return FALSE;
		}
		//CString strValue;
		
		while (!rs.IsEOF())
		{
			
			
			BLACKLIST blackList;
			// Get value "Address"
			rs.GetFieldValue( _T("Address"), var);
			switch(var.m_dwType)
			{
				case DBVT_STRING:
                        StringCchPrintf(blackList.strURL, MAX_PATH, _T("%s"), var.m_pstring->GetBuffer(var.m_pstring->GetLength()));
                        break;
                    case DBVT_ASTRING:
                        StringCchPrintf(blackList.strURL, MAX_PATH, _T("%s"),  var.m_pstringA->GetBuffer(var.m_pstringA->GetLength()));
                        break;
                    case DBVT_WSTRING:
                        StringCchPrintf(blackList.strURL, MAX_PATH, _T("%s"),  var.m_pstringW->GetBuffer(var.m_pstringW->GetLength()));
                        break;
                    default:
                        StringCchPrintf(blackList.strURL, MAX_PATH, _T(""));
			}

			// Get value "DateAdd"
			rs.GetFieldValue( _T("DateAdd"), var);
			
			CString strDateTime;
			strDateTime.Format(_T("%d-%d-%d %d:%d:%d"), var.m_pdate->year, var.m_pdate->month, var.m_pdate->day,
														var.m_pdate->hour, var.m_pdate->minute, var.m_pdate->second);

			StringCchPrintf(blackList.strRecordDate, MAX_STRINGDATE, _T("%s"), strDateTime.GetBuffer());

			// Get value "Status"
			rs.GetFieldValue( _T("Status"), var);
			if(var.m_dwType == DBVT_BOOL)
			{
				if(var.m_boolVal)
				{
					blackList.iActive = 1;
				}
				else
					blackList.iActive = 0;
			}

			// Get value "Admin"
			rs.GetFieldValue( _T("Admin"), var);
			if(var.m_dwType == DBVT_BOOL)
			{
				if(var.m_boolVal)
				{
					blackList.iAddedBy = 1;
				}
				else
					blackList.iAddedBy = 0;
			}
			blackListDAO.AddBlackList(blackList);
			
			// Next record
			rs.MoveNext();
		}
	}
	CATCH_ALL(e)
	{
		TCHAR  errMsg[255];
		e->GetErrorMessage(errMsg, 255);
		
		return FALSE;
	}
	END_CATCH_ALL
	return TRUE;
}


BOOL CAccessConvertDB::ConvertService()
{
	LPCTSTR query = _T("SELECT Menuname,MenuCost  FROM MenuItem;");
	TRY
	{
		CRecordset rs(&m_db);
		if(!rs.Open(CRecordset::dynaset, query, CRecordset::none))
		{
			return FALSE;
		}
		short sFieldCount = rs.GetODBCFieldCount();

		CDBVariant var;
		CString strValue;

		

		CServiceDAO serviceDAO;
		if(!serviceDAO.ConnectToDB(m_ConnectStringToMYSQL.m_strUsername,
									m_ConnectStringToMYSQL.m_strPasssword,
									m_ConnectStringToMYSQL.m_strServerAddress,
									m_ConnectStringToMYSQL.m_strDatabaseName)
			)
		{
			::MessageBox(NULL, _T("Không thể kết nối cơ sở dữ liệu đến csm server"), _T("Thông báo"),MB_ICONSTOP);
			return FALSE;
		}
		//CString strValue;
		
		while (!rs.IsEOF())
		{
			CSM_SERVICE csm_Service;
			
			// Get MenuName
			rs.GetFieldValue( _T("Menuname"), var);
			VariantToCstring(var, strValue);
			StringCchPrintf(csm_Service.strServiceName, MAX_SERVICENAME, _T("%s"), strValue.GetBuffer());
			strValue.Empty();
			var.Clear();
			
			// Get MenuCost
			rs.GetFieldValue( _T("MenuCost"), var);
			csm_Service.iServicePrice = (int)var.m_dblVal;
			
			serviceDAO.AddService(csm_Service);
			
			// Next record
			rs.MoveNext();
		}
	}
	CATCH_ALL(e)
	{
		TCHAR  errMsg[255];
		e->GetErrorMessage(errMsg, 255);
		return FALSE;
	}
	END_CATCH_ALL
	return TRUE;
}


BOOL CAccessConvertDB::ConvertUser()
{
	LPCTSTR query = _T("SELECT Hoten,Username,Type,RecordDate,ExpiredDate,Address,phone,Email,IDCard,RemainTime,TimeUsed,FreeMinutes FROM Member WHERE Type = 0 or Type = 2;");
	TRY
	{
		CRecordset rs(&m_db);
		if(!rs.Open(CRecordset::dynaset, query, CRecordset::none))
		{
			return FALSE;
		}
		short sFieldCount = rs.GetODBCFieldCount();

		
		
		CUserDAO userDAO;
		if(!userDAO.ConnectToDB(m_ConnectStringToMYSQL.m_strUsername,
									m_ConnectStringToMYSQL.m_strPasssword,
									m_ConnectStringToMYSQL.m_strServerAddress,
									m_ConnectStringToMYSQL.m_strDatabaseName)
			)
		{
			::MessageBox(NULL, _T("Không thể kết nối cơ sở dữ liệu đến csm server"), _T("Thông báo"),MB_ICONSTOP);
			return FALSE;
		}

		CDBVariant var;
		CString strValue;

		
		while (!rs.IsEOF())
		{
			CSM_USER csm_User;
			// Get Fullname
			rs.GetFieldValue(_T("Hoten"), var);
			VariantToCstring(var, strValue);
			StringCchPrintf(csm_User.strName, MAX_NAME, _T("%s"), strValue.GetBuffer());
			strValue.Empty();
			var.Clear();


			// Get Username
			rs.GetFieldValue(_T("Username"), var);
			VariantToCstring(var, strValue);
			StringCchPrintf(csm_User.strUsername, MAX_USERNAME, _T("%s"), strValue.GetBuffer());
			strValue.Empty();
			var.Clear();

			// Set (get) UserType
			int iUserType = 2;
			rs.GetFieldValue(_T("Type"), var);
			VariantToCstring(var, strValue);
			StringCchPrintf(csm_User.strUserType, 3, _T("%s"), strValue.GetBuffer());
			strValue.Empty();
			var.Clear();
			
			if(wcscmp(csm_User.strUserType, _T("0")) == 0)
			{
				iUserType = 4;
				StringCchPrintf(csm_User.strUserType, 6, _T("4"));
			}
			
				
			// Get RecordDate
			rs.GetFieldValue(_T("RecordDate"), var);
			VariantToCstring(var, strValue);
			StringCchPrintf(csm_User.strRecordDate, MAX_STRINGDATE, _T("%s"), strValue.GetBuffer());
			strValue.Empty();
			var.Clear();

			// Get ExpiredDate
			rs.GetFieldValue(_T("ExpiredDate"), var);
			VariantToCstring(var, strValue);
			StringCchPrintf(csm_User.strExpiryDate, MAX_STRINGDATE, _T("%s"), strValue.GetBuffer());
			strValue.Empty();
			var.Clear();

			// Get Address
			rs.GetFieldValue(_T("Address"), var);
			VariantToCstring(var, strValue);
			StringCchPrintf(csm_User.strAddress, MAX_ADDRESS, _T("%s"), strValue.GetBuffer());
			strValue.Empty();
			var.Clear();

			// Get Phone
			rs.GetFieldValue(_T("phone"), var);
			VariantToCstring(var, strValue);
			StringCchPrintf(csm_User.strPhone, MAX_PHONE, _T("%s"), strValue.GetBuffer());
			strValue.Empty();
			var.Clear();

			// Get Email
			rs.GetFieldValue(_T("Email"), var);
			VariantToCstring(var, strValue);
			StringCchPrintf(csm_User.strEmail, MAX_EMAIL, _T("%s"), strValue.GetBuffer());
			strValue.Empty();
			var.Clear();

			// Get RemainTime
			rs.GetFieldValue(_T("RemainTime"), var);
			
			VariantToCstring(var, strValue);
			StringCchPrintf(csm_User.strRemainTime, MAX_INTTIME, _T("%s"), strValue.GetBuffer());
			strValue.Empty();
			var.Clear();

			// Get TimeUsed
			rs.GetFieldValue(_T("TimeUsed"), var);
			VariantToCstring(var, strValue);
			StringCchPrintf(csm_User.strTimeUsed, MAX_INTTIME, _T("%s"), strValue.GetBuffer());
			strValue.Empty();
			var.Clear();

			// Get FreeMinutes
			rs.GetFieldValue(_T("FreeMinutes"), var);
			VariantToCstring(var, strValue);
			StringCchPrintf(csm_User.strFreeTime, MAX_INTTIME, _T("%s"), strValue.GetBuffer());
			strValue.Empty();
			var.Clear();


			// Set active
			StringCchPrintf(csm_User.strActive, 3, _T("1"));

			// Set birthday
			StringCchPrintf(csm_User.strBirthDay, MAX_INTTIME, _T("0000-00-00 0:0:0"));
			
			CStringConverter stringConverter;
			int iRemainTime = atoi(stringConverter.UnicodeToUTF8(csm_User.strRemainTime));
			int iRemainMoney = userDAO.GetMoneyFromTime(iRemainTime, iUserType);
			StringCchPrintf(csm_User.strRemainMoney, MAX_MONEY, _T("%d"), iRemainMoney);
			
			

			//Insert user to csm db
			userDAO.AddUser(csm_User);
			
			// Next record
			rs.MoveNext();
		}
	}
	CATCH_ALL(e)
	{
		TCHAR  errMsg[255];
		e->GetErrorMessage(errMsg, 255);
		return FALSE;
	}
	END_CATCH_ALL
	return TRUE;

}