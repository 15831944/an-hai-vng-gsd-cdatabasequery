#include "StdAfx.h"
#include "MySqlNhanVienDAO.h"
#include <Strsafe.h>

CMySqlNhanVienDAO::CMySqlNhanVienDAO(char *pcUserName, char *pcPassword, char *pcServerAddress, char *pcDatabase)
{
}

CMySqlNhanVienDAO::~CMySqlNhanVienDAO(void)
{
}

size_t CMySqlNhanVienDAO::GetAll(list<NHANVIEN> *plstNhanVien){

	plstNhanVien->clear();

	for (size_t i = 0; i < 10; i++) {
		NHANVIEN nhanVien;
		nhanVien.cGioiTinh = 1;
		nhanVien.iMaNhanVien = i + 1;

		StringCchCopy(nhanVien.strDiaChi, MAX_ADDRESS + 1, _T("227 Nguyễn Văn Cừ, Q5, TP HCM"));
		StringCchCopy(nhanVien.strSoDienThoai, MAX_PHONE + 1, _T("0978 657 443"));
		StringCchCopy(nhanVien.strTenNhanVien, MAX_NAME + 1, _T("Nguuyễn Văn Á"));
		StringCchCopy(nhanVien.strTenPhongBan, MAX_NAME + 1, _T("Game Software Development"));

		plstNhanVien->push_back(nhanVien);
	}

	return plstNhanVien->size();
}

bool CMySqlNhanVienDAO::AddNhanVien(const NHANVIEN &nhanVien) {
	return true;
}

bool CMySqlNhanVienDAO::UpdateNhanVien(const NHANVIEN &nhanVien) {
	return true;
}

bool CMySqlNhanVienDAO::DeleteNhanVien(int iMaNhanVien) {
	return true;
}