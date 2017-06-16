/*
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
 */

#include <stdlib.h>
#include <string.h>
#include <microutils.h>
#include <xmailer.h>
#include "xm_smtp_defs.h"
#ifndef CONFIG_H
#define CONFIG_H
#include <config.h>
#endif

char *_xm_smtp_mp_msg_fmt(const char *bound, const char *msg_ct, const char *msg) {
    char *ret, *qp;
    qp = xm_qprint(msg);
#define _XM_SMTP_MP_MSG_FMT \
    "--%s\r\n" \
    "Content-Type: %s\r\n" \
    "Content-Transfer-Encoding: quoted-printable\r\n" \
    "\r\n" \
    "%s"
    ret = mu_fmt(_XM_SMTP_MP_MSG_FMT, bound, msg_ct, qp);
#undef _XM_SMTP_MP_MSG_FMT
    free(qp);
    return ret;
}

char *_xm_smtp_mp_attach_fmt(const char *bound, const char *name, const char *ct, const unsigned char *b64) {
#define _SMTP_MP_ATTACH_FMT \
    "--%s\r\n" \
    "Content-Type: %s; name=\"%s\"\r\n" \
    "Content-Disposition: attachment; filename=\"%s\"\r\n" \
    "Content-Transfer-Encoding: base64\r\n" \
    "\r\n" \
    "%s"
    return mu_fmt(_SMTP_MP_ATTACH_FMT, bound, ct, name, name, b64);
#undef _SMTP_MP_ATTACH_FMT
}

char *_xm_smtp_append(char **src, const char *new) {
    char *tmp;
    size_t len = *src ? strlen(*src) : 0;
    if (!(tmp = realloc(*src, len + strlen(new) + sizeof(char)))) {
        free(tmp);
        return NULL;
    }
    *src = tmp;
    strcpy(*src + len, new);
    return *src;
}

static bool _xm_smtp_mp_attach_split_cb(void *cls, const char *step, const char *token) {
    struct _xm_smtp_multipart *rec = cls;
    unsigned char *b64;
    char *filename, *name, *ct, *buf;
    if (token) {
        filename = mu_trim(token);
        name = mu_filename(filename);
        ct = strdup(mu_fntomime(name));
        b64 = xm_ftob64(filename);
        buf = _xm_smtp_mp_attach_fmt(rec->bound, name, ct, b64);
        free(filename);
        free(name);
        free(ct);
        free(b64);
        if (buf) {
            rec->payld = _xm_smtp_append(&rec->payld, buf);
            free(buf);
        }
    }
    return true;
}

char *_xm_smtp_payld_fmt(enum XM_SMTP_CT ct, const char *subject, const char *from, const char *to, const char *cc,
                         const char *msg, const char *attach, xm_bound_gen_cb bound_gen_cb) {
    struct _xm_smtp_multipart rec;
    char *res, *dt, *hcc, *t, *hct, *end;
    dt = mu_now_rfc2822();
    if (mu_is_empty(cc))
        hcc = strdup("");
    else
        hcc = mu_fmt("Cc: %s\r\n", cc);
    t = mu_fmt("text/%s; charset=UTF-8", ct == XM_SMTP_CT_TEXT ? "plain" : "html");
    if (attach) {
        bound_gen_cb(rec.bound);
        hct = mu_fmt("multipart/mixed; boundary=\"%s\"", rec.bound);
        rec.payld = _xm_smtp_mp_msg_fmt(rec.bound, t, msg ? msg : "");
        if (rec.payld)
            mu_split(attach, ",", &_xm_smtp_mp_attach_split_cb, &rec);
        end = mu_fmt("--%s--", rec.bound);
        rec.payld = _xm_smtp_append(&rec.payld, end);
        free(end);
    } else {
        hct = t;
        t = NULL;
        rec.payld = strdup(msg);
    }
#define _XM_SMTP_PAYLD_FMT \
    "MIME-Version: 1.0\r\n" \
    "Date: %s\r\n" \
    "Subject: %s\r\n" \
    "From: %s\r\n" \
    "To: %s\r\n" \
    "%s" \
    "Content-Type: %s\r\n" \
    "User-Agent: " PACKAGE_STRING "\r\n" \
    "\r\n" \
    "%s"
    res = mu_fmt(_XM_SMTP_PAYLD_FMT, dt, subject ? subject : "", from, to, hcc, hct, rec.payld);
#undef _XM_SMTP_PAYLD_FMT
    free(dt);
    free(hcc);
    free(t);
    free(hct);
    free(rec.payld);
    return res;
}