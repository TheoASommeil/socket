#include "mesg.h"
#include <unistd.h>
#include <cstdio>
#include <errno.h>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

ssize_t mesg_send(int fd, struct mymesg *mptr)
{
	return write(fd, mptr, MESGHDRSIZE + mptr->mesg_len);
}

ssize_t mesg_recv(int fd, struct mymesg *mptr)
{
	int n = 0;
	if ( (n = read(fd, mptr, MESGHDRSIZE)) == 0)
		return 0;
	else if ( n != MESGHDRSIZE) {
		printf("client mesg header excepted %lu, got %d\n", MESGHDRSIZE, n);
		return 0;
	}

	int len = 0;
	if ( (len = mptr->mesg_len) > 0) {
		if ( (n = read(fd, mptr->mesg_data, len)) != len)
			printf("client mesg data excepted %d, got %d\n", len, n);
	}
	return len;
}


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
	struct mymesg mesg, mesg_r;

	memset(&mesg, 0, sizeof(mesg));
	memset(&mesg_r, 0, sizeof(mesg_r));

	fgets(mesg.mesg_data, MAXMESGDATA, stdin);

	int len = strlen(mesg.mesg_data);
	if (mesg.mesg_data[len-1] == '\n')
		--len;
	mesg.mesg_len = len;
	mesg.mesg_type = 1;

	mesg_send(wd, &mesg);
	int n = 0;
	while ( (n = mesg_recv(rd, &mesg_r)) > 0) {
		write(STDOUT_FILENO, mesg_r.mesg_data, mesg_r.mesg_len);
	}
}