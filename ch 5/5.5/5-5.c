#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char** argv)
{
	off_t offset, offsetdup;
	int fd, fddup;

	fd = open(argv[1], O_WRONLY);
	if (fd == -1)
	{
		perror("open");
		return 1;
	}

	fddup = dup(fd);
	if (fddup == -1)
	{
		perror("dup");
		return 1;
	}

	offset = lseek(fd, 10, SEEK_CUR);
	printf("offset of original descriptor is %ld\n", (long)offset);

	offsetdup = lseek(fddup, 10, SEEK_CUR);
	printf("offset of duplicate descriptor is %ld\n", (long)offsetdup);

	offset = lseek(fd, 0, SEEK_CUR);
	offsetdup = lseek(fddup, 0, SEEK_CUR);
	printf("after: offset of original is %ld, offset of duplicate is %ld\n", (long)offset, (long)offsetdup);
	
	close(fddup);
	close(fd);
	return 0;
}
