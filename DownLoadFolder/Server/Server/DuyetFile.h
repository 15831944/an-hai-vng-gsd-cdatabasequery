#pragma once
#include "vector"
using namespace std;

typedef struct INFOFILE
{
	TCHAR strFileName[MAX_PATH];
	BYTE cTypeFile;
	LONGLONG llSize;
	LONGLONG llLastModified;
}INFOFILE;

class CDuyetFile
{
private:
	
	TCHAR strRootPath[MAX_PATH];
public:
	CDuyetFile(void);
	~CDuyetFile(void);
	void ClearList();
	const TCHAR* FormatPath(const TCHAR *strPath);
	bool ApproveFolder(const TCHAR *strPath,const HANDLE *hFile);
	bool ApproveFolderToList(const TCHAR *strPath, const TCHAR* strFileName);
	//void WriteToFile();
	void WriteElementToFile(const INFOFILE& infoFile, const HANDLE &hFile);

	//void WriteToFileText();
	void WriteElementToFileText(const INFOFILE& infoFile, const HANDLE &hFile);
};
