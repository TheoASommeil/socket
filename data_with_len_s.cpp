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
		printf("server mesg header excepted %lu, got %d\n", MESGHDRSIZE, n);
		return 0;
	}

	int len = 0;
	if ( (len = mptr->mesg_len) > 0) {
		if ( (n = read(fd, mptr->mesg_data, len)) != len)
			printf("server mesg data excepted %d, got %d\n", len, n);
	}
	return len;
}

void server(int, int);

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

	readfd = open(FIFO1, O_RDONLY, 0);
	writefd = open(FIFO2, O_WRONLY, 0);
	server(readfd, writefd);
	_exit(0);
}

void server(int rd, int wd)
{
	struct mymesg mesg;
	memset(&mesg, 0, sizeof(mesg));
	mesg.mesg_type = 1;

	int n = 0;
	if ( (n = mesg_recv(rd, &mesg)) == 0) {
		printf("pathname missing...\n");
		return;
	}

	mesg.mesg_data[n] = '\0';

	FILE* fd;
	if ( (fd = fopen(mesg.mesg_data, "r")) == 0) {
		snprintf(mesg.mesg_data + n, sizeof(mesg.mesg_data) - n, ":cannot open, %s", strerror(errno));
		mesg.mesg_len = strlen(mesg.mesg_data);
		mesg_send(wd, &mesg);
	} else {
		memset(mesg.mesg_data, 0, sizeof(mesg.mesg_data));
		while (fgets(mesg.mesg_data, MAXMESGDATA, fd) != NULL) {
			mesg.mesg_len = strlen(mesg.mesg_data);
			mesg_send(wd, &mesg);
			memset(mesg.mesg_data, 0, sizeof(mesg.mesg_data));
		}
		fclose(fd);
	}
	mesg.mesg_len = 0;
	mesg_send(wd, &mesg);
}