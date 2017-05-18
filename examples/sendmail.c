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
#include <stdlib.h>
#include <stdbool.h>
#include <xmailer.h>
#include "getpass.h"

/* Exemple:
 * ./sendmail "My Name <myname@gmail.com>" \
 *      "Contact 1 <contact1@gmail.com>, Contact 2 <contact2@gmail.com>" \
 *      "My subject" "My message." "attachment1.txt, attachment2.pdf, attachment-N.jpg" */
#define USE_FMT "Use: %s <from> <to> <subject> <message> [attachments]\n"

#define HOST "smtp.gmail.com"
#define PORT 587
#define SSL true
#define VRFY_CERT false

#define CHECK_ARG(test, arg_name) do { \
    if (!(test)) { \
        fprintf(stderr, "Argument '%s' cannot be empty\n", (arg_name)); \
        exit(EXIT_FAILURE); \
    } \
} while (0)

static void err_cb(void *cls, const char *msg) {
    fprintf(stderr, "%s\n", msg);
}

int main(int argc, char *argv[]) {
    struct xm_smtp *smtp;
    char user[256];
    const char *from, *to, *subject, *msg, *attach;
    char *pwd;
    bool ok;
    if (argc < 5) {
        fprintf(stderr, USE_FMT, argv[0]);
        exit(EXIT_FAILURE);
    }
    from = argv[1];
    CHECK_ARG(from, "from");
    to = argv[2];
    CHECK_ARG(to, "to");
    subject = argv[3];
    CHECK_ARG(subject, "subject");
    msg = argv[4];
    CHECK_ARG(msg, "msg");
    printf("Inform your SMTP user:\n");
    CHECK_ARG(scanf("%s", user) > 0, "user");
    pwd = GETPWD("Inform your SMTP password:\n");
    CHECK_ARG(pwd, "pwd");
    attach = argv[5];
    xm_initialize();
    smtp = xm_smtp_new(HOST, PORT, SSL, VRFY_CERT, user, pwd, 0, false, NULL, &err_cb, NULL);
    free(pwd);
    ok = xm_smtp_ok(smtp);
    if (ok) {
        ok = xm_smtp_send(smtp, XM_SMTP_CT_TEXT, from, to, subject, msg, attach, NULL, NULL, &err_cb, NULL);
        if (ok)
            printf("E-mail successfully sent!\n");
    }
    xm_smtp_free(smtp);
    xm_finalize();
    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}