#include "StdAfx.h"
#include "MySqlNhanVienDAO.h"
#include "MyException.h"
#include "StringConverter.h"

CMySqlNhanVienDAO::CMySqlNhanVienDAO(char *pcUserName, char *pcPassword, char *pcServerAddress, char *pcDatabase)
{
	m_pSQLDataAccessHelper = new CMySQLDataAccessHelper(pcUserName, pcPassword, pcServerAddress, pcDatabase);
}

CMySqlNhanVienDAO::~CMySqlNhanVienDAO(void)
{
	if (m_pSQLDataAccessHelper != NULL) {
		delete m_pSQLDataAccessHelper ;
	}
}

size_t CMySqlNhanVienDAO::GetAll(list<NHANVIEN> *plstNhanVien){

	if (NULL == plstNhanVien) {
		throw new CMyException("NULL Pointer", NULL_POINTER);
	}

	plstNhanVien->clear();

	char *strQuery = "SELECT maNhanVien, tenNhanVien, gioiTinh, diaChi, soDienThoai, tenPhong FROM NHANVIEN nv, PHONGBAN PB WHERE nv.maphong = pb.maphong order by manhanvien";
	MYSQL_RES *pMySQLQueryResult = m_pSQLDataAccessHelper->ExecuteQuery(strQuery);

	int iNumFields = mysql_num_fields(pMySQLQueryResult);
	int MAX_NUM_FIELDS = 6;
	CStringConverter stringConverter;

	if (MAX_NUM_FIELDS == iNumFields) {

		MYSQL_ROW mySQLResultRow;

		while ((mySQLResultRow = mysql_fetch_row(pMySQLQueryResult))) {

			NHANVIEN nhanVien;

			nhanVien.iMaNhanVien = atoi(mySQLResultRow[0]);

			const TCHAR *ptcBuffer = stringConverter.UTF8ToUnicode(mySQLResultRow[1]);
			_tcscpy_s(nhanVien.strTenNhanVien, MAX_NAME + 1, ptcBuffer);

			nhanVien.cGioiTinh = (char)atoi(mySQLResultRow[2]);

			ptcBuffer = stringConverter.UTF8ToUnicode(mySQLResultRow[3]);
			_tcscpy_s(nhanVien.strDiaChi, MAX_ADDRESS + 1, ptcBuffer);

			ptcBuffer = stringConverter.UTF8ToUnicode(mySQLResultRow[4]);
			_tcscpy_s(nhanVien.strSoDienThoai, MAX_PHONE + 1, ptcBuffer);

			ptcBuffer = stringConverter.UTF8ToUnicode(mySQLResultRow[5]);
			_tcscpy_s(nhanVien.strTenPhongBan, MAX_NAME + 1, ptcBuffer);


			plstNhanVien->push_back(nhanVien);
		}
	}

	return plstNhanVien->size();
}

bool CMySqlNhanVienDAO::AddNhanVien(const NHANVIEN &nhanVien, int iMaPhongBan) {
	
	char cstrTenNhanVien[MAX_NAME*2 + 1];
	char cstrDiaChi[MAX_ADDRESS*2 + 1];
	char cstrSoDienThoai[MAX_PHONE*2 + 1];

	CStringConverter stringConverter;
	
	char *pcBuffer = stringConverter.UnicodeToUTF8(nhanVien.strTenNhanVien);
	strcpy_s(cstrTenNhanVien, MAX_NAME*2 + 1, pcBuffer);

	pcBuffer = stringConverter.UnicodeToUTF8(nhanVien.strDiaChi);
	strcpy_s(cstrDiaChi, MAX_ADDRESS*2 + 1, pcBuffer);

	pcBuffer = stringConverter.UnicodeToUTF8(nhanVien.strSoDienThoai);
	strcpy_s(cstrSoDienThoai, MAX_PHONE*2 + 1, pcBuffer);

	CStringA cstrQuery;
	cstrQuery.Format("INSERT INTO NhanVien (TenNhanVien,GioiTinh,DiaChi,SoDienThoai,MaPhong) VALUES ('%s',%d,'%s','%s',%d)", cstrTenNhanVien, nhanVien.cGioiTinh, cstrDiaChi, cstrSoDienThoai, iMaPhongBan);
	m_pSQLDataAccessHelper->ExecuteNonQuery(cstrQuery.GetBuffer());

	return TRUE;
}

bool CMySqlNhanVienDAO::UpdateNhanVien(const NHANVIEN &nhanVien, int iMaPhongBan) {

	char cstrTenNhanVien[MAX_NAME*2 + 1];
	char cstrDiaChi[MAX_ADDRESS*2 + 1];
	char cstrSoDienThoai[MAX_PHONE*2 + 1];

	CStringConverter stringConverter;
	
	char *pcBuffer = stringConverter.UnicodeToUTF8(nhanVien.strTenNhanVien);
	strcpy_s(cstrTenNhanVien, MAX_NAME*2 + 1, pcBuffer);

	pcBuffer = stringConverter.UnicodeToUTF8(nhanVien.strDiaChi);
	strcpy_s(cstrDiaChi, MAX_ADDRESS*2 + 1, pcBuffer);

	pcBuffer = stringConverter.UnicodeToUTF8(nhanVien.strSoDienThoai);
	strcpy_s(cstrSoDienThoai, MAX_PHONE*2 + 1, pcBuffer);

	CStringA cstrQuery;
	cstrQuery.Format("UPDATE NHANVIEN SET tenNhanVien = '%s', gioiTinh = %d, diaChi = '%s', soDienThoai = '%s', MAPHONG = %d where manhanvien = %d", cstrTenNhanVien, nhanVien.cGioiTinh,  cstrDiaChi, cstrSoDienThoai, iMaPhongBan, nhanVien.iMaNhanVien);
	m_pSQLDataAccessHelper->ExecuteNonQuery(cstrQuery.GetBuffer());
	return true;
}

bool CMySqlNhanVienDAO::DeleteNhanVien(int iMaNhanVien) {
	CStringA cstrQuery;
	cstrQuery.Format("delete from nhanvien where manhanvien = %d", iMaNhanVien);
	m_pSQLDataAccessHelper->ExecuteNonQuery(cstrQuery.GetBuffer());
	return true;
}