#ifndef MESG_H_
#define MESG_H_
#include <sys/types.h>

#define PIPE_BUF 512 * 8

#define MAXMESGDATA (PIPE_BUF - 2*sizeof(long))
#define MESGHDRSIZE (sizeof(struct mymesg) - MAXMESGDATA)

struct mymesg
{
	long mesg_len;
	long mesg_type;
	char mesg_data[MAXMESGDATA];
};

ssize_t mesg_send(int, struct mymesg *);
ssize_t mesg_recv(int, struct mymesg *);
#endif