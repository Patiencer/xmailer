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

#include <stdio.h>
#include <string.h>
#include <openssl/rand.h>
#include <microutils.h>
#include <xmailer.h>

void xm_bound_gen(char *bound) {
    int i;
    char c;
    RAND_bytes((unsigned char *) bound, XM_BOUND_SIZE);
    for (i = 0; i < XM_BOUND_SIZE; i++) {
        c = (char) (bound[i] & 0xf);
        if (c < 10)
            c += '0';
        else
            c += 'a' - 10;
        bound[i] = c;
    }
    bound[XM_BOUND_SIZE] = 0;
}

char *xm_qprint(const char *str) {
    const char *line_s, *line_e, *in_limit, *tmp;
    char *op, *pp, *res;
    char para[100];
    char out[4];
    int len;
    size_t line_out_s, pp_remain, out_remain;
    if (!str)
        return NULL;
    in_limit = str + strlen(str);
    size_t cur_line_len = 0;
    line_out_s = (strlen(str) * (sizeof(char) * 3)) + sizeof(char);
    res = malloc(line_out_s * sizeof(char));
    if (!res)
        return NULL;
    line_s = NULL;
    line_e = str;
    op = res;
    out_remain = line_out_s;
    do {
        if (!line_s)
            line_s = str;
        else
            line_s = line_e;
#define _XM_QP_TERM "\r\n"
        line_e = strstr(line_s, _XM_QP_TERM);
        if (!line_e)
            line_e = str + strlen(str);
        else
            line_e += strlen(_XM_QP_TERM);
        para[0] = '\0';
        pp = para;
        pp_remain = sizeof(para);
        cur_line_len = 0;
        tmp = line_s;
        while ((tmp < line_e)) {
            if (*tmp < ' ' || *tmp == '=' || *tmp > (sizeof(char) * 126)) {
                snprintf(out, sizeof(out), "=%02X", (unsigned char) *tmp);
                len = sizeof(char) * 3;
            } else {
                snprintf(out, sizeof(out), "%c", *tmp);
                len = sizeof(char);
            }
            if (cur_line_len + len >= (sizeof(char) * 76)) {
                snprintf(op, out_remain, "%s=%s", para, _XM_QP_TERM);
                op += strlen(para);
                out_remain -= strlen(para);
                para[0] = '\0';
                pp_remain = sizeof(para);
                pp = para;
                cur_line_len = (size_t) -1;
            }
            snprintf(pp, pp_remain, "%s", out);
            pp += len;
            pp_remain -= len;
            tmp++;
            cur_line_len += len;
        }
        snprintf(op, out_remain, "%s%s", para, _XM_QP_TERM);
#undef _XM_QP_TERM
        op += (strlen(para) + (sizeof(char) * 2));
        out_remain -= (strlen(para) + (sizeof(char) * 2));
    } while ((line_e < in_limit) && (line_e && *line_e != '\0'));
    return res;
}

unsigned char *xm_ftob64(const char *filename) {
    FILE *fp;
    unsigned char *buf, *res;
    size_t size, len;
    fp = fopen(filename, "rb");
    if (!fp)
        return NULL;
    fseek(fp, 0, SEEK_END);
    size = (size_t) ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (!(buf = malloc(size))) {
        fclose(fp);
        return NULL;
    }
    if ((len = fread(buf, 1, size, fp)) < 1) {
        free(buf);
        fclose(fp);
        return NULL;
    }
    fclose(fp);
    res = mu_b64enc(buf, size, &len);
    free(buf);
    return res;
}