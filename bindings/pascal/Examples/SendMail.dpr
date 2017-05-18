program SendMail;

{$IFDEF FPC}
 {$MODE DELPHI}
{$ENDIF}

uses
{$IFDEF UNIX}
  CThreads,
{$ENDIF}
  Interfaces,
  Forms,
  frmMain;

begin
  RequireDerivedFormResource := True;
  Application.Initialize;
  Application.CreateForm(TfrMain, frMain);
  Application.Run;
end.
