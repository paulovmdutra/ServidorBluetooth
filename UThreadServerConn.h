//---------------------------------------------------------------------------

#ifndef UThreadServerConnH
#define UThreadServerConnH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include "UBTService.h"
#include "USdtfx.h"

//---------------------------------------------------------------------------
class ThreadServerConn : public TThread
{
private:
	BOOL 				isConn;
	TEventServerConn    *event;
	BluetoothServer     *bluetoothServer;
	MessageEvent 	    *messageEvent;

protected:
	void __fastcall Execute();
public:

	__fastcall ThreadServerConn(bool CreateSuspended);
	void __fastcall startServer();
	BOOL __fastcall isConnection();
	void __fastcall setEvent(TEventServerConn *event);
	void __fastcall setMessageEvent(MessageEvent *event);
	BluetoothServer *getBluetoothServer();

};




//---------------------------------------------------------------------------
#endif
