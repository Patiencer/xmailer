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

unit libxmailer;

{$IFDEF FPC}
 {$MODE DELPHI}
{$ELSE}
 {$IFDEF MSWINDOWS}
  {$DEFINE HAVE_DELAYED}
  {$WARN SYMBOL_PLATFORM OFF}
 {$ENDIF}
{$ENDIF}

interface

const
  LIB_NAME =
{$IFDEF MSWINDOWS}
    'libxmailer-2'
{$ELSE}
 {$IFDEF FPC}
    'xmailer'
 {$ELSE}
    'libxmailer'
 {$ENDIF}
{$ENDIF};

type
{$IF NOT DEFINED(FPC) AND DEFINED(NEXTGEN) AND DEFINED(POSIX)}
  PAnsiChar = PUTF8Char;
{$ENDIF}
  Pcchar = PAnsiChar;
  Pcvoid = Pointer;
  cbool = Boolean;
  cint = LongInt;
  clong = {$IFDEF CPU64}Int64{$ELSE}LongInt{$ENDIF};
  cenum = cint;

const
  XM_ERR_SIZE = 512;
  XM_SMTP_TIMEOUT = 5000;

procedure xm_initialize; cdecl; external LIB_NAME
{$IFDEF HAVE_DELAYED}delayed{$ENDIF};

procedure xm_finalize; cdecl; external LIB_NAME
{$IFDEF HAVE_DELAYED}delayed{$ENDIF};

type
  XM_SMTP_CT = cenum;
const
  XM_SMTP_CT_TEXT = 0;
  XM_SMTP_CT_HTML = 1;

type
  Pxm_smtp = ^xm_smtp;
  xm_smtp = record
  end;

  xm_err_cb = procedure(cls: Pcvoid; const msg: Pcchar); cdecl;

  xm_bound_gen_cb = procedure(buf: Pcchar); cdecl;

function xm_smtp_new(const host: Pcchar; port: clong; tls: cbool;
  vrfy_cert: cbool; const user: Pcchar; const pwd: Pcchar; timeout: clong;
  debug: cbool; bound_gen_cb: xm_bound_gen_cb; err_cb: xm_err_cb;
  err_cls: Pcvoid): Pxm_smtp; cdecl; external LIB_NAME
{$IFDEF HAVE_DELAYED}delayed{$ENDIF};

procedure xm_smtp_free(smtp: Pxm_smtp); cdecl; external LIB_NAME
{$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function xm_smtp_ok(smtp: Pxm_smtp): cbool; cdecl; external LIB_NAME
{$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function xm_smtp_send(smtp: Pxm_smtp; ct: XM_SMTP_CT; const from: Pcchar;
  const &to: Pcchar; const subject: Pcchar; const msg: Pcchar;
  const attach: Pcchar; const cc: Pcchar; const bcc: Pcchar; err_cb: xm_err_cb;
  err_cls: Pcvoid): cbool; cdecl; external LIB_NAME
{$IFDEF HAVE_DELAYED}delayed{$ENDIF};

const
  XM_BOUND_SIZE = 32;
  XM_HEX_UCHARS = '0123456789ABCDEF';

procedure xm_bound_gen(bound: Pcchar); cdecl; external LIB_NAME
{$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function xm_qprint(const str: Pcchar): Pcchar; cdecl; external LIB_NAME
{$IFDEF HAVE_DELAYED}delayed{$ENDIF};

function xm_ftob64(const filename: Pcchar): Pcchar; cdecl; external LIB_NAME
{$IFDEF HAVE_DELAYED}delayed{$ENDIF};

implementation

end.

