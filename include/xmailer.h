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

#ifndef XMAILER_H
#define XMAILER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef XM_EXTERN
# define XM_EXTERN extern
#endif

#ifndef __nonnull
# define __nonnull(params)
#endif

/* global */

/**
 * \brief Max size of a message error.
 */
#define XM_ERR_SIZE 512

/**
 * \brief SMTP request timeout.
 */
#define XM_SMTP_TIMEOUT 5000

/**
 * \brief Initializes the library.
 * \warning Must be called at least once per application.
 */
XM_EXTERN void xm_initialize();

/**
 * \brief Finalizes the library.
 * \warning Must be called at least once per application.
 */
XM_EXTERN void xm_finalize();

/* SMTP */

/**
 * \brief Content type used for sending emails.
 */
enum XM_SMTP_CT {
    /// Plain text.
    XM_SMTP_CT_TEXT = 0,
    /// HTML text.
    XM_SMTP_CT_HTML = 1
};

/**
 * \brief Object to configure SMTP connection.
 */
struct xm_smtp;

/**
 * \brief Callback for handling errors.
 * \param cls Callback closure.
 * \param msg Error message.
 */
typedef void (*xm_err_cb)(void *cls, const char *msg);

/**
 * \brief Callback for boundary generation.
 */
typedef void (*xm_bound_gen_cb)(char *buf);

/**
 * \brief Creates a new SMTP object.
 * \param host Host name.
 * \param port Port number.
 * \param tls Start TLS.
 * \param vrfy_cert Verify certificate.
 * \param user User name.
 * \param pwd Password.
 * \param timeout Connection timeout.
 * \param debug Enable debugging.
 * \param bound_gen_cb Boundary generation callback.
 * \param err_cb Error callback.
 * \param err_cls Error closure.
 * \return Instance of SMTP object.
 */
XM_EXTERN struct xm_smtp *xm_smtp_new(const char *host, long port, bool tls, bool vrfy_cert, const char *user,
                                      const char *pwd, long timeout, bool debug, xm_bound_gen_cb bound_gen_cb,
                                      xm_err_cb err_cb, void *err_cls) __nonnull((1, 5, 6));

/**
 * \brief Frees a SMTP object instance.
 * \param smtp SMTP object instance.
 */
XM_EXTERN void xm_smtp_free(struct xm_smtp *smtp);

/**
 * \brief Checks if a SMTP object instance is assigned and configured.
 * \param smtp SMTP object instance.
 * \return **true* if assigned and configured, **false** otherwise.
 */
XM_EXTERN bool xm_smtp_ok(struct xm_smtp *smtp);

/**
 * \brief Send e-mail using SMTP protocol.
 * \param smtp SMTP instance object.
 * \param ct Content type.
 * \param from Sender. Valid examples:
 * <pre>
 * myname@@host.com
 * My Name<myname@@host.com>
 * </pre>
 * \param to Receiver(s) (comma separated). Valid examples:
 * <pre>
 * contact@@host.com
 * contact1@@host.com, contact2@@host.com
 * Contact <contact@@host.com>
 * Contact 1 <contact1@@host.com>, Contact 2 <contact2@@host.com>
 * </pre>
 * \param subject Subject.
 * \param msg Message.
 * \param attach Attachments (comma separated).
 * \param cc _Carbon Copy_.
 * \param bcc _Blind Carbon Copy_.
 * \param err_cb Error callback.
 * \param err_cls Error callback closure.
 * \return **true** if successfully sent, **false** otherwise.
 */
XM_EXTERN bool xm_smtp_send(struct xm_smtp *smtp, enum XM_SMTP_CT ct, const char *from, const char *to,
                            const char *subject, const char *msg, const char *attach, const char *cc, const char *bcc,
                            xm_err_cb err_cb, void *err_cls) __nonnull((1, 3, 4, 5, 6));

/* utils */

/**
 * \brief Max size of a boundary hash.
 */
#define XM_BOUND_SIZE 32

/**
 * \brief Hexadecimal representation. (uppercase)
 */
#define XM_HEX_UCHARS (unsigned char) "0123456789ABCDEF"

/**
 * \brief Generates a unique boundary hash in place.
 * \param bound Address of a result string (eg: `bound[XM_BOUND_SIZE]`).
 */
XM_EXTERN void xm_bound_gen(char *bound) __nonnull((1));

/**
 * \brief Quoted-printable encoding.
 * \param str String to be encoded.
 * \return Encoded string.
 * \warning Caller **must** free the returned string.
 */
XM_EXTERN char *xm_qprint(const char *str) __nonnull((1));

/**
 * \brief Encodes a file to base64.
 * \param filename File to be encoded.
 * \return File encoded to base64.
 */
XM_EXTERN unsigned char *xm_ftob64(const char *filename) __nonnull((1));

#ifdef __cplusplus
}
#endif

#endif