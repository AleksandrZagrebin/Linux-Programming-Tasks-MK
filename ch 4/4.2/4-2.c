//-----------------------------------------------------------------------------
//
// Exercises 4.2 The-Linux-Programming-Interface-Michael-Kerrisk
// try:
// > gcc linuxapi-4-2.c -D_GNU_SOURCE -o cp 
// > ./cp 1.txt 2.txt
// > ./cp --help
//
// optional for compilation: -DDEBUG, -DBUF_SIZE
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <inttypes.h>
#include <sys/stat.h>

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

int main(int argc, char **argv)
{
    int srcFd, dstFd;
    ssize_t numRead, numWritten;
    char buffer[BUF_SIZE];
    struct stat st;
    off_t size, pos, data, hole, p;
    ssize_t off;
    
    if (strcmp(argv[1], "--help") == 0)
    {
        fprintf(stdout, "Usage: %s src dst\n", argv[0]);
        exit(EXIT_SUCCESS);
    }
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s src dst\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    srcFd = open(argv[1], O_RDONLY);
    if (srcFd == -1)
    {
        fprintf(stderr, "Cannot open src %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    if (fstat(srcFd, &st) == -1)
    {
        fprintf(stderr, "Cannot stat %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    dstFd = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, st.st_mode & 07777);
    if (dstFd == -1)
    {
        fprintf(stderr, "Cannot open/create file %s\n", argv[2]);
        exit(EXIT_FAILURE);
    }

#ifdef DEBUG
    printf("Copy %s -> %s (size %" PRIdMAX ")\n", argv[1], argv[2], (intmax_t)st.st_size);
#endif

    size = st.st_size;
    pos  = 0;

    while (pos < size)
    {
        errno = 0;
        data = lseek(srcFd, pos, SEEK_DATA);
        if (data < 0)
        {
            if (errno == ENXIO)
            {
#ifdef DEBUG
                printf("Hole [%" PRIdMAX ", %" PRIdMAX ")\n", (intmax_t)pos, (intmax_t)size);
#endif
                break;
            }
            fprintf(stderr, "Cannot SEEK_DATA in %s\n", argv[1]);
            exit(EXIT_FAILURE);
        }

#ifdef DEBUG
        if (data > pos)
            printf("Hole [%" PRIdMAX ", %" PRIdMAX ")\n", (intmax_t)pos, (intmax_t)data);
#endif

        errno = 0;
        hole = lseek(srcFd, data, SEEK_HOLE);
        if (hole < 0)
        {
            fprintf(stderr, "Cannot SEEK_HOLE in %s\n", argv[1]);
            exit(EXIT_FAILURE);
        }

#ifdef DEBUG
        printf("Data [%" PRIdMAX ", %" PRIdMAX ")\n", (intmax_t)data, (intmax_t)hole);
#endif

        if (lseek(srcFd, data, SEEK_SET) < 0)
        {
            fprintf(stderr, "Cannot lseek src %s\n", argv[1]);
            exit(EXIT_FAILURE);
        }
        if (lseek(dstFd, data, SEEK_SET) < 0)
        {
            fprintf(stderr, "Cannot lseek dst %s\n", argv[2]);
            exit(EXIT_FAILURE);
        }

        p = data;
        while (p < hole)
        {
            size_t want = (hole - p) < BUF_SIZE ? (size_t)(hole - p) : BUF_SIZE;

            numRead = read(srcFd, buffer, want);
            if (numRead == -1)
            {
                fprintf(stderr, "Cannot read %s\n", argv[1]);
                exit(EXIT_FAILURE);
            }
            if (numRead == 0)
                break;

            off = 0;
            while (off < numRead)
            {
                numWritten = write(dstFd, buffer + off, numRead - off);
                if (numWritten <= 0)
                {
                    fprintf(stderr, "Cannot write %s\n", argv[2]);
                    exit(EXIT_FAILURE);
                }
                off += numWritten;
            }

            p += numRead;
        }

        pos = hole;
    }

    if (ftruncate(dstFd, size) == -1)
    {
        fprintf(stderr, "Cannot ftruncate %s\n", argv[2]);
        exit(EXIT_FAILURE);
    }

    if (close(srcFd) == -1)
    {
        fprintf(stderr, "Cannot close %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    if (close(dstFd) == -1)
    {
        fprintf(stderr, "Cannot close %s\n", argv[2]);
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
