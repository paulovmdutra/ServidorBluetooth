//---------------------------------------------------------------------------


#pragma hdrstop

#include "UBTService.h"
#include "USdtfx.h"
#include "UTThreadServer.h"
#include "UFrmPrincipal.h"

using namespace std;

/* Define an example conditional function that depends on the pQos field */
int CALLBACK ConditionAcceptFunc(
	LPWSABUF lpCallerId,
	LPWSABUF lpCallerData,
	LPQOS pQos,
	LPQOS lpGQOS,
	LPWSABUF lpCalleeId,
	LPWSABUF lpCalleeData,
	GROUP FAR * g,
	DWORD_PTR dwCallbackData
	)
{

	if (pQos != NULL) {
		RtlZeroMemory(pQos, sizeof(QOS));
		return CF_ACCEPT;
	} else
		return CF_REJECT;
}

int build_uuid(GUID *uuid, char *service)
{

	char service_buf[37];

	if(service == NULL || strcmp(service, "0") == 0 || strlen(service) == 0) {
		// Use public browse group
		strcpy(service_buf, "00000000-0000-1000-8000-00805F9B34FB");
		service_buf[4] = '1';
		service_buf[5] = '0';
		service_buf[6] = '0';
		service_buf[7] = '2';
		str2uuid(service_buf, uuid);
	}

	// 128 bit
	else if(strlen(service) == 36) {
		if(service[8] != '-' || service[13] != '-' ||
		   service[18] != '-' || service[23] != '-' ) {
			return(-1);
		}
		str2uuid(service, uuid);
	}

	// they left 0x on?
	else if(strlen(service) == 6){
		if(service[0] == '0' && service[1] == 'x' || service[1] == 'X') {
			strcpy(service_buf, "00000000-0000-1000-8000-00805F9B34FB");
			service_buf[4] = service[2];
			service_buf[5] = service[3];
			service_buf[6] = service[4];
			service_buf[7] = service[5];
			str2uuid(service_buf, uuid);
		}

		else {
			return(-1);
		}
	}

	// 16 bit
	else if(strlen(service) == 4) {
		strcpy(service_buf, "00000000-0000-1000-8000-00805F9B34FB");
		service_buf[4] = service[0];
		service_buf[5] = service[1];
		service_buf[6] = service[2];
		service_buf[7] = service[3];
		str2uuid(service_buf, uuid);
	}

	else {
		return(-1);
	}

	return(0);
}

int str2uuid( const char *uuid_str, GUID *uuid )
{
	// Parse uuid128 standard format: 12345678-9012-3456-7890-123456789012
	int i;
	char buf[20] = { 0 };

	strncpy(buf, uuid_str, 8);
	uuid->Data1 = strtoul( buf, NULL, 16 );
	memset(buf, 0, sizeof(buf));

	strncpy(buf, uuid_str+9, 4);
	uuid->Data2 = (unsigned short) strtoul( buf, NULL, 16 );
	memset(buf, 0, sizeof(buf));

	strncpy(buf, uuid_str+14, 4);
	uuid->Data3 = (unsigned short) strtoul( buf, NULL, 16 );
	memset(buf, 0, sizeof(buf));

	strncpy(buf, uuid_str+19, 4);
	strncpy(buf+4, uuid_str+24, 12);
	for( i=0; i<8; i++ ) {
		char buf2[3] = { buf[2*i], buf[2*i+1], 0 };
		uuid->Data4[i] = (unsigned char)strtoul( buf2, NULL, 16 );
	}

	return 0;
}


//---------------------------------------------------------------------------
//	Implementa��o da classe BluetoothServer
//---------------------------------------------------------------------------

BluetoothServer::BluetoothServer()
{
	ulRetCode = 0;
	iAddrLen = sizeof(SOCKADDR_BTH);
	iCxnCount = 0;
	iLengthReceived = 0;
	iTotalLengthReceived = 0;

//	szDataBuffer = {0};
	pszDataBufferIndex = NULL;
	lptstrThisComputerName = NULL;
	dwLenComputerName = MAX_COMPUTERNAME_LENGTH + 1;
	LocalSocket = INVALID_SOCKET;
	ClientSocket = INVALID_SOCKET;
	//wsaVersion = {0};
	/*
		 Essa estrutura fornece informa��es relevantes sobre um determinado servi�o, incluindo
		 a identifica��o de classe de servi�o, de servi�o, aplic�veis nome do espa�o de identificadores
		 de protocolo e informa��es, bem como um conjunto de endere�os de transporte em que o servi�o
		 scuta.
	*/
	//wsaQuerySet = {0};


 //	SockAddrBthLocal = {0};
	lpCSAddrInfo = NULL;
	bContinue = true;
	iMaxCxnCycles = 1;
	iClientSize = sizeof(saClient);
	isConn = false;


}

void BluetoothServer::setEvent(TEventServerConn *event)
{
	this->event = event;
}

SOCKET  BluetoothServer::getLocalSocket()
{

	return this->LocalSocket;

}

BOOL BluetoothServer::isConnected()
{
	return this->isConn;
}

int BluetoothServer::startService()
{

	char szDataBuffer[CXN_TRANSFER_DATA_LENGTH] =  {0};

	/*
		 Essa estrutura fornece informa��es relevantes sobre um determinado servi�o, incluindo
		 a identifica��o de classe de servi�o, de servi�o, aplic�veis nome do espa�o de identificadores
		 de protocolo e informa��es, bem como um conjunto de endere�os de transporte em que o servi�o
		 scuta.
	*/
	WSAQUERYSET        wsaQuerySet = {0};
	SOCKADDR_BTH       SockAddrBthLocal  = {0};

	Flags       =0;
	RecvBytes   =0;

	LONG       ulRetCode = 0;
	WSADATA    WSAData   = {0};

	isConn = false;



		if ( ( ulRetCode = WSAStartup(MAKEWORD(2, 2), &WSAData) ) != 0 )
		{
			printf("-FATAL- | Unable to initialize Winsock version 2.2\n");
		}
		else
			printf("WSAStartup() is OK!\n");

		// Both of these fixed-size allocations can be on the stack
		if ( ( lpCSAddrInfo = (LPCSADDR_INFO) HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(CSADDR_INFO) ) ) == NULL)
		{
			printf("!ERROR! | Unable to allocate memory for CSADDR_INFO\n");
			ulRetCode = 1;
			closeConnection();

			if (event != NULL) {
				event->messageConnection("!ERROR! | Unable to allocate memory for CSADDR_INFO");
			}

		}
		else{

		   printf("HeapAlloc() for CSADDR_INFO (address) is OK!\n");

		   if (event != NULL) {

				event->messageConnection("HeapAlloc() for CSADDR_INFO (address) is OK!");
		   }

		}

		if ( ( lptstrThisComputerName = (LPTSTR) HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, dwLenComputerName ) ) == NULL)
		{

			printf("!ERROR! | Unable to allocate memory for CSADDR_INFO\n");

			if (event != NULL) {
				event->messageConnection("!ERROR! | Unable to allocate memory for CSADDR_INFO");
			}

			ulRetCode = 1;
			closeConnection();

		}
		else{

			printf("HeapAlloc() for CSADDR_INFO (local computer name) is OK!\n");

			if (event != NULL) {
				event->messageConnection("HeapAlloc() for CSADDR_INFO (local computer name) is OK!");
			}

		}


   /*		if ( !GetComputerName(lptstrThisComputerName, &dwLenComputerName) )
		{
			printf("=CRITICAL= | GetComputerName() call failed. WSAGetLastError=[%d]\n", WSAGetLastError());

			if (event != NULL) {
				event->messageConnection("=CRITICAL= | GetComputerName() call failed. WSAGetLastError=[%d]\n");
			}

			ulRetCode = 1;
			closeConnection();
		}
		else
		{
			printf("GetComputerName() is pretty fine!\n");
			printf("Local computer name: %S\n", lptstrThisComputerName);

			LPSTR str = "Local computer name: %S\n";
			if (event != NULL) {
				event->messageConnection(str);
			}

		}
	 */

		lptstrThisComputerName =L"BluetoothServer";

		// Open a bluetooth socket using RFCOMM protocol
		printf("Opening local socket using socket()...\n");

		if ( ( LocalSocket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM) ) == INVALID_SOCKET)
		{
			printf("=CRITICAL= | socket() call failed. WSAGetLastError = [%d]\n", WSAGetLastError());
			ulRetCode = 1;
			closeConnection();
			if (event != NULL) {
				event->messageConnection("=CRITICAL= | socket() call failed. WSAGetLastError = [%d]\n" + WSAGetLastError());
			}
		}


		if ( (2 <= g_iOutputLevel) | (LocalSocket != INVALID_SOCKET))
		{
			printf("*INFO* | socket() call succeeded! Socket = [0x%X]\n", LocalSocket);

			if (event != NULL) {
				event->messageConnection("*INFO* | socket() call succeeded! Socket");
			}

		}

		// Setting address family to AF_BTH indicates winsock2 to use Bluetooth port
		SockAddrBthLocal.addressFamily = AF_BTH;
		SockAddrBthLocal.port = BT_PORT_ANY; //1;


		// bind() associates a local address and port combination

		// with the socket just created. This is most useful when

		// the application is a server that has a well-known port

		// that clients know about in advance.
		printf("Preparando BIND\n\n");

		if ( bind(LocalSocket, (SOCKADDR*) &SockAddrBthLocal, sizeof(SockAddrBthLocal) )  == SOCKET_ERROR)
		{
			printf("=CRITICAL= | bind() call failed w/socket = [0x%X]. Error=[%d]\n", LocalSocket, WSAGetLastError());
			ulRetCode = 1;
			closeConnection();

			if (event != NULL) {
				event->messageConnection("=CRITICAL= | bind() call failed. Error: " + WSAGetLastError() );
			}

		}


		if ( (2 <= g_iOutputLevel) | (bind(LocalSocket, (struct sockaddr *) &SockAddrBthLocal, sizeof(SOCKADDR_BTH) )  != SOCKET_ERROR))
		{
			printf("*INFO* | bind() call succeeded!\n");

			if (event != NULL) {
				event->messageConnection("*INFO* | bind() call succeeded!\n");
			}

		}

		if  ( ( ulRetCode = getsockname(LocalSocket, (struct sockaddr *)&SockAddrBthLocal, &iAddrLen) ) == SOCKET_ERROR)
		{

			printf("=CRITICAL= | getsockname() call failed w/socket = [0x%X]. WSAGetLastError=[%d]\n", LocalSocket, WSAGetLastError());
			ulRetCode = 1;
			closeConnection();
			if (event != NULL) {
				event->messageConnection("=CRITICAL= | getsockname() call failed w/socket");
			}

		}
		else
		{

			 printf("getsockname() is pretty fine!\n");
			 printf("Local address: 0x%x\n", SockAddrBthLocal.btAddr);

		}

	if (ulRetCode == 1)
	{
		return 1;
	}

	// CSADDR_INFO
	lpCSAddrInfo[0].LocalAddr.iSockaddrLength = sizeof( SOCKADDR_BTH );
	lpCSAddrInfo[0].LocalAddr.lpSockaddr = (LPSOCKADDR)&SockAddrBthLocal;
	lpCSAddrInfo[0].RemoteAddr.iSockaddrLength = sizeof( SOCKADDR_BTH );
	lpCSAddrInfo[0].RemoteAddr.lpSockaddr = (LPSOCKADDR)&SockAddrBthLocal;
	lpCSAddrInfo[0].iSocketType = SOCK_STREAM;
	lpCSAddrInfo[0].iProtocol = BTHPROTO_RFCOMM;

	// If we got an address, go ahead and advertise it.
	ZeroMemory(&wsaQuerySet, sizeof(WSAQUERYSET));

	wsaQuerySet.dwSize = sizeof(WSAQUERYSET);
	wsaQuerySet.lpServiceClassId = (LPGUID) &g_guidServiceClass;

	// should be something like "Sample Bluetooth Server"
	wsaQuerySet.lpszServiceInstanceName = lptstrThisComputerName; //Nome do servi�o
	wsaQuerySet.lpszComment = L"Example Service instance registered in the directory service through RnR";
	wsaQuerySet.dwNameSpace = NS_BTH;
	wsaQuerySet.dwNumberOfCsAddrs = 1;      // Must be 1.
	wsaQuerySet.lpcsaBuffer = lpCSAddrInfo; // Required.

	// As long as we use a blocking accept(), we will have a race
	// between advertising the service and actually being ready to
	// accept connections.  If we use non-blocking accept, advertise
	// the service after accept has been called.
	if ( WSASetService(&wsaQuerySet, RNRSERVICE_REGISTER, 0) == SOCKET_ERROR)
	{
		printf("=CRITICAL= | WSASetService() call failed. Error=[%d]\n", WSAGetLastError());

		ulRetCode = 1;
		closeConnection();

		if (event != NULL) {
			event->messageConnection("=CRITICAL= | WSASetService() call failed");
		}

	}
	else{

		printf("WSASetService() looks fine!\n");

		if (event != NULL) {
			event->messageConnection("WSASetService() looks fine!\n");
		}

	}

	// listen() call indicates winsock2 to listen on a given socket for any incoming connection.
	if ( listen(LocalSocket, SOMAXCONN) == SOCKET_ERROR)
	{

		printf("=CRITICAL= | listen() call failed w/socket = [0x%X]. Error=[%d]\n", LocalSocket, WSAGetLastError());
		ulRetCode = 1;
		closeConnection();

		if (event != NULL) {
			event->messageConnection("=CRITICAL= | listen() call failed w/socket");
		}

	}

	if (ulRetCode == 0){

		isConn = true;

		if (event != NULL) {
			event->connected();
		}

	}

	return ulRetCode;

}

void BluetoothServer::closeConnection()
{

	if ( INVALID_SOCKET != ClientSocket )
	{
		this->isConn = false;
		closesocket(ClientSocket);
		ClientSocket = INVALID_SOCKET;
	}

	if ( INVALID_SOCKET != LocalSocket )
	{
		this->isConn = false;
		closesocket(LocalSocket);
		LocalSocket = INVALID_SOCKET;

	}

	if ( NULL != lptstrThisComputerName )
	{
		this->isConn = false;
		HeapFree(GetProcessHeap(), 0, lptstrThisComputerName);
		lptstrThisComputerName = NULL;

	}

	if ( NULL != lpCSAddrInfo )
	{
		this->isConn = false;
		HeapFree(GetProcessHeap(), 0, lpCSAddrInfo);
		lpCSAddrInfo = NULL;
	}

	if(WSACleanup() == 0) {

		 printf("WSACleanup() is OK!\n");

		 this->isConn = false;

		 if (event != NULL) {
			event->disconnected();
		 }

	}
	else{

		 this->isConn = false;
		 printf("WSACleanup() failed miserably with error code %d\n", WSAGetLastError());

	}


}

//---------------------------------------------------------------------------

#pragma package(smart_init)
