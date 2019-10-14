#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <cstring>
#include <sys/wait.h>
void client(int, int);
void server(int, int);

#define FIFO1 "/tmp/fifo.1"
#define FIFO2 "/tmp/fifo.2"
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
const static int MAXLINE = 1024;

int main()
{
	pid_t child_pid;
	int readfd, writefd;

	if ( (mkfifo(FIFO1, FILE_MODE) < 0) && errno != EEXIST)
		printf("camt create %s\n", FIFO1);
	if ( (mkfifo(FIFO2, FILE_MODE) < 0) && errno != EEXIST) {
		unlink(FIFO1);
		printf("camt create %s\n", FIFO2);
	}

	if ((child_pid = fork()) == 0) {
		readfd = open(FIFO1, O_RDONLY, 0);
		writefd = open(FIFO2, O_WRONLY, 0);

		server(readfd, writefd);
		_exit(0);
	}
	writefd = open(FIFO1, O_WRONLY, 0);
	readfd = open(FIFO2, O_RDONLY, 0);
	client(readfd, writefd);
	waitpid(child_pid, NULL, 0);
	unlink(FIFO1);
	unlink(FIFO2);
	_exit(0);
}

void client(int rd, int wd)
{
	char buff[MAXLINE] = { 0 };
	fgets(buff, MAXLINE, stdin);

	int len = strlen(buff);
	if (buff[len - 1] == '\n')
		--len;
	int n = write(wd, buff, len);
	printf("client write %d bytes\n", n);

	memset(buff, 0, sizeof(buff));
	while ( (n = read(rd, buff, MAXLINE)) > 0)
		write(STDOUT_FILENO, buff, n);
}

void server(int rd, int wd)
{
	char buff[MAXLINE] = { 0 };
	int n;

	if ( (n = read(rd, buff, MAXLINE)) == 0) 
		printf("EOF while reading pathname\n");

	/*
	//这段代码读不出内容
	int fd;
	if ( (fd = open(buff, O_RDONLY)) < 0) {
		snprintf(buff + n, sizeof(buff) - n, ": cant open, %s\n", strerror(errno));
		n = strlen(buff);
		write(wd, buff, n);
	} else {
		while (n = read(fd, buff, MAXLINE) > 0)
			write(wd, buff, n);
		close(fd);
	}
	*/

	FILE* fd = fopen(buff, "r");
	if ( NULL == fd) {
		snprintf(buff + n, sizeof(buff) - n, ": cant open, %s\n", strerror(errno));
		n = strlen(buff);
		write(wd, buff, n);
	} else {
		memset(buff, 0, sizeof(buff));
		while (fgets(buff, 1024, fd) != NULL) {
			write(wd, buff, strlen(buff));
			memset(buff, 0, sizeof(buff));
		}
		fclose(fd);
	}
}