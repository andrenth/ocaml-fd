#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int recv_fd(int);

int
my_open(const char *pathname, int mode)
{
    int     fd, sockfd[2], status;
    pid_t   childpid;
    char    argsockfd[10], argmode[10];

    socketpair(AF_LOCAL, SOCK_STREAM, 0, sockfd);

    if ( (childpid = fork()) == 0) { /* child process */
        close(sockfd[0]);
        snprintf(argsockfd, sizeof(argsockfd), "%d", sockfd[1]);
        snprintf(argmode, sizeof(argmode), "%d", mode);
        execl("./openfile", "openfile", argsockfd, pathname, argmode,
              (char *) NULL);
        perror("execl error");
        exit(1);
    }

    /* parent process - wait for the child to terminate */
    close(sockfd[1]);           /* close the end we don't use */

    waitpid(childpid, &status, 0);
    if (WIFEXITED(status) == 0) {
        perror("child did not terminate");
        exit(1);
    }
    if ( (status = WEXITSTATUS(status)) == 0)
        fd = recv_fd(sockfd[0]);
    else {
        errno = status;         /* set errno value from child's status */
        fd = -1;
    }

    close(sockfd[0]);
    return (fd);
}
