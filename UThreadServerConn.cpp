//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UThreadServerConn.h"
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
//      void __fastcall ThreadServerConn::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------
__fastcall ThreadServerConn::ThreadServerConn(bool CreateSuspended)
	: TThread(CreateSuspended)
{

	bluetoothServer = new BluetoothServer();

}

BluetoothServer *ThreadServerConn::getBluetoothServer()
{
	return this->bluetoothServer;
}

void __fastcall ThreadServerConn::startServer()
{

   bluetoothServer->setEvent(event);

   if (bluetoothServer->startService() == 0)
   {
		SOCKET socket = bluetoothServer->getLocalSocket();

		TThreadServer *threadServer = new TThreadServer(true);
		threadServer->setServerEvent(event);
		threadServer->setLocalSocket(socket);
		threadServer->setMessageEvent(messageEvent);
		threadServer->Start();
   }

}

void __fastcall ThreadServerConn::setEvent(TEventServerConn *event)
{
	this->event = event;
}

void __fastcall ThreadServerConn::setMessageEvent(MessageEvent *event)
{
	this->messageEvent = event;
}

//---------------------------------------------------------------------------
void __fastcall ThreadServerConn::Execute()
{
	startServer();
}

//---------------------------------------------------------------------------
BOOL __fastcall ThreadServerConn::isConnection()
{
	return this->isConn;
}
