#include <sys/msg.h>
#include <cstdio>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>

#define SVMSG_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

void show_help();

void msgcreate(int, char**);
void msgsnd(int, char**);
void msgrcv(int, char**);
void msgrmid(int, char**);

int main(int argc, char** argv)
{
	if (1 == argc) {
		printf("need more arguments...\n");
		printf("use --help for help\n");
		_exit(0);
	}

	if (0 == strncmp("--help", argv[1])) {
		show_help();
		_exit(0);
	}

	--argc; ++argv;

	if (0 == strncmp("-c", argv[1])) {
		msgcreate(--argc, ++argv);
		_exit(0);
	} else if (0 == strncmp("-s", argv[1])) {
		msgsnd(--argc, ++argv);
		_exit(0);
	} else if (0 == strncmp("-r", argv[1])) {
		msgrcv(--argc, ++argv);
		_exit(0);
	} else if (0 == strncmp("-r", argv[1])) {
		msgrmid(--argc, ++argv);
		_exit(0);
	} else {
		printf("cant recogenize %s, usage --help for more info.\n", argv[1]);
		_exit(0);
	}
}

void show_help()
{
	printf("haha, no reference!!\n");
}

void msgcreate(int argc, char** argv)
{

}

void msgsnd(int argc, char** argv)
{

}

void msgrcv(int argc, char** argv)
{

}

void msgrmid(int argc, char** argv)
{

}
