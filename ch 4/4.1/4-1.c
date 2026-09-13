//-----------------------------------------------------------------------------
//
// Exercises 4.1 The-Linux-Programming-Interface-Michael-Kerrisk
// try:
// > gcc -o tee 4-1.c 
// > ./tee 1.txt
// > ./tee 1.txt -a
// > ./tee --help
//
// optional for compilation: -DDEBUG, -DBUF_SIZE
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

int main(int argc, char **argv)
{
    int fd;
    ssize_t numRead;
    char buffer[BUF_SIZE];

    if (argc < 2 || argc > 3)
    {
        fprintf(stderr, "Usage: %s file [-a]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "--help") == 0)
    {
        fprintf(stdout, "Usage: %s file [-a]\n", argv[0]);
        exit(EXIT_SUCCESS);
    }

    if (argc == 2)
    {
        fd = open(argv[1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
        if (fd == -1)
        {
            fprintf(stderr, "Cannot open/create file %s\n", argv[1]);
            exit(EXIT_FAILURE);
        }
#ifdef DEBUG
        printf("Create/open file %s\n", argv[1]);
#endif
    }
    else if (strcmp(argv[2], "-a") == 0)
    {
        fd = open(argv[1], O_WRONLY | O_APPEND);
        if (fd == -1)
        {
            fprintf(stderr, "No such file %s\n", argv[1]);
            exit(EXIT_FAILURE);
        }
#ifdef DEBUG
        printf("Append to file %s\n", argv[1]);
#endif
    }
    else
    {
        fprintf(stderr, "Wrong argument %s\n", argv[2]);
        exit(EXIT_FAILURE);
    }

    while ((numRead = read(STDIN_FILENO, buffer, BUF_SIZE)) > 0)
    {
        if (write(fd, buffer, numRead) != numRead)
        {
            fprintf(stderr, "Cannot write %s\n", argv[1]);
            exit(EXIT_FAILURE);
        }
        if (write(STDOUT_FILENO, buffer, numRead) != numRead)
        {
            fprintf(stderr, "Cannot write %s\n", argv[1]);
            exit(EXIT_FAILURE);
        }
    }

    if (numRead == -1)
    {
        fprintf(stderr, "Cannot read from stdin\n");
        exit(EXIT_FAILURE);
    }

    if (close(fd) == -1)
    {
        fprintf(stderr, "Cannot close %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
