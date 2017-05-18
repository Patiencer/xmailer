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

unit XMailerSMTP;

{$IFDEF FPC}
 {$MODE DELPHI}
{$ENDIF}

interface

uses
  SysUtils,
  Classes,
  libxmailer,
  XMailerUtils;

resourcestring
  SXMailerSMTPInvalidCfg = 'Invalid configuration';

type

  { EXMailerSMTP }

  EXMailerSMTP = class(Exception);

  { TXMailerSMTPErrorEvent }

  TXMailerSMTPErrorEvent = procedure(ASender: TObject;
    const AErrorMsg: string) of object;

  { TXMailerSMTP }

  TXMailerSMTP = class(TComponent)
  private class var
    GInitialized: Boolean;
  private
    FPrepared: Boolean;
    Fsmtp: Pxm_smtp;
    FDebug: Boolean;
    FHost: string;
    FOnError: TXMailerSMTPErrorEvent;
    FPassword: string;
    FPort: Word;
    FTimeout: Cardinal;
    FTLS: Boolean;
    FUser: string;
    FVerifyCert: Boolean;
    function GetHandle: Pointer;
  protected
    class procedure DoError(Acls: Pcvoid; const Amsg: Pcchar); cdecl; static;
  public
    constructor Create(AOwner: TComponent); override;
    destructor Destroy; override;
    class procedure Initialize;
    class procedure Finalize;
    procedure Prepare;
    procedure Unprepare;
    property Handle: Pointer read GetHandle;
    property Prepared: Boolean read FPrepared write FPrepared;
  published
    property Host: string read FHost write FHost;
    property Port: Word read FPort write FPort default 587;
    property TLS: Boolean read FTLS write FTLS default True;
    property VerifyCert: Boolean read FVerifyCert write FVerifyCert
      default False;
    property User: string read FUser write FUser;
    property Password: string read FPassword write FPassword;
    property Timeout: Cardinal read FTimeout write FTimeout
      default XM_SMTP_TIMEOUT;
    property Debug: Boolean read FDebug write FDebug default False;
    property OnError: TXMailerSMTPErrorEvent read FOnError write FOnError;
  end;

procedure Register;

implementation

procedure Register;
begin
  RegisterComponents('XMailer', [TXMailerSMTP]);
end;

{ TXMailerSMTP }

constructor TXMailerSMTP.Create(AOwner: TComponent);
begin
  inherited Create(AOwner);
  FPort := 587;
  FTimeout := XM_SMTP_TIMEOUT;
  FTLS := True;
end;

destructor TXMailerSMTP.Destroy;
begin
  Unprepare;
  inherited Destroy;
end;

class procedure TXMailerSMTP.Initialize;
begin
  if not GInitialized then
  begin
    xm_initialize;
    GInitialized := True;
  end;
end;

class procedure TXMailerSMTP.Finalize;
begin
  if GInitialized then
  begin
    xm_finalize;
    GInitialized := False;
  end;
end;

function TXMailerSMTP.GetHandle: Pointer;
begin
  Result := Fsmtp;
end;

class procedure TXMailerSMTP.DoError(Acls: Pcvoid; const Amsg: Pcchar);
var
  VSMTP: TXMailerSMTP absolute Acls;
begin
  if Assigned(VSMTP.OnError) then
    VSMTP.OnError(VSMTP, TrimRight(C2S(Amsg)));
end;

procedure TXMailerSMTP.Prepare;
begin
  Initialize;
  if Assigned(Fsmtp) then
    Exit;
  Fsmtp := xm_smtp_new(S2C(FHost), FPort, FTLS, FVerifyCert, S2C(FUser),
    S2C(FPassword), FTimeout, FDebug, nil, @DoError, Self);
  if not xm_smtp_ok(Fsmtp) then
  begin
    Unprepare;
    raise EXMailerSMTP.CreateRes(@SXMailerSMTPInvalidCfg);
  end;
  FPrepared := True;
end;

procedure TXMailerSMTP.Unprepare;
begin
  if not Assigned(Fsmtp) then
    Exit;
  xm_smtp_free(Fsmtp);
  Fsmtp := nil;
  FPrepared := False;
end;

initialization

finalization
  TXMailerSMTP.Finalize;

end.
