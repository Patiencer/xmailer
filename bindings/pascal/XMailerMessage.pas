(*
 * libxmailer - Cross-platform C library to send e-mails from any application.
 *
 * Copyright (c) 2013-2017 Silvio Clecio
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *)

unit XMailerMessage;

{$IFDEF FPC}
 {$MODE DELPHI}
{$ENDIF}

interface

uses
  SysUtils,
  Classes,
  libxmailer,
  XMailerUtils,
  XMailerSMTP;

resourcestring
  SXMailerMessageSMTPNotAssigned = 'SMTP not assigned';

type

  { EXMailerMessage }

  EXMailerMessage = class(Exception);

  { TXMailerContentType }

  TXMailerContentType = (xmctText, xmctHTML);

  { TXMailerMessage }

  TXMailerMessage = class(TComponent)
  private
    FAttachments: TStringList;
    FBCC: string;
    FCC: string;
    FContentType: TXMailerContentType;
    FFrom: string;
    FMessage: TStringList;
    FSMTP: TXMailerSMTP;
    FSubject: string;
    FTo: string;
    procedure SetAttachments(AValue: TStringList);
    procedure SetMessage(AValue: TStringList);
  protected
    procedure Notification(AComponent: TComponent;
      AOperation: TOperation); override;
    class procedure DoErrCb(Acls: Pcvoid; const Amsg: Pcchar); cdecl; static;
  public
    constructor Create(AOwner: TComponent); override;
    destructor Destroy; override;
    function Send: Boolean;
  published
    property SMTP: TXMailerSMTP read FSMTP write FSMTP;
    property ContentType: TXMailerContentType read FContentType
      write FContentType;
    property From: string read FFrom write FFrom;
    property &To: string read FTo write FTo;
    property Subject: string read FSubject write FSubject;
    property CC: string read FCC write FCC;
    property BCC: string read FBCC write FBCC;
    property Message: TStringList read FMessage write SetMessage;
    property Attachments: TStringList read FAttachments write SetAttachments;
  end;

procedure Register;

implementation

procedure Register;
begin
  RegisterComponents('XMailer', [TXMailerMessage]);
end;

{ TXMailerMessage }

constructor TXMailerMessage.Create(AOwner: TComponent);
begin
  inherited Create(AOwner);
  FMessage := TStringList.Create;
  FAttachments := TStringList.Create;
end;

destructor TXMailerMessage.Destroy;
begin
  FAttachments.Free;
  FMessage.Free;
  inherited Destroy;
end;

procedure TXMailerMessage.SetAttachments(AValue: TStringList);
begin
  if Assigned(AValue) then
    FAttachments.Clear
  else
    FAttachments.Assign(AValue);
end;

procedure TXMailerMessage.SetMessage(AValue: TStringList);
begin
  if Assigned(AValue) then
    FMessage.Assign(AValue)
  else
    FMessage.Clear;
end;

procedure TXMailerMessage.Notification(AComponent: TComponent;
  AOperation: TOperation);
begin
  inherited Notification(AComponent, AOperation);
  if (AOperation = opRemove) and (AComponent = FSMTP) then
    SMTP := nil;
end;

class procedure TXMailerMessage.DoErrCb(Acls: Pcvoid; const Amsg: Pcchar);
var
  VSMTP: TXMailerSMTP absolute Acls;
begin
  if Assigned(VSMTP.OnError) then
    VSMTP.OnError(VSMTP, TrimRight(C2S(Amsg)));
end;

function TXMailerMessage.Send: Boolean;
const
  Vct: array[TXMailerContentType] of cenum = (XM_SMTP_CT_TEXT, XM_SMTP_CT_HTML);
begin
  if not Assigned(FSMTP) then
    raise EXMailerMessage.CreateRes(@SXMailerMessageSMTPNotAssigned);
  FSMTP.Prepare;
  Result := xm_smtp_send(FSMTP.Handle, Vct[FContentType], S2C(FFrom), S2C(FTo),
    S2C(FSubject), S2C(TrimRight(FMessage.Text)), S2C(FAttachments.CommaText),
    S2C(FCC), S2C(FBCC), @DoErrCb, FSMTP);
end;

end.
