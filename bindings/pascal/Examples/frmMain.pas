unit frmMain;

{$IFDEF FPC}
 {$MODE DELPHI}
{$ENDIF}

interface

uses
  StdCtrls,
  Classes,
  Forms,
  Dialogs,
  ComCtrls,
  Spin,
  XMailerSMTP,
  XMailerMessage;

type

  { TfrMain }

  TfrMain = class(TForm)
    btSend: TButton;
    chVerifyCert: TCheckBox;
    chTLS: TCheckBox;
    cbType: TComboBox;
    edHost: TEdit;
    edFrom: TEdit;
    edSubject: TEdit;
    edTo: TEdit;
    edPort: TSpinEdit;
    edTimeout: TSpinEdit;
    edUser: TEdit;
    edPwd: TEdit;
    lbMessage: TLabel;
    lbAttachments: TLabel;
    lbSubject: TLabel;
    lbTo: TLabel;
    lbFrom: TLabel;
    lbType: TLabel;
    lbTimeout: TLabel;
    lbHost: TLabel;
    lbPort: TLabel;
    lbUser: TLabel;
    lbPwd: TLabel;
    lsAttachments: TListBox;
    OpenDialog1: TOpenDialog;
    txMessage: TMemo;
    pcMain: TPageControl;
    tsMessage: TTabSheet;
    tsConfig: TTabSheet;
    XMailerMessage1: TXMailerMessage;
    XMailerSMTP1: TXMailerSMTP;
    procedure btSendClick(Sender: TObject);
    procedure lsAttachmentsClick(Sender: TObject);
    procedure lsAttachmentsDblClick(Sender: TObject);
    procedure XMailerSMTP1Error(ASender: TObject; const AErrorMsg: string);
  public
    procedure Prepare;
  end;

var
  frMain: TfrMain;

implementation

{$R *.lfm}

{ TfrMain }

procedure TfrMain.XMailerSMTP1Error(ASender: TObject; const AErrorMsg: string);
begin
  raise EXMailerSMTP.Create(AErrorMsg);
end;

procedure TfrMain.Prepare;
begin
  XMailerSMTP1.Host := edHost.Text;
  XMailerSMTP1.Port := edPort.Value;
  XMailerSMTP1.TLS := chTLS.Checked;
  XMailerSMTP1.VerifyCert := chVerifyCert.Checked;
  XMailerSMTP1.User := edUser.Text;
  XMailerSMTP1.Password := edPwd.Text;
  XMailerSMTP1.Timeout := edTimeout.Value;
end;

procedure TfrMain.btSendClick(Sender: TObject);
begin
  btSend.Enabled := False;
  try
    Prepare;
    XMailerMessage1.ContentType := TXMailerContentType(cbType.ItemIndex);
    XMailerMessage1.From := edFrom.Text;
    XMailerMessage1.&To := edTo.Text;
    XMailerMessage1.Subject := edSubject.Text;
    XMailerMessage1.Message.Assign(txMessage.Lines);
    XMailerMessage1.Attachments.Assign(lsAttachments.Items);
    if XMailerMessage1.Send then
      ShowMessage('E-mail successfully sent');
  finally
    btSend.Enabled := True;
  end;
end;

procedure TfrMain.lsAttachmentsClick(Sender: TObject);
begin
  if OpenDialog1.Execute then
    lsAttachments.Items.Add(OpenDialog1.FileName);
end;

procedure TfrMain.lsAttachmentsDblClick(Sender: TObject);
begin
  lsAttachments.DeleteSelected;
end;

end.
