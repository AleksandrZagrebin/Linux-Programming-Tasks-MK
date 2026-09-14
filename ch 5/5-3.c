#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#ifndef BYTE
#define BYTE 'a'
#endif

int main(int argc, char **argv)
{
    int fd;
    int yeslseek = 0;
    long bytes;
    char *buf;

    if (argc == 2 && strcmp(argv[1], "--help") == 0)
    {
        printf("Usage: %s filename num-bytes [x]\n", argv[0]);
        return EXIT_SUCCESS;
    }

    if (argc < 3 || argc > 4)
    {
        fprintf(stderr, "Wrong usage\nUsage: %s filename num-bytes [x]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *end;
    errno = 0;
    bytes = strtol(argv[2], &end, 10);
    if (errno != 0 || *end != '\0' || bytes <= 0)
    {
        fprintf(stderr, "Invalid num-bytes: %s\n", argv[2]);
        exit(EXIT_FAILURE);
    }

    if (argc == 3)
    {
        fd = open(argv[1], O_WRONLY | O_APPEND);
        yeslseek = 0;
#ifdef DEBUG
        printf("Open %s with O_APPEND\n", argv[1]);
#endif
    }
    else
    {
        if (strcmp(argv[3], "x") != 0)
        {
            fprintf(stderr, "Wrong usage\nUsage: %s filename num-bytes [x]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        fd = open(argv[1], O_WRONLY);
        yeslseek = 1;
#ifdef DEBUG
        printf("Open %s for lseek\n", argv[1]);
#endif
    }

    if (fd == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }

    buf = malloc(bytes);
    if (buf == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    memset(buf, BYTE, bytes);

#ifdef DEBUG
    printf("Allocated %ld bytes, filled with '%c'\n", bytes, BYTE);
#endif

    if (yeslseek)
    {
        if (lseek(fd, 0, SEEK_END) == -1)
        {
            perror("lseek");
            exit(EXIT_FAILURE);
        }
    }

    if (write(fd, buf, bytes) != bytes)
    {
        perror("write");
        exit(EXIT_FAILURE);
    }

#ifdef DEBUG
    printf("Wrote %ld bytes to %s\n", bytes, argv[1]);
#endif

    free(buf);
    close(fd);
    return EXIT_SUCCESS;
}
