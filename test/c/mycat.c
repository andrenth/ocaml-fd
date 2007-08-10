#include <sys/types.h>
#include <sys/stat.h>

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int my_open(const char *, int);

int
main(int argc, char **argv)
{
    int     fd, n;
    char    buff[BUFSIZ];

    if (argc != 2) {
        perror("usage: mycat <pathname>");
        exit(1);
    }

    if ((fd = my_open(argv[1], O_RDONLY)) < 0) {
        fprintf(stderr, "cannot open %s\n", argv[1]);
        exit(1);
    }

    while ((n = read(fd, buff, BUFSIZ)) > 0) {
        write(STDOUT_FILENO, buff, n);
    }

    exit(0);
}

