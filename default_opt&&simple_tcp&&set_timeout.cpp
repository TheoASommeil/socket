#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <cstdlib>

union Val {
	int i_val;
	long l_val;
	struct linger linger_val;
	struct timeval timeval_val;
};

static char *sock_str_flag(union Val*, int);
static char *sock_str_int(union Val*, int);
static char *sock_str_linger(union Val*, int);
static char *sock_str_timeval(union Val*, int);

struct Sock_opts {
	const char* opt_str;
	int opt_level;
	int opt_name;
	char* (*opt_val_str)(union Val*, int);
};

Sock_opts sock_opts[] = {
	{ "SO_BROADCAST",   SOL_SOCKET, SO_BROADCAST,   sock_str_flag },	//允许发送广播数据包
	{ "SO_DEBUG",       SOL_SOCKET, SO_DEBUG,       sock_str_flag },	//开启调试追踪
	{ "SO_DONTROUTE",   SOL_SOCKET, SO_DONTROUTE,   sock_str_flag },	//绕过外出路由表查询
	{ "SO_ERROR",       SOL_SOCKET, SO_ERROR,       sock_str_int  },	//获取待处理错误并清除
	{ "SO_KEEPALIVE",   SOL_SOCKET, SO_KEEPALIVE,   sock_str_flag },	//周期性测试连接是否仍存活
	{ "SO_LINGER",      SOL_SOCKET, SO_LINGER,      sock_str_linger },	//若有数据待发送则延迟关闭
	{ "SO_OOBINLINE",   SOL_SOCKET, SO_OOBINLINE,   sock_str_flag },	//让接收到的带外数据继续在线留存
	{ "SO_RCVBUF",      SOL_SOCKET, SO_RCVBUF,      sock_str_int },		//接收缓存区大小
	{ "SO_SNDBUF",      SOL_SOCKET, SO_SNDBUF,      sock_str_int },		//发送缓存区大小
	{ "SO_RCVLOWAT",    SOL_SOCKET, SO_RCVLOWAT,    sock_str_int },		//接收缓存区低水位标记
	{ "SO_SNDLOWAT",    SOL_SOCKET, SO_SNDLOWAT,    sock_str_int },		//发送缓存区低水位标记
	{ "SO_RCVTIMEO",    SOL_SOCKET, SO_RCVTIMEO,    sock_str_timeval },	//接收超时
	{ "SO_SNDTIMEO",    SOL_SOCKET, SO_SNDTIMEO,    sock_str_timeval },	//发送超时
	{ "SO_REUSEADDR",   SOL_SOCKET, SO_REUSEADDR,   sock_str_flag },	//允许重用本地地址
#ifdef SO_REUSEPORT
	{ "SO_REUSEPORT",   SOL_SOCKET, SO_REUSEPORT,   sock_str_flag },	//允许重用本地端口
#else
	{ "SO_REUSEPORT", 0, 0, NULL },
#endif
	{ "SO_TYPE",        SOL_SOCKET, SO_TYPE,        sock_str_int },		//取得套接字类型
	{ "IP_TOS",         IPPROTO_IP, IP_TOS,         sock_str_int },		//服务类型和优先权
	{ "IP_TTL",         IPPROTO_IP, IP_TTL,         sock_str_int },		//存活时间
	{ NULL, 0, 0, NULL }
};

static char strres[128];

static char *sock_str_flag(union Val* ptr, int len)
{
	if (len != sizeof(int))
		snprintf(strres, sizeof(strres), "size(%d) not sizeof(int)", len);
	else
		snprintf(strres, sizeof(strres), "default = %s", ptr->i_val == 0 ? "off" : "on");

	return strres;
}

static char *sock_str_int(union Val* ptr, int len)
{
	if (len != sizeof(int))
		snprintf(strres, sizeof(strres), "size(%d) not sizeof(int)", len);
	else
		snprintf(strres, sizeof(strres), "default = %d", ptr->i_val);

	return strres;
}

static char *sock_str_linger(union Val* ptr, int len)
{
	if (len != sizeof(linger))
		snprintf(strres, sizeof(strres), "size(%d) not sizeof(linger)", len);
	else
		snprintf(strres, sizeof(strres), "l_onoff = %d l_linger = %d", ptr->linger_val.l_onoff, ptr->linger_val.l_linger);

	return strres;
}

static char *sock_str_timeval(union Val* ptr, int len)
{
	if (len != sizeof(timeval))
		snprintf(strres, sizeof(strres), "size(%d) not sizeof(timeval)", len);
	else
		snprintf(strres, sizeof(strres), "tv_sec = %ld tv_usec = %ld", ptr->timeval_val.tv_sec, ptr->timeval_val.tv_usec);

	return strres;
}

void GetAllDefalutOptValue()
{
	Sock_opts* ptr = sock_opts;
	Val val;
	socklen_t len;
	int sockfd;

	while (ptr->opt_str != NULL) {
		printf("%s : ", ptr->opt_str);
		if (ptr->opt_val_str == NULL) 
			printf("undifined\n");
		else {
			switch (ptr->opt_level) {
				case SOL_SOCKET : case IPPROTO_IP : case IPPROTO_TCP :
					sockfd = socket(AF_INET, SOCK_STREAM, 0);
					break;
			#ifdef IPV6
				case IPPROTO_IPV6 :
					sockfd = socket(AF_INET6, SOCK_STREAM, 0);
					break;
			#endif
			#ifdef IPPROTO_SCTP
				case IPPROTO_SCTP :
					sockfd = socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
					break;
			#endif
				default :
					printf("cant create sockfd for levle %d\n", ptr->opt_level);
			}

			len = sizeof(val);
			if (-1 == getsockopt(sockfd, ptr->opt_level, ptr->opt_name, &val, &len))
				printf("getsockopt error\n");
			else
				printf("%s\n", (*ptr->opt_val_str)(&val, len));
			close(sockfd);
			++ptr;
		}
	}
}

void CreateTCPClientAndSendSimpleMsg()
{
	struct sockaddr_in serv_addr;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(6886);
	inet_pton(AF_INET, "128.64.6.114", &serv_addr.sin_addr);	//将字符串形式表示的地址转换为数值形式

	connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	char buffer[128] = { 0 };
	snprintf(buffer, sizeof(buffer), "hello world");
	send(sockfd, buffer, strlen(buffer), 0);
}

static int MAXSIZE = 1024;

int IsReadable(int sockfd, int sec)
{
	fd_set rset;
	struct timeval tv;
	FD_ZERO(&rset);
	FD_SET(sockfd, &rset);

	tv.tv_sec = sec;
	tv.tv_usec = 0;

	return select(sockfd + 1, &rset, NULL, NULL, &tv);
}

int SetTimeOut()
{
	int n;
	struct sockaddr_in serv_addr;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(6886);
	inet_pton(AF_INET, "128.64.6.114", &serv_addr.sin_addr);	//将字符串形式表示的地址转换为数值形式

	if ( (n = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0) {
		close(sockfd);
		if (EINTR == errno)
			exit(0);
	}

	char recvbuf[MAXSIZE + 1] = { 0 };
	int ret = IsReadable(sockfd, 20);
	printf("select return %d\n", ret);
	
	if (ret < 0) {
		printf("select error\n");
	} else if (0 == ret) {
		printf("timeout\n");
	} else {
		if ( (n = recvfrom(sockfd, recvbuf, MAXSIZE, 0, NULL, NULL)) < 0)
			close(sockfd);
		printf("recv %d byte data\n", n);
		printf("%s\n", recvbuf);
	}
	return n;
}

int main()
{
	// GetAllDefalutOptValue();

	// CreateTCPClientAndSendSimpleMsg();

	SetTimeOut();
	return 0;
}