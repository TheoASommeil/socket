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
		msgcreate(argc, argv);
		_exit(0);
	} else if (0 == strncmp("-s", argv[1])) {
		msgsend(argc, argv);
		_exit(0);
	} else if (0 == strncmp("-r", argv[1])) {
		msgrecv(argc, argv);
		_exit(0);
	} else if (0 == strncmp("-r", argv[1])) {
		msgrmid(argc, argv);
		_exit(0);
	} else {
		printf("can't recogenize %s, usage --help for more info.\n", argv[1]);
		_exit(0);
	}
}

void show_help()
{
	printf("haha, no reference!!\n");
}

void msgcreate(int argc, char** argv)
{
	int oflag = SVMSG_MODE | IPC_CREAT;
	int i = 0;

	while ( (i = getopt(argc, argv, "e")) != -1) {
		switch (i) {
			case 'e' :
				oflag |= IPC_EXCL;
				break;
		}
	}

	if (optind != argc - 1) {
		printf("usage: sysV_simp -c [ -e ] <pathname>\n");
		_exit(0);
	}

	int mqid = msgget(ftok(argv[optind]), oflag);
	_exit(0);
}

void msgsend(int argc, char** argv)
{
	if (argc != 3) {
		printf("usage: sysV_simp -s <pathname> <#bytes> <type>\n");
		_exit(0);
	}

	int len = atoi(argv[1]);
	long type = atol(argv[2]);
	int mqid = msgget(ftok(argv[0], 0), MSG_W);

	struct msgbuf* ptr = calloc(sizeof(long) + len, sizeof(char));
	ptr->mtype = type;
	msgsnd(mqid, ptr, len, 0);
	_exit(0);
}

#define MAXMSG (8192 + sizeof(long))

void msgrecv(int argc, char** argv)
{
	int i = 0;
	int flag = 0;
	long type = 0;

	while ( (i = getopt(argc, argv, "nt:")) != -1) {
		switch (i) {
			case 'n':
			flag |= IPC_NOWAIT;
			break;
			case 't':
			type = atol(optarg);
			break;
		}
	}

	if (optind != argc -1) {
		printf("usage: sysV_simp -r  [ -n ] [ -t type ] <pathname>\n");
		_exit(0);
	}

	int mqid = msgget(ftok(argv[optind], 0), MSG_R);

	struct msgbuf* buff;
	buff = calloc(MAXMSG);

	ssize_t n = nsgrcv(mqid, buff, MAXMSG, type, flag);
	printf("read %d bytes, type = %ld\n", n, buff->mtype);
	_exit(0);
}

void msgrmid(int argc, char** argv)
{
	if (argc != 1) {
		printf("usage: sysV_simp -r <pathname>");
		_exit(0);
	}

	int mqid = msgget(ftok(argv[0], 0), 0);
	msgctl(mqid, IPC_RMID, NULL);
	_exit(0);
}
