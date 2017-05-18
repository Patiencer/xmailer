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

unit XMailerUtils;

{$IFDEF FPC}
 {$MODE DELPHI}
{$ENDIF}

interface

uses
  SysUtils,
  libxmailer;

function C2S(const cs: pcchar): string; inline;

function S2C(const S: string): pcchar; inline;

function QuotedPrintableEncode(const S: string): string;

function FileToBase64(const AFileName: TFileName): string;

implementation

function C2S(const cs: pcchar): string;
var
  R: RawByteString;
begin
  if not Assigned(cs) then
    Exit('');
  SetString(R, cs, Length(cs) * SizeOf(AnsiChar));
  SetCodePage(R, CP_UTF8, False);
  Result := string(R);
end;

function S2C(const S: string): pcchar;
begin
  Result := pcchar(AnsiString(S));
end;

function QuotedPrintableEncode(const S: string): string;
begin
  Result := C2S(xm_qprint(S2C(S)));
end;

function FileToBase64(const AFileName: TFileName): string;
begin
  Result := C2S(xm_ftob64(S2C(AFileName)));
end;

end.
