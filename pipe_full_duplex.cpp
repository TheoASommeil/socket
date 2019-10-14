#include <cstdio>
#include <unistd.h>
#include <sys/wait.h>
//测试下来不支持全双工，不能在fd[0]中写，不能在fd[1]中读
int main()
{
	int fd[2], fd_[2];
	char c;
	int n;
	pipe(fd);

	int child_pid;
	if ( (child_pid = fork()) == 0) {
		sleep(2);
		if ( (n = read(fd[0], &c, 1)) != 1) {
			printf("child read returned %d\n", n);
			_exit(0);
		}
		printf("child read %c\n", c);
		if ((n = write(fd[0], "c", 1)) != 1) {
			printf("child write returned %d\n", n);
		}
		_exit(0);
	}

	write(fd[1], "p", 1);
	if ( (n = read(fd[1], &c, 1)) != 1)
		printf("parent read returned %d\n", n);
	printf("parent read %c\n", c);
	waitpid(child_pid, NULL, 0);
	_exit(0);
}