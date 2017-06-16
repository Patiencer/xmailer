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

#include <stdbool.h>
#include <curl/curl.h>
#include <microutils.h>
#include "xm_strs.h"
#include "xm_smtp_defs.h"
#include "xm_smtp.h"

static size_t _xm_smtp_read_data_cb(void *buf, size_t size, size_t nmemb, void *cls) {
    struct _xm_smtp_payld *payld = cls;
    size_t len = payld->size - payld->pos;
    if (len > size * nmemb)
        len = size * nmemb;
    memcpy(buf, payld->data + payld->pos, len);
    payld->pos += len;
    return len;
}

struct xm_smtp *xm_smtp_new(const char *host, long port, bool tls, bool vrfy_cert, const char *user, const char *pwd,
                            long timeout, bool debug, xm_bound_gen_cb bound_gen_cb, xm_err_cb err_cb, void *err_cls) {
    struct xm_smtp *smtp;
    long tm;
    if (mu_is_empty(host)) {
        _XM_ERR(err_cb, err_cls, S_XM_REQ_ARG, "host");
        return NULL;
    }
    if (mu_is_empty(user)) {
        _XM_ERR(err_cb, err_cls, S_XM_REQ_ARG, "user");
        return NULL;
    }
    if (mu_is_empty(pwd)) {
        _XM_ERR(err_cb, err_cls, S_XM_REQ_ARG, "pwd");
        return NULL;
    }
    if (!(smtp = calloc(1, sizeof(struct xm_smtp)))) {
        _XM_ERR(err_cb, err_cls, S_XM_OBJ_ERR, "smtp");
        return NULL;
    }
    smtp->debug = debug;
    if (bound_gen_cb)
        smtp->bound_gen_cb = bound_gen_cb;
    else
        smtp->bound_gen_cb = xm_bound_gen;
    smtp->curl = curl_easy_init();
    if (!smtp->curl) {
        free(smtp);
        _XM_ERR(err_cb, err_cls, S_XM_OBJ_ERR, "curl");
        return NULL;
    }
    tm = timeout > 0 ? timeout : XM_SMTP_TIMEOUT;
    curl_easy_setopt(smtp->curl, CURLOPT_VERBOSE, smtp->debug);
    curl_easy_setopt(smtp->curl, CURLOPT_URL, host);
    if (port > 0)
        curl_easy_setopt(smtp->curl, CURLOPT_PORT, port);
    if (tls) {
        curl_easy_setopt(smtp->curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
        if (!vrfy_cert) {
            curl_easy_setopt(smtp->curl, CURLOPT_SSL_VERIFYHOST, 0L);
            curl_easy_setopt(smtp->curl, CURLOPT_SSL_VERIFYPEER, 0L);
        }
    }
    curl_easy_setopt(smtp->curl, CURLOPT_USERNAME, user);
    curl_easy_setopt(smtp->curl, CURLOPT_PASSWORD, pwd);
    curl_easy_setopt(smtp->curl, CURLOPT_CONNECTTIMEOUT_MS, tm);
    curl_easy_setopt(smtp->curl, CURLOPT_TIMEOUT_MS, tm * 6);
    curl_easy_setopt(smtp->curl, CURLOPT_READFUNCTION, &_xm_smtp_read_data_cb);
    curl_easy_setopt(smtp->curl, CURLOPT_UPLOAD, true);
    smtp->ok = true;
    return smtp;
}

void xm_smtp_free(struct xm_smtp *smtp) {
    if (smtp) {
        if (smtp->curl)
            curl_easy_cleanup(smtp->curl);
        free(smtp);
    }
}

bool xm_smtp_ok(struct xm_smtp *smtp) {
    return smtp && smtp->ok;
}

static bool _xm_stolist_cb(void *cls, const char *step, const char *token) {
    struct curl_slist **list;
    char *email, *tmp;
    if (token) {
        list = cls;
        email = mu_ibetween(token, "<", ">");
        if (email) {
            tmp = email;
            email = mu_trim(email);
            free(tmp);
        } else
            email = mu_trim(token);
        if (*list)
            *list = curl_slist_append(*list, email);
        else
            *list = curl_slist_append(NULL, email);
        free(email);
    }
    return true;
}


void _xm_stolist(struct curl_slist **list, const char *to) {
    mu_split(to, ",", &_xm_stolist_cb, list);
}

bool xm_smtp_send(struct xm_smtp *smtp, enum XM_SMTP_CT ct, const char *from, const char *to,
                  const char *subject, const char *msg, const char *attach, const char *cc, const char *bcc,
                  xm_err_cb err_cb, void *err_cls) {
    struct _xm_smtp_payld *payld;
    struct curl_slist *to_list;
    CURLcode code;
    char *from_, *tmp;
    bool ret;
    if (!smtp) {
        _XM_ERR(err_cb, err_cls, S_XM_REQ_ARG, "smtp");
        return false;
    }
    if (!smtp->ok) {
        _XM_ERR(err_cb, err_cls, S_XM_SMTP_INV_CFG);
        return false;
    }
    if (mu_is_empty(from)) {
        _XM_ERR(err_cb, err_cls, S_XM_REQ_ARG, "from");
        return false;
    }
    if (mu_is_empty(to)) {
        _XM_ERR(err_cb, err_cls, S_XM_REQ_ARG, "to");
        return false;
    }
    payld = calloc(1, sizeof(struct _xm_smtp_payld));
    if (!payld) {
        _XM_ERR(err_cb, err_cls, S_XM_OBJ_ERR, "payload");
        return false;
    }
    from_ = mu_ibetween(from, "<", ">");
    if (from_) {
        tmp = from_;
        from_ = mu_trim(from_);
        free(tmp);
    } else
        from_ = mu_trim(from);
    curl_easy_setopt(smtp->curl, CURLOPT_MAIL_FROM, from_);
    payld->data = _xm_smtp_payld_fmt(ct, subject, from_, to, cc, msg, attach, smtp->bound_gen_cb);
    payld->size = strlen(payld->data);
    /*curl_easy_setopt(smtp->curl, CURLOPT_INFILESIZE, payld->pos);*/
    curl_easy_setopt(smtp->curl, CURLOPT_READDATA, payld);
    to_list = NULL;
    _xm_stolist(&to_list, to);
    if (!mu_is_empty(cc))
        _xm_stolist(&to_list, cc);
    if (!mu_is_empty(bcc))
        _xm_stolist(&to_list, bcc);
    curl_easy_setopt(smtp->curl, CURLOPT_MAIL_RCPT, to_list);
    if (smtp->debug) {
        fprintf(stdout, "=== XMAILER:START ===\n%s\n=== XMAILER:END ===\n", payld->data);
        fflush(stdout);
    }
    code = curl_easy_perform(smtp->curl);
    free(from_);
    free(payld->data);
    free(payld);
    if (to_list)
        curl_slist_free_all(to_list);
    ret = code == CURLE_OK;
    if (!ret && err_cb)
        err_cb(err_cls, curl_easy_strerror(code));
    return ret;
}