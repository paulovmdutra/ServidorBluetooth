//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UFrmPrincipal.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFrmPrincipal *FrmPrincipal;

using namespace std;

/*
   Processa os eventos do servidor
*/
class TEventSv : public TEventServerConn
{
	private:
		TFrmPrincipal *form;

	public:

	   void  messageConnection(const char *message);
	   void  connected();
	   void  disconnected();
	   void  setForm(TFrmPrincipal *form);

};

void TEventSv::messageConnection(const char *message)
{

};

void TEventSv::connected()
{

	if (this->form->bluetoothServer->isConnected())
	{
		this->form->btnIniciar->Enabled = false;
		this->form->btnPararServer->Enabled = true;
		form->lstStatus->Items->Add("Servidor Bluetooth conectado.");

	}

};

void TEventSv::disconnected()
{
	form->lstStatus->Items->Add("Servidor Bluetooth desconectado.");
};

void  TEventSv::setForm(TFrmPrincipal *form)
{
   this->form = form;
}

/*
  Implementa a classe MessageEvent para processar
  as mensagens enviadas pelo cliente.
*/
class TMessageHandleEvent : public MessageEvent
{
	private:
		TFrmPrincipal *form;

	public:
		void  receiveMessage(const char *message);
		void  setForm(TFrmPrincipal *form);
};

void  TMessageHandleEvent::receiveMessage(const char *message)
{
	const char *UP = "UP";
	const char *DOWN = "DOWN";
	const char *LEFT = "LEFT";
	const char *RIGHT = "RIGHT";
	const char *RETURN = "RETURN";

	if ( std::strcmp(message, UP) == 0 ) {
		keybd_event( VK_UP, 0x45, KEYEVENTF_EXTENDEDKEY | 0,  0 );
	}
	else
		if ( std::strcmp(message , DOWN) == 0 ) {
			keybd_event( VK_DOWN, 0x45, KEYEVENTF_EXTENDEDKEY | 0,  0 );
		}
		else
			if ( std::strcmp(message, LEFT) == 0 ) {
				keybd_event( VK_LEFT, 0x45, KEYEVENTF_EXTENDEDKEY | 0,  0 );
			}
			else
				if (std::strcmp(message , RIGHT )== 0) {
					keybd_event( VK_RIGHT, 0x45, KEYEVENTF_EXTENDEDKEY | 0,  0 );
				}
				else
					if (std::strcmp(message , RETURN )== 0) {
						keybd_event( VK_RETURN, 0x45, KEYEVENTF_EXTENDEDKEY | 0,  0 );
					}

	form->lstStatus->Items->Add(message);
}

void  TMessageHandleEvent::setForm(TFrmPrincipal *form)
{
   this->form = form;
}


//---------------------------------------------------------------------------
__fastcall TFrmPrincipal::TFrmPrincipal(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFrmPrincipal::btnIniciarClick(TObject *Sender)
{

	if ( (this->bluetoothServer == NULL) || (!this->bluetoothServer->isConnected()) ) {

		LONG      ulRetCode = 0;
		WSADATA    WSAData = {0};
		ULONGLONG  ululRemoteBthAddr = 0;

		TEventSv *event = new TEventSv();
		event->setForm(this);

		TMessageHandleEvent *msgEvent = new TMessageHandleEvent();
		msgEvent->setForm(this);

		this->thread = new ThreadServerConn(true);
		this->thread->setEvent(event);
		this->thread->setMessageEvent(msgEvent);

		this->bluetoothServer = thread->getBluetoothServer();

		thread->Start();

	}

}
//---------------------------------------------------------------------------

void __fastcall TFrmPrincipal::FormClose(TObject *Sender, TCloseAction &Action)
{
  this->Hide();
  Action = caNone;
}
//---------------------------------------------------------------------------

void __fastcall TFrmPrincipal::mnuIniciarServidorClick(TObject *Sender)
{
	this->btnIniciar->Click();
}
//---------------------------------------------------------------------------

void __fastcall TFrmPrincipal::mnuPararServidorClick(TObject *Sender)
{
	this->btnPararServer->Click();
}
//---------------------------------------------------------------------------

void __fastcall TFrmPrincipal::trayIconDblClick(TObject *Sender)
{
	this->Show();
}
//---------------------------------------------------------------------------

void __fastcall TFrmPrincipal::btnPararServerClick(TObject *Sender)
{

	if ( (this->bluetoothServer != NULL) && (this->bluetoothServer->isConnected()) ) {

		this->bluetoothServer->closeConnection();

		if (!this->bluetoothServer->isConnected())
		{
			btnIniciar->Enabled = true;
			btnPararServer->Enabled = false;
		}

	}

}
//---------------------------------------------------------------------------

void __fastcall TFrmPrincipal::btnLimparLogClick(TObject *Sender)
{
	lstStatus->Items->Clear();
}
//---------------------------------------------------------------------------

void __fastcall TFrmPrincipal::mnuSairClick(TObject *Sender)
{
	btnPararServer->Click();
	Application->Terminate();

}
//---------------------------------------------------------------------------

void __fastcall TFrmPrincipal::mnuMainSairClick(TObject *Sender)
{
	 mnuSair->Click();
}
//---------------------------------------------------------------------------
