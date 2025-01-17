//---------------------------------------------------------------------------

#ifndef UFrmPrincipalH
#define UFrmPrincipalH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>

#include "USdtfx.h"
#include "UBTService.h"
#include <ExtCtrls.hpp>
#include "UThreadServerConn.h"
#include <Menus.hpp>
#include <pngimage.hpp>

//---------------------------------------------------------------------------
class TFrmPrincipal : public TForm
{
__published:	// IDE-managed Components
	TTrayIcon *trayIcon;
	TButton *btnPararServer;
	TListBox *lstStatus;
	TButton *btnIniciar;
	TImage *Image1;
	TPopupMenu *mnuPopup;
	TMenuItem *mnuPararServidor;
	TMenuItem *mnuIniciarServidor;
	TButton *btnLimparLog;
	TMenuItem *mnuSair;
	TMainMenu *mainMenu;
	TMenuItem *mnuMainSair;
	void __fastcall btnIniciarClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall mnuIniciarServidorClick(TObject *Sender);
	void __fastcall mnuPararServidorClick(TObject *Sender);
	void __fastcall trayIconDblClick(TObject *Sender);
	void __fastcall btnPararServerClick(TObject *Sender);
	void __fastcall btnLimparLogClick(TObject *Sender);
	void __fastcall mnuSairClick(TObject *Sender);
	void __fastcall mnuMainSairClick(TObject *Sender);
private:	// User declarations
	ThreadServerConn *thread;

public:		// User declarations
	BluetoothServer *bluetoothServer;
	__fastcall TFrmPrincipal(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFrmPrincipal *FrmPrincipal;
//---------------------------------------------------------------------------
#endif
