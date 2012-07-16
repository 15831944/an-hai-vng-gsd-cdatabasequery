#include "stdafx.h"
#include "ThreadFunction.h"
#include "Messages.h"
#include "MessageHandler.h"

bool SendDownloadFileMessage(SOCKET sConnectSocket, const TCHAR strFileName[], int *piLastError){

	MESSAGE_HEADER messageHeader;
	messageHeader.iType = DOWNLOAD_FILE;
	messageHeader.iLength = 0;

	if (send(sConnectSocket,  (char*)&messageHeader, sizeof(MESSAGE_HEADER), 0) == SOCKET_ERROR) {
		*piLastError = HandleError(sConnectSocket);
		return false;
	}

	FILE_NAME_MESSAGE fileNameMessage;
	_tcscpy_s(fileNameMessage.strFileName, MAX_PATH, strFileName);

	if (send(sConnectSocket, (char*)&fileNameMessage, sizeof(FILE_NAME_MESSAGE), 0) == SOCKET_ERROR) {
		*piLastError = HandleError(sConnectSocket);
		return false;
	}
	return true;
}

bool ReceiveFileSize(SOCKET sConnectSocket, unsigned __int64 *pi64FizeSize, int *piLastError) {
	
	int bytesReceived = recv(sConnectSocket, (char*)pi64FizeSize, sizeof(unsigned __int64), 0);

	if(sizeof(unsigned __int64) != bytesReceived) {
		*piLastError = HandleError(sConnectSocket);
		return false;
	}
	return true;
}

bool WriteData(SOCKET sConnectSocket, const TCHAR strFileName[], const unsigned __int64 &i64FizeSize, int *piLastError) {
	
	unsigned __int64 i64Count = 0;

	HANDLE hFile = ::CreateFile(strFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	
	if (INVALID_HANDLE_VALUE == hFile) {
		*piLastError = HandleError(sConnectSocket);
		return false;
	}

	const int BUFFER_LENGTH = 2048;
	char strBuffer[BUFFER_LENGTH] = {0};
	int bytesReceived = 0;
	DWORD dwBytesWritten = 0;

	while (i64Count < i64FizeSize) {

		bytesReceived = recv(sConnectSocket, strBuffer, BUFFER_LENGTH, 0);

		if (bytesReceived < 0) {
			*piLastError = HandleError(sConnectSocket);
			CloseHandle(hFile);
			return false;
		}

		::WriteFile(hFile, strBuffer, bytesReceived, &dwBytesWritten, NULL);
		i64Count += bytesReceived;
	}
	CloseHandle(hFile);
	return true;
}

bool CheckFileExist(SOCKET sConnectSocket, int *piLastError) {

	MESSAGE_HEADER messageHeader;
	ZeroMemory(&messageHeader, sizeof(MESSAGE_HEADER));
	int bytesReceived = recv(sConnectSocket,(char*)&messageHeader, sizeof(MESSAGE_HEADER), 0);

	if (sizeof(MESSAGE_HEADER) != bytesReceived ) {
		*piLastError = HandleError(sConnectSocket);
		return false;
	}

	if (FILE_NOT_FOUND == messageHeader.iType) {

		HandleError(sConnectSocket);
		*piLastError = 0;
		return false;

	} else if (FILE_FOUND == messageHeader.iType) {
		return true;
	}

	return true;
}

DWORD WINAPI SocketCommunicationThreadFunction(LPVOID lpParam ) {

	SOCKET *pSocket = (SOCKET*)lpParam ;
	SOCKET sConnectSocket = *pSocket;
	delete pSocket;

	int iLastError;
	if (SendDownloadFileMessage(sConnectSocket, _T("abc.exe"), &iLastError) == false) {
		return iLastError;
	}

	if (CheckFileExist(sConnectSocket, &iLastError) == false) {
			::OutputDebugString(_T("khong ton tai file muon down\n"));
	} else {

		unsigned __int64 i64FizeSize;

		if (ReceiveFileSize(sConnectSocket, &i64FizeSize, &iLastError) == false) {
			return iLastError;
		}

		if (WriteData(sConnectSocket, _T("d:\\abc.exe"), i64FizeSize, &iLastError) == false) {
			return iLastError;
		}

		shutdown(sConnectSocket, SD_BOTH);
		closesocket(sConnectSocket);
	}
	return 0;
}

/*
DWORD WINAPI SocketCommunicationThreadFunction(LPVOID lpParam ) {

	SOCKET *pSocket = (SOCKET*)lpParam ;
	SOCKET sConnectSocket = *pSocket;
	delete pSocket;

	int iLastError;
	if (SendDownloadFileMessage(sConnectSocket, _T("xxx.txt"), &iLastError) == false) {
		return iLastError;
	}

	/*
	MESSAGE_HEADER messageHeader;
	messageHeader.iType = DOWNLOAD_FILE;
	messageHeader.iLength = 0;

	if (send(sConnectSocket,  (char*)&messageHeader, sizeof(MESSAGE_HEADER), 0) == SOCKET_ERROR) {
		return HandleError(sConnectSocket);
	}

	FILE_NAME_MESSAGE fileNameMessage;
	_tcscpy_s(fileNameMessage.strFileName, MAX_PATH, _T("kms.exe"));

	if (send(sConnectSocket, (char*)&fileNameMessage, sizeof(FILE_NAME_MESSAGE), 0) == SOCKET_ERROR) {
		return HandleError(sConnectSocket);
	}
	

	MESSAGE_HEADER messageHeader;
	ZeroMemory(&messageHeader, sizeof(MESSAGE_HEADER));
	int bytesReceived = recv(sConnectSocket,(char*)&messageHeader, sizeof(MESSAGE_HEADER), 0);

	if (sizeof(MESSAGE_HEADER) != bytesReceived ) {
		return HandleError(sConnectSocket);
	}

	if (FILE_NOT_FOUND == messageHeader.iType) {
		::OutputDebugString(_T("khong tim thay file\n"));
	} else if (FILE_FOUND == messageHeader.iType) {
		::OutputDebugString(_T("tim thay file\n"));
		unsigned __int64 i64FizeSize;
		bytesReceived = recv(sConnectSocket, (char*)&i64FizeSize, sizeof(i64FizeSize), 0);
		if(sizeof(i64FizeSize) != bytesReceived) {
			return HandleError(sConnectSocket);
		}
		unsigned __int64 i64Count = 0;
		HANDLE hFile = ::CreateFile(_T("d:\\DTLite4454-0315.exe"),	GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == hFile) {
			return HandleError(sConnectSocket);
		}

		const int BUFFER_LENGTH = 2048;
		char strBuffer[BUFFER_LENGTH] = {0};

		while (i64Count < i64FizeSize) {
			bytesReceived = recv(sConnectSocket, strBuffer, BUFFER_LENGTH, 0);
			if (bytesReceived < 0) {
				break;
			}
			DWORD dwBytesWritten = 0;
			::WriteFile(hFile, strBuffer, bytesReceived, &dwBytesWritten, NULL);
			i64Count += bytesReceived;
		}
		CloseHandle(hFile);
	}

	shutdown(sConnectSocket, SD_BOTH);
	closesocket(sConnectSocket);
	return 0;
}
*/