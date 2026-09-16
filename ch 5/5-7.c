//-----------------------------------------------------------------------------
//
// Exercises 4.2 The-Linux-Programming-Interface-Michael-Kerrisk
// try:
// > gcc 5-7.c -o readwritev
//
// optional for compilation: -DDEBUG
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/uio.h>

ssize_t my_writev(int fd, const struct iovec *iov, int iovcnt)
{
    if (iovcnt <= 0)
        return 0;

    size_t total = 0;
    for (int i = 0; i < iovcnt; i++)
        total += iov[i].iov_len;

    if (total == 0)
        return 0;

    char *buf = malloc(total);
    if (buf == NULL)
    {
        errno = ENOMEM;
        return -1;
    }

    size_t off = 0;
    for (int i = 0; i < iovcnt; i++)
    {
        if (iov[i].iov_len == 0)
            continue;
        memcpy(buf + off, iov[i].iov_base, iov[i].iov_len);
        off += iov[i].iov_len;
    }

#ifdef DEBUG
    printf("my_writev: total=%zu\n", total);
#endif

    ssize_t n = write(fd, buf, total);

#ifdef DEBUG
    printf("my_writev: write -> %zd\n", n);
#endif

    free(buf);
    return n;
}

ssize_t my_readv(int fd, const struct iovec *iov, int iovcnt)
{
    if (iovcnt <= 0)
        return 0;

    size_t total = 0;
    for (int i = 0; i < iovcnt; i++)
        total += iov[i].iov_len;

    if (total == 0)
        return 0;

    char *buf = malloc(total);
    if (buf == NULL)
    {
        errno = ENOMEM;
        return -1;
    }

    ssize_t n = read(fd, buf, total);

#ifdef DEBUG
    printf("my_readv: read -> %zd\n", n);
#endif

    if (n > 0)
    {
        size_t left = (size_t)n;
        size_t off = 0;
        for (int i = 0; i < iovcnt && left > 0; i++)
        {
            size_t chunk = iov[i].iov_len < left ? iov[i].iov_len : left;
            if (chunk == 0)
                continue;
            memcpy(iov[i].iov_base, buf + off, chunk);
            off += chunk;
            left -= chunk;
        }
    }

    free(buf);
    return n;
}

int main(void)
{
    struct iovec iov[3];
    char a[] = "hello ";
    char b[] = "world ";
    char c[] = "from readv/writev\n";

    int fd = open("test.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd == -1)
        return EXIT_FAILURE;

    iov[0].iov_base = a;
    iov[0].iov_len  = strlen(a);
    iov[1].iov_base = b;
    iov[1].iov_len  = strlen(b);
    iov[2].iov_base = c;
    iov[2].iov_len  = strlen(c);

    if (my_writev(fd, iov, 3) == -1)
    {
        close(fd);
        return EXIT_FAILURE;
    }
    close(fd);

    fd = open("test.txt", O_RDONLY);
    if (fd == -1)
        return EXIT_FAILURE;

    char r1[6]  = {0};
    char r2[6]  = {0};
    char r3[32] = {0};

    iov[0].iov_base = r1;
    iov[0].iov_len  = sizeof(r1) - 1;
    iov[1].iov_base = r2;
    iov[1].iov_len  = sizeof(r2) - 1;
    iov[2].iov_base = r3;
    iov[2].iov_len  = sizeof(r3) - 1;

    ssize_t n = my_readv(fd, iov, 3);
    if (n == -1)
    {
        close(fd);
        return EXIT_FAILURE;
    }

#ifdef DEBUG
    printf("my_readv: total read %zd\n", n);
#endif

    printf("%s%s%s", r1, r2, r3);

    close(fd);
    return EXIT_SUCCESS;
}
