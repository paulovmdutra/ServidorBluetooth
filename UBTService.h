//---------------------------------------------------------------------------

#ifndef UBTServiceH
#define UBTServiceH

#include "USdtfx.h"
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <vcl.h>


#define SIO_RCVALL _WSAIOW(IOC_VENDOR,1)

// {B62C4E8D-62CC-404b-BBBF-BF3E3BBB1374}
// {2d266186-01fb-47c2-8d9f-10b8ec891363}
//DEFINE_GUID(g_guidServiceClass, 0xb62c4e8d, 0x62cc, 0x404b, 0xbb, 0xbf, 0xbf, 0x3e, 0x3b, 0xbb, 0x13, 0x74);

DEFINE_GUID(g_guidServiceClass, 0x2d266186, 0x01fb, 0x47c2, 0x8d, 0x9f, 0x10, 0xb8, 0xec, 0x89, 0x13, 0x63);

int CALLBACK ConditionAcceptFunc(
	LPWSABUF lpCallerId,
	LPWSABUF lpCallerData,
	LPQOS pQos,
	LPQOS lpGQOS,
	LPWSABUF lpCalleeId,
	LPWSABUF lpCalleeData,
	GROUP FAR * g,
	DWORD_PTR dwCallbackData
	);

//------------------------------------------------------------------------
// Function: BT_SetService
// Purpose: Registers a new SDP record
//------------------------------------------------------------------------
#define SDP_RECORD_SIZE   0x0000004f
#define SDP_CHANNEL_OFFSET 32

#define CXN_BDADDR_STR_LEN                17   // 6 two-digit hex values plus 5 colons
#define CXN_TRANSFER_DATA_LENGTH          1024   // length of the data to be transferred
#define CXN_MAX_INQUIRY_RETRY             3
#define CXN_DELAY_NEXT_INQUIRY            15
#define DATA_BUFSIZE					  4096
// This just redundant!!!!

int  g_ulMaxCxnCycles = 1, g_iOutputLevel = 0;

struct
{
	BTHNS_SETBLOB   b;
	unsigned char   uca[SDP_RECORD_SIZE];
} bigBlob;



using namespace std;

//
// Eventos
//
class TEventServerConn
{
	public :
	   virtual void  messageConnection(const char *message) = 0;
	   virtual void  connected() = 0;
	   virtual void  disconnected() = 0;
};


class MessageEvent
{
	public :
	   virtual void  receiveMessage(const char *message) = 0;
};

//
// Classe que representa o servidor
//
class BluetoothServer
{
	private:

		TEventServerConn 		 *event;
		ULONG                	 ulRetCode;
		int                  	 iAddrLen;
		int 					 iCxnCount;
		int 					 iLengthReceived;
		int 					 iTotalLengthReceived;
		char *             		 pszDataBufferIndex;
		LPTSTR             		 lptstrThisComputerName;
		DWORD              		 dwLenComputerName;
		SOCKET             		 LocalSocket;
		SOCKET 			   		 ClientSocket;
		WSAVERSION         		 wsaVersion;

		LPCSADDR_INFO      		 lpCSAddrInfo;
		BOOL 					 bContinue;
		int  					 iMaxCxnCycles;

		struct sockaddr_in 		 saClient;

		int iClientSize;
		WSABUF  				 wsabuf;
		DWORD 					 RecvBytes;
		DWORD 					 Flags;
		unsigned long 			 comprimento;
		unsigned long 			 flags;

		WSAOVERLAPPED 			 RecvOverlapped;
		char 					 buffer[DATA_BUFSIZE];
		BOOL 					 isConn;

	public:

		BluetoothServer();
		int startService();
		void closeConnection();
		void setEvent(TEventServerConn *event);
		SOCKET getLocalSocket();
		BOOL isConnected();

};

int str2uuid( const char *uuid_str, GUID *uuid );
int build_uuid(GUID *uuid, char *service);


//---------------------------------------------------------------------------
#endif
