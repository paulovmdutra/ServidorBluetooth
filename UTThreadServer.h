//---------------------------------------------------------------------------

#ifndef UTThreadServerH
#define UTThreadServerH
//---------------------------------------------------------------------------
#include <Classes.hpp>

#include "USdtfx.h"
#include "UBTService.h"

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

//---------------------------------------------------------------------------
class TThreadServer : public TThread
{
private:

	SOCKET   LocalSocket;
	BOOL     isConn;
	MessageEvent *messageEvent;
	TEventServerConn *serverEvent;
	void __fastcall startReceive();

protected:
	void __fastcall Execute();

public:
	__fastcall TThreadServer(bool CreateSuspended);
	void __fastcall setLocalSocket(SOCKET socket);
	void setMessageEvent(MessageEvent *messageEvent);
	void setServerEvent(TEventServerConn *event);
	BOOL __fastcall isConnected();

};
//---------------------------------------------------------------------------
#endif
