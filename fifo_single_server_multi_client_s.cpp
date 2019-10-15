#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string>
#include <cstring>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define SERV_FIFO "/tmp/serv_fifo"
const static int MAXLINE = 1024;

int main()
{
	if ((mkfifo(SERV_FIFO, FILE_MODE)) < 0 && errno != EEXIST)
		printf("cant create %s\n", SERV_FIFO);

	int rd = open(SERV_FIFO, O_RDONLY, 0);
	int yummyd = open(SERV_FIFO, O_WRONLY, 0);

	int n = 0;
	char buffer[MAXLINE + 1] = { 0 };
	char* ptr;
	while ( (n = read(rd, buffer, MAXLINE)) > 0) {
		if (buffer[n - 1] == '\n')
			--n;
		buffer[n] = '\0';

		if ( (ptr = strchr(buffer, ' ')) == NULL) {
			printf("bogus  request : %s\n", buffer);
			continue;
		}
		
		*ptr++ = 0;
		pid_t pid = atol(buffer);
		char fifoname[MAXLINE] = { 0 };
		snprintf(fifoname, sizeof(fifoname), "/tmp/fifo.%ld", (long)pid);

		int fd, wd;
		if ( (wd = open(fifoname, O_WRONLY, 0)) < 0) {
			printf("cannot open fifo:%s\n", fifoname);
			continue;
		}

		if ( (fd = open(ptr, O_RDONLY)) < 0) {
			snprintf(buffer + n, sizeof(buffer) - n, ": cannot open, %s\n", strerror(errno));
			n = strlen(ptr);
			write(wd, ptr, n);
			close(wd);
		} else {
			memset(buffer, 0, sizeof(buffer));
			while ( (n = read(fd, buffer, MAXLINE)) > 0) {
				write(wd, buffer, n);
			}
			close(fd);
			close(wd);
		}
	}
	_exit(0);
}