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
	pid_t pid = getpid();
	char fifoname[MAXLINE] = { 0 };
	snprintf(fifoname, sizeof(fifoname), "/tmp/fifo.%ld", (long) pid);

	if ( (mkfifo(fifoname, FILE_MODE)) < 0 && errno != EEXIST) {
		printf("cannot create %s\n", fifoname);
	}

	char buff[MAXLINE] = { 0 };
	snprintf(buff, sizeof(buff), "%ld ", (long) pid);
	int len = strlen(buff);
	char* ptr = buff + len;

	fgets(ptr, MAXLINE - len, stdin);
	len = strlen(buff);
	int wd = open(SERV_FIFO, O_WRONLY, 0);
	write(wd, buff, len);

	int rd = open(fifoname, O_RDONLY, 0);
	int n;
	memset(buff, 0, sizeof(buff));
	while ( (n = read(rd, buff, MAXLINE)) > 0) {
		write(STDOUT_FILENO, buff, n);
	}
	close(rd);
	unlink(fifoname);
	_exit(0);
}