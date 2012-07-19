#include "StdAfx.h"
#include "ConnectSocket.h"
#include "ServerDlg.h"
#include "Messages.h"

CConnectSocket::~CConnectSocket(void)
{
	if (m_ptcFolderName != NULL) {
		delete []m_ptcFolderName;
	}
}

CConnectSocket::CConnectSocket(SOCKET sConnectSocket, CServerDlg *pdlgServer) {

	m_sConnectSocket = sConnectSocket;
	m_dlgServer = pdlgServer;
	m_hThread = NULL;

	m_ptcFolderName = NULL;
}

void CConnectSocket::SetThreadHandle(HANDLE hThread) {
	 m_hThread = hThread;
}

const TCHAR* CConnectSocket::RecevieFolderName( int uiLength )
{
	m_ptcFolderName = new TCHAR[uiLength + 1];

	if (NULL == m_ptcFolderName) {
		return NULL;
	}

	int uiFolderNameBytesLength = uiLength*sizeof(TCHAR);
	int bytesReceived = recv(m_sConnectSocket, (char*)m_ptcFolderName, uiFolderNameBytesLength , 0);

	if (bytesReceived != uiFolderNameBytesLength) {
		return NULL;
	}

	m_ptcFolderName[uiLength] = 0;
	return m_ptcFolderName;
}

BOOL CConnectSocket::GetMessageHeader( int *piMessageType, int *puiMessageLength /*= NULL*/ )
{

	MESSAGE_HEADER messageHeader;
	ZeroMemory(&messageHeader, sizeof(MESSAGE_HEADER));

	int bytesReceived = recv(m_sConnectSocket, (char*)&messageHeader, sizeof(MESSAGE_HEADER), 0);
	if (bytesReceived != sizeof(MESSAGE_HEADER)) {
		return FALSE;
	}

	*piMessageType = messageHeader.iType;
	if (NULL != puiMessageLength) {
		*puiMessageLength = messageHeader.uiLength;
	}

	return TRUE;
}

CServerDlg* CConnectSocket::GetDlg() {
	return m_dlgServer;
}

int CConnectSocket::Destroy()
{
	if (m_hThread != NULL) {
		CloseHandle(m_hThread);
	}

	if (m_sConnectSocket != INVALID_SOCKET) {
		shutdown(m_sConnectSocket, SD_BOTH);
		closesocket(m_sConnectSocket);
	}
	delete this;
	return GetLastError();
}
