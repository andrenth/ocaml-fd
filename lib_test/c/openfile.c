#include <sys/types.h>
#include <sys/stat.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int send_fd(int, int);

int
main(int argc, char **argv)
{
    int fd;

    if (argc != 4) {
        perror("openfile <sockfd#> <filename> <mode>");
        exit(1);
    }

    if ((fd = open(argv[2], atoi(argv[3]))) < 0)
        exit((errno > 0) ? errno : 255);

    return send_fd(atoi(argv[1]), fd);
}
