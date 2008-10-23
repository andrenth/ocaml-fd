/*
 * Copyright 2007 Digirati Informática, Serviços e Telecomunicações
 *
 * This file is distributed under the terms of version 2.1 of the GNU
 * Lesser General Public License. See the LICENSE file for details.
 */

#include <sys/types.h>
#include <sys/socket.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef __GLIBC__
#define __USE_GNU
#endif

#include <unistd.h>

#include <caml/mlvalues.h>
#include <caml/alloc.h>
#include <caml/callback.h>
#include <caml/fail.h>
#include <caml/memory.h>
#include <caml/signals.h>

/*
 * Starting with ocaml-3.09.3, CAMLreturn can only be used for ``value''
 * types. CAMLreturnT was only added in 3.09.4, so we define our own
 * version here if needed.
 */
#ifndef CAMLreturnT
#define CAMLreturnT(type, result)             \
    do {                                      \
        type caml__temp_result = (result);    \
        caml_local_roots = caml__frame;       \
        return (caml__temp_result);           \
    } while (0)
#endif

static void
fd_error(char *s)
{
    CAMLlocal2(exn, msg);

    exn = alloc_small(2, 0);
    msg = caml_copy_string(s);
    Field(exn, 0) = *caml_named_value("Fd.Fd_error");
    Field(exn, 1) = msg;
    caml_raise(exn);
}

/*
 * These functions are adapted from Stevens, Fenner and Rudoff, UNIX Network
 * Programming, Volume 1, Third Edition. We use CMSG_LEN instead of CMSG_SPACE
 * for the msg_controllen field of struct msghdr to avoid breaking LP64
 * systems (cf. Postfix source code).
 */

CAMLprim value
ocaml_send_fd(value fd_val, value sendfd_val)
{
    CAMLparam2(fd_val, sendfd_val);
    struct msghdr msg;
    struct iovec iov[1];
    int fd = Int_val(fd_val);
    int sendfd = Int_val(sendfd_val);
    ssize_t ret;

#if defined(CMSG_SPACE) && !defined(NO_MSGHDR_MSG_CONTROL)
    union {
        struct cmsghdr cmsg; /* for alignment */
        char control[CMSG_SPACE(sizeof sendfd)];
    } control_un;
    struct cmsghdr *cmsgp;

    memset(&msg, 0, sizeof msg);
    msg.msg_control = control_un.control;
    msg.msg_controllen = CMSG_LEN(sizeof sendfd);

    cmsgp = CMSG_FIRSTHDR(&msg);
    cmsgp->cmsg_len = CMSG_LEN(sizeof sendfd);
    cmsgp->cmsg_level = SOL_SOCKET;
    cmsgp->cmsg_type = SCM_RIGHTS;
    *(int *)CMSG_DATA(cmsgp) = sendfd;
#else
    msg.msg_accrights = (caddr_t)&sendfd;
    msg.msg_accrightslen = sizeof sendfd;
#endif

    /*
     * We are only interested in sending the descriptor, but we need to
     * send at least one byte of data.
     */
    iov[0].iov_base = "";
    iov[0].iov_len = 1;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    caml_enter_blocking_section();
    ret = sendmsg(fd, &msg, 0);
    caml_leave_blocking_section();

    if (ret == -1)
        fd_error(strerror(errno));
    CAMLreturn (Val_unit);
}

CAMLprim value
ocaml_recv_fd(value fd_val)
{
    CAMLparam1(fd_val);
    struct msghdr msg;
    int fd = Int_val(fd_val);
    int recvfd;
    ssize_t ret;
    struct iovec iov[1];
    char buf[1];

#if defined(CMSG_SPACE) && !defined(NO_MSGHDR_MSG_CONTROL)
    union {
        struct cmsghdr cmsg; /* just for alignment */
        char control[CMSG_SPACE(sizeof recvfd)];
    } control_un;
    struct cmsghdr *cmsgp;

    memset(&msg, 0, sizeof msg);
    msg.msg_control = control_un.control;
    msg.msg_controllen = CMSG_LEN(sizeof recvfd);
#else
    msg.msg_accrights = (caddr_t)&recvfd;
    msg.msg_accrightslen = sizeof recvfd;
#endif

    iov[0].iov_base = buf;
    iov[0].iov_len = sizeof buf;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    caml_enter_blocking_section();
    ret = recvmsg(fd, &msg, 0);
    caml_leave_blocking_section();

    if (ret == -1)
        fd_error(strerror(errno));

#if defined(CMSG_SPACE) && !defined(NO_MSGHDR_MSG_CONTROL)
    cmsgp = CMSG_FIRSTHDR(&msg);
    if (cmsgp == NULL || cmsgp->cmsg_len != CMSG_LEN(sizeof recvfd))
        fd_error("No descriptor was passed");
    if (cmsgp->cmsg_level != SOL_SOCKET || cmsgp->cmsg_type != SCM_RIGHTS)
        fd_error("Invalid protocol");
    CAMLreturn (Val_int(*(int *)CMSG_DATA(cmsgp)));
#else
    if (msg.msg_accrightslen != sizeof recvfd)
        fd_error("Invalid protocol");
    CAMLreturn (Val_int(recvfd));
#endif
}

/*
 * fexecve(3) is Linux-specific, and thus we only provide it if we're
 * linking against glibc. If this function is available in other
 * platforms, please contact the authors and let us know.
 */
#ifdef __GLIBC__

static char **
array_of_value(value v)
{
    CAMLparam1(v);
    char **arr;
    mlsize_t size, i;

    size = Wosize_val(v);
    arr = caml_stat_alloc((size + 1) * sizeof(char *));
    for (i = 0; i < size; i++)
        arr[i] = String_val(Field(v, i));
    arr[size] = NULL;

    CAMLreturnT (char **, arr);
}

CAMLprim value
ocaml_fexecve(value fd_val, value argv_val, value envp_val)
{
    CAMLparam3(fd_val, argv_val, envp_val);
    char **argv;
    char **envp;

    argv = array_of_value(argv_val);
    envp = array_of_value(envp_val);

    fexecve(Int_val(fd_val), argv, envp);

    caml_stat_free(argv);
    caml_stat_free(envp);
    fd_error(strerror(errno));

    CAMLreturn (Val_unit); /* not reached */
}

#else

CAMLprim value
ocaml_fexecve(value fd_val, value argv_val, value envp_val)
{
    CAMLparam3(fd_val, argv_val, envp_val);
    fd_error("Your system does not support fexecve(3)");
    CAMLreturn (Val_unit); /* not reached */
}

#endif /* __GLIBC__ */
