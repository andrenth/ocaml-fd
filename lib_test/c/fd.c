#include <sys/types.h>
#include <sys/socket.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>

/*
 * These functions are adapted from Stevens, Fenner and Rudoff, UNIX Network
 * Programming, Volume 1, Third Edition. We use CMSG_LEN instead of CMSG_SPACE
 * for the msg_controllen field of struct msghdr to avoid breaking LP64
 * systems (cf. Postfix source code).
 */

int
send_fd(int fd, int sendfd)
{
    struct msghdr msg;
    struct iovec iov[1];
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

    ret = sendmsg(fd, &msg, 0);

    if (ret == -1)
        return -1;
    return 0;
}

int
recv_fd(int fd)
{
    struct msghdr msg;
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

    ret = recvmsg(fd, &msg, 0);

    if (ret == -1)
        return -1;

#if defined(CMSG_SPACE) && !defined(NO_MSGHDR_MSG_CONTROL)
    cmsgp = CMSG_FIRSTHDR(&msg);
    if (cmsgp == NULL || cmsgp->cmsg_len != CMSG_LEN(sizeof recvfd))
        return -1;
    if (cmsgp->cmsg_level != SOL_SOCKET || cmsgp->cmsg_type != SCM_RIGHTS)
        return -1;
    return *(int *)CMSG_DATA(cmsgp);
#else
    if (msg.msg_accrightslen != sizeof recvfd)
        return -1;
    return recvfd;
#endif
}
