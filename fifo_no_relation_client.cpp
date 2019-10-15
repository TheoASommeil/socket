#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <cstring>
void client(int, int);

#define FIFO1 "/tmp/fifo.1"
#define FIFO2 "/tmp/fifo.2"
#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
const static int MAXLINE = 1024;

int main()
{
	int readfd, writefd;

	if ( (mkfifo(FIFO1, FILE_MODE) < 0) && errno != EEXIST)
		printf("camt create %s\n", FIFO1);
	if ( (mkfifo(FIFO2, FILE_MODE) < 0) && errno != EEXIST) {
		unlink(FIFO1);
		printf("camt create %s\n", FIFO2);
	}

	writefd = open(FIFO1, O_WRONLY, 0);
	readfd = open(FIFO2, O_RDONLY, 0);
	client(readfd, writefd);
	unlink(FIFO1);	//删除所用fifo
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