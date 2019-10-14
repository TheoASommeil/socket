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

const static int MAXLINE = 1024;

int main()
{
	int pipe1[2];
	int pipe2[2];

	pid_t child_pid;

	pipe(pipe1);
	pipe(pipe2);

	if ((child_pid = fork()) == 0) {
		close(pipe1[0]);
		close(pipe2[1]);

		server(pipe2[0], pipe1[1]);
		_exit(0);
	}

	close(pipe1[1]);
	close(pipe2[0]);

	client(pipe1[0], pipe2[1]);
	waitpid(child_pid, NULL, 0);
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