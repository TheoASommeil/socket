#include <sys/msg.h>
#include <cstdio>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdlib>

#define SVMSG_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int main()
{
	int msqid = msgget(IPC_PRIVATE, SVMSG_MODE | IPC_CREAT);

	struct msqid_ds info;
	struct msgbuf buf;
	buf.mtype = 1;
	buf.mtext[0] = 1;
	msgsnd(msqid, &buf, 1, 0);

	msgctl(msqid, IPC_STAT, &info);

	printf("read-write : %03o, cbytes = %lu, qnum = %lu, qbytes = %lu\n",
		info.msg_perm.mode & 0777, (unsigned long) info.msg_cbytes,
		(unsigned long) info.msg_qnum, (unsigned long) info.msg_qbytes);

	system("ipcs -q");

	msgctl(msqid, IPC_RMID, NULL);
	_exit(0);
}