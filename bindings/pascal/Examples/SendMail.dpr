program SendMail;

{$IFDEF FPC}
 {$MODE DELPHI}
{$ENDIF}

uses
{$IFDEF FPC}
 {$IFDEF UNIX}
  CThreads,
 {$ENDIF}
  Interfaces,
{$ENDIF}
  Forms,
  frmMain in 'frmMain.pas' { frMain };

begin
{$IFDEF FPC}
  RequireDerivedFormResource := True;
{$ELSE}
  Application.MainFormOnTaskbar := True;
{$ENDIF}
  Application.Initialize;
  Application.CreateForm(TfrMain, frMain);
  Application.Run;
end.
