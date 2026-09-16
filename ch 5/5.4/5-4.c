//-----------------------------------------------------------------------------
//
// Exercises 5.4 The-Linux-Programming-Interface-Michael-Kerrisk
// try:
// > gcc 5-4.c -o dup
//
// optional for compilation: -DDEBUG
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int my_dup(int oldfd)
{
    return fcntl(oldfd, F_DUPFD, 0);
}

int my_dup2(int oldfd, int newfd)
{
    if (oldfd == newfd)
    {
        if (fcntl(oldfd, F_GETFL) == -1)
            return -1;
        return newfd;
    }

    if (close(newfd) == -1 && errno != EBADF)
        return -1;

    int fd = fcntl(oldfd, F_DUPFD, newfd);
    if (fd == -1)
        return -1;

    if (fd != newfd)
    {
        close(fd);
        errno = EBADF;
        return -1;
    }

    return fd;
}

int main(void)
{
    int fd = open("/etc/passwd", O_RDONLY);
    if (fd == -1)
        return EXIT_FAILURE;

#ifdef DEBUG
    printf("open -> fd=%d\n", fd);
#endif

    int copy = my_dup(fd);
    if (copy == -1)
        return EXIT_FAILURE;

#ifdef DEBUG
    printf("my_dup -> %d\n", copy);
#endif

    if (my_dup2(fd, 10) == -1)
        return EXIT_FAILURE;

#ifdef DEBUG
    printf("my_dup2 -> 10\n");
#endif

    close(fd);
    close(copy);
    close(10);

    return EXIT_SUCCESS;
}
