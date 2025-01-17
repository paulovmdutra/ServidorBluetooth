//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UTThreadServer.h"
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(&UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall TThreadServer::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

void TThreadServer::setServerEvent(TEventServerConn *event)
{
	this->serverEvent = event;
}

void __fastcall TThreadServer::startReceive()
{

	WSAOVERLAPPED 	   RecvOverlapped;
	WSABUF  		   wsabuf;
	char               szDataBuffer[CXN_TRANSFER_DATA_LENGTH] = {0};
	char *             pszDataBufferIndex = NULL;
	ULONG              ulRetCode = 0;
	int                iAddrLen = sizeof(SOCKADDR_BTH), iCxnCount = 0, iLengthReceived = 0, iTotalLengthReceived = 0;
	int  			   iMaxCxnCycles = 1;
	BOOL 			   bContinue;
	SOCKET             ClientSocket = INVALID_SOCKET;
	char 			   buffer[DATA_BUFSIZE];
	DWORD 		  	   RecvBytes;
	DWORD 	    	   Flags;


	while(ulRetCode == 0)
	{

				if ( (2 <= g_iOutputLevel) |  (listen(LocalSocket, SOMAXCONN) != SOCKET_ERROR))
				{
					printf("*INFO* | listen() call succeeded!\n");

					if (serverEvent != NULL)
					{
						serverEvent->messageConnection("*INFO* | listen() call succeeded!");
					}

				}

				printf("Listening...\n");
				if (serverEvent != NULL)
				{
					serverEvent->messageConnection("Listening...");
				}

				printf("Vai come�ar sera\n");

				// accept() call indicates winsock2 to wait for any

				// incoming connection request from a remote socket.

				// If there are already some connection requests on the queue,

				// then accept() extracts the first request and creates a new socket and

				// returns the handle to this newly created socket. This newly created

				// socket represents the actual connection that connects the two sockets.
				//if ( ( ClientSocket = WSAAccept(LocalSocket, NULL, NULL, &ConditionAcceptFunc, NULL) ) == INVALID_SOCKET)

				if ( ( ClientSocket = accept(LocalSocket, NULL, NULL) ) == INVALID_SOCKET)
				{
					printf("=CRITICAL= | accept() call failed. Error=[%d]\n", WSAGetLastError());
					ulRetCode = 1;
					break; // Break out of the for loop
				}

				if ( (2 <= g_iOutputLevel) | (ClientSocket != INVALID_SOCKET))
				{
					this->isConn = true;
					printf("*INFO* | accept() call succeeded. CientSocket = [0x%X]\n", ClientSocket);
				}

				// Read data from the incoming stream
				bContinue = TRUE;
				pszDataBufferIndex = &szDataBuffer[0];

				wsabuf.len = DATA_BUFSIZE;
				wsabuf.buf =buffer;

				// Make sure the RecvOverlapped struct is zeroed out
				SecureZeroMemory((PVOID) &RecvOverlapped, sizeof(WSAOVERLAPPED) );

				// Create an event handle and setup an overlapped structure.
				RecvOverlapped.hEvent = WSACreateEvent();

				if (RecvOverlapped.hEvent  == NULL) {

					printf("WSACreateEvent failed: %d\n", WSAGetLastError());
					//freeaddrinfo(result);
					//closesocket(ConnSocket);
		//			return 1;
				}


		   //	}
			  //	ShowMessage("Teste");
			//Recebendo os dados

			iLengthReceived = 1;

			while (iLengthReceived > 0) // bContinue && (iTotalLengthReceived < CXN_TRANSFER_DATA_LENGTH) )
			{

				// recv() call indicates winsock2 to receive data

				// of an expected length over a given connection.

				// recv() may not be able to get the entire length

				// of data at once.  In such case the return value,

				// which specifies the number of bytes received,

				// can be used to calculate how much more data is

				// pending and accordingly recv() can be called again.
				char buffer[CXN_TRANSFER_DATA_LENGTH] = {0};

				//recv(ClientSocket,
				iLengthReceived = 0;
				pszDataBufferIndex = &buffer[0];

				iLengthReceived = recv(ClientSocket, pszDataBufferIndex, (CXN_TRANSFER_DATA_LENGTH - iTotalLengthReceived), 0);

				//iLengthReceived = WSARecv(ClientSocket, &wsabuf,  1, &RecvBytes, &flags, NULL, NULL); //&RecvOverlapped, NULL);

				switch ( iLengthReceived )
				{

					case 0: // socket connection has been closed gracefully

						printf("Socket connection has been closed gracefully!\n");
						bContinue = FALSE;
						break;

					case SOCKET_ERROR:

						printf("=CRITICAL= | recv() call failed. Error=[%d]\n", WSAGetLastError());
						bContinue = FALSE;
						ulRetCode = 1;
						break;

					default: // most cases when data is being read

						/*
						iLengthReceived = WSAWaitForMultipleEvents(1, &RecvOverlapped.hEvent, TRUE, INFINITE, TRUE);

						if (iLengthReceived == WSA_WAIT_FAILED) {
							printf("WSAWaitForMultipleEvents failed with error: %d\n", WSAGetLastError());
							break;
						}

						iLengthReceived = WSAGetOverlappedResult(ClientSocket, &RecvOverlapped, &RecvBytes, FALSE, &Flags);
						if (iLengthReceived == FALSE) {
							printf("WSARecv operation failed with error: %d\n", WSAGetLastError());
							break;
						}

						printf("Read %d bytes\n", RecvBytes);

						WSAResetEvent(RecvOverlapped.hEvent);

						// If 0 bytes are received, the connection was closed
						if (RecvBytes == 0 )
							break;
						*/

						//pszDataBufferIndex += iLengthReceived;
						//iTotalLengthReceived += iLengthReceived;

						if ( (2 <= g_iOutputLevel) |  (iLengthReceived != SOCKET_ERROR))
						{
							//printf("*INFO* | Receiving data of length = [%d]. Current Total = [%d]\n", iLengthReceived, iTotalLengthReceived);

							if (serverEvent != NULL)
							{
								serverEvent->messageConnection("*INFO* | Receiving data of length");
							}

						}

						break;

				}

				if (ulRetCode == 0)
				{

					if ( CXN_TRANSFER_DATA_LENGTH != iTotalLengthReceived )
					{
						printf("+WARNING+ | Data transfer aborted mid-stream. Expected Length = [%d], Actual Length = [%d]\n",
							   CXN_TRANSFER_DATA_LENGTH, iTotalLengthReceived);
					}

					printf("*INFO* | Received following data string from remote device:\n%s\n", buffer);

					if (messageEvent != NULL) {
						messageEvent->receiveMessage(buffer);
					}

				}

			}


			if ( INVALID_SOCKET != ClientSocket )
			{

				// Close the connection
				if ( closesocket(ClientSocket) == SOCKET_ERROR)
				{
					printf("=CRITICAL= | closesocket() call failed w/socket = [0x%X]. Error=[%d]\n", LocalSocket, WSAGetLastError());
					ulRetCode = 1;
				}
				else
				{
					// Make the connection invalid regardless
					ClientSocket = INVALID_SOCKET;

					if ( (2 <= g_iOutputLevel) | (closesocket(ClientSocket) != SOCKET_ERROR) )
					{
						printf("*INFO* | closesocket() call succeeded w/socket=[0x%X]\n", ClientSocket);
					}

				}

				closesocket(ClientSocket);
				ClientSocket = INVALID_SOCKET;

			}

	}



}

__fastcall TThreadServer::TThreadServer(bool CreateSuspended)
	: TThread(CreateSuspended)
{

}

BOOL __fastcall TThreadServer::isConnected()
{
	return this->isConn;
}

void __fastcall TThreadServer::setLocalSocket(SOCKET socket)
{
   this->LocalSocket = socket;
}

void TThreadServer::setMessageEvent(MessageEvent *messageEvent)
{
	this->messageEvent = messageEvent;
}

//---------------------------------------------------------------------------
void __fastcall TThreadServer::Execute()
{
	startReceive();
}
//---------------------------------------------------------------------------
